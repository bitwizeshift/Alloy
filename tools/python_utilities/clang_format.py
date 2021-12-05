"""
This module contains a wrapper around the clang-format binary
"""

from pathlib import Path
from typing import List, Tuple
from python_utilities.tool import Binary


class ClangFormat(Binary):
    """
    A driver class for the `clang-tidy` binary

    This wraps the majority of the functionality from `clang-tidy` into a
    simple class
    """

    def __init__(self,
                 program: Path = None,
                 version_override: Tuple[int, int, int] = None,
                 in_place: bool = False,
                 werror: bool = False,
                 sort_includes: bool = False):
        """
        Constructs this driver by specifying arguments that may be reused
        across different executions

        :param Path program: the path to the clang-tidy executable
        :param Tuple[int,intint] version_override: Whether to override the
                                                   known clang-tidy version
        :param bool werror: Whether to error on format warnings
        :param bool sort_includes: Whether to sort includes
        :param bool in_place: Set this to modify the files in-place
        """
        # If the binary is not specified, try finding it in path
        super().__init__(program or 'clang-format')

        self._command: List[str] = []
        self._version = version_override

        if sort_includes:
            self._command.append("--sort-includes")

        if in_place:
            self._command.append("-i")

        if werror:
            self._command.append('--Werror')

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
                f"Unable to extract clang-format version. Error: '{stderr}'")

        VERSION_REGEX = re.compile(".*version (\\d+)\\.(\\d+)\\.(\\d+).*")

        for line in stdout.split('\n'):
            match = VERSION_REGEX.match(line)
            if match is None:
                continue

            (major, minor, patch) = match.groups()
            self._version = (int(major), int(minor), int(patch))

            return self._version

        raise RuntimeError(
            f"Unable to extract clang-format version from output.")

    def _postfix_command(self,
                         dry_run: bool = False,
                         assume_filename: Path = None,
                         in_place: bool = False,
                         files: List[Path] = None) -> List[str]:
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

        if dry_run:
            command.append("--dry-run")

        if assume_filename is not None:
            command.append(f"--assume-filename={assume_filename}")

        if files is not None:
            command.extend([str(x) for x in files])

        if in_place and "-i" not in command:
            command.append("-i")

        return command
