
CPMAddPackage(
  NAME CPMLicenses.cmake
  GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
  VERSION 0.0.7
)

#===============================================================================
# Normal Dependencies
#===============================================================================

#-------------------------------------------------------------------------------
# libtga Dependency
#-------------------------------------------------------------------------------

CPMAddPackage("gh:aseprite/tga#main")

set_property(
  TARGET tga-lib
  PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  "${tga_SOURCE_DIR}"
)

#-------------------------------------------------------------------------------
# libfmt Dependency
#-------------------------------------------------------------------------------

CPMAddPackage(
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 12.1.0
  OPTIONS
    "FMT_INSTALL OFF"
    "FMT_DOC OFF"
    "FMT_TEST OFF"
    "FMT_FUZZ OFF"
    "FMT_CUDA_FUZZ OFF"
    "FMT_OS OFF"
    "FMT_MODULE OFF"
)

#-------------------------------------------------------------------------------
# Imgui
#-------------------------------------------------------------------------------

CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  VERSION 1.83
  DOWNLOAD_ONLY YES
)
set(IMGUI_SOURCE_DIR "${imgui_SOURCE_DIR}" CACHE STRING "")

#===============================================================================
# Test Dependencies
#===============================================================================

if (ALLOY_COMPILE_TESTS)
  #-----------------------------------------------------------------------------
  # Catch2 Dependency
  #-----------------------------------------------------------------------------

  CPMAddPackage(
    GITHUB_REPOSITORY catchorg/Catch2
    VERSION 3.15.0
    SYSTEM YES
    OPTIONS
      "CATCH_DEVELOPMENT_BUILD OFF"
      "CATCH_INSTALL_DOCS OFF"
      "CATCH_INSTALL_EXTRAS OFF"
  )

  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${Catch2_SOURCE_DIR}/extras" PARENT_SCOPE)

  #-----------------------------------------------------------------------------
  # Trompeloeil
  #-----------------------------------------------------------------------------

  CPMAddPackage(
    GITHUB_REPOSITORY rollbear/trompeloeil
    VERSION 49
    OPTIONS
      "TROMPELOEIL_INSTALL_DOCS OFF"
  )
endif ()
