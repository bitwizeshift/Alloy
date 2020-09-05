#!/usr/bin/env python3
"""
Script for bootstrapping cmake
"""

import argparse
from pathlib import Path
argument_parser = argparse.ArgumentParser()
argument_parser.add_argument(
  "--db",
  help="The path to the compilation database",
  type=Path,
  action="store"
)

def root_path():
  import os

  path = os.path.realpath(__file__) # ./tools/bootstrap.py
  path = os.path.dirname(path)      # ./tools/
  path = os.path.dirname(path)      # ./

  return Path(path)

if __name__ == "__main__":

  ROOT_PATH = root_path()

  # Handle argument parsing
  args, files = argument_parser.parse_known_args()
  db_path = args.db

  command = [
    "clang-tidy",
    "--format-style=file",
    "--checks=-*,modernize-use-trailing-return-type",
    "--fix",
    f"-p={db_path}",
  ]
  command.extend(files)

  print(command)

  import subprocess
  subprocess.run(command, cwd=ROOT_PATH)
