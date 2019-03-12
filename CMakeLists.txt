cmake_minimum_required(VERSION 3.5)

if (NOT PROJECT_NAME)
  set(NOT_SUBPROJECT TRUE)
endif ()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" "${CMAKE_MODULE_PATH}")

##############################################################################
# Project Options
##############################################################################

option(BUILD_SHARED_LIBS "Whether to build this library as a shared library" ON)
option(ALLOY_COMPILE_SELF_CONTAINMENT_TESTS "Include each public header independently in a .cpp file to determine header self-containment" ON)
option(ALLOY_COMPILE_UNIT_TESTS "Compile and run the unit tests for this library" OFF)
option(ALLOY_COMPILE_EXTRAS "Compiles extras that ship with this library" OFF)
option(ALLOY_COMPILE_EXAMPLES "Compiles example programs that use this library" OFF)
option(ALLOY_ENABLE_CLANG_TIDY "Enables clang-tidy integration" OFF)
option(ALLOY_GENERATE_DOCS "Generates doxygen documentation" OFF)
option(ALLOY_INSTALL_DOCS "Install documentation for this library" OFF)
option(ALLOY_VERBOSE_CONFIGURE "Verbosely configures this library project" OFF)
option(ALLOY_ENABLE_EXCEPTIONS "Enable exception handling" ON)

set(ALLOY_PRECISION "float" CACHE STRING "Precision level for floating point")
set(ALLOY_PRECISION_VALUES "float" "double")
set_property(CACHE ALLOY_PRECISION PROPERTY STRINGS ${ALLOY_PRECISION_VALUES})
mark_as_advanced(ALLOY_PRECISION)

option(ALLOY_USE_TRIG_TABLE "Precomputes a trig table for math use, rather than using hardware. The table size is from 'ALLOY_TRIG_TABLE_SIZE'" OFF)
set(ALLOY_TRIG_TABLE_SIZE "4096" CACHE STRING "Number of entries in trig table")
mark_as_advanced(ALLOY_TRIG_TABLE_SIZE)

# Validation

if (NOT ALLOY_PRECISION IN_LIST ALLOY_PRECISION_VALUES)
  message(FATAL_ERROR "Invalid ALLOY_PRECISION '${ALLOY_PRECISION}'."
                      "Precision must be one of: " ${ALLOY_PRECISION_VALUES})
endif ()
if (NOT ALLOY_TRIG_TABLE_SIZE MATCHES "[1-9][0-9].*")
  message(FATAL_ERROR "Invalid ALLOY_TRIG_TABLE_SIZE '${ALLOY_TRIG_TABLE_SIZE}'."
                      "Table size must be numeric.")
endif ()

##############################################################################
# Project
##############################################################################

if (ALLOY_COMPILE_UNIT_TESTS)
  enable_testing()
endif ()

project(Alloy
  VERSION "0.0.1"
  LANGUAGES CXX
)

set(ALLOY_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} CACHE STRING "major version of the Alloy game engine" FORCE)
set(ALLOY_VERSION_MINOR ${PROJECT_VERSION_MINOR} CACHE STRING "minor version of the Alloy game engine" FORCE)
set(ALLOY_VERSION_PATCH ${PROJECT_VERSION_PATCH} CACHE STRING "patch version of the Alloy game engine" FORCE)
set(ALLOY_VERSION ${PROJECT_VERSION} CACHE STRING "version of Alloy game engine" FORCE)

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")

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

# read license, indent each line by 2 spaces
file(READ "LICENSE.txt" license_text)
string(REPLACE "\n" "\n  " LICENSE_TEXT "  ${license_text}")

# Generate Files
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/version.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/version.hpp"
  @ONLY
)

set(ALLOY_PRECISION "ALLOY_PRECISION_${ALLOY_PRECISION}")
string(TOUPPER "${ALLOY_PRECISION}" ALLOY_PRECISION)

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/core/config.hpp"
  @ONLY
)

#-----------------------------------------------------------------------------

file(GLOB generated_header_files "${CMAKE_CURRENT_BINARY_DIR}/include/**/*.hpp")

set(header_files
  include/alloy/core/intrinsics.hpp
  include/alloy/core/precision.hpp
  include/alloy/core/precision/fixed.hpp
  include/alloy/core/precision/half.hpp
  include/alloy/core/precision/real.hpp
  include/alloy/core/macros.hpp

  include/alloy/math/angle.hpp
  include/alloy/math/angle/basic_angle.hpp
  include/alloy/math/angle/degree.hpp
  include/alloy/math/angle/gradian.hpp
  include/alloy/math/angle/radian.hpp
  include/alloy/math/constants.hpp
  include/alloy/math/interpolation.hpp
  include/alloy/math/simplex.hpp
  include/alloy/math/trigonometry.hpp
  include/alloy/math/matrix/matrix2.hpp
  include/alloy/math/matrix/matrix3.hpp
  include/alloy/math/matrix/matrix4.hpp
  include/alloy/math/matrix/matrix_utilities.hpp
  include/alloy/math/matrix.hpp
  include/alloy/math/vector/vector2.hpp
  include/alloy/math/vector/vector3.hpp
  include/alloy/math/vector/vector4.hpp
  include/alloy/math/vector/vector_utilities.hpp
  include/alloy/math/vector.hpp
  include/alloy/math/quaternion.hpp
  include/alloy/math/math.hpp
)

set(inline_header_files
  include/alloy/core/intrinsics/clang.hpp
  include/alloy/core/intrinsics/gcc.hpp
  include/alloy/core/intrinsics/msvc.hpp
  include/alloy/core/precision/detail/real.inl

  include/alloy/math/angle/detail/basic_angle.inl
  include/alloy/math/angle/detail/degree.inl
  include/alloy/math/angle/detail/gradian.inl
  include/alloy/math/angle/detail/radian.inl
  include/alloy/math/vector/detail/vector2.inl
  include/alloy/math/vector/detail/vector3.inl
  include/alloy/math/vector/detail/vector4.inl
  include/alloy/math/vector/detail/vector_utilities.inl
)

set(source_files
  src/alloy/math/simplex.cpp
  src/alloy/math/trigonometry.cpp
  src/alloy/math/quaternion.cpp
)

add_library(Alloy
  ${generated_header_files}
  ${header_files}
  ${inline_header_files}
  ${source_files}
)
add_library(Alloy::Alloy ALIAS Alloy)

target_compile_features(Alloy
  PUBLIC cxx_std_17
)
set_target_properties(Alloy
  PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(Alloy
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

##############################################################################
# Clang Tidy
##############################################################################

include(ClangTidy)

if (ALLOY_ENABLE_CLANG_TIDY)
  target_enable_clang_tidy(Alloy REQUIRED)
endif ()

##############################################################################
# Header Self-Containment Tests
##############################################################################

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(Alloy.SelfContainmentTest
    TARGET Alloy
    HEADERS ${header_files}
  )
  target_sources(Alloy.SelfContainmentTest PRIVATE ${inline_header_files})
endif ()

##############################################################################
# Unit Tests
##############################################################################

if (ALLOY_COMPILE_UNIT_TESTS)
  add_subdirectory(test)
endif ()

##############################################################################
# Extras
##############################################################################

if (ALLOY_COMPILE_EXTRAS)
  add_subdirectory(extra)
endif ()

##############################################################################
# Examples
##############################################################################

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory(example)
endif ()

##############################################################################
# Documentation
##############################################################################

if (ALLOY_GENERATE_DOCS)
  add_subdirectory(docs)
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

# Includes
install(
  DIRECTORY "include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(
  DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
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