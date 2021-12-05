"""
This module provides concrete/fixed paths around the repository
"""


class _ProjectPaths:
    """
    A collection of paths around the repository
    """

    def __init__(self):
        """
        Constructs the _ProjectPaths without a known repository path
        """
        self._repository_root = None

    @property
    def repository_root(self):
        """
        Computes and returns the root of the repository

        This command uses `git` to resolve the underlying path
        :return Path: a path to the repository root
        """
        import subprocess as sp
        from pathlib import Path

        if self._repository_root is not None:
            return self._repository_root

        result = sp.run(["git", "rev-parse", "--show-toplevel"],
                        capture_output=True)

        if result.returncode == 0:
            path = result.stdout.decode().strip()
            self._repository_root = Path(path)
        else:
            # ./tools/python-utilities/paths.py
            path = Path(__file__).resolve()
            self._repository_root = path.parents[3]

        return self._repository_root

    @property
    def tools_root(self):
        """
        Computes the root directory for the tooling code

        :return Path: a path to the tooling directory
        """
        return self.repository_root / "tools"

    @property
    def library_root(self):
        """
        Computes the root directory for the library code

        :return Path: a path to the library directory
        """
        return self.repository_root / "lib"

    @property
    def binary_root(self):
        """
        Computes the root directory for the binary code

        :return Path: a path to the binary directory
        """
        return self.repository_root / "bin"

    @property
    def document_root(self):
        """
        Computes the root directory for the documentation

        :return Path: a path to the documentation directory
        """
        return self.repository_root / "doc"

    @property
    def example_root(self):
        """
        Computes the root directory for the examples

        :return Path: a path to the examples directory
        """
        return self.repository_root / "example"

    @property
    def extra_root(self):
        """
        Computes the root directory for extras

        :return Path: a path to the extras directory
        """

        return self.repository_root / "extra"


ProjectPaths = _ProjectPaths()
