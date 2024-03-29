/*****************************************************************************
 * \file assert.hpp
 *
 * \brief This header defines the assert macros used throughout the engine.
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019-2021 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_ASSERT_HPP
#define ALLOY_CORE_ASSERT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/config.hpp"
#include "alloy/core/macros.hpp"
#include "alloy/core/intrinsics.hpp"

#include <cstddef>     // std::size_t
#include <type_traits> // std::is_constructible

#ifdef ALLOY_ASSERT
# error ALLOY_ASSERT defined before assert.hpp included
#endif

#ifdef ALLOY_ALWAYS_ASSERT
# error ALLOY_ALWAYS_ASSERT defined before assert.hpp included
#endif

#ifdef ALLOY_ASSERT_AND_ASSUME
# error ALLOY_ASSERT_AND_ASSSUME defined before assert.hpp included
#endif

#ifdef ALLOY_THROW_IF
# error ALLOY_THROW_IF defined before assert.hpp included
#endif

#ifdef ALLOY_ALWAYS_ASSERT
# error ALLOY_ALWAYS_ASSERT defined before assert.hpp included
#endif

#define ALLOY_INTERNAL_ASSERT_1(condition)                                    \
  ALLOY_INTERNAL_ASSERT_2(condition,nullptr)

#define ALLOY_INTERNAL_ASSERT_2(condition,message)                            \
  ((ALLOY_LIKELY(condition))                                                  \
    ? ((void)0)                                                               \
    : [](const char* file_name,                                               \
         int line_number,                                                     \
         const char* function_name)                                           \
        ALLOY_FORCE_INLINE_LAMBDA                                             \
      {                                                                       \
        ::alloy::core::detail::assert_internal(                               \
          ALLOY_STRINGIZE(condition),                                         \
          message,                                                            \
          file_name,                                                          \
          static_cast<std::size_t>(line_number),                              \
          function_name                                                       \
        );                                                                    \
      }( __FILE__, __LINE__, __func__ ) )

//! \def ALLOY_ASSERT(condition, message)
//!
//! \brief A runtime assertion when \a condition fails, displaying \a message
//!        to the user.
//!
//! An assertion will report the error, the source location of the error,
//! and trigger a breakpoint (if the debugger is open). After the breakpoint
//! trigger, it invokes std::terminate.
//!
//! \param condition the condition that, when false, triggers an assertion
//! \param message   the message for the failure [optional]
#define ALLOY_ALWAYS_ASSERT(...) \
  ALLOY_JOIN( ALLOY_INTERNAL_ASSERT_, ALLOY_COUNT_VA_ARGS(__VA_ARGS__) )(__VA_ARGS__)

//! \def ALLOY_ASSERT(condition, message, ...)
//!
//! \brief A runtime assertion when \a condition fails, displaying \a message
//!        to the user.
//!
//! This simply calls 'ALLOY_ALWAYS_ASSERT' when assertions are enabled
//!
//! \param condition the condition that, when false, triggers an assertion
//! \param message   the message for the failure [optional]
#if !defined(NDEBUG)
# define ALLOY_ASSERT(...) ALLOY_ALWAYS_ASSERT(__VA_ARGS__)
#else
# define ALLOY_ASSERT(...) (void) 0
#endif

//! \def ALLOY_ASSERT_AND_ASSUME(condition, message, ...)
//!
//! \brief Like \ref ALLOY_ASSERT, asserts a precondition when in debug. In
//!        release, it translates into a compiler 'assume' intrinsic to
//!        optimize the path away.
//!
//! \param condition the condition that, when false, triggers an assertion
#if !defined(NDEBUG)
# define ALLOY_ASSERT_AND_ASSUME(cond) ALLOY_ASSERT(cond)
#else
# define ALLOY_ASSERT_AND_ASSUME(cond) ALLOY_ASSUME(cond)
#endif

//! \def ALLOY_ASSERT_OR_THROW(condition, exception, message)
//!
//! \brief An assertion that either throws the given exception, if exceptions
//!        are enabled, or calls the default assert
//!
//! \param condition the condition that, when false, triggers either an
//!                  assertion or throws an exception
//! \param exception the exception to throw
//! \param message   the message for the failure
#if ALLOY_CORE_EXCEPTIONS_ENABLED
# define ALLOY_THROW_IF(condition,exception,message)                           \
  static_assert(                                                               \
    std::is_constructible<exception,decltype(message)>::value,                 \
    ALLOY_STRINGIZE(exception) " is not constructible from the given message"  \
  );                                                                           \
  ((ALLOY_LIKELY(condition))                                                   \
    ? ((void)0)                                                                \
    : []{ throw exception{ message }; }() /* NOLINT */                         \
  )
#else
# define ALLOY_THROW_IF(condition,exception,message) \
  static_assert( std::is_constructible<exception,decltype(message)>::value, \
                 ALLOY_STRINGIZE(exception) " is not constructible from the given message" ); \
  ALLOY_ASSERT(condition,message)
#endif

namespace alloy::core {

  /// \brief A function type that is capable of handling assertions
  ///
  /// Assertion handlers are required to be marked `noexcept` since assertions
  /// are never meant to be testable cases; they are meant to always indicate
  /// program invariants, and nothing more.
  ///
  /// Assertion handlers are meant to be user-configurable functions that may
  /// provide more input to the failure, such as providing a stack-trace,
  /// writing out to a log file, etc.
  using assert_handler_fn = void(const char*) noexcept;

  /// \brief Sets a user-configurable handler for behaviors to connect on
  ///        assertion failures
  ///
  /// If \p handler is `nullptr`, the default assertion handler is installed
  ///
  /// \param handler the handler to install
  /// \return the previously installed handler
  ALLOY_CORE_API
  assert_handler_fn* set_assert_handler(assert_handler_fn* handler) noexcept;

  namespace detail {

    [[noreturn]] ALLOY_COLD ALLOY_CORE_API
    void assert_internal(const char* condition,
                         const char* message,
                         const char* file_name,
                         std::size_t line,
                         const char* function_name) noexcept;
  } // namespace detail
} // namespace alloy::core

#endif /* ALLOY_CORE_ASSERT_HPP */
