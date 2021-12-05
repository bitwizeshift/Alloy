#!/usr/bin/env python3

"""
This script is used to validate that the python codestyle follows the PEP-8
formatting.

This script may operate in several different modes by checking either the
staging area or
"""

from pathlib import Path
from typing import Callable
from python_utilities.reporter import Reporter, ReporterOptions
from python_utilities.reporter import *
from python_utilities.git import Git
from python_utilities.paths import ProjectPaths


class TrailingWhitespaceVerifier(Verifier, Fixer):
    """
    A verifier that makes sure that no file contains trailing whitespace
    """

    def __init__(self, read_source: Callable[[Path], str]):
        super().__init__()
        self._read_source = read_source

    def verify(self, path: Path) -> ReportEntry:

        content = self._read_source(path)

        lines = content.split('\n')

        stderr = None
        line_number = 1
        for line in lines:
            line_number += 1
            if line.rstrip() != line:
                stderr = stderr or ""
                stderr += f"{path}:{line_number}: trailing whitespace detected"

        return ReportEntry(
            stderr is None,
            stderr=stderr
        )

    def on_fix(self, path: Path) -> ReportEntry:
        import fileinput

        for line in fileinput.input(path, inplace=True):
            print(line.rstrip())

        return ReportEntry(True)


def main():
    import argparse

    EXTENSIONS = [".hpp", ".h", ".cpp", ".c", ".md", ".py"]
    parser = argparse.ArgumentParser(
        description="Validates that lines in sourcefiles don't contain "
        "trailing whitespace"
    )
    git = Git(cwd=ProjectPaths.repository_root)
    options = ReporterOptions.make_options_for(
        TrailingWhitespaceVerifier,
        parser,
        git=git,
        extensions=EXTENSIONS
    )

    verifier = TrailingWhitespaceVerifier(lambda p: options.read_file(p))

    reporter = Reporter(
        "whitespace",
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
