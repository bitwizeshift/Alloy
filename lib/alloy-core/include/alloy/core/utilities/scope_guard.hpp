/*****************************************************************************
 * \file scope_guard.hpp
 *
 * \brief This header defines a comparator for piecewise-based comparisons
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
#ifndef ALLOY_CORE_UTILITIES_SCOPE_GUARD_HPP
#define ALLOY_CORE_UTILITIES_SCOPE_GUARD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/config.hpp" // ALLOY_CORE_EXCEPTIONS_ENABLED
#include "alloy/core/macros.hpp" // ALLOY_UNIQUE_NAME

#include <exception>   // std::uncaught_exceptions
#include <utility>     // std::move
#include <type_traits> // std::is_nothrow_move_constructible, std::decay_t
#include <limits>

namespace alloy::core {
  namespace detail {

    //==========================================================================
    // internal class : on_exit_policy
    //==========================================================================

    class on_exit_policy
    {
    public:
      on_exit_policy() noexcept
        : m_should_execute{true}
      {

      }
      on_exit_policy(on_exit_policy&&) = default;
      on_exit_policy(const on_exit_policy&) = default;
      on_exit_policy& operator=(on_exit_policy&&) = default;
      on_exit_policy& operator=(const on_exit_policy&) = default;

      void release() noexcept
      {
        m_should_execute = false;
      }

      bool should_execute() const noexcept
      {
        return m_should_execute;
      }

    private:

      bool m_should_execute;
    };

    //==========================================================================
    // internal class : on_fail_policy
    //==========================================================================

    class on_fail_policy
    {
    public:
      on_fail_policy() noexcept
        : m_exception_count{std::uncaught_exceptions()}
      {

      }
      on_fail_policy(on_fail_policy&&) = default;
      on_fail_policy(const on_fail_policy&) = default;
      on_fail_policy& operator=(on_fail_policy&&) = default;
      on_fail_policy& operator=(const on_fail_policy&) = default;

      void release() noexcept
      {
        m_exception_count = std::numeric_limits<int>::max();
      }

      bool should_execute() const noexcept
      {
        return m_exception_count < std::uncaught_exceptions();
      }

    private:

      int m_exception_count;
    };

    //==========================================================================
    // internal class : on_success_policy
    //==========================================================================

    class on_success_policy
    {
    public:
      on_success_policy() noexcept
        : m_exception_count{std::uncaught_exceptions()}
      {

      }

      on_success_policy(on_success_policy&&) = default;
      on_success_policy(const on_success_policy&) = default;
      on_success_policy& operator=(on_success_policy&&) = default;
      on_success_policy& operator=(const on_success_policy&) = default;

      void release() noexcept
      {
        m_exception_count = -1;
      }

      bool should_execute() const noexcept
      {
        return m_exception_count == std::uncaught_exceptions();
      }

    private:

      int m_exception_count;
    };

    //==========================================================================
    // internal class : basic_scope_guard
    //==========================================================================

    template <typename Fn, typename ExitPolicy>
    class basic_scope_guard : public ExitPolicy
    {
    public:
      template <typename ExitFunction,
                typename = std::enable_if_t<std::is_constructible_v<Fn,ExitFunction>>>
      explicit basic_scope_guard(ExitFunction&& fn)
        noexcept(std::is_nothrow_move_constructible<Fn>::value)
        : ExitPolicy{},
          m_function{std::forward<ExitFunction>(fn)}
      {

      }

      basic_scope_guard(basic_scope_guard&& other)
        noexcept(std::is_nothrow_move_constructible<Fn>::value)
        : ExitPolicy{other},
          m_function{std::move(other)}
      {

      }

      ~basic_scope_guard()
        noexcept(noexcept(std::declval<Fn&>()()))
      {
        if (should_execute()) {
          m_function();
        }
      }

      basic_scope_guard(const basic_scope_guard&) = delete;
      basic_scope_guard& operator=(const basic_scope_guard&) = delete;
      basic_scope_guard& operator=(basic_scope_guard&&) = delete;

      //----------------------------------------------------------------------
      // Modifiers
      //----------------------------------------------------------------------
    public:

      using ExitPolicy::release;

      //----------------------------------------------------------------------
      // Observers
      //----------------------------------------------------------------------
    public:

      using ExitPolicy::should_execute;

    private:

      Fn m_function;
    };
  } // namespace detail


  //==========================================================================
  // class : scope_exit<Fn>
  //==========================================================================

  ////////////////////////////////////////////////////////////////////////////
  /// \brief An exit handler for handling both sucess an error cases
  ///
  /// This will always execute the stored function unless 'release' has been
  /// called.
  ///
  /// \code
  /// auto guard = scope::scope_exit{[&]{
  ///   ...
  /// }};
  /// \endcode
  ///
  /// \tparam Fn the function type for the scope to execute
  ////////////////////////////////////////////////////////////////////////////
  template <typename Fn>
  class scope_exit
    : private detail::basic_scope_guard<Fn,detail::on_exit_policy>
  {
    using base_type = detail::basic_scope_guard<Fn,detail::on_exit_policy>;

  public:

    using base_type::base_type;

    using base_type::release;

    using base_type::should_execute;
  };

  template <typename Fn>
  scope_exit(Fn) -> scope_exit<std::decay_t<Fn>>;

  //==========================================================================
  // class : scope_success<Fn>
  //==========================================================================

  ////////////////////////////////////////////////////////////////////////////
  /// \brief An exit handler for handling non-throwing cases
  ///
  /// This will execute as long as an exception has not been thrown in the
  /// same frame the scope was created in. You can manually disengage
  /// executing this handler by calling \c release.
  ///
  /// \code
  /// auto guard = scope::scope_success{[&]{
  ///   ...
  /// }};
  /// \endcode
  ///
  /// \tparam Fn the function type for the scope to execute
  ////////////////////////////////////////////////////////////////////////////
  template <typename Fn>
  class scope_success
    : private detail::basic_scope_guard<Fn,detail::on_success_policy>
  {
    using base_type = detail::basic_scope_guard<Fn,detail::on_success_policy>;

  public:

    using base_type::base_type;

    using base_type::release;

    using base_type::should_execute;
  };

  template <typename Fn>
  scope_success(Fn) -> scope_success<std::decay_t<Fn>>;


  //==========================================================================
  // class : scope_fail<Fn>
  //==========================================================================

  ////////////////////////////////////////////////////////////////////////////
  /// \brief An exit handler for handling throwing cases
  ///
  /// This will execute when an exception has been thrown in the same frame
  /// the scope was created in. You can manually disengage executing this
  /// handler by calling \c release.
  ///
  /// \code
  /// auto guard = scope::scope_fail{[&]{
  ///   ...
  /// }};
  /// \endcode
  ///
  /// \tparam Fn the function type for the scope to execute
  ////////////////////////////////////////////////////////////////////////////
  template <typename Fn>
  class scope_fail
    : private detail::basic_scope_guard<Fn,detail::on_fail_policy>
  {
    using base_type = detail::basic_scope_guard<Fn,detail::on_fail_policy>;

  public:

    using base_type::base_type;

    using base_type::release;

    using base_type::should_execute;
  };

  template <typename Fn>
  scope_fail(Fn) -> scope_fail<std::decay_t<Fn>>;

#define ALLOY_ON_SCOPE_EXIT(fn) \
  const auto ALLOY_UNIQUE_NAME(scope_guard_on_exit_) \
    = ::alloy::core::scope_exit(fn)

#define ALLOY_ON_SCOPE_SUCCESS(fn) \
  const auto ALLOY_UNIQUE_NAME(scope_guard_on_success_) \
    = ::alloy::core::scope_success(fn)

#define ALLOY_ON_SCOPE_FAIL(fn) \
  const auto ALLOY_UNIQUE_NAME(scope_guard_on_fail_) \
    = ::alloy::core::scope_fail(fn)

} // namespace alloy::core

#endif /* ALLOY_CORE_UTILITIES_SCOPE_GUARD_HPP */
