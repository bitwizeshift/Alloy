
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
# Imgui
#-------------------------------------------------------------------------------

CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  VERSION 1.83
  GIT_SHALLOW TRUE
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
    GIT_SHALLOW TRUE
    SYSTEM YES
    OPTIONS
      "CATCH_DEVELOPMENT_BUILD OFF"
      "CATCH_INSTALL_DOCS OFF"
      "CATCH_INSTALL_EXTRAS OFF"
      "CATCH_CONFIG_NO_COUNTER ON"
  )

  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${Catch2_SOURCE_DIR}/extras" PARENT_SCOPE)
endif ()
