cmake_minimum_required(VERSION 3.8)

# Disable defaulting the warning flags for MSVC
# this isn't bad, it's just inconvenient
cmake_policy(SET CMP0092 NEW)

if (PROJECT_NAME)
  set(ALLOY_IS_SUBPROJECT TRUE)
endif ()

set(ALLOY_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_MODULE_PATH "${ALLOY_MODULE_PATH}" "${CMAKE_MODULE_PATH}")

##############################################################################
# Project Options
##############################################################################

option(CMAKE_EXPORT_COMPILE_COMMANDS "Enable exporting compile-commands within CMake" ON)
option(BUILD_SHARED_LIBS "Whether to build this library as a shared library" ON)

option(ALLOY_COMPILE_SELF_CONTAINMENT_TESTS "Include each public header independently in a .cpp file to determine header self-containment" ON)
option(ALLOY_COMPILE_TESTS "Compile and run the unit tests for this library" OFF)
option(ALLOY_COMPILE_EXTRAS "Compiles extras that ship with this library" OFF)
option(ALLOY_COMPILE_EXAMPLES "Compiles example programs that use this library" OFF)
option(ALLOY_ENABLE_CLANG_TIDY "Enables clang-tidy integration" OFF)
option(ALLOY_ENABLE_EXCEPTIONS "Enable exception handling" ON)

# Core settings

set(ALLOY_PRECISION "float" CACHE STRING "Precision level for floating point")
set(ALLOY_PRECISION_VALUES "float" "double")
set_property(CACHE ALLOY_PRECISION PROPERTY STRINGS ${ALLOY_PRECISION_VALUES})
mark_as_advanced(ALLOY_PRECISION)

if (NOT ALLOY_PRECISION IN_LIST ALLOY_PRECISION_VALUES)
  message(FATAL_ERROR "Invalid ALLOY_PRECISION '${ALLOY_PRECISION}'."
                      "Precision must be one of: " ${ALLOY_PRECISION_VALUES})
endif ()

# IO settings

set(ALLOY_EVENT_SIZE 64 CACHE STRING "Event size for alloy::io::event objects")
set(ALLOY_EVENT_ALIGN 32 CACHE STRING "Event alignment for alloy::io::event objects")

if (NOT ALLOY_EVENT_SIZE MATCHES "[0-9]+")
  message(FATAL_ERROR "Invalid ALLOY_EVENT_SIZE '${ALLOY_EVENT_SIZE}'."
                      "Event size must be positive integral value")
endif ()
if (NOT ALLOY_EVENT_ALIGN MATCHES "[0-9]+")
  message(FATAL_ERROR "Invalid ALLOY_EVENT_ALIGN '${ALLOY_EVENT_ALIGN}'."
                      "Event alignment must be positive integral value")
endif ()

# System settings

set(ALLOY_ENTITY_TYPE "std::uint32_t" CACHE STRING "Underlying type for entities")
set(ALLOY_ENTITY_TYPE_VALUES "std::uint16_t" "std::uint32_t" "std::uint64_t")
set_property(CACHE ALLOY_ENTITY_TYPE PROPERTY STRINGS ${ALLOY_ENTITY_TYPE_VALUES})
mark_as_advanced(ALLOY_ENTITY_TYPE)

if (NOT ALLOY_ENTITY_TYPE IN_LIST ALLOY_ENTITY_TYPE_VALUES)
  message(FATAL_ERROR "Invalid ALLOY_ENTITY_TYPE '${ALLOY_ENTITY_TYPE}'."
                      "Entity type must be one of: " ${ALLOY_ENTITY_TYPE_VALUES})
endif ()

##############################################################################
# Project
##############################################################################

if (ALLOY_COMPILE_TESTS)
  enable_testing()
endif ()

# Builds on Windows using Ninja seem to cause issues where unit tests are
# unable to link to the *.dlls, seemingly due to the Path environmen variable
# not being updated. This forces the binaries to all be in the same directory
# which ensures that they link correctly
if (WIN32 AND BUILD_SHARED_LIBS)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/product")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/product")
endif ()

project(Alloy
  VERSION "0.0.1"
  LANGUAGES CXX
)

