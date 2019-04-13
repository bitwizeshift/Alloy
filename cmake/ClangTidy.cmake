# Copyright (c) Matthew Rodusek
# Distributed under the OSI-approved MIT License. See accompanying
# file LICENSE.txt or https://opensource.org/licenses/MIT for details.

#.rst:
# ClangTidy
# ---------
#
# ``clang-tidy`` support in CMake
#
# This module only works for CMake versions 3.5 or above, due to a few
# CMake limitations. In 3.5, only the ``add_clang_tidy_target`` work, 
# whereas anything above 3.5 can support the ``enable_clang_tidy`` commands
# in addition to the ``add_clang_tidy_target``. 
# This limitation is a result of the ``CMAKE_EXPORT_COMPILE_COMMANDS`` not 
# being present prior to this.
#
# This module defines the following variables:
#
#   ``CLANG_TIDY_EXECUTABLE`` 
#     Path to the ``clang-tidy`` executable, or ``NOTFOUND`` if it cannot be 
#     found.

set(__FIND_ROOT_PATH_MODE_PROGRAM ${CMAKE_FIND_ROOT_PATH_MODE_PROGRAM})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

find_program(CLANG_TIDY_EXECUTABLE clang-tidy QUIET)
mark_as_advanced(CLANG_TIDY_EXECUTABLE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ${__FIND_ROOT_PATH_MODE_PROGRAM})
set(__FIND_ROOT_PATH_MODE_PROGRAM)

# Only added in 3.5.0
if (CMAKE_VERSION VERSION_LESS 3.5.0)
  message(FATAL_ERROR "ClangTidy: Minimum cmake version 3.5 not satisfied (CMake ${CMAKE_VERSION})")  
endif ()
set(CMAKE_EXPORT_COMPILE_COMMANDS True CACHE INTERNAL "")

if (CMAKE_VERSION VERSION_LESS 3.5.0)
  include(CMakeParseArguments)
endif ()

#.rst:
# .. command:: add_clang_tidy_target
#
# This command sets the clang-tidy language and arguments globally for all
# targets defined after the invocation at any directory scope deeper than
# the current setting
#
# .. code-block:: cmake
#
#   add_clang_tidy_target(<name>
#                         [TARGETS <target>...]
#                         [SOURCES <source>...]
#                         [CLANG_TIDY_ARGS <arg>...])
# 
# Creates a new ``clang-tidy`` target with the name ``<name>``.
# The options are:
#
#   ``TARGETS``
#     Target(s) to base the ``clang-tidy`` args on. Any ``<target>``
#     will have its sources used for the ``clang-tidy`` target
#
#   ``SOURCES``
#     Source files to build
#
#   ``CLANG_TIDY_ARGS``
#     Arguments to forward directly to ``clang-tidy``
function(add_clang_tidy_target target)
  if (NOT CLANG_TIDY_EXECUTABLE)
    message(FATAL_ERROR "add_clang_tidy_target: clang-tidy not found.")
  endif ()

  cmake_parse_arguments(
    CLANG_TIDY # Prefix
    ""         # Option-args
    ""         # Single-args
    "SOURCES;TARGETS;CLANG_TIDY_ARGS" # Multi-args
    ${ARGN}
  )

  set(sources)
  foreach (source_target IN LISTS CLANG_TIDY_TARGETS)
    list(APPEND sources $<TARGET_PROPERTY:${source_target},SOURCES>)
  endforeach ()
  foreach (source IN LISTS CLANG_TIDY_SOURCES)
    list(APPEND sources "${source}")
  endforeach ()

  if (NOT sources)
    message(FATAL_ERROR "add_clang_tidy_target: No sources specified")
  endforeach ()

  add_custom_target(
    "${target}"
    COMMAND "${CLANG_TIDY_EXECUTABLE}"
      ${sources}
      ${CLANG_TIDY_CLANG_TIDY_ARGS}
      -list-checks
      "-p=\"${CMAKE_BINARY_DIR}/compile_commands.json\""
    COMMENT "Executing clang-tidy"
  )

  # Add dependencies to each target
  foreach (source_target IN LISTS CLANG_TIDY_TARGETS)
    add_dependencies("${target}" ${source_target})
  endforeach ()

endfunction()

