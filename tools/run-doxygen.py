#!/usr/bin/env python3
"""
Script for generating doxygen output
"""

def root_path():
  import os

  root_path = os.path.realpath(__file__) # ./tools/run-doxygen.py
  root_path = os.path.dirname(root_path) # ./tools/
  root_path = os.path.dirname(root_path) # ./

  return root_path


if __name__ == "__main__":

  import subprocess
  import os

  doxyfile_path = os.path.join(root_path(),".codedocs")

  subprocess.run(["doxygen", doxyfile_path],
                  cwd=root_path(),
                  check=True)
