cmake_minimum_required(VERSION 3.12)

set(source_files
  src/main.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_executable(${PROJECT_NAME}LoggingExample
  ${source_files}
)

target_link_libraries(${PROJECT_NAME}LoggingExample
  PRIVATE Alloy::Core
  PRIVATE Alloy::IO
  PRIVATE Alloy::Extra::Logging
)

set_target_properties(${PROJECT_NAME}LoggingExample
  PROPERTIES
    OUTPUT_NAME "logging-example"
)
