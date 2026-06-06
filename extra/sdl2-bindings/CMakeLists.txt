##############################################################################
# Target
##############################################################################

set(header_files
  include/alloy/extra/sdl2-bindings/api.hpp
  include/alloy/extra/sdl2-bindings/sdl2_window.hpp
  include/alloy/extra/sdl2-bindings/sdl2_gl_window.hpp
  include/alloy/extra/sdl2-bindings/sdl2_pump_source.hpp
)

set(source_files
  src/alloy/extra/sdl2-bindings/sdl2_window.cpp
  src/alloy/extra/sdl2-bindings/sdl2_gl_window.cpp
  src/alloy/extra/sdl2-bindings/sdl2_pump_source.cpp
)
source_group(
  TREE "${CMAKE_CURRENT_LIST_DIR}/src"
  PREFIX "Source Files"
  FILES ${source_files}
)

add_library(${PROJECT_NAME}ExtraSDL2Bindings)
add_library(${PROJECT_NAME}::Extra::SDL2Bindings ALIAS
  ${PROJECT_NAME}ExtraSDL2Bindings
)

target_sources(${PROJECT_NAME}ExtraSDL2Bindings
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS include
      FILES ${header_files}
  PRIVATE
    ${source_files}
)

target_compile_features(${PROJECT_NAME}ExtraSDL2Bindings
  PUBLIC cxx_std_17
)
set_target_properties(${PROJECT_NAME}ExtraSDL2Bindings
  PROPERTIES
    OUTPUT_NAME "alloy-extra-sdl2-bindings"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    FOLDER "Optional Component Targets"
)
if (BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME}ExtraSDL2Bindings
    PUBLIC ALLOY_EXTRA_SDL2_EXPORT=1
  )
else()
  target_compile_definitions(${PROJECT_NAME}ExtraSDL2Bindings
    PUBLIC ALLOY_EXTRA_SDL2_STATIC=1
  )
endif()

target_link_libraries(${PROJECT_NAME}ExtraSDL2Bindings
  PUBLIC SDL2::SDL2
  PUBLIC ${PROJECT_NAME}::IO
)
if (UNIX)
  # Workaround for SDL2 not building on *nix platforms due to a bug:
  # https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023
  target_include_directories(${PROJECT_NAME}ExtraSDL2Bindings
    PUBLIC $<BUILD_INTERFACE:/usr/include/SDL2/>
  )
endif ()

# Header Self-Containment Tests

include(SurrogateTarget)

if (ALLOY_COMPILE_SURROGATE_TESTS)
  add_surrogate_target(${PROJECT_NAME}ExtraSDL2Bindings.Surrogate
    TARGET ${PROJECT_NAME}ExtraSDL2Bindings
  )
endif ()

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example/sdl2-window")
endif ()
