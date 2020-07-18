##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/extra/win32-bindings/api.hpp
  include/alloy/extra/win32-bindings/win32_filesystem_monitor.hpp
)

set(source_files
  src/alloy/extra/win32-bindings/win32_filesystem_monitor.cpp
  src/alloy/extra/win32-bindings/windows.hpp
)

add_library(${PROJECT_NAME}ExtraWin32Bindings
  ${header_files}
  ${source_files}
)
add_library(${PROJECT_NAME}::Extra::Win32Bindings ALIAS ${PROJECT_NAME}ExtraWin32Bindings)

target_compile_features(${PROJECT_NAME}ExtraWin32Bindings
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}ExtraWin32Bindings
  PROPERTIES
    OUTPUT_NAME "alloy-extra-win32-bindings"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
if (BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME}ExtraWin32Bindings
    PRIVATE ALLOY_EXTRA_WIN32_EXPORT=1
  )
else()
  target_compile_definitions(${PROJECT_NAME}ExtraWin32Bindings
    PUBLIC ALLOY_EXTRA_WIN32_STATIC=1
  )
endif()

target_include_directories(${PROJECT_NAME}ExtraWin32Bindings
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME}ExtraWin32Bindings
  PUBLIC ${PROJECT_NAME}::IO
)

# Header Self-Containment Tests

include(AddSelfContainmentTest)

if (ALLOY_COMPILE_SELF_CONTAINMENT_TESTS)
  add_self_containment_test(${PROJECT_NAME}ExtraWin32Bindings.SelfContainmentTest
    TARGET ${PROJECT_NAME}ExtraWin32Bindings
    HEADERS ${header_files}
  )
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example/filesystem-watcher")
endif ()
