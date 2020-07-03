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
#include "alloy/core/macros.hpp"

#include <cstdlib> // std::abort

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
// Pragma
//------------------------------------------------------------------------------

#if defined(ALLOY_COMPILER_PRAGMA)
# error ALLOY_COMPILER_PRAGMA defined before including intrinsics.hpp
#endif

//! \def ALLOY_COMPILER_PRAGMA(x)
//!
//! \brief Converts any input \p x into a single pragma
//!
//! \param x the pragma input
#define ALLOY_COMPILER_PRAGMA(x) _Pragma(#x)

//------------------------------------------------------------------------------
// clang : Pragma
//------------------------------------------------------------------------------

#if defined(ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH)
# error ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH defined before including intrunsics.hpp
#endif
#if defined(ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP)
# error ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP defined before including intrunsics.hpp
#endif
#if defined(ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE)
# error ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE defined before including intrunsics.hpp
#endif

//------------------------------------------------------------------------------

//! \def ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH()
//!
//! \brief Preserves the current diagnostics state for the clang compiler on the
//!        stack, which can be undone by ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()
#if defined(__clang__)
# define ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH() ALLOY_COMPILER_PRAGMA(clang diagnostic push)
#else
# define ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH()
#endif

//! \def ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()
//!
//! \brief Recovers the diagnostic state from the stack
#if defined(__clang__)
# define ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP() ALLOY_COMPILER_PRAGMA(clang diagnostic pop)
#else
# define ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()
#endif

//! \def ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(flag)
//!
//! \brief Ignores the specified warning flag to prevent errors
//!
//! \param flag the flag to ignore
#if defined(__clang__)
# define ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(flag) ALLOY_COMPILER_PRAGMA(clang diagnostic ignored #flag)
#else
# define ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(flag)
#endif

//------------------------------------------------------------------------------
// GCC : Pragma
//------------------------------------------------------------------------------

#if defined(ALLOY_COMPILER_GCC_DIAGNOSTIC_PUSH)
# error ALLOY_COMPILER_GCC_DIAGNOSTIC_PUSH defined before including intrunsics.hpp
#endif
#if defined(ALLOY_COMPILER_GCC_DIAGNOSTIC_POP)
# error ALLOY_COMPILER_GCC_DIAGNOSTIC_POP defined before including intrunsics.hpp
#endif
#if defined(ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE)
# error ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE defined before including intrunsics.hpp
#endif

//------------------------------------------------------------------------------

//! \def ALLOY_COMPILER_GCC_DIAGNOSTIC_PUSH()
//!
//! \brief Preserves the current diagnostics state for the gcc compiler on the
//!        stack, which can be undone by ALLOY_COMPILER_GCC_DIAGNOSTIC_POP()
#if defined(__GNUC__) && !defined(__clang__)
# define ALLOY_COMPILER_GCC_DIAGNOSTIC_PUSH() ALLOY_COMPILER_PRAGMA(GCC diagnostic push)
#else
# define ALLOY_COMPILER_GCC_DIAGNOSTIC_PUSH()
#endif

//! \def ALLOY_COMPILER_GCC_DIAGNOSTIC_POP()
//!
//! \brief Recovers the diagnostic state from the stack
#if defined(__GNUC__) && !defined(__clang__)
# define ALLOY_COMPILER_GCC_DIAGNOSTIC_POP() ALLOY_COMPILER_PRAGMA(GCC diagnostic pop)
#else
# define ALLOY_COMPILER_GCC_DIAGNOSTIC_POP()
#endif

//! \def ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE(flag)
//!
//! \brief Ignores the specified warning flag to prevent errors
//!
//! \param flag the flag to ignore
#if defined(__GNUC__) && !defined(__clang__)
# define ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE(flag) ALLOY_COMPILER_PRAGMA(GCC diagnostic ignored #flag)
#else
# define ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE(flag)
#endif

//------------------------------------------------------------------------------
// MSVC : Pragma
//------------------------------------------------------------------------------

