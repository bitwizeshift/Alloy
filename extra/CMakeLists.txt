# Extras
include(CMakeDependentOption)
find_package(SDL2 QUIET)

cmake_dependent_option(ALLOY_COMPILE_EXTRAS_USE_DISCRETE_GPU
  "Compiles the Discrete GPU-enabling static library" ON
  "ALLOY_COMPILE_EXTRAS" OFF
)
cmake_dependent_option(ALLOY_COMPILE_EXTRAS_SDL2
  "Compiles the SDL implementation of the 'io' library" ON
  "ALLOY_COMPILE_EXTRAS;SDL2_FOUND" OFF
)

if (ALLOY_COMPILE_EXTRAS_USE_DISCRETE_GPU)
  add_subdirectory("use-discrete-gpu")
endif ()

if (ALLOY_COMPILE_EXTRAS_SDL2)
  add_subdirectory("sdl2-bindings")
endif ()
