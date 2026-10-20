#[[
  Copyright (c) Matthew Rodusek
  SPDX-License-Identifier: MIT
  See the LICENSE file at the root of this repository for the full text.
]]

# clang-aarch64-toolchain
# =======================
#
# An LLVM toolchain for building ELF executables in x86_64 architecture
#
# This toolchain also provides a mechanism for compiling and linking EFI
# applications via `add_efi_executable(...)` which models itself after CMake's
# built-in `add_executable(...)`.
#

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
