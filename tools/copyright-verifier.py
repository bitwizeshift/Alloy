#!/usr/bin/env python3
"""
Script for verifying that copyright years represent the correct modification
dates.
"""

import argparse
import os
from pathlib import Path
from typing import List


def root_path():
    """
    Computes the root path of the repository, relative to the current script
    """
    import os

    path = os.path.realpath(__file__)  # ./tools/copyright-verifier.py
    path = os.path.dirname(path)       # ./tools/
    path = os.path.dirname(path)       # ./

    return Path(path)


def find_years_modified(file: Path) -> List[int]:
    """
    Finds the years that a file was modified for the given path
    """

    command = [
        "git", "log",
        "--date=format:%Y",
        "--pretty=%ad",
        "--follow",
        "--no-show-signature",
        os.path.abspath(file)
    ]

    # print(" ".join(command))

    import subprocess
    result = subprocess.run(command, cwd=ROOT_PATH, capture_output=True)

    if len(result.stderr) != 0 or result.returncode != 0:
        raise SystemError(
            f"Git failed to produce years for file. Error {result.stderr}")

    output = result.stdout.decode("utf-8").strip().split('\n')

    # Since we're modifying a file, make sure that it updates to include today's
    # year as well since this may not have yet updated the date modified
    import datetime
    years = [int(x) for x in output]
    years.append(datetime.datetime.now().year)

    unique_output = list(dict.fromkeys(years))
    unique_output.sort()

    return unique_output


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


def coallesce_years(years: List[str]) -> List[ModifiedRange]:
    """
    Coallesces the years from the list of years and converts it to a list of
    ranges
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


def check_license(file: Path) -> bool:
    """
    Checks whether a license contains the valid text, and outputs a failure when
    not valid
    """

    import re
    import sys

    years = find_years_modified(file)
    ranges = coallesce_years(years)

    expected_years = ", ".join([str(r) for r in ranges])

    discovered = 0
    for line in open(file):
        content = line
        copyright_regex = "Copyright \(c\) (.*) Matthew Rodusek All rights reserved\."
        years_search = re.search(copyright_regex, content)

        if not years_search:
            continue

        if discovered > 0:
            print(
                f"Error: {file}: multiple copyright statements found", file=sys.stderr)
        discovered += 1

        years = years_search.group(1)

        if years != expected_years:
            print(
                f"Error: {file}: expected '{expected_years}', found '{years}'")
            return False
    return discovered <= 1


def fix_license(file: Path):
    """
    Fixes the license at the given path, if any fix is required
    """

    years = find_years_modified(file)
    ranges = coallesce_years(years)

    import re
    import fileinput
    import sys

    expected_years = ", ".join([str(r) for r in ranges])

    error = False
    discovered = 0
    for line in fileinput.input(file, inplace=True):
        content = line
        copyright_regex = "Copyright \(c\) (.*) Matthew Rodusek All rights reserved\."
        years_search = re.search(copyright_regex, content)

        # Pass the line through if it doesn't contain the copyright
        if not years_search:
            print(content, end='')
            continue

        if discovered > 0:
            print(
                f"Error: {file} Multiple copyright statements found", file=sys.stderr)
        discovered += 1

        # If it does, validate it
        years = years_search.group(1)

        if years != expected_years:
            content = content.replace(years, expected_years)

        print(content, end='')

    if error:
        return False

    return discovered <= 1


def validate_license(file: Path, fix: bool) -> bool:

    if not fix:
        return check_license(file)
    else:
        try:
            fix_license(file)
        except:
            return False
        return True


def main():
    import sys

    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument(
        "--fix",
        help="Enable to automatically fix which files may be outdated",
        action='store_true'
    )

    args, files = argument_parser.parse_known_args()

    requires_fixing = []
    for file in files:
        if not validate_license(file, fix=args.fix):
            requires_fixing.append(file)

    # Recommend auto-fix
    if len(requires_fixing) != 0:
        print("The following files require their license fixed:", file=sys.stderr)
        for file in requires_fixing:
            print(f"* {file}", file=sys.stderr)
        print("", file=sys.stderr)
        print("This may be autofixed with the following command:", file=sys.stderr)

        command = "copyright-verifier.py --fix " + " ".join(requires_fixing)

        print(command)
        exit(1)


ROOT_PATH = root_path()

if __name__ == "__main__":
    main()
