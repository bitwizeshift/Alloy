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
add_alloy_library(${PROJECT_NAME}ExtraSDL2Bindings
  ALIAS       ${PROJECT_NAME}::Extra::SDL2Bindings
  OUTPUT_NAME alloy-extra-sdl2-bindings
  API_PREFIX  ALLOY_EXTRA_SDL2
  FOLDER      "Optional Component Targets"
  HIDDEN_VISIBILITY
  HEADERS     ${header_files}
  SOURCES     ${source_files}
  DEPENDENCIES
    PUBLIC
      SDL3::SDL3-static
      ${PROJECT_NAME}::Platform
)

# Examples

if (ALLOY_COMPILE_EXAMPLES)
  add_subdirectory("example/sdl2-window")
endif ()
