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
  # Glob all files in all source locations
  for subpath in ["lib", "bin", "extra", "example"]:
    for extension in ["hpp", "inl", "cpp"]:
      glob_expression = "{}/**/*.{}".format(subpath,extension)
      files.extend(glob.glob(glob_expression,recursive=True))

  # TODO(bitwizeshift):
  #   Make clang-format run in-place once the formatting guideline is stable
  command = ["clang-format","-style=file"]
  command.extend(files)

  subprocess.run(command, cwd=ROOT_PATH)
