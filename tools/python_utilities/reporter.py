"""
This module provides a reporter utility for generating consistent output for
any script that makes use of this
"""

from pathlib import Path
from re import S
import subprocess
from typing import List, Callable, TextIO, Type, Union, Optional
from abc import ABC, abstractmethod
from python_utilities.git import Git
from python_utilities.sources import SourceGroup, read_file
from python_utilities.format import AnsiFormat
import sys


_PROGRESS_ARG = "--progress"
_NO_PROGRESS_ARG = "--no-progress"


class ReportEntry:
    """
    The result of the report process
    """

    @classmethod
    def from_bytes(cls,
                   succeeded: bool,
                   stdout: bytes = None,
                   stderr: bytes = None):

        stdout_str = None
        if stdout is not None:
            stdout_str = stdout.decode().strip()
            stdout_str = None if stdout_str == "" else stdout_str

        stderr_str = None
        if stderr is not None:
            stderr_str = stderr.decode().strip()
            stderr_str = None if stderr_str == "" else stderr_str

        return cls(succeeded, stdout, stderr)

    @classmethod
    def from_completed_process(cls, result: subprocess.CompletedProcess):
        return ReportEntry.from_bytes(
            result.returncode == 0,
            result.stdout,
            result.stderr
        )

    def __init__(self,
                 succeeded: bool = True,
                 stdout: str = None,
                 stderr: str = None):
        self.succeeded: int = succeeded
        self.stdout: str = stdout
        self.stderr: str = stderr

    def print(self, message: str) -> None:
        if message is None:
            return

        self.stdout = self.stdout or ""
        self.stdout += message

    def print_error(self, message: str) -> None:
        if message is None:
            return

        self.stderr = self.stderr or ""
        self.stderr += message


class Reportable(ABC):
    """
    A base class for a reportable object
    """

    def command(self) -> List[str]:
        """
        The command used to generate the reportable

        By default, this returns a normalized sys.argv
        """
        import sys
        executable = Path(sys.argv[0]).resolve().name

        cmd = [str(executable), *sys.argv[1:]]

        # Remove any '--verbose' or '-v' flags that may be present
        for flag in [*self.verbose_flags(), _PROGRESS_ARG, _NO_PROGRESS_ARG]:
            if flag in cmd:
                cmd.remove(flag)

        return cmd

    @staticmethod
    def verbose_flags() -> List[str]:
        """
        Flag(s) that can be used to generate "verbose" output. By default,
        this is -v and --verbose; though this may be overridden in subclasses.

        :return List[str]: the list of flags to enable verbose output
        """
        return ["-v", "--verbose"]


class Verifier(Reportable):
    """
    An object capable of verifying a single file

    Verifiers are used in conjunction with Reporters to generate reports.
    Verifiers must be thread-safe
    """

    def __init__(self):
        pass

    @abstractmethod
    def verify(self, file: Path) -> ReportEntry:
        """
        Checks whether the specified file passes verification

        :param Path file: the file to verify
        """
        ...


class Fixer(Reportable):
    """
    An object capable of fixing a single file
    """

    def fix(self, file: Path) -> Optional[ReportEntry]:
        """
        Fixes the specified file and generates a report entry

        If the file does not need to be fixed, returns None

        :param Path file: the file to be fixed
        :return Optional[ReportEntry]: An entry indicating success. None if no
            fix is needed.
        """

        # If we are a verifier, then check that the file needs to be fixed.
        if isinstance(self, Verifier) and self.verify(file).succeeded:
            return None

        # TODO: Should there be a post-verification as well?

        return self.on_fix(file)

    @abstractmethod
    def on_fix(self, file: Path) -> ReportEntry:
        """
        Hook for performing the fix logic. This is the function that should be
        overridden in a derived class

        :param Path file: the file to be fixed
        :return ReportEntry: the generated report containing its success
        """
        ...

    @staticmethod
    def fix_flags() -> List[str]:
        """
        Flag(s) that can be used to generate the "fix" command. By default,
        this is just --fix; though this may be overridden in subclasses.

        :return List[str]: the list of flags to generate fixes
        """
        return ["--fix"]


