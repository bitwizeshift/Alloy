#!/usr/bin/env bash
################################################################################
# Travis-CI : install
# -------------------
#
# Installs the required tools for the CI build
################################################################################

set -e

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
  sudo apt-get install python3-pip
  sudo python3 -m pip install --upgrade --force setuptools
  sudo python3 -m pip install --upgrade --force pip
  python3 -m pip install --user conan==1.11.1
elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
  brew install cmake ninja python3 ccache || true
  brew upgrade cmake ninja python3 ccache || true
  export PATH="/usr/local/opt/ccache/libexec:$PATH" # For ccache setup
  python3 -m pip install conan==1.11.1
else
  echo >&2 "Unknown TRAVIS_OS_NAME: '${TRAVIS_OS_NAME}'"
  exit 1
fi