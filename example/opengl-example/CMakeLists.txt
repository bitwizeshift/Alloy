cmake_minimum_required(VERSION 4.3)

set(source_files
  src/main.cpp
  src/shaders.hpp
  src/sdl_window_target.hpp
  src/sdl_window_target.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_executable(${PROJECT_NAME}OpenGLExample
  ${source_files}
)

target_link_libraries(${PROJECT_NAME}OpenGLExample
  PRIVATE Alloy::CommonFlags
  PRIVATE Alloy::Core
  PRIVATE Alloy::Platform
  PRIVATE Alloy::Render
  PRIVATE Alloy::Render::GL
  PRIVATE Alloy::Extra::SDL2Bindings
)

set_target_properties(${PROJECT_NAME}OpenGLExample
  PROPERTIES
    OUTPUT_NAME "opengl-example"
    FOLDER "Example Targets"
)
