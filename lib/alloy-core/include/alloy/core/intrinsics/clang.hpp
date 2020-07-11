/*****************************************************************************
 * \file clang.hpp
 * \brief This header detects compiler-specific features for Clang c++
 *
 * \note This is an internal header file, included by other library headers.
 *       Do not attempt to use it directly.
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2018 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef ALLOY_CORE_INTRINSICS_CLANG_HPP
#define ALLOY_CORE_INTRINSICS_CLANG_HPP

#ifndef __clang__
# error This header can only be included when compiling with clang compiler
#endif

#include "alloy/core/macros.hpp" // ALLOY_STRINGIZE

//------------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_APPLE_CLANG
# error ALLOY_COMPILER_APPLE_CLANG defined before including clang.hpp
#endif

#if defined(__apple_build_version__)
# define ALLOY_COMPILER_APPLE_CLANG 1
# define ALLOY_COMPILER_NAME "AppleClang"
#else
# define ALLOY_COMPILER_CLANG 1
# define ALLOY_COMPILER_NAME "Clang"
#endif
#define ALLOY_COMPILER_VERSION_STRING \
  ALLOY_COMPILER_NAME " " \
  ALLOY_STRINGIZE(__clang_major__) "." \
  ALLOY_STRINGIZE(__clang_minor__) "." \
  ALLOY_STRINGIZE(__clang_patchlevel__)
#define ALLOY_COMPILER_VERSION __clang_major__ * 1000000 + __clang_minor__ * 1000 + __clang_patchlevel__
#define ALLOY_COMPILER_VERSION_MAJOR __clang_major__
#define ALLOY_COMPILER_VERSION_MINOR __clang_minor__
#define ALLOY_COMPILER_VERSION_PATCH __clang_patchlevel__

//-----------------------------------------------------------------------------

#define ALLOY_COMPILER_ASM_SYNTAX ALLOY_COMPILER_ASM_SYNTAX_ATT

//-----------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_BREAKPOINT
# error ALLOY_COMPILER_BREAKPOINT defined before including clang.hpp
#endif

#define ALLOY_COMPILER_BREAKPOINT __builtin_trap()

//-----------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_WEAK_SUPPORTED
# error ALLOY_COMPILER_WEAK_SUPPORTED defined before including clang.hpp
#endif

#define ALLOY_COMPILER_WEAK_SUPPORTED 1

#define ALLOY_WEAK __attribute__((weak))

#define ALLOY_RESTRICT __restrict__
#define ALLOY_ASSUME(x) (void) 0
#define ALLOY_LIKELY(x) __builtin_expect(!!(x), 1)
#define ALLOY_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define ALLOY_FORCE_INLINE [[gnu::always_inline]] inline
#define ALLOY_NO_INLINE [[gnu::noinline]]
#define ALLOY_COMPILER_UNREACHABLE __builtin_unreachable()

//-----------------------------------------------------------------------------
// C++ Extensions Detections
//-----------------------------------------------------------------------------

#if defined(__AVX__)
# define ALLOY_COMPILER_HAS_AVX 1
#else
# define ALLOY_COMPILER_HAS_AVX 0
#endif
#if defined(__AVX2__)
# define ALLOY_COMPILER_HAS_AVX2 1
#else
# define ALLOY_COMPILER_HAS_AVX2 0
#endif
#if (defined(__SSE__) && __SSE__) || defined(__SSE_MATH__ )
# define ALLOY_COMPILER_HAS_SSE 1
#else
# define ALLOY_COMPILER_HAS_SSE 0
#endif
#if defined(__SSE2__) || defined(__SSE2_MATH__ )
# define ALLOY_COMPILER_HAS_SSE2 1
#else
# define ALLOY_COMPILER_HAS_SSE2 0
#endif
#if defined(__SSE3__)
# define ALLOY_COMPILER_HAS_SSE3 1
#else
# define ALLOY_COMPILER_HAS_SSE3 0
#endif
#if defined(__SSE4_1__)
# define ALLOY_COMPILER_HAS_SSE4_1 1
#else
# define ALLOY_COMPILER_HAS_SSE4_1 0
#endif
#if defined(__SSE4_2__)
# define ALLOY_COMPILER_HAS_SSE4_2 1
#else
# define ALLOY_COMPILER_HAS_SSE4_2 0
#endif

//----------------------------------------------------------------------------

#if defined(__EXCEPTIONS)
# define ALLOY_COMPILER_EXCEPTIONS_ENABLED 1
#else
# define ALLOY_COMPILER_EXCEPTIONS_ENABLED 0
#endif

//----------------------------------------------------------------------------
// Symbol Import/Export (DLL/shared library)
//----------------------------------------------------------------------------

#define ALLOY_COMPILER_SYMBOL_EXPORT  __attribute__((visibility("default")))
#define ALLOY_COMPILER_SYMBOL_IMPORT
#define ALLOY_COMPILER_SYMBOL_VISIBLE __attribute__((visibility("default")))
#define ALLOY_COMPILER_SYMBOL_LOCAL   __attribute__((visibility("hidden")))

#endif /* ALLOY_CORE_INTRINSICS_CLANG_HPP */
