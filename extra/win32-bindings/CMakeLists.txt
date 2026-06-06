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
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}ExtraWin32Bindings)
add_library(${PROJECT_NAME}::Extra::Win32Bindings ALIAS ${PROJECT_NAME}ExtraWin32Bindings)

target_sources(${PROJECT_NAME}ExtraWin32Bindings
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS include
      FILES ${header_files}
  PRIVATE
    ${source_files}
)

target_compile_features(${PROJECT_NAME}ExtraWin32Bindings
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}ExtraWin32Bindings
  PROPERTIES
    OUTPUT_NAME "alloy-extra-win32-bindings"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "Optional Component Targets"
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

target_link_libraries(${PROJECT_NAME}ExtraWin32Bindings
  PUBLIC ${PROJECT_NAME}::IO
)

# Header Self-Containment Tests

include(SurrogateTarget)

if (ALLOY_COMPILE_SURROGATE_TESTS)
  add_surrogate_target(${PROJECT_NAME}ExtraWin32Bindings.Surrogate
    TARGET ${PROJECT_NAME}ExtraWin32Bindings
  )
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example/filesystem-watcher")
endif ()
