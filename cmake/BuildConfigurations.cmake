# build configurations
# ====================
#
# Adds support for different build configurations

#[[
  The MIT License (MIT)

  Copyright (c) 2022 Matthew Rodusek

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
]]

# default to debug when building CMake
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build configuration specified. Defaulting to 'Debug'")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "The active build configuration")
endif ()

# Add additional build configurations if unspecified, and building for Clang
# without cross-compiling

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CROSSCOMPILING)
  message(STATUS "Sanitizers and coverage builds not currently supported")
  return ()
endif ()

#------------------------------------------------------------------------------
# UBSan
#------------------------------------------------------------------------------

list(APPEND CMAKE_CONFIGURATION_TYPES "UBSan")

# Flags are from https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
set(ubsan_compile_flags "-O0" "-g" "-fsanitize=undefined")
set(ubsan_link_flags "-fsanitize=undefined")

foreach (flag IN LISTS ubsan_compile_flags)
  string(APPEND CMAKE_C_FLAGS_UBSAN " ${flag}")
  string(APPEND CMAKE_CXX_FLAGS_UBSAN " ${flag}")
  add_compile_options($<$<CONFIG:UBSan>:${flag}>)
endforeach ()

foreach (flag IN LISTS ubsan_link_flags)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_UBSAN " ${flag}")
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_UBSAN " ${flag}")
  add_link_options($<$<CONFIG:UBSan>:${flag}>)
endforeach ()

#------------------------------------------------------------------------------
# ASan
#------------------------------------------------------------------------------

list(APPEND CMAKE_CONFIGURATION_TYPES "ASan")

# Flags are from https://clang.llvm.org/docs/AddressSanitizer.html
set(asan_compile_flags "-O1" "-g" "-fsanitize=address" "-fno-omit-frame-pointer")
set(asan_link_flags "-fsanitize=address")

foreach (flag IN LISTS asan_compile_flags)
  string(APPEND CMAKE_C_FLAGS_ASAN " ${flag}")
  string(APPEND CMAKE_CXX_FLAGS_ASAN " ${flag}")
  add_compile_options($<$<CONFIG:ASan>:${flag}>)
endforeach ()

foreach (flag IN LISTS asan_link_flags)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_ASAN " ${flag}")
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_ASAN " ${flag}")
  add_link_options($<$<CONFIG:ASan>:${flag}>)
endforeach ()

#------------------------------------------------------------------------------
# MSan
#------------------------------------------------------------------------------

list(APPEND CMAKE_CONFIGURATION_TYPES "MSan")

# Flags are from https://clang.llvm.org/docs/MemorySanitizer.html
set(msan_compile_flags "-O2" "-g" "-fsanitize=memory" "-fno-omit-frame-pointer")
set(msan_link_flags "-fsanitize=memory")

foreach (flag IN LISTS msan_compile_flags)
  string(APPEND CMAKE_C_FLAGS_MSAN " ${flag}")
  string(APPEND CMAKE_CXX_FLAGS_MSAN " ${flag}")
  add_compile_options($<$<CONFIG:MSan>:${flag}>)
endforeach ()

foreach (flag IN LISTS msan_link_flags)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_MSAN " ${flag}")
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_MSAN " ${flag}")
  add_link_options($<$<CONFIG:MSan>:${flag}>)
endforeach ()

#------------------------------------------------------------------------------
# Leak
#------------------------------------------------------------------------------

list(APPEND CMAKE_CONFIGURATION_TYPES "LSan")

# Flags are from https://clang.llvm.org/docs/LeakSanitizer.html
set(lsan_compile_flags "-O0" "-g" "-fsanitize=leak")
set(lsan_link_flags "-fsanitize=leak")

foreach (flag IN LISTS lsan_compile_flags)
  string(APPEND CMAKE_C_FLAGS_LSAN " ${flag}")
  string(APPEND CMAKE_CXX_FLAGS_LSAN " ${flag}")
  add_compile_options($<$<CONFIG:LSan>:${flag}>)
endforeach ()

foreach (flag IN LISTS lsan_link_flags)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_LSAN " ${flag}")
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_LSAN " ${flag}")
  add_link_options($<$<CONFIG:LSan>:${flag}>)
endforeach ()

#------------------------------------------------------------------------------
# Coverage
#------------------------------------------------------------------------------

list(APPEND CMAKE_CONFIGURATION_TYPES "Coverage")

set(coverage_compile_flags "-O0" "-g" "--coverage")

foreach (flag IN LISTS coverage_compile_flags)
  string(APPEND CMAKE_C_FLAGS_COVERAGE " ${flag}")
  string(APPEND CMAKE_CXX_FLAGS_COVERAGE " ${flag}")
  add_compile_options($<$<CONFIG:Coverage>:${flag}>)
endforeach ()
