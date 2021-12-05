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


class TrailingNewlineVerifier(Verifier, Fixer):
    """
    A verifier that makes sure that the last line of a file is a newline
    """

    def __init__(self, read_source: Callable[[Path], str]):
        super().__init__()
        self._read_source = read_source

    def verify(self, path: Path) -> ReportEntry:

        content = self._read_source(path)

        lines = content.split('\n')

        # Last line must be empty
        return ReportEntry(lines[-1] == "")

    def on_fix(self, path: Path) -> ReportEntry:
        with path.open('a') as file:
            file.write('\n')
        return ReportEntry(True)


def main():
    import argparse

    EXTENSIONS = [".hpp", ".h", ".cpp", ".c", ".md", ".py"]
    parser = argparse.ArgumentParser(
        description="Validates that source files end with a trailing newline",
    )
    git = Git(cwd=ProjectPaths.repository_root)
    options = ReporterOptions.make_options_for(
        TrailingNewlineVerifier,
        parser,
        git=git,
        extensions=EXTENSIONS
    )

    verifier = TrailingNewlineVerifier(lambda p: options.read_file(p))

    reporter = Reporter(
        "trailing newline",
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