#if defined(ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH)
# error ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH defined before including intrunsics.hpp
#endif
#if defined(ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP)
# error ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP defined before including intrunsics.hpp
#endif
#if defined(ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE)
# error ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE defined before including intrunsics.hpp
#endif

//------------------------------------------------------------------------------

//! \def ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH()
//!
//! \brief Preserves the current diagnostics state for the msvc compiler on the
//!        stack, which can be undone by ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP()
#if defined(_MSVC) && !defined(__clang__)
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH() ALLOY_COMPILER_PRAGMA(warning(push))
#else
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH()
#endif

//! \def ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP()
//!
//! \brief Recovers the diagnostic state from the stack
#if defined(_MSVC) && !defined(__clang__)
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP() ALLOY_COMPILER_PRAGMA(warning(pop))
#else
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP()
#endif

//! \def ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE(flag)
//!
//! \brief Ignores the specified warning number from MSVC
//!
//! \param error the error code to ignore
#if defined(_MSVC) && !defined(__clang__)
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE(error) ALLOY_COMPILER_PRAGMA(warning(disable: error))
#else
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE(error)
#endif

//------------------------------------------------------------------------------
// Generic : Pragma
//------------------------------------------------------------------------------

#if defined(ALLOY_COMPILER_DIAGNOSTIC_PUSH)
# error ALLOY_COMPILER_DIAGNOSTIC_PUSH defined before including intrinsics.hpp
#endif
#if defined(ALLOY_COMPILER_DIAGNOSTIC_POP)
# error ALLOY_COMPILER_DIAGNOSTIC_POP defined before including intrinsics.hpp
#endif
#if defined(ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE)
# error ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE defined before including intrinsics.hpp
#endif

//------------------------------------------------------------------------------

//! \def ALLOY_COMPILER_DIAGNOSTIC_PUSH()
//!
//! \brief Preserves the current diagnostics state for the current compiler on
//!        the stack, which can be undone by ALLOY_COMPILER_DIAGNOSTIC_PUSH()
#if defined(__clang__)
# define ALLOY_COMPILER_DIAGNOSTIC_PUSH() ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH()
#elif defined(__GNUC__)
# define ALLOY_COMPILER_DIAGNOSTIC_PUSH() ALLOY_COMPILER_GCC_DIAGNOSTIC_PUSH()
#elif defined(_MSC_VER)
# define ALLOY_COMPILER_DIAGNOSTIC_PUSH() ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH()
#else
# define ALLOY_COMPILER_DIAGNOSTIC_PUSH()
#endif

//! \def ALLOY_COMPILER_DIAGNOSTIC_POP()
//!
//! \brief Recovers the diagnostic state from the stack
#if defined(__clang__)
# define ALLOY_COMPILER_DIAGNOSTIC_POP() ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()
#elif defined(__GNUC__)
# define ALLOY_COMPILER_DIAGNOSTIC_POP() ALLOY_COMPILER_GCC_DIAGNOSTIC_POP()
#elif defined(_MSC_VER)
# define ALLOY_COMPILER_DIAGNOSTIC_POP() ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP()
#else
# define ALLOY_COMPILER_DIAGNOSTIC_POP()
#endif

//! \def ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(flag)
//!
//! \brief Ignores the specified warning number from gnu-like compilers like
//!        clang and gcc
//!
//! \param error the error code to ignore
#if defined(__clang__)
# define ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(flag) ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(flag)
#elif defined(__GNUC__)
# define ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(flag) ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE(flag)
#else
# define ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(flag)
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

namespace alloy::core {

  struct compiler final
  {
    compiler() = delete;
    ~compiler() = delete;

    /// \brief A meta function to indicate to the compiler that a sequence
    ///        of types is not used
    ///
    /// This is used to suppress 'unused template argument' warnings, and
    /// to indicate to the reader that a type is intentionally unused
    template <typename...>
    static constexpr void unused() noexcept{}

