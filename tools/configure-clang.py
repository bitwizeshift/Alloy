#!/usr/bin/env python3
"""
Script for bootstrapping cmake for clang builds using ninja
"""


def main():
    import sys
    from python_utilities.tool import PythonToolScript

    script = PythonToolScript("configure.py")

    return script.execute(
        *sys.argv[1:],
        "--compiler", "clang++"
    )


if __name__ == "__main__":
    import sys
    sys.exit(main())
