#!/usr/bin/env bash
################################################################################
# Travis-CI : deploy-doxygen
# --------------------------
#
# Generates doxygen output, and pushes it to the gh-pages branch of the Alloy
# repository
################################################################################

set -e

doxygen "$(pwd)/.codedocs"

mkdir -p dist/api
mv build/doc/* dist/api

# TODO(bitwizeshift): move documentation