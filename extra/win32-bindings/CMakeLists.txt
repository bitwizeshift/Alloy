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
add_alloy_library(${PROJECT_NAME}ExtraWin32Bindings
  ALIAS       ${PROJECT_NAME}::Extra::Win32Bindings
  OUTPUT_NAME alloy-extra-win32-bindings
  API_PREFIX  ALLOY_EXTRA_WIN32
  FOLDER      "Optional Component Targets"
  HIDDEN_VISIBILITY
  HEADERS     ${header_files}
  SOURCES     ${source_files}
  DEPENDENCIES
    PUBLIC ${PROJECT_NAME}::IO
)

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example/filesystem-watcher")
endif ()
