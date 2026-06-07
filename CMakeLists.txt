cmake_minimum_required(VERSION 3.25...4.3)

if (PROJECT_NAME)
  set(ALLOY_IS_SUBPROJECT TRUE)
endif ()

set(ALLOY_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_MODULE_PATH "${ALLOY_MODULE_PATH}" "${CMAKE_MODULE_PATH}")

include(CPM)

##############################################################################
# Project Options
##############################################################################

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Enable exporting compile-commands within CMake")
option(BUILD_SHARED_LIBS "Whether to build this library as a shared library" ON)

option(ALLOY_COMPILE_SURROGATE_TESTS "Compile each public header independently in a surrogate .cpp file to verify header self-containment" ON)
option(ALLOY_COMPILE_TESTS "Compile and run the unit tests for this library" OFF)
option(ALLOY_COMPILE_BENCHMARKS "Compile and run the benchmarks for this library" OFF)
option(ALLOY_COMPILE_EXTRAS "Compiles extras that ship with this library" OFF)
option(ALLOY_COMPILE_EXAMPLES "Compiles example programs that use this library" OFF)
option(ALLOY_ENABLE_CLANG_TIDY "Enables clang-tidy integration" OFF)
option(ALLOY_ENABLE_COVERAGE "Instrument the build and enable the 'coverage' target" OFF)
option(ALLOY_ENABLE_EXCEPTIONS "Enable exception handling" ON)
option(ALLOY_PATCH_COMPILE_COMMANDS "Patches the 'compile_commands.json' file to the build directory" OFF)

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

#-----------------------------------------------------------------------------

# default to debug when building CMake
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "The active build configuration")
endif ()

#-----------------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------------

# Dependencies are managed before compile-flags to ensure that we don't
# trip on our dependency's errors.

add_subdirectory("third_party")

#-----------------------------------------------------------------------------

# Defines the 'Alloy::CommonFlags' interface target carrying the project's
# warning flags. First-party targets opt in by linking it (via the helpers in
# AlloyTargets), so the flags never leak into third-party dependencies.
include(CommonFlags)

if (ALLOY_COMPILE_TESTS)
  include(CTest)
endif ()

##############################################################################
# Build Configurations
##############################################################################

# Included after project() so the compiler is known: this registers the custom
# build configurations (sanitizers and 'Coverage') and their flags, which must
# be set before any library targets are created.
include(BuildConfigurations)

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

# Include utilities
include(AlloyTargets)

# Library Targets

add_subdirectory("lib/alloy-core")
add_subdirectory("lib/alloy-io")
add_subdirectory("lib/alloy-render")
add_subdirectory("lib/alloy-render-gl")
add_subdirectory("lib/alloy-audio")
add_subdirectory("lib/alloy-engine")

# Create one interface library
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}
  INTERFACE
    Alloy::Core
    Alloy::IO
#   Alloy::Render
#   Alloy::Audio
#   Alloy::Engine
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

# Coverage target (created last, once every test target has been registered)

if (ALLOY_ENABLE_COVERAGE)
  include(CodeCoverage)
  enable_coverage_target()
endif ()

if (ALLOY_COMPILE_BENCHMARKS)
  enable_benchmark_target()
endif ()

##############################################################################
# Installation
##############################################################################

if (ALLOY_IS_SUBPROJECT)
  return()
endif ()

include(PatchCompileCommands)

# Installation is currently disabled: exporting AlloyEngine pulls the vendored
# tga-lib into the AlloyTargets export set, which is not yet packaged. The
# install rules below are kept modern and ready for when that is resolved.
return ()

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