    /// \brief A meta function to indicate to the compiler that a sequence
    ///        of values is unused
    ///
    /// This is used to suppress 'unused variable' warnings, and to indicate
    /// to the reader that a variable is intentionally unused
    template <typename...Types>
    static constexpr void unused(const Types&...) noexcept{}

    /// \brief A meta function to indicate to the compiler that a static value
    ///        is unused
    ///
    /// This is used to suppress 'unused variable' warnings, and to indicate
    /// to the reader that a variable is intentionally unused
    template <auto>
    static constexpr void unused() noexcept{}

    /// \brief Provides a hint to the compiler that the pointer \p p is aligned
    ///        to the \p N boundary
    ///
    /// \note The returned pointer is the only pointer known to the compiler to
    ///       be aligned; the input should be discarded.
    ///
    /// \tparam N the alignment boundary of the pointer
    /// \param p the pointer to indicate alignment of
    /// \return a pointer that the compiler knows is aligned
    template <std::size_t N, typename T>
    [[nodiscard]]
    static constexpr T* assume_aligned(T* p) noexcept;

    /// \brief Provides a hint to the compiler that the pointer \p p is not null
    ///
    /// \note The returned pointer is the only pointer known to the compiler to
    ///        be not-null; the input should be discarded
    ///
    /// This may help to avoid certain generated assemblies which make
    /// assumptions on the result of a pointer being null by allowing any
    /// related code paths to be completely optimized out.
    ///
    /// \param p the pointer to indicate is not-null
    /// \return a pointer that the compiler knows is not null
    template <typename T>
    [[gnu::returns_nonnull]]
    static constexpr T* assume_not_null(T* p) noexcept;

    /// \brief Provides a hint to the compiler that the code path after this
    ///        function is not reachable
    [[noreturn]]
    static void unreachable() noexcept;

    /// \brief Produces a breakpoint and crashes the application when invoked
    [[noreturn]]
    static void breakpoint() noexcept;
  };

} // namespace alloy::core

template <std::size_t N, typename T>
ALLOY_FORCE_INLINE constexpr T* alloy::core::compiler::assume_aligned(T* p)
  noexcept
{
#if defined(__clang__) || (defined(__GNUC__) && !defined(__ICC))
  return static_cast<T*>(__builtin_assume_aligned(p, N));
#elif defined(_MSC_VER)
  if ((reinterpret_cast<std::uintptr_t>(p) & ((1 << N) - 1)) == 0) {
    return p;
  } else {
    __assume(0);
  }
#elif defined(__ICC)
  switch (N) {
    case 2:
      __assume_aligned(p, 2);
      break;
    case 4:
      __assume_aligned(p, 4);
      break;
    case 8:
      __assume_aligned(p, 8);
      break;
    case 16:
      __assume_aligned(p, 16);
      break;
    case 32:
      __assume_aligned(p, 32);
      break;
    case 64:
      __assume_aligned(p, 64);
      break;
    case 128:
      __assume_aligned(p, 128);
      break;
  }
  return p;
#else // Unknown compiler — do nothing
  return p;
#endif
}

template<typename T>
ALLOY_FORCE_INLINE constexpr T* alloy::core::compiler::assume_not_null(T* p)
  noexcept
{
#if defined(_MSC_VER)
  if (p != nullptr) {
    return p;
  } else {
    __assume(0);
  }
#else
  // For gcc / clang, [[gnu::returns_nonnull]] upholds the non-nullability
  // invariant
  return p;
#endif
}


ALLOY_FORCE_INLINE void alloy::core::compiler::unreachable()
  noexcept
{
  ALLOY_UNREACHABLE();
}

ALLOY_FORCE_INLINE void alloy::core::compiler::breakpoint()
  noexcept
{
  ALLOY_BREAKPOINT();

  // Breakpoints should not even exist in production code.
  // Hopefully crashing will get someone's attention
  std::abort();
}

#endif /* ALLOY_CORE_INTRINSICS_HPP */
