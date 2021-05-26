add_subdirectory("tga")
# If we are compiling unit tests, then we need catch
if (ALLOY_COMPILE_TESTS)
  add_subdirectory("catch")
endif ()
