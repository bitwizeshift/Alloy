##############################################################################
# Configured Files
##############################################################################

# read license, indent each line by 2 spaces
file(READ "${PROJECT_SOURCE_DIR}/LICENSE" license_text)
string(REPLACE "\n" "\n  " LICENSE_TEXT "  ${license_text}")

set(ALLOY_PLATFORM_EVENT_SIZE ${ALLOY_EVENT_SIZE})
set(ALLOY_PLATFORM_EVENT_ALIGN ${ALLOY_EVENT_ALIGN})
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/platform/config.hpp"
  @ONLY
)
unset(ALLOY_PLATFORM_EVENT_SIZE)
unset(ALLOY_PLATFORM_EVENT_ALIGN)

##############################################################################
# Target
##############################################################################

set(generated_header_files
  ${CMAKE_CURRENT_BINARY_DIR}/include/alloy/platform/config.hpp
)

set(header_files
  include/alloy/platform/api.hpp
  include/alloy/platform/buffers/mutable_buffer.hpp
  include/alloy/platform/buffers/const_buffer.hpp
  include/alloy/platform/events/keyboard_events.hpp
  include/alloy/platform/events/window_events.hpp
  include/alloy/platform/events/filesystem_events.hpp
  include/alloy/platform/event.hpp
  include/alloy/platform/keyboard.hpp
  include/alloy/platform/sequence_detector.hpp
  include/alloy/platform/window.hpp
  include/alloy/platform/message_pump.hpp
  include/alloy/platform/filesystem/disk_filesystem.hpp
  include/alloy/platform/filesystem/filesystem_monitor.hpp
  include/alloy/platform/filesystem/file_adapter.hpp
  include/alloy/platform/filesystem/file_stream.hpp
  include/alloy/platform/filesystem/file_stream_finalizer.hpp
  include/alloy/platform/filesystem/file.hpp
  include/alloy/platform/filesystem/null_filesystem.hpp
  include/alloy/platform/filesystem/random_filesystem.hpp
  include/alloy/platform/filesystem/memory_filesystem.hpp
  include/alloy/platform/filesystem/xor_file_adapter.hpp
)

set(source_files
  src/alloy/platform/window.cpp
  src/alloy/platform/message_pump.cpp
  src/alloy/platform/filesystem/disk_filesystem.cpp
  src/alloy/platform/filesystem/filesystem_monitor.cpp
  src/alloy/platform/filesystem/file.cpp
  src/alloy/platform/filesystem/null_filesystem.cpp
  src/alloy/platform/filesystem/random_filesystem.cpp
  src/alloy/platform/filesystem/memory_filesystem.cpp
  src/alloy/platform/filesystem/xor_file_adapter.cpp
)
add_alloy_library(${PROJECT_NAME}Platform
  ALIAS             ${PROJECT_NAME}::Platform
  OUTPUT_NAME       alloy-platform
  EXPORT_NAME       Platform
  API_PREFIX        ALLOY_PLATFORM
  HIDDEN_VISIBILITY
  HEADERS           ${header_files}
  GENERATED_HEADERS ${generated_header_files}
  SOURCES           ${source_files}
  DEPENDENCIES
    PUBLIC ${PROJECT_NAME}::Core
)

# Unit Tests

if (ALLOY_COMPILE_TESTS)
  add_subdirectory("test")
endif ()

##############################################################################
# Installation
##############################################################################

# Installation is currently disabled: exporting AlloyEngine pulls the vendored
# tga-lib into the AlloyTargets export set, which is not yet packaged. The
# install rules below are kept modern and ready for when that is resolved.
return ()

include(GNUInstallDirs)

install(
  TARGETS "${PROJECT_NAME}Platform"
  EXPORT AlloyTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET HEADERS   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILE_SET generated DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
