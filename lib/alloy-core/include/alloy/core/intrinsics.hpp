/*****************************************************************************
 * \file intrinsics.hpp
 *
 * \brief This header defines various compiler intrinsics behind macro
 *        definitions in order to provide support for systems that may not
 *        support the same functionality.
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019-2020, 2022 Matthew Rodusek All rights reserved.

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
#include <cstdint> // std::uintptr_t
#include <type_traits> // std::invoke_result_t
#include <functional>  // std::invoke

//=============================================================================
// macro definitions
//=============================================================================

//-----------------------------------------------------------------------------
// Keywords
//-----------------------------------------------------------------------------

#if defined(ALLOY_RESTRICT)
# error ALLOY_RESTRICT defined before including intrinsics.hpp
#endif
#if defined(ALLOY_NOOP)
# error ALLOY_NOOP defined before including intrinsics.hpp
#endif

//! \def ALLOY_RESTRICT
//!
//! \brief Defines a portable symbol for the C++ extension for "restrict"
//!        parameters
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_RESTRICT __restrict__
#elif defined(_MSC_VER)
# define ALLOY_RESTRICT __restrict
#else
# define ALLOY_RESTRICT
#endif

//! \def ALLOY_NOOP()
//!
//! \brief An operation that does nothing (no-op)
#define ALLOY_NOOP() ((void)0)

//-----------------------------------------------------------------------------
// Hinting / Path Analysis
//-----------------------------------------------------------------------------

#if defined(ALLOY_LIKELY)
# error ALLOY_LIKELY defined before including intrinsics.hpp
#endif
#if defined(ALLOY_UNLIKELY)
# error ALLOY_UNLIKELY defined before including intrinsics.hpp
#endif
#if defined(ALLOY_ASSUME)
# error ALLOY_ASSUME defined before including intrinsics.hpp
#endif
#if defined(ALLOY_COLD)
# error ALLOY_COLD defined before including intrinsics.hpp
#endif
#if defined(ALLOY_HOT)
# error ALLOY_HOT defined before including intrinsics.hpp
#endif
#if defined(ALLOY_UNREACHABLE)
# error ALLOY_UNREACHABLE defined before including intrinsics.hpp
#endif

//! \def ALLOY_LIKELY(x)
//!
//! \brief An intrinsic for hinting to the compiler that condition \p x will
//!        likely be taken
//!
//! \param x the condition to test
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_LIKELY(x) __builtin_expect(!!(x), 1)
#else
# define ALLOY_LIKELY(x) x
#endif

//! \def ALLOY_UNLIKELY(x)
//!
//! \brief An intrinsic for hinting to the compiler that condition \p x will
//!        unlikely be taken
//!
//! \param x the condition to test
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
# define ALLOY_UNLIKELY(x) x
#endif

//! \def ALLOY_ASSUME(x)
//!
//! \brief An intrinsic for hinting to the compiler that the condition \p x
//!        will always be true.
//!
//! A condition that violates an assumed condition will result in undefined
//! behavior
//!
//! \param x the condition
#if defined(_MSC_VER)
# define ALLOY_ASSUME(x) __assume(x)
#else
# define ALLOY_ASSUME(x) static_cast<void>(x)
#endif

//! \def ALLOY_COLD
//!
//! \brief A special attribute to indicate functions / handlers that are
//!        unlikely to be accessed frequently (the "cold" path).
//!
//! Functions documented as "cold" will be pushed to their own code section of
//! "cold" handlers on compatible compilers, and might also hint at better
//! optimizations by allowing branch-analysis to determine that conditions
//! reaching such handlers are unlikely.
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_COLD __attribute__((cold, noinline))
#elif defined(_MSC_VER)
# pragma section("cold",execute)
# define ALLOY_COLD __declspec(code_seg("cold")) __declspec(noinline)
#else
# define ALLOY_COLD
#endif

//! \def ALLOY_HOT
//!
//! \brief A special attribute to indicate functions / handlers that are
//!        likely to be accessed frequently (the "hot" path)
//!
//! Functions documented as "hot" will be more aggressively optimized for speed
//! on compatible systems
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_HOT __attribute__((hot))
#else
# define ALLOY_HOT
#endif

//! \def ALLOY_UNREACHABLE()
//!
//! \brief Signals to the compiler that the current operation is unreachable
//!
//! Using ALLOY_UNREACHABLE provides an optimization hint to the compiler, but
//! may be dangerous if used in a context that is reachable -- as this will
//! result in undefined behavior.
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
# define ALLOY_UNREACHABLE() __assume(0)
#else
# define ALLOY_UNREACHABLE() ALLOY_NOOP()
#endif

//-----------------------------------------------------------------------------
// Inlining
//-----------------------------------------------------------------------------

#if defined(ALLOY_FORCE_INLINE)
# error ALLOY_FORCE_INLINE defined before including intrinsics.hpp
#endif
#if defined(ALLOY_FORCE_INLINE_LAMBDA)
# error ALLOY_FORCE_INLINE_LAMBDA defined before including intrinsics.hpp
#endif
#if defined(ALLOY_NO_INLINE)
# error ALLOY_NO_INLINE defined before including intrinsics.hpp
#endif

//! \def ALLOY_FORCE_INLINE
//!
//! \brief An intrinsic for forcing the compiler to inline a function
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
# define ALLOY_FORCE_INLINE __forceinline
#else
# define ALLOY_FORCE_INLINE inline
#endif

//! \def ALLOY_FORCE_INLINE_LAMBDA
//!
//! \brief Attribute for marking lambdas to be forced to inline
//!
//! This is defined independently from the ALlOY_FORCE_INLINE symbol, since
//! different compilers support this through different means. MSVC requires
//! a new attribute in VS 2019 v16.7, `[[msvc::force_inline]]`, whereas
//! clang and gcc require the `__attribute__` syntax, since
//! `[[gnu::always_inline]]` cannot be applied to class types.
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_FORCE_INLINE_LAMBDA __attribute__((always_inline))
#elif defined(_MSC_VER)
# define ALLOY_FORCE_INLINE_LAMBDA // VS 2019 v16.7 is set to support [[msvc::force_inline]]
#else
# define ALLOY_FORCE_INLINE_LAMBDA
#endif

//! \def ALLOY_NO_INLINE
//!
//! \brief An intrinsic for requiring the compiler *not* to inline a function
#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
# define ALLOY_NO_INLINE __declspec(noinline)
#else
# define ALLOY_NO_INLINE
#endif

//-----------------------------------------------------------------------------
// Debugging
//-----------------------------------------------------------------------------

#if defined(ALLOY_BREAKPOINT)
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
#elif defined(__GNUC__) || defined(__clang__)
# define ALLOY_BREAKPOINT() __builtin_trap()
#elif defined(_MSC_VER)
# define ALLOY_BREAKPOINT() __debugbreak()
#else
# include <csignal>
# if defined(SIGTRAP)
#   define ALLOY_BREAKPOINT() ::std::raise(SIGTRAP)
# else
#   define ALLOY_BREAKPOINT() ::std::raise(SIGABRT)
# endif
#endif

//-----------------------------------------------------------------------------
// Export Symbols
//-----------------------------------------------------------------------------

#if defined(ALLOY_COMPILER_SYMBOL_EXPORT)
# error ALLOY_COMPILER_SYMBOL_EXPORT defined before including gcc.hpp
#endif
#if defined(ALLOY_COMPILER_SYMBOL_IMPORT)
# error ALLOY_COMPILER_SYMBOL_IMPORT defined before including gcc.hpp
#endif
#if defined(ALLOY_COMPILER_SYMBOL_VISIBLE)
# error ALLOY_COMPILER_SYMBOL_VISIBLE defined before including gcc.hpp
#endif
#if defined(ALLOY_COMPILER_SYMBOL_LOCAL)
# error ALLOY_COMPILER_SYMBOL_LOCAL defined before including gcc.hpp
#endif

#if defined(__GNUC__) || defined(__clang__)
# define ALLOY_COMPILER_SYMBOL_EXPORT  __attribute__((visibility("default")))
# define ALLOY_COMPILER_SYMBOL_IMPORT
# define ALLOY_COMPILER_SYMBOL_VISIBLE __attribute__((visibility("default")))
# define ALLOY_COMPILER_SYMBOL_LOCAL   __attribute__((visibility("hidden")))
#elif defined(_MSC_VER)
# define ALLOY_COMPILER_SYMBOL_EXPORT  __declspec((dllexport))
# define ALLOY_COMPILER_SYMBOL_IMPORT  __declspec((dllimport))
# define ALLOY_COMPILER_SYMBOL_VISIBLE
# define ALLOY_COMPILER_SYMBOL_LOCAL
#else
# define ALLOY_COMPILER_SYMBOL_EXPORT
# define ALLOY_COMPILER_SYMBOL_IMPORT
# define ALLOY_COMPILER_SYMBOL_VISIBLE
# define ALLOY_COMPILER_SYMBOL_LOCAL
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
#if defined(_MSC_VER) && !defined(__clang__)
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH() ALLOY_COMPILER_PRAGMA(warning(push))
#else
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_PUSH()
#endif

//! \def ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP()
//!
//! \brief Recovers the diagnostic state from the stack
#if defined(_MSC_VER) && !defined(__clang__)
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP() ALLOY_COMPILER_PRAGMA(warning(pop))
#else
# define ALLOY_COMPILER_MSVC_DIAGNOSTIC_POP()
#endif

//! \def ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE(flag)
//!
//! \brief Ignores the specified warning number from MSVC
//!
//! \param error the error code to ignore
#if defined(_MSC_VER) && !defined(__clang__)
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

namespace alloy::core {

  //===========================================================================
  // static class : compiler
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of compiler intrinsics wrapped into a static class
  ///
  /// It is preferable to attempt to use these function wrappers, rather than
  /// using the macros that have been defined for the same purpose. These
  /// wrappers help to prevent conditional code by defining things in a
  /// standard, 0-overhead, reusable manner.
  /////////////////////////////////////////////////////////////////////////////
  struct compiler final
  {
    compiler() = delete;
    ~compiler() = delete;

    //-------------------------------------------------------------------------
    // Conditional Paths
    //-------------------------------------------------------------------------

    /// \brief Executes conditional code that only runs in debug builds, when
    ///        the symbol NDEBUG is not present
    ///
    /// \param fn the function to execute
    /// \param args arguments to the function
    template <typename Fn, typename...Args>
    static constexpr auto debug_only(Fn&& fn, Args&&...args) -> void;

    /// \brief Executes conditional code that only runs in release builds, when
    ///        the symbol NDEBUG is present
    ///
    /// \param fn the function to execute
    /// \param args arguments to the function
    template <typename Fn, typename...Args>
    static constexpr auto release_only(Fn&& fn, Args&&...args) -> void;

    //-------------------------------------------------------------------------
    // Unused
    //-------------------------------------------------------------------------

    /// \brief A meta function to indicate to the compiler that a sequence
    ///        of types is not used
    ///
    /// This is used to suppress 'unused template argument' warnings, and
    /// to indicate to the reader that a type is intentionally unused
    template <typename...>
    static constexpr auto unused() noexcept -> void{}

    /// \brief A meta function to indicate to the compiler that a sequence
    ///        of values is unused
    ///
    /// This is used to suppress 'unused variable' warnings, and to indicate
    /// to the reader that a variable is intentionally unused
    template <typename...Types>
    static constexpr auto unused([[maybe_unused]] const Types&...) noexcept -> void{}

    /// \brief A meta function to indicate to the compiler that a static value
    ///        is unused
    ///
    /// This is used to suppress 'unused variable' warnings, and to indicate
    /// to the reader that a variable is intentionally unused
    template <auto...>
    static constexpr auto unused() noexcept -> void{}

    //-------------------------------------------------------------------------
    // Assumptions
    //-------------------------------------------------------------------------

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
    static constexpr auto assume_aligned(T* p) noexcept -> T*;

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
#if defined(__GNUC__) || defined(__clang__)
    [[gnu::returns_nonnull]]
#endif
    static constexpr auto assume_not_null(T* p) noexcept -> T*;

    /// \brief Assumes that a given function will be a cold path
    ///
    /// Cold functions will attempt to be optimized for size, rather than speed,
    /// and will not be inlined into the current context. For supported systems,
    /// this may also hint to the optimzer that branches leading to the cold
    /// handler will be marked "unlikely" for prediction purposes.
    ///
    /// \note `assume_cold` is not marked `constexpr` like the `assume_hot`
    ///       counterpart, since `constexpr` implies `noinline`, but `noinline`
    ///       conflicts with `inline` declarations (despite `inline` actually
    ///       referring to linker deduping and not true "inlining")
    ///
    /// \param fn the cold function to invoke
    /// \param args the arguments to pass to the function
    /// \return the result from \p fn
    template <typename Fn, typename...Args>
    static auto assume_cold(Fn&& fn, Args&&...args)
      noexcept(std::is_nothrow_invocable<Fn, Args...>::value)
      -> std::invoke_result_t<Fn,Args...>;

    /// \brief Assumes that a given function will be a hot path
    ///
    /// Hot functions will try to ensure the path is optimized more aggressively
    /// on systems that support it (GCC, clang), and will hint to the optimizer
    /// that any branches leading to the hot branch will be predicted more
    /// accurately.
    ///
    /// \param fn the cold function to invoke
    /// \param args the arguments to pass to the function
    /// \return the result from \p fn
    template <typename Fn, typename...Args>
    static constexpr auto assume_hot(Fn&& fn, Args&&...args)
      noexcept(std::is_nothrow_invocable<Fn, Args...>::value)
      -> std::invoke_result_t<Fn,Args...>;

    /// \brief Provides a hint to the compiler that the code path after this
    ///        function is not reachable
    [[noreturn]]
    static auto unreachable() noexcept -> void;

    //-------------------------------------------------------------------------
    // Debugging
    //-------------------------------------------------------------------------

    /// \brief Produces a breakpoint and crashes the application when invoked
    [[noreturn]]
    static auto breakpoint() noexcept -> void;
  };

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Conditional Paths
//-----------------------------------------------------------------------------

template <typename Fn, typename...Args>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compiler::debug_only(Fn&& fn, Args&&...args)
  -> void
{
#if !defined(NDEBUG)
  std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
#else
  compiler::unused(fn, args...);
#endif
}
template <typename Fn, typename...Args>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compiler::release_only(Fn&& fn, Args&&...args)
  -> void
{
#if defined(NDEBUG)
  std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
#else
  compiler::unused(fn, args...);
#endif
}

//-----------------------------------------------------------------------------
// Assumptions
//-----------------------------------------------------------------------------

template <std::size_t N, typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compiler::assume_aligned(T* p)
  noexcept -> T*
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

template <typename T>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compiler::assume_not_null(T* p)
  noexcept -> T*
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

//-----------------------------------------------------------------------------
// Branch Prediction
//-----------------------------------------------------------------------------

template <typename Fn, typename...Args>
ALLOY_COLD
auto alloy::core::compiler::assume_cold(Fn&& fn, Args&&...args)
  noexcept(std::is_nothrow_invocable<Fn, Args...>::value)
  -> std::invoke_result_t<Fn, Args...>
{
  return std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
}

template <typename Fn, typename...Args>
ALLOY_HOT ALLOY_FORCE_INLINE constexpr
auto alloy::core::compiler::assume_hot(Fn&& fn, Args&&...args)
  noexcept(std::is_nothrow_invocable<Fn, Args...>::value)
  -> std::invoke_result_t<Fn, Args...>
{
  return std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
}

ALLOY_FORCE_INLINE
auto alloy::core::compiler::unreachable()
  noexcept -> void
{
  ALLOY_UNREACHABLE();
}

//-----------------------------------------------------------------------------
// Debugging
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::compiler::breakpoint()
  noexcept -> void
{
  ALLOY_BREAKPOINT();

  // Breakpoints should not even exist in production code.
  // Hopefully crashing will get someone's attention
  std::abort();
}

#endif /* ALLOY_CORE_INTRINSICS_HPP */
