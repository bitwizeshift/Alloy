"""
This module defines the generic API used for managing checks to be done by
git hooks.
"""

import subprocess
from abc import ABC, abstractmethod
from python_utilities.format import AnsiFormat
from typing import List, TextIO, Callable
from pathlib import Path
import sys


class CheckResult:

    @classmethod
    def from_completed_process(cls, process: subprocess.CompletedProcess):
        return cls(
            stdout=process.stdout.decode(),
            stderr=process.stderr.decode(),
            returncode=process.returncode
        )

    def __init__(self,
                 returncode: int = 0,
                 stdout: str = "",
                 stderr: str = ""):
        self.returncode = returncode
        self.stdout = stdout
        self.stderr = stderr

    def print_message(self, *args, break_at: int = None, indent: int = None):
        self.stdout += CheckResult._format_message(
            *args,
            break_at=break_at,
            indent=indent
        )

    def print_error(self, *args, break_at: int = None, indent: int = None):
        self.returncode = 1
        self.stderr += CheckResult._format_message(
            *args,
            break_at=break_at,
            indent=indent
        )

    @staticmethod
    def _format_message(*args, break_at: int = None, indent: int = None):
        import os
        import textwrap
        if indent is None:
            indent_str = ""
        else:
            indent_str = " " * indent

        column_max = break_at or min(os.get_terminal_size().columns, 68)
        result = ""
        for text in args:
            wrapped_text = textwrap.wrap(
                text,
                width=column_max,
                initial_indent=indent_str,
                subsequent_indent=indent_str
            )
            for line in wrapped_text:
                result += line
                result += '\n'
        return result


class Check(ABC):
    """
    A wrapper around tool checks that need to be executed
    """

    def __init__(self, message=None):
        """
        Constructs this check from the given arguments

        :param str message: the message to print while validating
        """
        self._message = message or ""

    @abstractmethod
    def execute(self) -> CheckResult:
        """
        Executes this script and returns the completed process

        :return CompletedProcess: the completed process
        """
        ...

    @property
    def message(self):
        """
        Gets the message to print
        """
        return self._message


class CheckScript(Check):
    """
    A wrapper around scripts used for checking the state of the repo
    """

    def __init__(self,
                 script: Path,
                 arguments: List[str] = None,
                 message: str = None):
        """
        Constructs this hook from the given arguments

        :param Path script: the script to execute
        :param List[str] Arguments: the list of arguments to add to the script
        :param str message: the message to print while validating
        """
        self._script = Path(script).resolve()
        self._command = [str(self._script)]
        if arguments is not None:
            self._command.extend(arguments)
        super().__init__(message=message or str(self._script.name))

    def execute(self) -> CheckResult:
        """
        Executes this script and returns the result

        :return CheckResult: the result of checking
        """
        process = subprocess.run(self._command, capture_output=True)
        return CheckResult.from_completed_process(process)


class CheckFunction(Check):
    """
    Wraps a callable object as a check action
    """

    def __init__(self, message, fn, *args, **kwargs):
        """
        Constructs this callable

        :param str message: the message to print while validating
        :param action: the action to perform
        """
        super().__init__(message=message or "")
        self._action = lambda args=args, kwargs=kwargs: fn(*args, **kwargs)

    def execute(self) -> CheckResult:
        """
        Executes the stored action and returns the result

        :return CheckResult: the result of checking
        """
        return self._action()


class CheckReporter:
    """
    A class for generating a report by running a sequence of scripts for
    pre-commit hook validation

    All scripts are relative to the specified hook-root, which by default is
    the tools directory
    """

    def __init__(self, message: str = None):
        """
        Constructs this CheckReporter

        :param str message: the message to print for the report
        """
        self._message: str = message
        self._hooks: List[Check] = []

    def add_check(self, hook: Check):
        """
        Adds a hook manually

        :param Check hook: the hook to add
        """
        self._hooks.append(hook)

    def report(self,
               stdout: TextIO = sys.stdout,
               column_width: int = None) -> int:
        """
        Generates a report by running each of the scripts in sequence.

        A report is considered failed if any one of the hooks fails. A failure
        from a hook does not stop subsequent checks from occurring.

        :param TextIO stdout: the output console to write to
        :param int column_width: the number of columns across to use for
                                 formatting
        :return int: 0 on success, 1 on any hook failure
        """

        import os
        stdout = stdout or sys.stdout

        column_width = column_width or min(os.get_terminal_size().columns, 80)

        count = 0
        is_passing = True

        if self._message is not None:
            divider = "=" * column_width
            print()
            print(self._message)
            print(divider)
        for hook in self._hooks:
            count += 1

            prefix = f"{count}."
            message = hook.message
            message_start = f"{prefix: <4}{message}"

            message_end = "Processing"
            padding_length = column_width - len(message_end)

            print(
                f"{message_start:.<{padding_length}}"
                f"{message_end}\r",
                file=stdout,
                end=''
            )

            process = hook.execute()

            if process.returncode == 0:
                lines: str = process.stdout
                color = AnsiFormat.GREEN
                state = "Passed"
            else:
                lines: str = process.stderr
                color = AnsiFormat.RED
                state = "Failed"

            message_end = state
            padding_length = column_width - len(message_end)

            print(
                f"{color}{message_start:.<{padding_length}}"
                f"{message_end}{AnsiFormat.RESET}",
                file=stdout
            )
            lines = lines.strip()
            if lines:
                for line in lines.split('\n'):
                    print(
                        f"    {color}|{AnsiFormat.RESET} {line}",
                        file=stdout
                    )

            is_passing = is_passing and (process.returncode == 0)

        return 0 if is_passing else 1
