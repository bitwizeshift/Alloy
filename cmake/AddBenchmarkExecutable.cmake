# Copyright (c) Matthew Rodusek
# Distributed under the OSI-approved MIT License. See accompanying
# file LICENSE.txt or https://opensource.org/licenses/MIT for details.

#.rst:
# AddBenchmarkExecutable
# ----------------------
#
# Declares benchmark executables and the target that runs them.

#.rst:
# .. command:: add_benchmark_executable
#
#   Behaves like ``add_executable`` but marks the target as a benchmark so the
#   ``benchmark`` target can discover it.
#
#   ::
#
#       add_benchmark_executable(<name> <source>...)
#
function(add_benchmark_executable name)
  add_executable("${name}" ${ARGN})

  set_property(TARGET "${name}" PROPERTY ALLOY_BENCHMARK_EXECUTABLE TRUE)
  set_property(GLOBAL APPEND PROPERTY ALLOY_BENCHMARK_EXECUTABLES "${name}")
endfunction()

#.rst:
# .. command:: enable_benchmark_target
#
#   Defines the ``benchmark`` target, which runs every declared benchmark
#   executable sequentially. The target is excluded from the default build and
#   runs only when invoked explicitly (``cmake --build ... --target benchmark``).
#   Call once, after all benchmark executables have been declared.
#
#   ::
#
#       enable_benchmark_target()
#
function(enable_benchmark_target)
  get_property(_benchmarks GLOBAL PROPERTY ALLOY_BENCHMARK_EXECUTABLES)
  if (NOT _benchmarks)
    return ()
  endif ()

  set(_commands "")
  foreach (_bench IN LISTS _benchmarks)
    list(APPEND _commands COMMAND "$<TARGET_FILE:${_bench}>")
  endforeach ()

  add_custom_target(benchmark
    ${_commands}
    DEPENDS ${_benchmarks}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMENT "Running benchmarks"
    USES_TERMINAL
    VERBATIM
  )
endfunction()
