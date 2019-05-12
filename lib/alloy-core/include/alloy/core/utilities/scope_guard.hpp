/*****************************************************************************
 * \file piecewise_compare.hpp
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

#include "alloy/core/macros.hpp" // ALLOY_UNIQUE_NAME

#include <utility>     // std::move
#include <type_traits> // std::is_nothrow_move_constructible, std::decay_t

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An RAII wrapper used for dispatching arbitrary functionality for
  ///        external resource cleanup
  ///
  /// \tparam Fn the function to invoke. Must not be a reference type, moveable,
  ///            and invocable without any arguments.
  //////////////////////////////////////////////////////////////////////////////
  template<typename Fn>
  class scope_guard
  {
    static_assert(!std::is_reference<Fn>::value);
    static_assert(std::is_move_constructible<Fn>::value);
    static_assert(std::is_invocable<Fn>::value);

    //--------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a scope_guard from a function
    ///
    /// \param fn the function to invoke
    scope_guard(Fn fn)
      noexcept(std::is_nothrow_move_constructible<Fn>::value);

    /// \brief Moves the contents of one scope_guard to this scope_guard
    ///
    /// \param other the other scope_guard to move
    scope_guard(scope_guard&& other)
      noexcept(std::is_nothrow_move_constructible<Fn>::value);
    scope_guard( const scope_guard& other ) = delete;

    //--------------------------------------------------------------------------

    ~scope_guard() noexcept;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of one scope_guard to this scope_guard
    ///
    /// \param other the other scope_guard to move
    /// \return reference to \c (*this)
    scope_guard& operator=( scope_guard&& other ) = delete;
    scope_guard& operator=( const scope_guard& other ) = delete;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    Fn m_fn;              ///< The function to invoke
    bool m_should_engage; ///< Whether or not the function should be invoked
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
  : m_fn{std::move(fn)},
    m_should_engage{true}
{

}

template<typename Fn>
inline alloy::core::scope_guard<Fn>::scope_guard( scope_guard&& other )
  noexcept(std::is_nothrow_move_constructible<Fn>::value)
  : m_fn{std::move(other.m_fn)},
    m_should_engage{other.m_should_engage}
{
  other.m_should_engage = false;
}

//------------------------------------------------------------------------------

template<typename Fn>
inline alloy::core::scope_guard<Fn>::~scope_guard()
  noexcept
{
  if (m_should_engage) {
    m_fn();
  }
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