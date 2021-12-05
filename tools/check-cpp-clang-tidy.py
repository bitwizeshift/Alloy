#!/usr/bin/env python3

"""
This script is used to validate that clang-tidy static validation passes
correctly
"""

from pathlib import Path
from python_utilities.reporter import *
from python_utilities.clang_tidy import ClangTidy, LineFilter, LineInterval


from python_utilities.git import Git
from python_utilities.paths import ProjectPaths

from typing import List
import sys


class ClangTidyVerifier(Verifier, Fixer):
    """
    A verifier written in terms of the clang-tidy
    """

    def __init__(self,
                 tidy: ClangTidy = ClangTidy(),
                 git: Git = Git(),
                 staged: bool = False):
        super().__init__()
        self._tidy = tidy
        self._git = git
        self._staged = staged

    def verify(self, path: Path) -> ReportEntry:

        diff = self._git.diff(
            [path],
            cached=self._staged,
            lines_of_context=5
        )

        line_filters = ClangTidyVerifier._diff_to_line_filter(diff)

        process = self._tidy.execute(
            files=[path],
            line_filters=line_filters,
            capture_output=True,
        )

        return ReportEntry.from_completed_process(process)

    def on_fix(self, path: Path) -> ReportEntry:

        diff = self._git.diff(
            [path],
            cached=self._staged,
            lines_of_context=5
        )

        line_filters = ClangTidyVerifier._diff_to_line_filter(diff)

        process = self._tidy.execute(
            files=[path],
            line_filters=line_filters,
            fix=True,
            capture_output=True,
        )

        return ReportEntry.from_completed_process(process)

    @staticmethod
    def _diff_to_line_filter(diff: List[Git.Diff]) -> List[LineFilter]:
        """
        Converts a git diff into a LineFilter for clang-tidy diagnostic
        filtering

        :param List[Git.Diff] diff: the git diff to translate
        :return List[LineFilter]: the line-filter
        """
        result: List[LineFilter] = []

        for entry in diff:
            filter = LineFilter(entry.to_path)

            for change in entry.to_changes:
                interval = LineInterval.closed_interval(
                    change.start,
                    change.end
                )

                filter.lines.append(interval)
            result.append(filter)

        return result


def main():
    import argparse
    from python_utilities.paths import ProjectPaths

    EXTENSIONS = [".h", ".hpp", ".c", ".cpp"]
    COMPILATION_DATABASE = \
        ProjectPaths.repository_root / "build" / "compile_commands.json"

    parser = argparse.ArgumentParser(
        description="Validates c++ static analysis conformance",
    )
    parser.add_argument(
        "-p", "--compilation-database",
        type=Path,
        default=COMPILATION_DATABASE,
        action="store",
        dest="compilation_database",
        help="Specifies the path to the compile_commands.json file",
    )

    git = Git(cwd=ProjectPaths.repository_root)
    options = ReporterOptions.make_options_for(
        ClangTidyVerifier,
        parser,
        git=git,
        extensions=EXTENSIONS
    )

    if not COMPILATION_DATABASE.exists():
        print(f"Compilation database not found.", file=sys.stderr)
        print(f"File expected at '{COMPILATION_DATABASE}'", file=sys.stderr)

    tidy = ClangTidy(
        database_path=COMPILATION_DATABASE
    )

    verifier = ClangTidyVerifier(tidy, staged=options.staged)

    reporter = Reporter(
        "c++ clang-tidy",
        verifier,
        options.files,
        show_progress=options.show_progress
    )

    return reporter.report(jobs=options.jobs, verbose=options.verbose)


if __name__ == "__main__":
    sys.exit(main())
