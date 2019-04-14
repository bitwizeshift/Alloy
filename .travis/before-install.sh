#!/usr/bin/env bash
################################################################################
# Travis-CI : before-install
# --------------------------
#
# Updates the repository for the underlying travis agent
################################################################################

set -e

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
  sudo apt-get update
elif [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
  brew update
else
  echo >&2 "Unknown TRAVIS_OS_NAME: '${TRAVIS_OS_NAME}'"
  exit 1
fi