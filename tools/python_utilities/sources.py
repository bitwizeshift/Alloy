"""
This module provides an enumeration for different source-groups that may be
globbed for scripts
"""

from python_utilities.git import Git
from typing import List, Callable
from pathlib import Path
from enum import Enum

SourceFilter = Callable[[Path], bool]


class SourceGroup(Enum):
    """
    An enumeration of a group of sources

    This class offers the ability to search for sources in the specified group
    """

    ALL = "all"
    STAGED = "staged"
    MODIFIED = "modified"
    INPUT = "input"

    def __str__(self) -> str:
        return self.value

    def find(self,
             git: Git,
             filter: SourceFilter = None) -> List[Path]:
        """
        Finds all files in the project from this source group

        :param Git git: the git instance to use
        :param SourceFilter filter: an optional filter for the sources
        :return List[Path]: the list of filtered paths
        """

        if self == SourceGroup.ALL:
            return SourceGroup.find_all(filter)
        if self == SourceGroup.STAGED:
            return SourceGroup.find_staged(git, filter)
        if self == SourceGroup.MODIFIED:
            return SourceGroup.find_modified(git, filter)
        if self == SourceGroup.INPUT:
            return []

        # This should be unreachable?
        raise ValueError(f"Invalid SourceGroup value: '{self}'")

    def find_with_extensions(self,
                             git: Git,
                             extensions: List[str]) -> List[Path]:
        """
        Finds all files in the project with the specific extension

        :param Git git: the git instance to use
        :param List[str] extensions: the list of valid extensions
        :return List[Path]: the list of filtered paths
        """
        return self.find(git, lambda x: x.suffix in extensions)

    @staticmethod
    def find_staged(git: Git,
                    filter: SourceFilter = None) -> List[Path]:
        """
        Finds all staged files in the project with an optional filter

        :param Git git: the git instance
        :param SourceFilter filter: the filter
        :return List[Path]: the list of staged file paths
        """
        filter: SourceFilter = filter or (lambda x: True)
        return [p for p in git.changed_files(cached=True) if filter(p)]

    @staticmethod
    def find_modified(git: Git,
                      filter: SourceFilter = None) -> List[Path]:
        """
        Finds all modified files in the project with an optional filter

        :param Git git: the git instance
        :param SourceFilter filter: the filter
        :return List[Path]: the list of modified file paths
        """
        filter: SourceFilter = filter or (lambda x: True)
        return [p for p in git.changed_files() if filter(p)]

    @staticmethod
    def find_all(filter: SourceFilter = None) -> List[Path]:
        """
        Finds all files in the project with an optional filter

        :param SourceFilter filter: the filter
        :return List[Path]: the list of modified file paths
        """
        from python_utilities.paths import ProjectPaths

        root = ProjectPaths.repository_root
        filter: SourceFilter = filter or (lambda x: True)

        return [p for p in root.rglob('*') if p.is_file() and filter(p)]


def read_file(file: Path, git: Git = None, staged=False) -> str:
    if staged:
        return git.show_file(file, cached=True)

    return file.read_text()
