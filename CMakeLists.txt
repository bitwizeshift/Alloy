cmake_minimum_required(VERSION 3.4)

enable_testing()

set(ALLOY_CMAKE_TEMPLATE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake" "${CMAKE_MODULE_PATH}")

##############################################################################
# Project Options
##############################################################################

option(ALLOY_COMPILE_SELF_CONTAINMENT_TESTS "Include each header independently in a .cpp file to determine header self-containment" OFF)
option(ALLOY_COMPILE_UNIT_TESTS "Compile and run the unit tests for this library" OFF)
option(ALLOY_GENERATE_DOCS "Generates doxygen documentation" OFF)
option(ALLOY_INSTALL_DOCS "Install documentation for this library" OFF)
option(ALLOY_VERBOSE_CONFIGURE "Verbosely configures this library project" OFF)

##############################################################################
# Project
##############################################################################

set(ALLOY_VERSION_MAJOR 0 CACHE STRING "major version of the Alloy game engine" FORCE)
set(ALLOY_VERSION_MINOR 0 CACHE STRING "minor version of the Alloy game engine" FORCE)
set(ALLOY_VERSION_PATCH 1 CACHE STRING "patch version of the Alloy game engine" FORCE)

set(ALLOY_VERSION "${ALLOY_VERSION_MAJOR}.${ALLOY_VERSION_MINOR}.${ALLOY_VERSION_PATCH}" CACHE STRING "version of Alloy game engine" FORCE)

project(Alloy
  VERSION "${ALLOY_VERSION}"
  LANGUAGES CXX
)

if( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake" )
  include("${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
  conan_basic_setup()
endif()

if( ALLOY_VERBOSE_CONFIGURE )
  message(STATUS "Alloy ${ALLOY_VERSION}")
endif()

#-----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# default to debug when building CMake
if( NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES )
  set(CMAKE_BUILD_TYPE Debug)
endif()

#-----------------------------------------------------------------------------

if( "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  add_compile_options(-Wall -Werror -pedantic )
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
  # TODO: Determine MSVC necessary compiler flags
endif()

# read license, indent each line by 2 spaces
file(READ "LICENSE.txt" license_text)
string(REPLACE "\n" "\n  " LICENSE_TEXT "  ${license_text}")

# Generate Files
configure_file( "${ALLOY_CMAKE_TEMPLATE_PATH}/version.hpp.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/version.hpp"
                @ONLY )

configure_file( "${ALLOY_CMAKE_TEMPLATE_PATH}/config.hpp.in"
                "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/config.hpp"
                @ONLY )

set(header_files
  include/alloy/math/types.hpp
)

set(inline_header_files

)

set(source_files
  src/dummy.cpp
)

add_library(Alloy 
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

target_include_directories(Alloy PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

##############################################################################
# Header Self-Containment Tests
##############################################################################

include(AddSelfContainmentTest)

if( ALLOY_COMPILE_SELF_CONTAINMENT_TESTS )
  add_self_containment_test(Alloy.SelfContainmentTest
    TARGET Alloy
    HEADERS ${header_files}
  )
  target_sources(Alloy.SelfContainmentTest PRIVATE ${inline_header_files})
endif()

##############################################################################
# Unit Tests
##############################################################################

if( ALLOY_COMPILE_UNIT_TESTS )
  add_subdirectory(test)
endif()

##############################################################################
# Documentation
##############################################################################

if( ALLOY_GENERATE_DOCS )
  add_subdirectory(docs)
endif()

##############################################################################
# Installation
##############################################################################

configure_file(
  "${ALLOY_CMAKE_TEMPLATE_PATH}/${PROJECT_NAME}ConfigVersion.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  @ONLY
)

configure_file(
  "${ALLOY_CMAKE_TEMPLATE_PATH}/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  @ONLY
)

#-----------------------------------------------------------------------------

include(GNUInstallDirs)

set(ALLOY_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

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
  TARGETS "Alloy"
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