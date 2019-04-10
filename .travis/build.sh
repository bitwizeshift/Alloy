#!/usr/bin/env bash
################################################################################
# Travis-CI : build
# -----------------
#
# Generates a CMake project, and builds it
################################################################################

# Make build directory and install dependencies
build_directory="build"

mkdir -p "${build_directory}"
cd "${build_directory}"
conan install ..

# Compile and execute unit tests
cmake .. \
      -G"Ninja" \
      -DALLOY_ENABLE_UNIT_TESTS=On \
      -DALLOY_ENABLE_HEADER_SELF_CONTAINMENT_TESTS=On \
      ${CMAKE_OPTIONS}

cmake --build . --config Release

# TODO: Sync binaries to AWS