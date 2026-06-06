# clang-aarch64-toolchain
# =======================
#
# An LLVM toolchain for building ELF executables in x86_64 architecture
#
# This toolchain also provides a mechanism for compiling and linking EFI
# applications via `add_efi_executable(...)` which models itself after CMake's
# built-in `add_executable(...)`.
#

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

set(cxx_compiler_list
  clang++
)
set(c_compiler_list
  clang
)
set(gcovr_list
  llvm-cov
)

if (DEFINED ENV{CLANG_VERSION})
  list(PREPEND cxx_compiler_list
    clang++-$ENV{CLANG_VERSION}    # Normal clang version
    clang++-mp-$ENV{CLANG_VERSION} # Macports version
  )
  list(PREPEND c_compiler_list
    clang-$ENV{CLANG_VERSION}    # Normal clang version
    clang-mp-$ENV{CLANG_VERSION} # Macports version
  )
  list(PREPEND gcovr_list
    llvm-cov-$ENV{CLANG_VERSION}
  )
endif ()

find_program(_CXX_CLANG_EXE
  NAMES ${cxx_compiler_list}
  REQUIRED
)

find_program(_C_CLANG_EXE
  NAMES ${c_compiler_list}
  REQUIRED
)

find_program(_GCOV_COMMAND
  NAMES ${gcovr_list}
)

# A simple function to test that the compiler has the correct version
function(test_compiler program version)

  exec_program("${program}"
    ARGS "--version"
    OUTPUT_VARIABLE output
    RETURN_VALUE result
  )

  if (NOT result EQUAL 0)
    message(FATAL_ERROR "Error executing `${program} --version`")
  endif ()

  if (NOT output MATCHES "clang version ${version}")
    message(FATAL_ERROR
      "Invalid clang version discovered while looking for version ${version}"
    )
  endif ()

endfunction()

if (DEFINED ENV{CLANG_VERSION})
  test_compiler("${_CXX_CLANG_EXE}" "$ENV{CLANG_VERSION}")
  test_compiler("${_C_CLANG_EXE}" "$ENV{CLANG_VERSION}")
endif ()

set(CMAKE_ASM_COMPILER "${_C_CLANG_EXE}")
set(CMAKE_C_COMPILER "${_C_CLANG_EXE}")
set(CMAKE_CXX_COMPILER "${_CXX_CLANG_EXE}")

if (_GCOV_COMMAND)
  set(GCOV_COMMAND "${_GCOV_COMMAND} gcov" CACHE STRING "Command for gcov execution")
endif ()
