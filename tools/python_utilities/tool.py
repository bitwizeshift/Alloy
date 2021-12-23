"""
This module provides utilities for wrapping binaries and scripts in a
consistent interface
"""

import subprocess
from typing import List, Dict, TextIO, overload
from pathlib import Path
from abc import abstractmethod, ABC
import sys


class Tool(ABC):
    """
    A tool that exists somewhere on this system
    """

    import subprocess

    def execute(self,
                *args,
                input: bytes = None,
                stdout: TextIO = None,
                stderr: TextIO = None,
                capture_output: bool = False,
                **kwargs) -> subprocess.CompletedProcess:
        """
        Executes the script by passing the specified args to it

        :param args: the arguments to forward to the script
        :return: the return code from executing the script
        """

        command = self.command(*args, **kwargs)

        args: Dict[str, object] = {}

        if stdout is not None:
            args['stdout'] = stdout
        if stderr is not None:
            args['stderr'] = stderr

        return subprocess.run(
            command,
            input=input,
            capture_output=capture_output,
            **args
        )

    def command(self, *args, **kwargs) -> List[str]:
        command: List[str] = []

        command.extend(self._prefix_command())
        command.extend(self._postfix_command(*args, **kwargs))

        return command

    @abstractmethod
    def _prefix_command(self) -> List[str]:
        """
        Generates the prefix of a command.

        This is used to produce the executor/script caller
        """
        pass

    @abstractmethod
    def _postfix_command(self, *args, **kwargs) -> List[str]:
        """
        Generates the postfix of a command, given arguments

        This is used to transform python command arguments into a list
        """
        pass


class Binary(Tool):
    """
    A base class for binaries that may be on a consumer's system
    """

    @overload
    def __init__(self, program: str): ...

    @overload
    def __init__(self, program: Path): ...

    def __init__(self, program):
        """

        """
        import shutil

        self._program: Path = None
        if isinstance(program, Path):
            self._program = program.resolve()
        elif isinstance(program, str):
            where = shutil.which(program)
            if where is not None:
                self._program = Path(where).resolve()

        if self._program is None or not self._program.exists():
            raise ValueError(f"Specified program '{program}' does not exist")

    def _prefix_command(self) -> List[str]:
        return [str(self.program)]

    @property
    def program(self) -> Path:
        return self._program


class ToolScript(Binary):
    """
    A base class for scripts in the "tools" path of this repository

    This hierarchy provides a simplified way of wrapping tool scripts along
    with their executor to support different languages of tooling (python,
    ruby, shell, etc).
    """

    def __init__(self, executor, script: str):
        """
        Constructs this ToolScript given the script name

        :param str script: the name of the script to execute
        """
        super().__init__(executor)
        from python_utilities.paths import ProjectPaths

        self._script = None
        if isinstance(script, Path):
            self._script: Path = script.resolve()
        elif isinstance(script, str):
            script_path = Path(script).resolve()

            if script_path.exists():
                self._script: Path = script_path
            else:
                self._script: Path = ProjectPaths.tools_root / script

        if self._script is None or not self._script.exists():
            raise ValueError(f"Specified script '{script}' does not exist")

    def execute(self,
                *args,
                stdin: TextIO = sys.stdin,
                stdout: TextIO = sys.stdout,
                stderr: TextIO = sys.stderr,
                **kwargs) -> int:
        """
        Executes the script by passing the specified args to it

        :param ... args: the arguments to forward to the script
        :param TextIO stdin: the standard input stream to use
        :param TextIO stdout: the stdandard output stream to write to
        :param TextIO stderr: the standard error stream to write to
        :param ... kwargs: any additional named arguments to forward
        :return: the return code from executing the script
        """

        command = self.command(*args, **kwargs)
        import subprocess

        result = subprocess.run(
            command,
            stdin=stdin or subprocess.DEVNULL,
            stdout=stdout or subprocess.DEVNULL,
            stderr=stderr or subprocess.DEVNULL,
        )

        return result.returncode

    def _prefix_command(self) -> List[str]:
        command: List[str] = []
        command.extend(super()._prefix_command())
        command.append(str(self._script))
        return command

    @property
    def script(self) -> Path:
        return self._script


class PythonToolScript(ToolScript):
    """
    A class for wrapping python scripts in this repository
    """

    def __init__(self, script: str):
        """
        Constructs this tools script

        :param str script: the name of the script to execute
        :param Path executor: the path to the python executor. Default python3
        """
        import shutil
        super().__init__(
            executor=Path(shutil.which("python3")),
            script=script
        )

    def _postfix_command(self, *args) -> List[str]:
        return [str(a) for a in args]
