# Copyright (c) Matthew Rodusek
# Distributed under the OSI-approved MIT License. See accompanying
# file LICENSE.txt or https://opensource.org/licenses/MIT for details.

#.rst:
# CommonFlags
# -----------
#
# Defines ``Alloy::CommonFlags``, an ``INTERFACE`` target carrying the compile
# flags that every first-party Alloy target opts into by linking it. Keeping the
# flags on a target instead of in a directory-scoped ``add_compile_options`` call
# means they never leak into third-party dependencies and can be composed
# per-target rather than imposed globally.

add_library(${PROJECT_NAME}CommonFlags INTERFACE)
add_library(${PROJECT_NAME}::CommonFlags ALIAS ${PROJECT_NAME}CommonFlags)

# GNU and native MSVC are unambiguous, so they are expressed as generator
# expressions. The Clang family is resolved at configure time below because
# distinguishing clang-cl (which simulates MSVC) from a GNU-frontend Clang relies
# on CMAKE_CXX_SIMULATE_ID, which has no portable generator expression.
target_compile_options(${PROJECT_NAME}CommonFlags
  INTERFACE
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wall;-Werror;-Wextra;-pedantic;-pedantic-errors;-Wno-newline-eof;-Wno-ignored-attributes;-Wno-c++20-extensions;-Wno-nontrivial-memcall>
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4;/WX>
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
    # clang-cl does not implement '-pedantic'/'-pedantic-errors', and treats
    # '-Wall' like '-Weverything', so several warnings are disabled explicitly.
    target_compile_options(${PROJECT_NAME}CommonFlags
      INTERFACE
        -Wall -Werror -Wextra
        # Unnecessary compatibility warnings for a modern C++ library.
        -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-newline-eof
        # A good warning, but unhelpful when coupled with -Werror.
        -Wno-padded
        # 'error_category' and similar standard types need exit-time destructors.
        -Wno-exit-time-destructors
        # Erroneously fires on valid library use such as std::lock_guard.
        -Wno-ctad-maybe-unsupported
        # MSVC's standard library requires '_'-prefixed macros to be conformant.
        -Wno-reserved-id-macro
        # TODO(#4): Undo once this no longer produces errors.
        -Wno-weak-vtables
        # Fires on switch statements that intentionally fall through 'default:'.
        -Wno-switch-enum
        # dllexport/dllimport on classes with inline functions trips this.
        -Wno-ignored-attributes
        # imgui has non-trivial memcopy calls.
        -Wno-nontrivial-memcall
    )
  else ()
    target_compile_options(${PROJECT_NAME}CommonFlags
      INTERFACE
        -Wall -Werror -Wextra -pedantic -pedantic-errors
        # Xcode enables -Wnewline-eof with -Wextra; a legacy requirement.
        -Wno-newline-eof
        # dllexport/dllimport on classes with inline functions trips this.
        -Wno-ignored-attributes
        -Wno-c++20-extensions
        # imgui has non-trivial memcopy calls.
        -Wno-nontrivial-memcall
    )
    if (APPLE)
      # Necessary for any uses of OpenGL on Apple.
      target_compile_options(${PROJECT_NAME}CommonFlags
        INTERFACE -Wno-deprecated-declarations
      )
    endif ()
  endif ()
endif ()
