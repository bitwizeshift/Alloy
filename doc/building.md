# How to build Alloy

This guide walks through all the required tools and software
dependencies in order to build the `alloy` game-engine.

## Requirements

### Required Binaries

- [x] [CMake 3.5](https://cmake.org) or greater
- [x] C++17 compatible compiler
  * `g++-8` or greater
  * `clang++-6` or greater
  * `MSVC`
  * etc

### Optional Binaries

- [x] [`python 3.7`](https://python.org) or greater \
  This is used frequently for the `tools` scripts
- [x] [`doxygen 1.8`](http://doxygen.nl/) or greater \
  This is used to generate documentation for the code
- [x] [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) \
  This is used to validate clang formatting
- [x] [`conan 10`](https://conan.io) or greater \
  This is used to pull software dependencies below.

## Software Dependencies

### Required Dependencies

**TODO**

### Optional Dependencies

- [x] [`Catch2`](https://github.com/catchorg/Catch2) for unit tests

## Building

1. Make a build directory

```sh
mkdir build
cd build
```

2. **Optional:** Install dependencies with `conan`

`conan` makes for a convenient, semi-cross-platform way of managing
all dependencies, rather than manually syncing dependencies on the system.

```sh
conan install ..
```

3. Use CMake to generate a project

For this examples, we will generate a [`ninja`](https://ninja-build.org/)
project, and will toggle support for the unit tests and project extras.

These are all optional; any generator can be chosen, and any
options may be chosen. For the complete list of options, check the root project's `CMakeLists.txt`

```sh
cmake .. -G"Ninja" -DALLOY_COMPILE_EXTRAS=On -DALLOY_COMPILE_UNIT_TESTS=On
```

4. Build the project

The project can be built by the following command:

```sh
cmake --build .
```

5. **Optional:** Run unit tests (if enabled)

The unit tests can be executed with `ctest` either through
building the `cmake` `test` target:

```sh
cmake --build . --target test
```

or through running `ctest` directly:

```sh
ctest .
```