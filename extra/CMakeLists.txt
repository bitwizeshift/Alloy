# Extras
include(CMakeDependentOption)
find_package(SDL2 QUIET)
find_package(fmt QUIET)

cmake_dependent_option(ALLOY_COMPILE_EXTRAS_USE_DISCRETE_GPU
  "Compiles the Discrete GPU-enabling static library" ON
  "ALLOY_COMPILE_EXTRAS" OFF
)
cmake_dependent_option(ALLOY_COMPILE_EXTRAS_LOGGING
  "Compiles the logging library" ON
  "ALLOY_COMPILE_EXTRAS;fmt_FOUND" OFF
)
cmake_dependent_option(ALLOY_COMPILE_EXTRAS_SDL2
  "Compiles the SDL implementation of the 'io' library" ON
  "ALLOY_COMPILE_EXTRAS;SDL2_FOUND" OFF
)
cmake_dependent_option(ALLOY_COMPILE_EXTRAS_IO_WIN32
  "Compiles the Win32 implementation of the 'io' library" ON
  "ALLOY_COMPILE_EXTRAS;WIN32" OFF
)

if (ALLOY_COMPILE_EXTRAS_USE_DISCRETE_GPU)
  add_subdirectory("use-discrete-gpu")
endif ()

if (ALLOY_COMPILE_EXTRAS_LOGGING)
  add_subdirectory("logging")
endif ()

if (ALLOY_COMPILE_EXTRAS_SDL2)
  add_subdirectory("sdl2-bindings")
endif ()

if (ALLOY_COMPILE_EXTRAS_IO_WIN32)
  add_subdirectory("win32-bindings")
endif ()