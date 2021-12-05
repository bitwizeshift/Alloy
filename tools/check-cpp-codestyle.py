#!/usr/bin/env python3

"""
This script is used to validate that the c++ codestyle follows the format as
specified in clang-format
"""

from pathlib import Path
from typing import Callable
from python_utilities.clang_format import ClangFormat
from python_utilities.reporter import *
from python_utilities.git import Git
from python_utilities.paths import ProjectPaths


class ClangFormatVerifier(Verifier, Fixer):
    """
    A verifier written in terms of the clang-format
    """

    def __init__(self,
                 read_source: Callable[[Path], str],
                 format: ClangFormat = ClangFormat(werror=True)):
        self._read_source = read_source
        self._format = format

    def verify(self, path: Path) -> ReportEntry:
        content = self._read_source(path)

        process = self._format.execute(
            dry_run=True,
            assume_filename=path,
            input=content.encode(),
            capture_output=True
        )

        return ReportEntry.from_completed_process(process)

    def on_fix(self, path: Path) -> ReportEntry:
        process = self._format.execute(
            in_place=True,
            capture_output=True,
            files=[path],
        )

        return ReportEntry.from_completed_process(process)


def main():
    import argparse

    EXTENSIONS = [".h", ".hpp", ".c", ".cpp"]
    parser = argparse.ArgumentParser(
        description="Validates c++ codestyle conformance",
    )

    git = Git(cwd=ProjectPaths.repository_root)
    options = ReporterOptions.make_options_for(
        ClangFormatVerifier,
        parser,
        git=git,
        extensions=EXTENSIONS
    )

    verifier = ClangFormatVerifier(lambda p: options.read_file(p))

    reporter = Reporter(
        "c++ codestyle",
        verifier,
        options.files,
        show_progress=options.show_progress
    )

    return reporter.report(jobs=options.jobs, verbose=options.verbose)


if __name__ == "__main__":
    import sys
    sys.exit(main())
