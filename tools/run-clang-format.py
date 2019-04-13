#!/usr/bin/env python3
"""
Script for running clang-format locally
"""

def root_path():
  import os

  path = os.path.realpath(__file__) # ./tools/run-clang-format.py
  path = os.path.dirname(path)      # ./tools/
  path = os.path.dirname(path)      # ./

  return path


if __name__ == "__main__":

  import glob
  import subprocess

  ROOT_PATH = root_path()

  files = []
  files.extend(glob.glob("include/**/*.hpp",recursive=True))
  files.extend(glob.glob("include/**/*.inl",recursive=True))
  files.extend(glob.glob("include/**/*.cpp",recursive=True))

  # TODO(bitwizeshift): Make clang-format run in-place
  command = ["clang-format","-style=file"]
  command.extend(files)

  subprocess.run(command, cwd=ROOT_PATH)
