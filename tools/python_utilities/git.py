"""
This module contains a wrapper around Git
"""

from __future__ import annotations
from pathlib import Path
from typing import List


class Git:
    """
    A wrapper class for the Git SCM utility
    """

    class LineInterval:
        """
        A line interval, as seen in a git diff
        """

        def __init__(self, start, end):
            self.start: int = start
            self.end: int = end
            pass

    class Diff:
        """
        The delta between two states of an object.

        This encodes both the 'from' and 'to' states for the file name,
        permissions, and line change numbers
        """

        def __init__(self):
            """
            Constructs this diff with None and empty values
            """
            self.from_path: Path = None
            self.to_path: Path = None
            self.from_changes: List[Git.LineInterval] = []
            self.to_changes: List[Git.LineInterval] = []
            self.from_permissions: str = None
            self.to_permissions: str = None
            self.from_sha1: str = None
            self.to_sha1: str = None

        def is_rename(self):
            """
            Queries whether this diff contains a file-rename operation
            """

            return (
                self.from_path != self.to_path and
                self.from_path is not None
                and self.to_path is not None
            )

        def is_deletion(self):
            """
            Queries whether this diff contains a file deletion operation
            """

            return self.to_path is None

        def is_addition(self):
            """
            Queries whether this diff contains a file addition operation
            """

            return self.from_path is None

    def __init__(self,
                 program: Path = None,
                 cwd: Path = None):
        """
        Constructs this git object

        :param Path program: the program or program name for the git executable
        """
        import shutil
        self._executable = Path(shutil.which(program or 'git'))
        self._cwd: Path = None
        if cwd is not None:
            self._cwd = Path(cwd).resolve()

    def is_tracked(self, file: Path) -> bool:
        """
        Queries whether a specific file is tracked by git

        :param Path file: the file to check
        :return bool: true if the file is tracked
        """
        import subprocess

        if file.is_absolute() and self._cwd is not None:
            file = file.relative_to(self._cwd)

        command = [
            str(self._executable),
            "ls-files",
            "--error-unmatch",
            str(file)
        ]
        process = subprocess.run(
            command,
            capture_output=True,
            cwd=self._cwd
        )

        return process.returncode == 0

    def years_modified(self, file: Path) -> List[int]:
        """
        Computes the years in which a specific file was modified

        :param Path file: the file to query
        :return List[int]: a list of years indicating the dates modified
        """

        command = [
            str(self._executable),
            "log",
            "--date=format:%Y",
            "--pretty=%ad",
            "--follow",
            "--no-show-signature",
            str(Path(file).absolute())
        ]

        output = Git._execute_command(command, cwd=self._cwd).strip()
        if output == "":
            return []
        lines = output.split('\n')

        return list(dict.fromkeys([int(x) for x in lines]))

    def diff(self,
             files: List[Path] = None,
             commitish: str = None,
             diff_algorithm: str = None,
             lines_of_context: int = 0,
             cached: bool = False) -> List[Git.Diff]:
        """
        Computes and returns the result of `git diff`

        Invokes `git diff` with the requested arguments, and returns this
        result as a pythonic object that can be inspected and used.
        """
        command = [
            str(self._executable),
            "diff",
            f"--unified={lines_of_context}",
            "--no-color",
            "--no-color-moved",
            "--no-color-moved-ws",
            "--no-prefix",
        ]
        if diff_algorithm is not None:
            command.append(f"--diff-algorithm={diff_algorithm}")
        if cached:
            command.append("--cached")
        if commitish is not None:
            command.append(str(commitish))
        if files is not None:

            def make_relative(file: Path):
                if file.is_absolute() and self._cwd is not None:
                    return file.relative_to(self._cwd)
                return file

            command.append("--")
            command.extend([str(make_relative(file)) for file in files])

        lines = Git._execute_command(
            command, cwd=self._cwd).strip().split('\n')

        if not lines:
            return []

        result = []
        change = None
        for line in lines:
            if line.startswith('diff --git'):
                if change is not None:
                    result.append(change)
                change = Git.Diff()
                items = line.split(' ')
                change.from_path = Path(items[2])
                change.to_path = Path(items[3])
                continue
            if line.startswith('--- '):
                p = Path(line.split(' ')[1].strip())
                change.from_path = None if p == Path('/dev/null') else p
                continue
            if line.startswith('+++ '):
                p = Path(line.split(' ')[1].strip())
                change.to_path = None if p == Path('/dev/null') else p
                continue
            if line.startswith('@@ '):
                items = line.split(' ')
                assert(items[0] == '@@')
                assert(items[3] == '@@')

                from_interval = Git._make_line_interval(
                    items[1][1:].split(','))
                to_interval = Git._make_line_interval(items[2][1:].split(','))

                if from_interval is not None:
                    change.from_changes.append(from_interval)

                if to_interval is not None:
                    change.to_changes.append(to_interval)
                continue

            if line.startswith('index '):
                # If the mode hasn't changed, this is the third entry
                items = line.split(' ')
                assert(items[0] == 'index')

                if len(items) == 3:
                    change.to_permissions = items[2]
                    change.from_permissions = items[2]

                change.from_sha1, change.to_sha1 = items[1].split('..')

                # git diff does not show us 'new' files coherently if a file is
                # empty. It doesn't give a `-- /dev/null ++ file`-style line,
                # so we have to use a best-guess by checking other output for
                # this.
                if change.from_sha1.startswith('0000000'):
                    change.from_path = None
                    change.from_permissions = None
                    change.from_sha1 = None
                continue
            if line.startswith('old file mode '):
                change.from_permissions = line[13:].strip()
                continue
            if line.startswith('new file mode '):
                change.to_permissions = line[13:].strip()
                continue
            if line.startswith('deleted file mode '):
                change.from_permissions = line[18:].strip()
                continue
        if change is not None:
            result.append(change)

        return result

    def show_file(self,
                  file: Path,
                  commitish: str = None,
                  cached: bool = False) -> str:
        """
        Shows the state of the file at the specified moment in time

        :param Path file: the file path to show
        :param str commitish: the commitish value to show the file at
        :param bool cached: whether to show the staged state of the file
        :return str: the utf-8 encoded file
        """

        if file.is_absolute() and self._cwd is not None:
            file = file.relative_to(self._cwd)

        command = [str(self._executable), "show"]
        if cached and commitish:
            raise ValueError(
                "Both 'cached' and 'commitish' cannot be active at the same "
                "time."
            )
        if cached:
            command.append(f":{file}")
        elif commitish is not None:
            command.append(f"{commitish}:{file}")
        else:
            command.append(f"HEAD:{file}")

        return Git._execute_command(command, cwd=self._cwd)

    def changed_files(self,
                      commitish: str = None,
                      cached: bool = False) -> List[Path]:
        """
        Computes the list of file names that were changed in a given diff

        :param str commitish: the sha1, link, range, branch, etc to query
                              against
        :param bool cached: whether to query against the --cached state
        :return List[Path]: the list of paths of files that have changed
        """

        command = [
            str(self._executable),
            "diff",
            "--name-only",
            "--diff-filter=d"
        ]
        if cached:
            command.append("--cached")
        if commitish is not None:
            command.append(str(commitish))

        lines = Git._execute_command(
            command, cwd=self._cwd).strip().split('\n')
        if self._cwd is not None:
            return [self._cwd / Path(p) for p in lines]
        else:
            return [Path(p) for p in lines]

    @staticmethod
    def _make_line_interval(entries: List[str]) -> Git.LineInterval:
        """
        Computes a line interval give the integral values in the  @@ ... @@
        diff line

        :param List[str] entries: the entries from the @@ line, split on `,`
        :return Git.LineInterval: the computed line interval
        """
        assert(len(entries) <= 2)
        start = int(entries[0])
        if len(entries) == 1:
            return Git.LineInterval(start, start)

        length = int(entries[1])
        if length == 0:
            return None

        return Git.LineInterval(start, start + length)

    @staticmethod
    def _execute_command(command: List[str], cwd=None) -> str:
        """
        Executes the given command, raising a runtime error on nonzero exit
        code

        :param List[str] command: the command as a list of arguments
        :return str: the result of stdout, encoded in utf-8
        """
        import subprocess

        process = subprocess.run(
            command,
            capture_output=True,
            cwd=cwd)

        if process.returncode != 0:
            command_string = " ".join(command)
            error = process.stderr.decode('utf-8')
            raise RuntimeError(f"Error running '{command_string}': {error}")

        return process.stdout.decode('utf-8')
