"""
This module contains a wrapper around the clang-tidy executable
"""

from pathlib import Path
from typing import List, Tuple, TextIO
from python_utilities.tool import Binary


class LineInterval:
    """
    A closed interval of lines
    """

    def __init__(self, start: int, end: int):
        self.start = start
        self.end = end

    def __str__(self):
        return f"[{self.start}, {self.end}]"

    @classmethod
    def from_start_and_length(cls, start: int, length: int):
        """
        Constructs a LineInterval from the start and a length

        :param int start: the start of the interval
        :param int length: the length of the interval
        :return: the interval
        """
        return cls(start, start + length)

    @classmethod
    def closed_interval(cls, start: int, end: int):
        """
        Constructs a LineInterval from a closed interval range of start to end

        :param int start: the start of the interval
        :param int end: the end of the interval
        :return: the interval
        """
        return cls(start, end)

    @classmethod
    def single_line(cls, line):
        """
        Constructs a LineInterval from a single line location

        :param int line: the line
        :return: the interval
        """
        return LineInterval.closed_interval(cls, line, line)


class LineFilter:
    """
    An association of file names and line-intervals with which to filter when
    specifying --line-filters
    """

    def __init__(self, file: Path):
        self.file = file
        self.lines = []

    def __str__(self):
        line_filter = ",".join(str(x) for x in self.lines)
        result = f'{{"name":"{self.file}","lines":[' + line_filter + ']}'
        return result


class ClangTidy(Binary):
    """
    A driver class for the `clang-tidy` binary

    This wraps the majority of the functionality from `clang-tidy` into a
    simple class
    """

    def __init__(self,
                 program: Path = None,
                 version_override: Tuple[int, int, int] = None,
                 checks: List[str] = None,
                 warnings_as_errors: List[str] = None,
                 format_style: str = None,
                 config_file: Path = None,
                 database_path: Path = None,
                 header_filter: str = None,
                 quiet: bool = False,
                 use_color: bool = False,
                 fix: bool = False,
                 fix_errors: bool = False):
        """
        Constructs this driver by specifying arguments that may be reused
        across different executions

        :param Path program: the path to the clang-tidy executable
        :param Tuple[int,intint] version_override: Whether to override the
                                                   known clang-tidy version
        :param List[str] checks: A list of checks for clang-tidy to invoke
        :param List[str] warnings_as_errors: A list of checks to error about
                                             rather than warn on
        :param str format_style: Argument for the --format-style flag
        :param Path config_file: The path to the clang-tidy configuration file
        :param Path database_path: The path to the compile-commands.json file
        :param str header_filter: A glob notation for allowed headers to scan
        :param bool quiet: Whether to invoke this in "quiet" mode
        :param bool use_color: Whether to provide color
        :param bool fix: Whether to automatically fix diagnostics
        :param bool fix_errors: Whether to automaticall fix error diagnostics
        """
        # If the binary is not specified, try finding it in path
        super().__init__(program or 'clang-tidy')

        self._command: List[str] = []
        self._version = version_override

        if checks is not None:
            checks_string = ",".join(checks)
            self._command.append(f'--checks={checks_string}')

        if warnings_as_errors is not None:
            # Allow for 'True' to toggle all warnings to errors
            if isinstance(warnings_as_errors, bool):
                warning_string = "*" if warnings_as_errors else ""
            else:
                warning_string = ",".join(warnings_as_errors)
            self._command.append(f'--warnings-as-errors={warning_string}')

        if format_style is not None:
            self._command.append(f'--format-style={format_style}')

        if config_file is not None:
            self._command.append(f'--config-file={config_file}')

        if database_path is not None:
            self._command.append(f'-p={database_path}')

        if header_filter is not None:
            self._command.append(f'--header-filter={header_filter}')

        if quiet:
            self._command.append("--quiet")

        # use-color was only added in clang-tidy 12.0.0
        if use_color and self.version >= (12, 0, 0):
            self._command.append("--use-color")

        if fix:
            self._command.append("--fix")

        if fix_errors:
            self._command.append("--fix-errors")

    @property
    def version(self) -> Tuple[int, int, int]:
        """
        Computes and returns the version of the ClangTidy binary as a property

        :return Tuple[int,int,int]: The (major,minor,patch) version as a tuple
        """

        if self._version is not None:
            return self._version

        import subprocess as sp
        import re

        result = sp.run([str(self.program), "--version"], capture_output=True)
        stdout = result.stdout.decode('utf-8')
        stderr = result.stderr.decode('utf-8')

        if result.returncode != 0:
            raise RuntimeError(
                f"Unable to extract clang-tidy version. Error: '{stderr}'")

        VERSION_REGEX = re.compile(".*version (\\d+)\\.(\\d+)\\.(\\d+).*")

        for line in stdout.split('\n'):
            match = VERSION_REGEX.match(line)
            if match is None:
                continue

            (major, minor, patch) = match.groups()
            self._version = (int(major), int(minor), int(patch))

            return self._version

        raise RuntimeError(
            f"Unable to extract clang-tidy version from output.")

    def _postfix_command(self,
                         files: List[Path] = None,
                         fix: bool = False,
                         fix_errors: bool = False,
                         line_filters: List[LineFilter] = None) -> List[str]:
        """
        Computes the command that will be executed given the files and line
        filters

        :param List[Path] files: the list of files to execute this on
        :param List[LineFilter] line_filters: the list of line filters to
                                              filter in checks
        :return List[str]: The command as a list of arguments
        """

        import copy
        command = copy.deepcopy(self._command)

        if line_filters is not None:
            line_filters_inner = ",".join(str(x) for x in line_filters)
            line_filters_arg = f"[{line_filters_inner}]"

            command.extend(["--line-filter", line_filters_arg])

        if files is not None:
            command.extend([str(x) for x in files])

        if fix and "--fix" not in command:
            command.extend("--fix")

        if fix_errors and "--fix-errors" not in command:
            command.extend("--fix-errors")

        return command
