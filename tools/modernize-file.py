#!/usr/bin/env python3
"""
Script for bootstrapping cmake
"""

import argparse
from pathlib import Path
from python_utilities.clang_tidy import ClangTidy
from python_utilities.git import Git
from python_utilities.paths import ProjectPaths
from python_utilities.sources import SourceGroup


def main():

    repository_root = ProjectPaths.repository_root
    argument_parser = argparse.ArgumentParser()
    argument_parser.add_argument(
        "--db",
        help="The path to the compilation database",
        type=Path,
        default=repository_root / "build" / "compile_commands.json",
        action="store"
    )
    argument_parser.add_argument(
        "--program",
        help="The path to or name of the clang-tidy binary to use",
        type=str,
        action="store"
    )
    argument_parser.add_argument(
        "-v", "--verbose",
        help="Verbosely print output from clang-tidy",
        action="store_true",
        default=False,
    )
    argument_parser.add_argument(
        "--source-group",
        help="The source group to glob and format",
        action="store",
        type=SourceGroup,
        default=SourceGroup.INPUT,
        choices=list(SourceGroup)
    )

    # Handle argument parsing
    args, files = argument_parser.parse_known_args()
    db_path = Path(args.db).resolve()

    tidy = ClangTidy(
        program=args.program,
        format_style="file",
        checks=["-*", "modernize-*"],
        warnings_as_errors=["-*"],
        use_color=True,
        fix=True,
        database_path=str(db_path),
        quiet=not args.verbose,
    )
    git = Git()
    group: SourceGroup = args.source_group

    extensions = [".hpp", ".cpp"]
    paths = [f.resolve() for f in group.find_with_extensions(git, extensions)]
    paths.extend([Path(f).resolve() for f in files])

    if args.verbose:
        command = " ".join(tidy.command(files=paths))

        print(f"Executing command: {command}")
        print()

    result = tidy.execute(files=files)

    if result == 0:
        plural = "file was" if len(files) == 1 else "Files were"
        print(f"The following {plural} modernized:")
        for file in files:
            print(f" * {file}")
    else:
        print("An error occurred while running this script.")
        if not args.verbose:
            print("Rerun this command with `--verbose` for more details.")

    return result


if __name__ == "__main__":
    import sys
    sys.exit(main())
