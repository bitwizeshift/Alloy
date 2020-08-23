cmake_minimum_required(VERSION 3.12)

set(source_files
  src/main.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_executable(${PROJECT_NAME}Win32FilesystemWatcherExample
  ${source_files}
)

target_link_libraries(${PROJECT_NAME}Win32FilesystemWatcherExample
  PRIVATE Alloy::Core
  PRIVATE Alloy::IO
  PRIVATE Alloy::Extra::Win32Bindings
)

set_target_properties(${PROJECT_NAME}Win32FilesystemWatcherExample
  PROPERTIES
    OUTPUT_NAME "win32-filesystem-watcher-example"
    FOLDER "Example Targets"
)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND
    "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")

  # clang-cl is complaining that 'event' is shadowed in each if/else ladder
  target_compile_options(${PROJECT_NAME}Win32FilesystemWatcherExample
    PRIVATE "-Wno-shadow"
  )
endif ()