class ProgressBar:
    """
    A simple/naive implementation of a console-I/O based progress-bar
    """

    def __init__(self,
                 total: int,
                 prefix: str = "Progress: ",
                 bar_width: int = 60,
                 filled: str = '█',
                 empty: str = '.',
                 stdout: TextIO = sys.stdout):
        """
        Constructs this progress bar

        :param int total: the total number of jobs in the progress bar
        :param str prefix: the string to prefix the job with
        :param int bar_width: the number of characters to use for the bar
        :param str filled: the character to use for the bar being filled
        :param str empty: the character to use for the bar when empty
        :param TextIO stdout: the output to write this to
        """

        self._bar_width = bar_width
        self._stdout = stdout
        self._total = total

        if len(filled) != 1:
            raise ValueError("'filled' must be a single character")
        if len(empty) != 1:
            raise ValueError("'empty' must be a single character")

        self._filled = filled
        self._empty = empty
        self._prefix = prefix

        self._format = "{message}[{filled:{empty}<{}}{empty}] ({},{})"
        self._back = '\r'
        self._last_message_length: int = 0

    def print_progress_at(self, current: int):
        """
        Prints the current progress of this bar given the current state

        :param int current: the current job completed
        """
        if current > self._total:
            raise ValueError(
                f"'current' cannot exceed total ({current} > {self._total})"
            )
        elif current < 0:
            raise ValueError(f"'current' cannot be below zero ({current} < 0)")

        ratio = float(current) / float(self._total)
        filled = self._filled * int(ratio * self._bar_width)
        percent = int(ratio * 100)

        format = "{prefix}[{filled:{empty}<{bar_width}}]" \
                 " {percent: >3}% ({current}/{total})"
        formatted_message = format.format(
            prefix=self._prefix,
            filled=filled,
            empty=self._empty,
            bar_width=self._bar_width,
            percent=percent,
            current=current,
            total=self._total
        )
        self.clear()
        print(formatted_message, end='\r', flush=True)
        self._last_message_length = len(formatted_message)

    def clear(self):
        """
        Clears the current progress bar so that it's no longer visible on
        screen.

        This effectively writes empty characters over the bar so that it does
        not peek through
        """
        print(' ' * self._last_message_length, end='\r', flush=True)
        self._last_message_length = 0


