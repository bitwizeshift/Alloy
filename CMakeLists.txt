cmake_minimum_required(VERSION 3.5)

if (NOT PROJECT_NAME)
  set(NOT_SUBPROJECT TRUE)
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

set(ALLOY_PRECISION "float" CACHE STRING "Precision level for floating point")
set(ALLOY_PRECISION_VALUES "float" "double")
set_property(CACHE ALLOY_PRECISION PROPERTY STRINGS ${ALLOY_PRECISION_VALUES})
mark_as_advanced(ALLOY_PRECISION)

# Validation

if (NOT ALLOY_PRECISION IN_LIST ALLOY_PRECISION_VALUES)
  message(FATAL_ERROR "Invalid ALLOY_PRECISION '${ALLOY_PRECISION}'."
                      "Precision must be one of: " ${ALLOY_PRECISION_VALUES})
endif ()

##############################################################################
# Project
##############################################################################

if (NOT CMAKE_TESTING_ENABLED AND ALLOY_COMPILE_TESTS)
  enable_testing()
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

if ( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
     "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  add_compile_options(-Wall -Werror -pedantic)
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
  # TODO: Determine MSVC necessary compiler flags
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
# INTERFACE Alloy::IO
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

if (NOT NOT_SUBPROJECT)
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