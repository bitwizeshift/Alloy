##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/io/window.hpp
  include/alloy/io/message_pump.hpp
  include/alloy/io/filesystem/filesystem_monitor.hpp
)

set(source_files
  src/alloy/io/window.cpp
  src/alloy/io/message_pump.cpp
  src/alloy/io/filesystem/filesystem_monitor.cpp
)

add_library(${PROJECT_NAME}IO
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::IO ALIAS ${PROJECT_NAME}IO)

target_compile_features(${PROJECT_NAME}IO
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}IO
  PROPERTIES
    OUTPUT_NAME "alloy-io"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME}IO
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}IO
  PUBLIC ${PROJECT_NAME}::Core
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}IO.SelfContainmentTest
    TARGET ${PROJECT_NAME}IO
    HEADERS ${header_files}
  )
endif ()

# Extras
include(CMakeDependentOption)
cmake_dependent_option(ALLOY_COMPILE_EXTRAS_IO_WIN32
  "Compiles the Win32 implementation of the 'io' library" ON
  "ALLOY_COMPILE_EXTRAS;WIN32" OFF
)

if (ALLOY_COMPILE_EXTRAS_IO_WIN32)
  add_subdirectory("extra/win32")
endif ()

# Unit Tests

if (ALLOY_COMPILE_TESTS)
  add_subdirectory("test")
endif ()

##############################################################################
# Installation
##############################################################################

if (NOT NOT_SUBPROJECT)
  return()
endif ()

include(GNUInstallDirs)

set(ALLOY_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Targets
install(
  TARGETS "${PROJECT_NAME}IO"
  EXPORT "${PROJECT_NAME}IOTargets"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
  EXPORT "${PROJECT_NAME}IOTargets"
  NAMESPACE "${PROJECT_NAME}::"
  DESTINATION "${ALLOY_CMAKE_CONFIG_DESTINATION}"
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
