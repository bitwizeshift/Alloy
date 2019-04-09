#!/usr/bin/env python3
"""
Script for running clang-format locally
"""

def _root_path():
  import os

  root_path = os.path.realpath(__file__) # ./tools/run-clang-format.py
  root_path = os.path.dirname(root_path) # ./tools/
  root_path = os.path.dirname(root_path) # ./

  return root_path


if __name__ == "__main__":

  import glob
  import subprocess

  ROOT_PATH = _root_path()

  files = []
  files.extend(glob.glob("include/**/*.hpp",recursive=True))
  files.extend(glob.glob("include/**/*.inl",recursive=True))
  files.extend(glob.glob("include/**/*.cpp",recursive=True))

  # TODO(bitwizeshift): Make clang-format run in-place
  command = ["clang-format","-style=file"]
  command.extend(files)

  subprocess.run(command, cwd=ROOT_PATH)
