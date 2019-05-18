cmake_minimum_required(VERSION 3.12)

add_executable(${PROJECT_NAME}Win32FilesystemWatcherExample
  src/main.cpp
)

target_link_libraries(${PROJECT_NAME}Win32FilesystemWatcherExample
  PRIVATE Alloy::Core
  PRIVATE Alloy::IO
  PRIVATE Alloy::Extra::Win32Bindings
)

set_target_properties(${PROJECT_NAME}Win32FilesystemWatcherExample
  PROPERTIES
    OUTPUT_NAME "win32-filesystem-watcher-example"
)
