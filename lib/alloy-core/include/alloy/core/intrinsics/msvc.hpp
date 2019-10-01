/*****************************************************************************
 * \file msvc.hpp
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
#ifndef ALLOY_CORE_INTRINSICS_MSVC_HPP
#define ALLOY_CORE_INTRINSICS_MSVC_HPP

#ifndef _MSC_VER
# error This header can only be included when compiling with msvc compiler
#endif

#include "alloy/core/macros.hpp" // ALLOY_STRINGIZE

//------------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_APPLE_MSVC
# error ALLOY_COMPILER_APPLE_MSVC defined before including clang.hpp
#endif

#define ALLOY_COMPILER_MSVC 1
#define ALLOY_COMPILER_NAME "Visual Studios"
#define ALLOY_COMPILER_VERSION_STRING \
  ALLOY_COMPILER_NAME " " ALLOY_STRINGIZE(_MSC_FULL_VER)
#define ALLOY_COMPILER_VERSION _MSC_FULL_VER
#define ALLOY_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
#define ALLOY_COMPILER_VERSION_MINOR (_MSC_VER % 100)
#define ALLOY_COMPILER_VERSION_PATCH (_MSC_FULL_VER - (_MSC_VER * 10000))

//-----------------------------------------------------------------------------

#define ALLOY_COMPILER_ASM_SYNTAX ALLOY_COMPILER_ASM_SYNTAX_INTEL

//-----------------------------------------------------------------------------

#define ALLOY_COMPILER_BREAKPOINT __debugbreak()

//-----------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_WEAK_SUPPORTED
# error ALLOY_COMPILER_WEAK_SUPPORTED defined before including clang.hpp
#endif

#if (_MSC_VER >= 1300)
#  define ALLOY_WEAK __declspec(selectany)
#  define ALLOY_COMPILER_WEAK_SUPPORTED 1
#else
#  define ALLOY_WEAK
#  define ALLOY_COMPILER_WEAK_SUPPORTED 0
#endif

#define ALLOY_RESTRICT __restrict

#define ALLOY_ASSUME(x) __assume(x)
#define ALLOY_LIKELY(x) x
#define ALLOY_UNLIKELY(x) x
#define ALLOY_FORCE_INLINE __forceinline
#define ALLOY_NO_INLINE __declspec(noinline)
#define ALLOY_COMPILER_UNREACHABLE __assume(0)

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

// MSVC doesn't define __SSE__ macros, so we have to check
// the _M_IX86_FP values as defined here:
// https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
#if defined(_M_IX86_FP)
# if _M_IX86_FP == 1 || _M_IX86_FP == 2
#   define ALLOY_COMPILER_HAS_SSE 1
# else
#   define ALLOY_COMPILER_HAS_SSE 0
# endif
# if _M_IX86_FP == 2
#   define ALLOY_COMPILER_HAS_SSE2 1
# else
#   define ALLOY_COMPILER_HAS_SSE2 0
# endif
# define ALLOY_COMPILER_HAS_SSE3 0
# define ALLOY_COMPILER_HAS_SSE4_1 0
# define ALLOY_COMPILER_HAS_SSE4_2 0
#else
# define ALLOY_COMPILER_HAS_SSE 0
# define ALLOY_COMPILER_HAS_SSE2 0
# define ALLOY_COMPILER_HAS_SSE3 0
# define ALLOY_COMPILER_HAS_SSE4_1 0
# define ALLOY_COMPILER_HAS_SSE4_2 0
#endif

//----------------------------------------------------------------------------

#if defined(_CPPUNWIND)
# define ALLOY_COMPILER_EXCEPTIONS_ENABLED 1
#else
# define ALLOY_COMPILER_EXCEPTIONS_ENABLED 0
#endif

//----------------------------------------------------------------------------
// Symbol Import/Export (DLL/shared library)
//----------------------------------------------------------------------------

#ifdef ALLOY_COMPILER_SYMBOL_EXPORT
# error ALLOY_COMPILER_SYMBOL_EXPORT defined before including clang.hpp
#endif
#ifdef ALLOY_COMPILER_SYMBOL_IMPORT
# error ALLOY_COMPILER_SYMBOL_IMPORT defined before including clang.hpp
#endif
#ifdef ALLOY_COMPILER_SYMBOL_VISIBLE
# error ALLOY_COMPILER_SYMBOL_VISIBLE defined before including clang.hpp
#endif
#ifdef ALLOY_COMPILER_SYMBOL_LOCAL
# error ALLOY_COMPILER_SYMBOL_LOCAL defined before including clang.hpp
#endif


// MSVC always uses declspec
#define ALLOY_COMPILER_SYMBOL_EXPORT  __declspec((dllexport))
#define ALLOY_COMPILER_SYMBOL_IMPORT  __declspec((dllimport))
#define ALLOY_COMPILER_SYMBOL_VISIBLE
#define ALLOY_COMPILER_SYMBOL_LOCAL

#endif /* ALLOY_CORE_INTRINSICS_MSVC_HPP */
