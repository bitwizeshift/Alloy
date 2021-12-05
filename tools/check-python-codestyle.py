#!/usr/bin/env python3

"""
This script is used to validate that the python codestyle follows the PEP-8
formatting.

This script may operate in several different modes by checking either the
staging area or
"""

from pathlib import Path
from typing import Callable
from python_utilities.reporter import *
from python_utilities.git import Git
from python_utilities.paths import ProjectPaths


class PyCodeStyleVerifier(Verifier, Fixer):
    """
    A verifier written in terms of the pycodestyle (pep-8) tool
    """

    def __init__(self, read_source: Callable[[Path], str]):
        super().__init__()
        self._read_source = read_source

    def verify(self, path: Path) -> ReportEntry:
        import subprocess
        content = self._read_source(path)

        process = subprocess.run(
            ["pycodestyle", "-"],
            input=content.encode(),
            capture_output=True,
        )

        # Piping to stdin results in the file paths listing as 'stdin:', which
        # makes it hard to diagnose. This modifies the output data to
        prefix = "stdin:"

        stdout = None
        stderr = None
        for line in process.stdout.decode().split('\n'):
            if line.startswith(prefix):
                stdout = stdout or ""
                line = line[len(prefix):]
                stdout += f"{path}:{line}\n"
        for line in process.stderr.decode().split('\n'):
            if line.startswith(prefix):
                stderr = stderr or ""
                line = line[len(prefix):]
                stderr += f"{path}:{line}\n"

        return ReportEntry(process.returncode == 0, stdout, stderr)

    def on_fix(self, path: Path) -> ReportEntry:
        import subprocess
        process = subprocess.run(
            ["autopep8", "--in-place", str(path.resolve())],
            capture_output=True
        )

        return ReportEntry.from_completed_process(process)


def main():
    import argparse

    EXTENSIONS = [".py"]
    parser = argparse.ArgumentParser(
        description="Validates python formatting compliance",
    )
    git = Git(cwd=ProjectPaths.repository_root)
    options = ReporterOptions.make_options_for(
        PyCodeStyleVerifier,
        parser,
        git=git,
        extensions=EXTENSIONS
    )

    verifier = PyCodeStyleVerifier(lambda p: options.read_file(p))

    reporter = Reporter(
        "python codestyle",
        verifier,
        options.files,
        show_progress=options.show_progress
    )

    return reporter.report(
        jobs=options.jobs,
        verbose=options.verbose,
        fix=options.fix
    )


if __name__ == "__main__":
    import sys
    sys.exit(main())
