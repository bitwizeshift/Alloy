SETLOCAL EnableDelayedExpansion

@REM  # Install 'conan' dependencies

makedir build
cd build
conan install ..

@REM  # Generate the CMake project

cmake
  .. \
  -A%PLATFORM% \
  -DALLOY_ENABLE_UNIT_TESTS=On \
  -DALLOY_ENABLE_HEADER_SELF_CONTAINMENT_TESTS=On \
|| exit /b !ERRORLEVEL!