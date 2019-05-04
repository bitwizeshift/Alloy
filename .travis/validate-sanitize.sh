#!/usr/bin/env bash
################################################################################
# Travis-CI : validate
# --------------------
#
# Validates the test projects
################################################################################

set -e

if [[ $# -lt 1 ]]; then
  echo >&2 "Expects 1 argument of [asan,ubsan,msan]"
  exit 1
fi

if [[ $1 != "ubsan" ]] && [[ $1 != "asan" ]] && [[ $1 != "msan" ]]; then
  echo >&2 "Unexpected argument '$1'. Expected 'ubsan','asan','msan'."
  exit 1
fi

sanitizer=$(echo $1 | tr '[:lower:]' '[:upper:]')

# Make build directory and install dependencies
build_directory="build"

mkdir -p "${build_directory}"
cd "${build_directory}"
conan install ..

# Compile and execute unit tests
cmake .. \
  -G"Ninja" \
  -DALLOY_COMPILE_TESTS=On \
  -DALLOY_COMPILE_HEADER_SELF_CONTAINMENT_TESTS=On \
  -DALLOY_ENABLE_${sanitizer}=1 \
  -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=gold

cmake --build . --config Release
cmake --build . --target test