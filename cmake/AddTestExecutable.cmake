# Copyright (c) Matthew Rodusek
# Distributed under the OSI-approved MIT License. See accompanying
# file LICENSE.txt or https://opensource.org/licenses/MIT for details.

#.rst:
# AddTestExecutable
# -----------------
#
# Declares an executable target that is a unit-test runner.
#
# ``add_test_executable`` behaves exactly like ``add_executable``, but
# additionally marks the resulting target as a test executable. This lets
# tooling -- such as code-coverage generation -- discover every test target
# authoritatively, without relying on naming conventions or which libraries a
# target happens to link against.
#
# ::
#
#     add_test_executable(<name> <source>...)
#
#     <name>      - The name of the test executable to create
#     <source>... - The sources for the executable (as for add_executable)
#
# The target is tagged with the ``ALLOY_TEST_EXECUTABLE`` boolean property and
# appended to the global ``ALLOY_TEST_EXECUTABLES`` property.

function(add_test_executable name)
  add_executable("${name}" ${ARGN})

  set_property(TARGET "${name}" PROPERTY ALLOY_TEST_EXECUTABLE TRUE)
  set_property(GLOBAL APPEND PROPERTY ALLOY_TEST_EXECUTABLES "${name}")
endfunction()