set(ALLOY_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE INTERNAL "Major version of the Alloy game engine")
set(ALLOY_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE INTERNAL "Minor version of the Alloy game engine")
set(ALLOY_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE INTERNAL "Patch version of the Alloy game engine")
set(ALLOY_VERSION ${PROJECT_VERSION} CACHE INTERNAL "Version of Alloy game engine")

# If using conan, only set the find paths. This project is trying to be
# fully CMake
if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  conan_set_find_paths()
endif ()

#-----------------------------------------------------------------------------

# default to debug when building CMake
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "The active build configuration")
endif ()

#-----------------------------------------------------------------------------

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND
    "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  # clang-cl does not appear to implement '-pedantic' or 'pedantic-errors',
  # so this case needs to be handled specifically
  add_compile_options(-Wall -Werror -Wextra)

  # clang-cl treats '-Wall' like '-Weverything' currently; so there are a few
  # warnings we need to manually disable.

  # Disable unnecessary compatibility and 'newline-eof'. This is a modern C++
  # library -- so these serve no purpose
  add_compile_options(-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-newline-eof)

  # Disable warnings on padding. This is a great warning, but not good when
  # coupled with -Werror
  add_compile_options(-Wno-padded)

  # Several utilities require static objects that have exit-time destructors,
  # including 'error_category' types from the standard.
  add_compile_options(-Wno-exit-time-destructors)

  # This is erroring on valid uses of the standard library, like std::lock_guard.
  add_compile_options(-Wno-ctad-maybe-unsupported)

  # This is a good warning *in general*, but not when using MSVC's standard
  # library, which requires defining "_" prefixed macros just to have it operate
  # like the C++ standard dictates.
  add_compile_options(-Wno-reserved-id-macro)

  # TODO(#4): Undo once this no longer produces errors
  add_compile_options(-Wno-weak-vtables)

  # Don't warn on ignored attributes; this breaks builds using dllexport/dllimport on
  # classes that may also define inline functions
  add_compile_options(-Wno-ignored-attributes)

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-Wall -Werror -Wextra -pedantic -pedantic-errors)

  # Xcode 11.2 seems to enable -Wnewline-eof with '-Wextra'. Disable this,
  # since it's a legacy requirement not needed in C++11 onward.
  add_compile_options(-Wno-newline-eof)

  # Don't warn on ignored attributes; this breaks builds using dllexport/dllimport on
  # classes that may also define inline functions
  add_compile_options(-Wno-ignored-attributes)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  add_compile_options(/W4 /WX)
endif ()

##############################################################################
# Clang Tidy
##############################################################################

include(ClangTidy)

if (ALLOY_ENABLE_CLANG_TIDY)
  enable_clang_tidy(REQUIRED)
endif ()

##############################################################################
# Targets
##############################################################################

# Library Targets

add_subdirectory("lib/alloy-core")
add_subdirectory("lib/alloy-io")
add_subdirectory("lib/alloy-render")
add_subdirectory("lib/alloy-audio")
add_subdirectory("lib/alloy-engine")

# Create one interface library
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
  INTERFACE Alloy::Core
  INTERFACE Alloy::IO
# INTERFACE Alloy::Render
# INTERFACE Alloy::Audio
# INTERFACE Alloy::Engine
)

# Binary Targets

add_subdirectory("bin/level-editor")
add_subdirectory("bin/material-viewer")
add_subdirectory("bin/scene-editor")

# Extras

if (ALLOY_COMPILE_EXTRAS)
  add_subdirectory("extra")
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example")
endif ()

##############################################################################
# Installation
##############################################################################

if (ALLOY_IS_SUBPROJECT)
  return()
endif ()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(ALLOY_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${ALLOY_CMAKE_CONFIG_DESTINATION}"
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY "SameMajorVersion"
)

# Targets
install(
  TARGETS "${PROJECT_NAME}"
  EXPORT "${PROJECT_NAME}Targets"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT "${PROJECT_NAME}Targets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${ALLOY_CMAKE_CONFIG_DESTINATION}"
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  DESTINATION "${ALLOY_CMAKE_CONFIG_DESTINATION}"
)

if (ALLOY_INSTALL_DOCS)
  install(
    FILES "${CMAKE_CURRENT_LIST_DIR}/doc/building.md"
          "${CMAKE_CURRENT_LIST_DIR}/doc/legal.md"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
  )
endif ()
