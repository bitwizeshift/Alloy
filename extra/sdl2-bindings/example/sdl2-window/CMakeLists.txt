cmake_minimum_required(VERSION 3.12)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(source_files
  src/main.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_executable(${PROJECT_NAME}SDL2WindowExample
  ${source_files}
)

if (MINGW)
  target_link_libraries(${PROJECT_NAME}SDL2WindowExample
    PRIVATE mingw32
  )
endif ()

target_link_libraries(${PROJECT_NAME}SDL2WindowExample
  PRIVATE Alloy::Core
  PRIVATE Alloy::IO
  PRIVATE Alloy::Extra::SDL2Bindings
  PRIVATE SDL2main
  PRIVATE OpenGL::GL
)

set_target_properties(${PROJECT_NAME}SDL2WindowExample
  PROPERTIES
    OUTPUT_NAME "sdl2-window-example"
    FOLDER "Example Targets"
)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND
    "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")

  # clang-cl is complaining that 'event' is shadowed in each if/else ladder
  target_compile_options(${PROJECT_NAME}SDL2WindowExample
    PRIVATE "-Wno-shadow"
  )
endif ()
