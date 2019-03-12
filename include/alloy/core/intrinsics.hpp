/*****************************************************************************
 * \file intrinsics.hpp
 *
 * \brief This header defines various compiler intrinsics behind macro
 *        definitions in order to provide support for systems that may not
 *        support the same functionality.
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_INTRINSICS_HPP
#define ALLOY_CORE_INTRINSICS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/config.hpp"

// Macros defined in this header:

// compiler details
#ifdef ALLOY_COMPILER_NAME
# error ALLOY_COMPILER_NAME defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_VERSION_STRING
# error ALLOY_COMPILER_VERSION_STRING defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_VERSION
# error ALLOY_COMPILER_VERSION defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_VERSION_MAJOR
# error ALLOY_COMPILER_VERSION_MAJOR defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_VERSION_MINOR
# error ALLOY_COMPILER_VERSION_MINOR defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_VERSION_PATCH
# error ALLOY_COMPILER_VERSION_PATCH defined before including intrinsics.hpp
#endif

// compiler intrinsics
#ifdef ALLOY_WEAK
# error ALLOY_WEAK defined before including intrinsics.hpp
#endif
#ifdef ALLOY_RESTRICT
# error ALLOY_RESTRICT defined before including intrinsics.hpp
#endif
#ifdef ALLOY_LIKELY
# error ALLOY_LIKELY defined before including intrinsics.hpp
#endif
#ifdef ALLOY_UNLIKELY
# error ALLOY_UNLIKELY defined before including intrinsics.hpp
#endif
#ifdef ALLOY_FORCE_INLINE
# error ALLOY_FORCE_INLINE defined before including intrinsics.hpp
#endif
#ifdef ALLOY_NO_INLINE
# error ALLOY_NO_INLINE defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_UNREACHABLE
# error ALLOY_COMPILER_UNREACHABLE defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_BREAKPOINT
# error ALLOY_COMPILER_BREAKPOINT defined before including intrinsics.hpp
#endif

// extensions
#ifdef ALLOY_COMPILER_HAS_AVX
# error ALLOY_COMPILER_HAS_AVX defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_HAS_AVX2
# error ALLOY_COMPILER_HAS_AVX2 defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_HAS_SSE
# error ALLOY_COMPILER_HAS_SSE defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_HAS_SSE2
# error ALLOY_COMPILER_HAS_SSE2 defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_HAS_SSE3
# error ALLOY_COMPILER_HAS_SSE3 defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_HAS_SSE4_1
# error ALLOY_COMPILER_HAS_SSE4_1 defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_HAS_SSE4_2
# error ALLOY_COMPILER_HAS_SSE4_2 defined before including intrinsics.hpp
#endif

// Export Symbols
#ifdef ALLOY_COMPILER_SYMBOL_EXPORT
# error ALLOY_COMPILER_SYMBOL_EXPORT defined before including gcc.hpp
#endif
#ifdef ALLOY_COMPILER_SYMBOL_IMPORT
# error ALLOY_COMPILER_SYMBOL_IMPORT defined before including gcc.hpp
#endif
#ifdef ALLOY_COMPILER_SYMBOL_VISIBLE
# error ALLOY_COMPILER_SYMBOL_VISIBLE defined before including gcc.hpp
#endif
#ifdef ALLOY_COMPILER_SYMBOL_LOCAL
# error ALLOY_COMPILER_SYMBOL_LOCAL defined before including gcc.hpp
#endif

// enumeration of ASM syntaxes
#ifdef ALLOY_COMPILER_ASM_SYNTAX_INTEL
# error ALLOY_COMPILER_ASM_SYNTAX_INTEL defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_ASM_SYNTAX_ATT
# error ALLOY_COMPILER_ASM_SYNTAX_ATT defined before including intrinsics.hpp
#endif
#ifdef ALLOY_COMPILER_ASM_SYNTAX_UNKNOWN
# error ALLOY_COMPILER_ASM_SYNTAX_UNKNOWN defined before including intrinsics.hpp
#endif

#define ALLOY_COMPILER_ASM_SYNTAX_INTEL    0
#define ALLOY_COMPILER_ASM_SYNTAX_ATT      1
#define ALLOY_COMPILER_ASM_SYNTAX_UNKNOWN -1

#if defined(__clang__)
# include "intrinsics/clang.hpp"
#elif defined(__GNUC__)
# include "intrinsics/gcc.hpp"
#elif defined(_MSC_VER)
# include "intrinsics/msvc.hpp"
#else
# error Unknown compiler
#endif

//------------------------------------------------------------------------------

#ifdef ALLOY_NOOP
# error ALLOY_NOOP defined before including intrinsics.hpp
#endif

//! \def ALLOY_NOOP()
//!
//! \brief An operation that does nothing (no-op)
#define ALLOY_NOOP() ((void)0)

//------------------------------------------------------------------------------

#ifdef ALLOY_UNREACHABLE
# error ALLOY_UNREACHABLE defined before including intrinsics.hpp
#endif

//! \def ALLOY_UNREACHABLE()
//!
//! \brief Signals to the compiler that the current operation is unreachable
//!
//! Using ALLOY_UNREACHABLE provides an optimization hint to the compiler, but
//! may be dangerous if used in a context that is reachable -- as this will
//! result in undefined behavior.
#if !defined(ALLOY_COMPILER_UNREACHABLE)
# define ALLOY_UNREACHABLE() ALLOY_NOOP()
#else
# define ALLOY_UNREACHABLE() ALLOY_COMPILER_UNREACHABLE
#endif

//------------------------------------------------------------------------------

#ifdef ALLOY_BREAKPOINT
# error ALLOY_BREAKPOINT defined before including intrinsics.hpp
#endif

//! \def ALLOY_BREAKPOINT()
//!
//! \brief Create breakpoint to halt code execution.
//!
//! This symbol can be redefined by the user by defining it before including any
//! library functions. This allows breakpoints to function differently if a user
//! specifies it to, and provides easier portability to other systems.
#if defined(NDEBUG)
# define ALLOY_BREAKPOINT() ALLOY_NOOP()
#elif defined(ALLOY_COMPILER_BREAKPOINT)
# define ALLOY_BREAKPOINT() ALLOY_COMPILER_BREAKPOINT
#else
# include <csignal>
# if defined(SIGTRAP)
#   define ALLOY_BREAKPOINT() ::std::raise(SIGTRAP)
# else
#   define ALLOY_BREAKPOINT() ::std::raise(SIGABRT)
# endif
#endif

#endif /* ALLOY_CORE_INTRINSICS_HPP */