class Reporter:
    """
    A class for generating reports of verification items

    Reporters enxapsulate the dispatching and managing of the general I/O
    for the files being verified
    """

    class _ProgressToken:
        """
        An internal type to share progress between different concurrent jobs
        """

        def __init__(self, total_jobs: int):
            self._progress_bar = ProgressBar(total_jobs)
            self._current_job = 0
            self._total_jobs = total_jobs

        def increment(self):
            self._current_job += 1

        def print(self):
            self._progress_bar.print_progress_at(self._current_job)

        def clear(self):
            self._progress_bar.clear()

    def __init__(self,
                 name: str,
                 reportable: Union[Verifier, Fixer],
                 files: List[Path],
                 show_progress: bool = True):
        import threading
        if not isinstance(reportable, Reportable):
            raise ValueError("'reportable' must be a Reportable")

        self._name = name
        self._reportable: Union[Verifier, Fixer] = reportable
        self._files: List[Path] = files
        self._passing_files: List[Path] = []
        self._failing_files: List[Path] = []
        self._lock = threading.Lock()
        self._token = Reporter._ProgressToken(len(files))
        self._show_progress = show_progress

    def report(self,
               jobs: int = None,
               fix: bool = False,
               verbose: bool = False) -> int:
        """
        Generates a report from the given report generator

        :param int jobs: the number of parallel jobs to support
        :param bool fix: whether to fix with the reportable
        :param bool verbose: enables printing verbose output
        :return int: 0 on success, nonzero on error
        """

        if fix and not isinstance(self._reportable, Fixer):
            raise ValueError(
                "fix argument cannot be true if reportable is not a Fixer"
            )
        if not fix and not isinstance(self._reportable, Verifier):
            raise ValueError(
                "fix argument cannot be false if reportable is not a Verifier"
            )

        from concurrent.futures import ThreadPoolExecutor, Future
        import multiprocessing

        if len(self._files) == 0:
            print(f"No files for {self._name} validation")
            return 0

        jobs = jobs or multiprocessing.cpu_count()

        executor = ThreadPoolExecutor(
            max_workers=jobs,
            thread_name_prefix=self._name
        )
        for file in self._files:

            callback: Callable[[Future[ReportEntry]], None] = None
            if fix:
                future = executor.submit(self._reportable.fix, file)
            else:
                future = executor.submit(self._reportable.verify, file)

            def callback(f, file=file):
                return self._on_job_complete(
                    file,
                    verbose,
                    f.result()
                )

            future.add_done_callback(callback)

        # Wait for all jobs to complete
        executor.shutdown(wait=True)
        self._token.clear()

        if fix:
            return self._report_fix()
        return self._report_verification(verbose)

    def _report_verification(self, verbose: bool) -> int:
        """
        Reports on verification results and returns whether this processed
        successfully

        :param bool verbose: enables verbose output
        :return int: the process return code
        """
        if self._failing_files == []:
            pass_count = len(self._passing_files)
            plural = "file" if pass_count == 1 else "files"
            print(f"{pass_count} {plural} passed {self._name} validation")
            return 0

        print(
            f"The following files failed {self._name} "
            f"validation:",
            file=sys.stderr
        )
        for file in self._failing_files:
            print(f" • {file}", file=sys.stderr)

        command = " ".join(self._reportable.command())

        if not verbose:
            print("", file=sys.stderr)
            print(
                f"Additional diagnostics may be provided by running:",
                file=sys.stderr
            )
            flag = self._reportable.verbose_flags()[0]
            Reporter._print_command(f"{command} {flag}", file=sys.stderr)

        if isinstance(self._reportable, Fixer):
            print("", file=sys.stderr)
            print(
                f"Fixes can be automatically applied to your work tree with:",
                file=sys.stderr
            )
            flag = self._reportable.fix_flags()[0]
            Reporter._print_command(f"{command} {flag}", file=sys.stderr)
            print("", file=sys.stderr)
        return 1

    def _report_fix(self) -> int:
        """
        Reports on the result of fix operations and returns whether this
        processed successfully

        :return int: the process return code
        """
        if self._failing_files == [] and self._passing_files == []:
            print(f"No files needed to be fixed for {self._name} validation")
            return 0

        if self._passing_files != []:
            print(
                f"The following files were automatically fixed for "
                f"{self._name} validation:"
            )

            for file in self._passing_files:
                print(f" • {file}")

        if self._failing_files != []:
            print(
                f"The following files could not be automatically fixed for "
                f"{self._name} validation:",
                file=sys.stderr
            )

            for file in self._failing_files:
                print(
                    f" • {file}",
                    file=sys.stderr
                )
            print("", file=sys.stderr)
            print(
                f"{AnsiFormat.BRIGHT_WHITE}NOTE:{AnsiFormat.RESET} These "
                f"files will require manual changes to pass {self._name} "
                f"validation!"
            )

        # Return '0' on success
        return len(self._failing_files)

    @staticmethod
    def _print_command(command: str, **kwargs):
        """
        Formatted prints a command using ansi color codes

        :param str command: the command to print
        """
        prefix = f"{AnsiFormat.UNDERLINE}{AnsiFormat.BRIGHT_WHITE}"
        suffix = f"{AnsiFormat.RESET}"
        print(f">  {prefix}{command}{suffix}", **kwargs)

    def _on_job_complete(self,
                         file: Path,
                         verbose: bool,
                         result: ReportEntry) -> None:
        """
        Callback issued when a job has completed

        :param Path file: the file for the job that has completed
        :param bool verbose: whether to print verbosely
        :param ReportEntry result: the result of the report
        """
        import sys
        with self._lock:
            self._token.increment()

            # If we are printing verbosely, then print all stdout and stderr
            # results to the screen on failure
            if verbose and result is not None and not result.succeeded:
                if self._show_progress:
                    self._token.clear()
                if result.stdout is not None:
                    print("-" * 80, file=sys.stdout)
                    print(f"stdout for {file}:", file=sys.stdout)
                    print("-" * 80, file=sys.stdout)
                    print(result.stdout, file=sys.stdout)
                if result.stderr is not None:
                    print("-" * 80, file=sys.stderr)
                    print(f"stderr for {file}:", file=sys.stderr)
                    print("-" * 80, file=sys.stderr)
                    print(result.stderr, file=sys.stderr)

            if self._show_progress:
                self._token.print()

            # A 'None' means there was nothing to process. This is only used
            # for fixes
            if result is None:
                return

            if result.succeeded:
                self._passing_files.append(file)
            else:
                self._failing_files.append(file)


