#!/usr/bin/env python3
"""
Script for verifying that copyright years represent the correct modification
dates.
"""

from pathlib import Path
from typing import List, Callable

from python_utilities.git import Git
from python_utilities.reporter import *
from python_utilities.git import Git
from python_utilities.paths import ProjectPaths


class ModifiedRange:
    """
    The range of years that a file was modified
    """

    def __init__(self, start: int, end: int = None):
        self.start = start
        self.end = end

    def __str__(self):
        if self.end is None:
            return f"{self.start}"
        else:
            return f"{self.start}-{self.end}"


class LicenseCopyrightVerifier(Verifier, Fixer):

    def __init__(self,
                 read_source: Callable[[Path], str],
                 copyright_regex: str,
                 git: Git = Git()):
        super().__init__()
        self._read_source = read_source
        self._copyright_regex = copyright_regex
        self._git = git

    def verify(self, file: Path) -> ReportEntry:
        """
        Checks whether a license contains the valid text, and outputs a failure
        when not valid
        """

        import re
        import datetime

        years = self._git.years_modified(file)
        years.append(datetime.datetime.now().year)
        years = list(dict.fromkeys(years))
        ranges = LicenseCopyrightVerifier._coallesce_years(years)

        expected_years = ", ".join([str(r) for r in ranges])

        content = self._read_source(file)

        discovered = 0
        stderr = None
        line_number = 0
        for line in content.split('\n'):
            line_number += 1
            years_search = re.search(self._copyright_regex, line)

            if not years_search:
                continue

            if discovered > 0:
                stderr = stderr or ""
                stderr += f"{file}:{line_number}: multiple copyright "
                stderr += "statements found\n"
                stderr += f"{line}\n"
            discovered += 1

            years = years_search.group(1)

            if years != expected_years:
                stderr = stderr or ""
                stderr += f"{file}:{line_number}: expected '{expected_years}',"
                stderr += " found '{years}'\n"
                return ReportEntry(False, stdout=None, stderr=stderr)

        return ReportEntry(discovered <= 1, stdout=None, stderr=stderr)

    def on_fix(self, file: Path) -> ReportEntry:
        """
        Fixes the license at the given path, if any fix is required
        """

        if self.verify(file).succeeded:
            return

        years = self._git.years_modified(file)
        ranges = LicenseCopyrightVerifier._coallesce_years(years)

        import re
        import fileinput

        expected_years = ", ".join([str(r) for r in ranges])

        for line in fileinput.input(file, inplace=True):
            years_search = re.search(self._copyright_regex, line)

            # Pass the line through if it doesn't contain the copyright
            if not years_search:
                print(line, end='')
                continue

            # If it does, validate it
            years = years_search.group(1)

            if years != expected_years:
                content = content.replace(years, expected_years)

            print(content, end='')
        return ReportEntry(True)

    @staticmethod
    def _coallesce_years(years: List[str]) -> List[ModifiedRange]:
        """
        Coallesces the years from the list of years and converts it to a list
        of ranges
        """
        assert len(years) >= 1

        if len(years) == 1:
            return [ModifiedRange(years[0])]

        years.sort()

        result = []
        previous = years[0]
        current_range = ModifiedRange(previous)

        for i in range(1, len(years)):
            current = years[i]

            if (previous + 1) == current:
                current_range.end = current
            else:
                result += [current_range]
                current_range = ModifiedRange(current)
            previous = current

        result.append(current_range)

        return result


def main():
    import argparse

    COPYRIGHT_REGEX = \
        "Copyright \\(c\\) (.*) Matthew Rodusek All rights reserved\\."
    EXTENSIONS = [".cpp", ".c", ".hpp", ".h"]

    parser = argparse.ArgumentParser(
        description="Validates accuracy of copyright license years",
    )
    git = Git(cwd=ProjectPaths.repository_root)
    options = ReporterOptions.make_options_for(
        LicenseCopyrightVerifier,
        parser,
        git=git,
        extensions=EXTENSIONS
    )

    verifier = LicenseCopyrightVerifier(
        lambda p: options.read_file(p),
        COPYRIGHT_REGEX
    )

    reporter = Reporter(
        "copyright license",
        verifier,
        options.files,
        show_progress=options.show_progress
    )

    return reporter.report(jobs=options.jobs, verbose=options.verbose)


if __name__ == "__main__":
    import sys
    sys.exit(main())
