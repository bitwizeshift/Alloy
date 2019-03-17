/*****************************************************************************
 * \file gcc.hpp
 * \brief This header detects compiler-specific features for Clang c++
 *
 * \note This is an internal header file, included by other library headers.
 *       Do not attempt to use it directly.
 *****************************************************************************/

/*
  The MIT License (MIT)

  Bit Standard Template Library.
  https://github.com/bitwizeshift/bit-stl

  Copyright (c) 2018 Matthew Rodusek

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
#ifndef ALLOY_CORE_INTRINSICS_GCC_HPP
#define ALLOY_CORE_INTRINSICS_GCC_HPP

#ifndef __GNUC__
# error This header can only be included when compiling with gcc compiler
#endif

#include "alloy/core/macros.hpp" // ALLOY_STRINGIZE

//------------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_APPLE_GCC
# error ALLOY_COMPILER_APPLE_GCC defined before including gcc.hpp
#endif

#define ALLOY_COMPILER_GCC 1
#define ALLOY_COMPILER_NAME "msvc"
#define ALLOY_COMPILER_VERSION_STRING \
  ALLOY_COMPILER_NAME " " \
  ALLOY_STRINGIZE(__GNUC__) "." \
  ALLOY_STRINGIZE(__GNUC_MINOR__) "." \
  ALLOY_STRINGIZE(__GNUC_PATCHLEVEL__)
#define ALLOY_COMPILER_VERSION __GNUC__ * 1000000 + __GNUC_MINOR__ * 1000 + __GNUC_PATCHLEVEL__
#define ALLOY_COMPILER_VERSION_MAJOR __GNUC__
#define ALLOY_COMPILER_VERSION_MINOR __GNUC_MINOR__
#define ALLOY_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__

//-----------------------------------------------------------------------------

#define ALLOY_COMPILER_ASM_SYNTAX ALLOY_COMPILER_ASM_SYNTAX_ATT

//-----------------------------------------------------------------------------

#define ALLOY_COMPILER_BREAKPOINT __builtin_trap()

//-----------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_WEAK_SUPPORTED
# error ALLOY_COMPILER_WEAK_SUPPORTED defined before including gcc.hpp
#endif

#define ALLOY_COMPILER_WEAK_SUPPORTED 1

#define ALLOY_WEAK __attribute__((weak))

//-----------------------------------------------------------------------------

#define ALLOY_RESTRICT __restrict__
#define ALLOY_LIKELY(x) __builtin_expect(!!(x), 1)
#define ALLOY_UNLIKELY(x) __builtin_expect(!!(x), 0)
#if (__GNUC__ >= 4)
# define ALLOY_FORCE_INLINE __attribute__((always_inline)) inline
# define ALLOY_NO_INLINE __attribute__((__noinline__))
#else
# define ALLOY_FORCE_INLINE inline
# define ALLOY_NO_INLINE __attribute__((__noinline__))
#endif
#if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)))
# define ALLOY_COMPILER_UNREACHABLE __builtin_unreachable()
#else
# define ALLOY_COMPILER_UNREACHABLE
#endif

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

#endif /* ALLOY_CORE_INTRINSICS_GCC_HPP */
