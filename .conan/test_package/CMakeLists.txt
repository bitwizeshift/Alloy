cmake_minimum_required(VERSION 3.8)

project(Alloy.ConanTestPackage)

include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_set_find_paths()

find_package(Alloy REQUIRED)

set(source_files
  src/main.cpp
)

add_executable(${PROJECT_NAME}
  ${source_files}
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE Alloy::Alloy
)
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    OUTPUT_NAME "alloy-test-package"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
