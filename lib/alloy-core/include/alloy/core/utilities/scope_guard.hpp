/*****************************************************************************
 * \file scope_guard.hpp
 *
 * \brief This header defines a comparator for piecewise-based comparisons
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
#ifndef ALLOY_CORE_UTILITIES_SCOPE_GUARD_HPP
#define ALLOY_CORE_UTILITIES_SCOPE_GUARD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/macros.hpp" // ALLOY_UNIQUE_NAME

#include <utility>     // std::move
#include <type_traits> // std::is_nothrow_move_constructible, std::decay_t

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An RAII wrapper used for dispatching arbitrary functionality for
  ///        external resource cleanup
  ///
  /// \tparam Fn the function to invoke. Must not be a reference type,
  ///            and must be invocable without any arguments.
  //////////////////////////////////////////////////////////////////////////////
  template<typename Fn>
  class scope_guard
  {
    static_assert(!std::is_reference<Fn>::value);
    static_assert(std::is_invocable<Fn>::value);

    //--------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a scope_guard from a function
    ///
    /// \param fn the function to invoke
    explicit scope_guard(Fn fn)
      noexcept(std::is_nothrow_move_constructible<Fn>::value);

    scope_guard(scope_guard&&) = delete;
    scope_guard(const scope_guard&) = delete;

    //--------------------------------------------------------------------------

    ~scope_guard() noexcept;

    //--------------------------------------------------------------------------

    scope_guard& operator=(scope_guard&&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    Fn m_fn; ///< The function to invoke
  };

  //============================================================================
  // non-member functions : class : scope_guard
  //============================================================================

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Creates a scope guard that executes at the end of the scope
  ///
  /// \param fn the function to execute when a scope is exited
  template<typename Fn>
  scope_guard<std::decay_t<Fn>> on_scope_exit( Fn&& fn )
    noexcept(std::is_nothrow_move_constructible<std::decay_t<Fn>>::value);

#define ALLOY_ON_SCOPE_EXIT(fn) \
  const auto ALLOY_UNIQUE_NAME(scope_guard_on_exit_) \
    = ::alloy::core::on_scope_exit(fn);

} // namespace alloy::core

//==============================================================================
// class : scope_guard
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Destructor
//------------------------------------------------------------------------------

template<typename Fn>
inline alloy::core::scope_guard<Fn>::scope_guard( Fn fn )
  noexcept(std::is_nothrow_move_constructible<Fn>::value)
  : m_fn{std::move(fn)}
{

}

//------------------------------------------------------------------------------

template<typename Fn>
inline alloy::core::scope_guard<Fn>::~scope_guard()
  noexcept
{
  m_fn();
}

//==============================================================================
// non-member functions : class : scope_guard
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

template<typename Fn>
inline alloy::core::scope_guard<std::decay_t<Fn>>
  alloy::core::on_scope_exit( Fn&& fn )
  noexcept(std::is_nothrow_move_constructible<std::decay_t<Fn>>::value)
{
  return scope_guard<std::decay_t<Fn>>{std::forward<Fn>(fn)};
}


#endif /* ALLOY_CORE_UTILITIES_SCOPE_GUARD_HPP */