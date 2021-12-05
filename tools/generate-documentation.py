#!/usr/bin/env python3
"""
Script for generating doxygen output
"""


def main():

    from python_utilities.paths import ProjectPaths
    import subprocess

    doxyfile_path = ProjectPaths.repository_root / ".codedocs"

    process = subprocess.run(
        ["doxygen", doxyfile_path],
        cwd=ProjectPaths.repository_root,
        check=True
    )

    return process.returncode


if __name__ == "__main__":
    import sys
    sys.exit(main())
