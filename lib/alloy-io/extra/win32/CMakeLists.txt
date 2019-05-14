##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/io/win32_filesystem_monitor.hpp
)

set(source_files
  src/alloy/io/win32_filesystem_monitor.cpp
  src/alloy/io/windows.hpp
)

add_library(${PROJECT_NAME}IOWin32 STATIC
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::IO::Win32 ALIAS ${PROJECT_NAME}IOWin32)

target_compile_features(${PROJECT_NAME}IOWin32
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}IOWin32
  PROPERTIES
    OUTPUT_NAME "alloy-io-win32"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(${PROJECT_NAME}IOWin32
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}IOWin32
  PUBLIC ${PROJECT_NAME}::IO
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}IOWin32.SelfContainmentTest
    TARGET ${PROJECT_NAME}IOWin32
    HEADERS ${header_files}
  )
endif ()