#.rst:
# .. command:: enable_clang_tidy
#
# This command sets the clang-tidy language and arguments globally for all
# targets defined after the invocation at any directory scope deeper than
# the current setting.
#
# Note: This command only works on CMake 3.6 or greater. If used on a 
#       cmake version less than 3.6, FATAL_ERROR is emitted.
#
# .. code-block:: cmake
#
#   enable_clang_tidy([REQUIRED]
#                     [LANGUAGES <language>...]
#                     [CLANG_TIDY_ARGS <arg>...])
# 
# The options are:
#
#   ``REQUIRED``
#     Creates a hard-error if clang-tidy is not found
#
#   ``LANGUAGES``
#     Languages to support (C,CXX)
#
#   ``CLANG_TIDY_ARGS`` 
#     Arguments to forward directly to clang-tidy
function(enable_clang_tidy)
  if (NOT CMAKE_VERSION VERSION_GREATER 3.5)
    message(FATAL_ERROR "enable_clang_tidy: built-in clang-tidy support only available in CMake 3.5 or above")
  endif ()

  cmake_parse_arguments(
    CLANG_TIDY                  # Prefix
    "REQUIRED"                  # Option-args
    ""                          # Single-args
    "LANGUAGES;CLANG_TIDY_ARGS" # Multi-args
    ${ARGN}
  )

  if (CLANG_TIDY_REQUIRED AND NOT CLANG_TIDY_EXECUTABLE)
    message(FATAL_ERROR "enable_clang_tidy REQUIRED: clang-tidy program not found")
  elseif (NOT CLANG_TIDY_EXECUTABLE)
    return()
  endif ()

  if (NOT CLANG_TIDY_LANGUAGES)
    set(CLANG_TIDY_LANGUAGES CXX) # default to C++ support
  endif ()

  foreach (lang IN LISTS CLANG_TIDY_LANGUAGES)
    set(CMAKE_${lang}_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};${CLANG_TIDY_CLANG_TIDY_ARGS}")
  endforeach ()

  set(CMAKE_CLANG_TIDY_ENABLED True CACHE INTERNAL "")
endfunction()

#.rst
# .. command:: target_enable_clang_tidy
#
# This command sets the clang-tidy language and arguments locally for the
# specified target
#
# Note: This command only works on CMake 3.6 or greater. If used on a 
#       cmake version less than 3.6, FATAL_ERROR is emitted.
#
# .. code-block:: cmake
#
#   enable_clang_tidy(<target>
#                     [REQUIRED]
#                     [LANGUAGES <language>...]
#                     [ARGS <arg>...])
# 
# Enables ``clang-tidy`` for the specified ``<target>``.
# The options are:
#
#   ``REQUIRED``
#     Creates a hard-error if clang-tidy is not found
#
#   ``LANGUAGES``
#     Languages to support (C,CXX)
#
#   ``CLANG_TIDY_ARGS`` 
#     Arguments to forward directly to clang-tidy
function(target_enable_clang_tidy target)
  if (NOT CMAKE_VERSION VERSION_GREATER 3.5)
    message(FATAL_ERROR 
      "target_enable_clang_tidy:" 
      "built-in clang-tidy support only available in CMake 3.5 or above"
    )
  endif ()

  cmake_parse_arguments(
    CLANG_TIDY                  # Prefix
    "REQUIRED"                  # Option-args
    ""                          # Single-args
    "LANGUAGES;CLANG_TIDY_ARGS" # Multi-args
    ${ARGN}
  )

  if (CLANG_TIDY_REQUIRED AND NOT CLANG_TIDY_EXECUTABLE)
    message(FATAL_ERROR 
      "target_enable_clang_tidy REQUIRED:" 
      "clang-tidy program not found"
    )
  elseif (NOT CLANG_TIDY_EXECUTABLE)
    return()
  endif ()

  if (NOT CLANG_TIDY_LANGUAGES)
    set(CLANG_TIDY_LANGUAGES CXX) # default to C++ support
  endif ()

  foreach (lang IN LISTS CLANG_TIDY_LANGUAGES )
    set_property(
      TARGET ${target} 
      PROPERTY "${lang}_CLANG_TIDY" 
      "${CLANG_TIDY_EXECUTABLE};${CLANG_TIDY_CLANG_TIDY_ARGS}"
    )
  endforeach ()
endfunction()