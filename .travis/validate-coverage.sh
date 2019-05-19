#!/usr/bin/env bash
################################################################################
# Travis-CI : validate
# --------------------
#
# Validates the test projects
################################################################################

set -e

# Make build directory and install dependencies
build_directory="build"

mkdir -p "${build_directory}"
cd "${build_directory}"
conan install ..

# Compile and execute unit tests
cmake .. \
  -G"Ninja" \
  -DALLOY_COMPILE_TESTS=On \
  -DALLOY_COMPILE_SELF_CONTAINMENT_TESTS=On \
  -DALLOY_ENABLE_COVERAGE=On

cmake --build . --config Release
cmake --build . --target test

# Collect all test paths
coveralls_args=""
for test_path in $(find $(pwd)/lib -name "test"); do
  coveralls_args="-e ${test_path} ${coveralls_args}"
done

coveralls \
  --build-root "$(pwd)/${build_directory}" \
  -E ".*CMakeFiles.*" \
  -e test \
  ${coveralls_args} \
  --gcov 'gcov-7' \
  --gcov-options '\-lpr'