# Copyright (c) Matthew Rodusek
# Distributed under the OSI-approved MIT License. See accompanying
# file LICENSE.txt or https://opensource.org/licenses/MIT for details.

#.rst:
# CodeCoverage
# ------------
#
# Code-coverage support for GCC and Clang builds.
#
# The ``--coverage`` instrumentation is carried by the ``Coverage`` build
# configuration (see ``BuildConfigurations``). This module adds
# ``alloy_add_coverage_target()``, which defines the explicit ``coverage`` target
# that runs the tests under that configuration and writes ``lcov.info``.

if (CMAKE_CROSSCOMPILING OR NOT
    (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
  message(FATAL_ERROR
    "ALLOY_ENABLE_COVERAGE requires a native GCC or Clang toolchain"
  )
endif ()

# lcov's '--gcov-tool' must be a single executable, but Clang's gcov-compatible
# data is read through 'llvm-cov gcov', so wrap that in a small script.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  find_program(ALLOY_GCOV_TOOL NAMES gcov REQUIRED)
else ()
  find_program(ALLOY_LLVM_COV NAMES llvm-cov REQUIRED)

  set(ALLOY_GCOV_TOOL "${CMAKE_BINARY_DIR}/llvm-gcov.sh")
  file(WRITE "${ALLOY_GCOV_TOOL}"
    "#!/usr/bin/env bash\nexec \"${ALLOY_LLVM_COV}\" gcov \"$@\"\n"
  )
  file(CHMOD "${ALLOY_GCOV_TOOL}"
    PERMISSIONS
      OWNER_READ OWNER_WRITE OWNER_EXECUTE
      GROUP_READ GROUP_EXECUTE
      WORLD_READ WORLD_EXECUTE
  )
endif ()

find_program(ALLOY_LCOV    NAMES lcov REQUIRED)
find_program(ALLOY_GENHTML NAMES genhtml)

#.rst:
# .. command:: enable_coverage_target
#
#   Defines the explicit ``coverage`` target -- and ``coverage-html`` when
#   ``genhtml`` is available. Call once, after every test executable has been
#   declared with ``add_alloy_test``.
#
#   ::
#
#       enable_coverage_target()
#
function(enable_coverage_target)
  get_property(_tests GLOBAL PROPERTY ALLOY_TEST_EXECUTABLES)

  set(_info "${CMAKE_BINARY_DIR}/lcov.info")
  set(_log  "${CMAKE_BINARY_DIR}/coverage.log")

  # lcov only silences an ignored error's warning when its type is named twice.
  set(_ignore
    mismatch
    source
    gcov
    empty
    format
    unsupported
    unused
    unused
    inconsistent
    inconsistent
  )
  list(JOIN _ignore "," _ignore)

  set(_lcov
    "'${ALLOY_LCOV}'"
    --gcov-tool "'${ALLOY_GCOV_TOOL}'"
    --ignore-errors "${_ignore}"
  )

  set(_reset ${_lcov}
    --directory "'${CMAKE_BINARY_DIR}'"
    --zerocounters -q
  )
  set(_ctest
    "'${CMAKE_CTEST_COMMAND}'"
    --build-config Coverage
    --output-on-failure
  )
  set(_capture ${_lcov}
    --capture
    --directory "'${CMAKE_BINARY_DIR}'"
    --base-directory "'${CMAKE_SOURCE_DIR}'"
    --no-external
    --exclude "'*/test/*'"
    --exclude "'*/_deps/*'"
    --output-file "'${_info}'"
  )

  list(JOIN _reset   " " _reset)
  list(JOIN _ctest   " " _ctest)
  list(JOIN _capture " " _capture)

  add_custom_command(
    OUTPUT "${_info}"
    COMMAND bash -c "{ ${_reset} && CTEST_PARALLEL_LEVEL=8 ${_ctest} && ${_capture} ; } > '${_log}' 2>&1"
    DEPENDS ${_tests}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Generating coverage information"
    VERBATIM
  )
  add_custom_target(coverage DEPENDS "${_info}")

  if (ALLOY_GENHTML)
    set(_html "${CMAKE_BINARY_DIR}/coverage-html/index.html")
    add_custom_command(
      OUTPUT "${_html}"
      COMMAND "${ALLOY_GENHTML}" "${_info}"
              --output-directory "${CMAKE_BINARY_DIR}/coverage-html"
      DEPENDS "${_info}"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
      COMMENT "Generating HTML coverage report"
      VERBATIM
    )
    add_custom_target(coverage-html DEPENDS "${_html}")
  endif ()
endfunction()
