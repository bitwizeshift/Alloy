#!/usr/bin/env python3
"""
Script for bootstrapping cmake for xcode
"""


def main():
    import sys
    from python_utilities.tool import PythonToolScript

    script = PythonToolScript("configure.py")

    return script.execute(
        *sys.argv[1:],
        "--generator", "Xcode"
    )


if __name__ == "__main__":
    import sys
    sys.exit(main())
