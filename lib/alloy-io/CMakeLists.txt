##############################################################################
# Configured Files
##############################################################################

# read license, indent each line by 2 spaces
file(READ "${PROJECT_SOURCE_DIR}/LICENSE" license_text)
string(REPLACE "\n" "\n  " LICENSE_TEXT "  ${license_text}")

set(ALLOY_IO_EVENT_SIZE ${ALLOY_EVENT_SIZE})
set(ALLOY_IO_EVENT_ALIGN ${ALLOY_EVENT_ALIGN})
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.hpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/alloy/io/config.hpp"
  @ONLY
)
unset(ALLOY_IO_EVENT_SIZE)
unset(ALLOY_IO_EVENT_ALIGN)

##############################################################################
# Target
##############################################################################

set(generated_header_files
  ${CMAKE_CURRENT_BINARY_DIR}/include/alloy/io/config.hpp
)

set(header_files
  include/alloy/io/api.hpp
  include/alloy/io/buffers/mutable_buffer.hpp
  include/alloy/io/buffers/const_buffer.hpp
  include/alloy/io/events/keyboard_events.hpp
  include/alloy/io/events/window_events.hpp
  include/alloy/io/events/filesystem_events.hpp
  include/alloy/io/event.hpp
  include/alloy/io/keyboard.hpp
  include/alloy/io/sequence_detector.hpp
  include/alloy/io/window.hpp
  include/alloy/io/message_pump.hpp
  include/alloy/io/filesystem/disk_filesystem.hpp
  include/alloy/io/filesystem/filesystem_monitor.hpp
  include/alloy/io/filesystem/file_adapter.hpp
  include/alloy/io/filesystem/file_stream.hpp
  include/alloy/io/filesystem/file_stream_finalizer.hpp
  include/alloy/io/filesystem/file.hpp
  include/alloy/io/filesystem/null_filesystem.hpp
  include/alloy/io/filesystem/random_filesystem.hpp
  include/alloy/io/filesystem/memory_filesystem.hpp
  include/alloy/io/filesystem/xor_file_adapter.hpp
)

set(source_files
  src/alloy/io/window.cpp
  src/alloy/io/message_pump.cpp
  src/alloy/io/filesystem/disk_filesystem.cpp
  src/alloy/io/filesystem/filesystem_monitor.cpp
  src/alloy/io/filesystem/file.cpp
  src/alloy/io/filesystem/null_filesystem.cpp
  src/alloy/io/filesystem/random_filesystem.cpp
  src/alloy/io/filesystem/memory_filesystem.cpp
  src/alloy/io/filesystem/xor_file_adapter.cpp
)
add_alloy_library(${PROJECT_NAME}IO
  ALIAS             ${PROJECT_NAME}::IO
  OUTPUT_NAME       alloy-io
  EXPORT_NAME       IO
  API_PREFIX        ALLOY_IO
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
  TARGETS "${PROJECT_NAME}IO"
  EXPORT AlloyTargets
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  FILE_SET HEADERS   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILE_SET generated DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
