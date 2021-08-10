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
source_group(
  TREE "${CMAKE_CURRENT_BINARY_DIR}/include"
  PREFIX "Generated Header Files"
  FILES ${generated_header_files}
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
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/include"
  PREFIX "Header Files"
  FILES ${header_files}
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
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}IO
  ${header_files}
  ${generated_header_files}
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
    FOLDER "Component Targets"
)
if (BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME}IO
    PRIVATE ALLOY_IO_EXPORT=1
  )
else()
  target_compile_definitions(${PROJECT_NAME}IO
    PUBLIC ALLOY_IO_STATIC=1
  )
endif()

target_include_directories(${PROJECT_NAME}IO
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
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

# Unit Tests

if (ALLOY_COMPILE_TESTS)
  add_subdirectory("test")
endif ()

##############################################################################
# Installation
##############################################################################

#if (ALLOY_IS_SUBPROJECT)
  return()
#endif ()

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
