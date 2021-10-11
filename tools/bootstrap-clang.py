#!/usr/bin/env python3
"""
Script for bootstrapping cmake for xcode
"""


def tool_path():
    import os
    import pathlib

    path = os.path.realpath(__file__)  # ./tools/bootstrap-*.py
    path = os.path.dirname(path)       # ./tools/

    return pathlib.Path(path)


if __name__ == "__main__":

    import subprocess
    import sys

    command = [
        "python3", tool_path() / "bootstrap.py",
        *sys.argv[1:],
        "--compiler", "clang",
        "--generator", "Ninja",
    ]

    subprocess.run(command)