class ReporterOptions:
    """
    A collection of basic options that all the different reporters will offer

    This class can be extended, and the @classmethod `parse_arguments`
    overridden in subclasses to support custom arguments per-script.
    """

    import argparse

    def __init__(self):
        self._git: Git = None
        self._staged: bool = False
        self._show_progress: bool = False
        self._verbose: bool = False
        self._group: SourceGroup = None
        self._files: List[Path] = []
        self._jobs: int = 1
        self._fix: bool = False

    @property
    def group(self) -> SourceGroup:
        return self._group

    @property
    def staged(self) -> bool:
        return self._staged

    @property
    def show_progress(self) -> bool:
        return self._show_progress

    @property
    def verbose(self) -> bool:
        return self._verbose

    @property
    def files(self) -> List[Path]:
        return self._files

    @property
    def jobs(self) -> int:
        return self._jobs

    @property
    def fix(self) -> bool:
        return self._fix

    def read_file(self, path: Path) -> str:
        return read_file(path, self._git, staged=self.staged)

    @classmethod
    def make_options_for(cls,
                         reportable: Type[Reportable],
                         parser: argparse.ArgumentParser,
                         git: Git = Git(),
                         extensions: List[str] = None):
        import multiprocessing

        processing_group = parser.add_argument_group("processing arguments")
        parser.usage = f"{Path(sys.argv[0]).name} [options] [file [file ...]]"

        processing_group.add_argument(
            "file",
            nargs='*',
            type=Path,
            help="Additional files to verify or fix",
        )

        processing_group.add_argument(
            "--source-group",
            help="The source group to glob and format. "
            "Note: --source-group=staged implies '--staged'",
            action="store",
            type=SourceGroup,
            default=SourceGroup.INPUT,
            choices=list(SourceGroup)
        )
        processing_group.add_argument(
            "--staged",
            help="Enable to check the staged state of the specified sources",
            dest="staged",
            action='store_true',
            default=None
        )
        processing_group.add_argument(
            "--no-staged",
            dest="staged",
            action="store_false"
        )
        processing_group.add_argument(
            "-j", "--jobs",
            metavar="N",
            help="Allow N jobs at once. Defaults to CPU-count parallelism.",
            action="store",
            dest="jobs",
            type=int,
            default=multiprocessing.cpu_count()
        )

        if issubclass(reportable, Fixer):
            processing_group.add_argument(
                *reportable.fix_flags(),
                help="Automatically applies any possible fixes in-place. "
                "Fixes are always applied to the current work-tree, and not "
                "the staged file.",
                action="store_true",
                default=False
            )

        output_group = parser.add_argument_group("output control arguments")

        output_group.add_argument(
            *reportable.verbose_flags(),
            help="Enables verbose output",
            action="store_true",
            default=False,
        )
        output_group.add_argument(
            _PROGRESS_ARG,
            help="Enables the progress bar while checking (default: True)",
            action='store_true',
            default=True
        )
        output_group.add_argument(
            _NO_PROGRESS_ARG,
            action="store_false",
            dest="progress",
        )

        args = parser.parse_args()

        group: SourceGroup = args.source_group

        if extensions is None:
            def find(): return group.find(git)
        else:
            def find(): return group.find_with_extensions(git, extensions)

        files = [f.resolve() for f in find()]
        input_files = [Path(f).resolve() for f in args.file]

        for f in input_files:
            if not f.exists():
                parser.error(f"Specified file does not exist: {f}")

        files.extend(input_files)

        if args.staged is None:
            read_from_stage = (group == SourceGroup.STAGED)
        else:
            read_from_stage = args.staged

        result = cls()
        result._git = git
        result._staged = read_from_stage
        result._show_progress = args.progress
        result._verbose = args.verbose
        result._group = group
        result._files = files
        result._jobs = args.jobs
        result._fix = args.fix if "fix" in args else False

        return result


__all__ = [
    'Reportable',
    'Fixer',
    'Verifier',
    'Reporter',
    'ReporterOptions',
    'ReportEntry',
    'ProgressBar'
]
