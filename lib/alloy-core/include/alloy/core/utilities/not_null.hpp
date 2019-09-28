/*****************************************************************************
 * \file not_null.hpp
 *
 * \brief This header defines a utility for asserting semantic correctness
 *        of non-nullable pointer types
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
#ifndef ALLOY_CORE_UTILITIES_NOT_NULL_HPP
#define ALLOY_CORE_UTILITIES_NOT_NULL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp"

#include <cstddef>     // std::nullptr_t
#include <utility>     // std::forward, std::move
#include <type_traits> // std::decay_t

#define ALLOY_IMPLICIT

namespace alloy::core {

  //============================================================================
  // class : not_null
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A wrapper type around a pointer to disallow null assignments
  ///
  /// \tparam T the underlying pointer type
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class not_null
  {
    static_assert(std::is_assignable<T&, std::nullptr_t>::value);
    static_assert(!std::is_same<T,std::nullptr_t>::value);

    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using pointer      = T;
    using reference    = decltype(*std::declval<T>())&;
    using element_type = std::decay_t<reference>;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    // TODO(bitwizeshift):
    //   Correct noexcept specifiers

    not_null() = delete;

    /// \brief Constructs a not_null from a pointer
    ///
    /// \param pointer the pointer
    constexpr not_null(T pointer) noexcept;

    /// \brief Constructs a not_null from any type convertible to the pointer
    ///        type
    ///
    /// \param u the type to instantiate the not_null from
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr not_null(U&& u) noexcept;

    /// \brief Move-converts a not_null from another one of covariant type
    ///
    /// \param other the other not_null to move
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr not_null(not_null<U>&& other) noexcept;

    /// \brief Copy-converts a not_null from another one of covariant type
    ///
    /// \param other the other not_null to copy
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr not_null(const not_null<U>& other) noexcept;

    /// \brief Move-constructs this not_null from an existing one
    ///
    /// \param other the other not_null to move
    constexpr not_null( not_null&& other ) noexcept = default;

    /// \brief Copy-constructs this not_null from an existing one
    ///
    /// \param other the other not_null to copy
    constexpr not_null( const not_null& other ) noexcept = default;

    not_null( std::nullptr_t ) = delete;

    //--------------------------------------------------------------------------

    not_null& operator=(T other) noexcept;
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    not_null& operator=(U&& other) noexcept;
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    not_null& operator=(not_null<U>&& other) noexcept;
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    not_null& operator=(const not_null<U>& other) noexcept;
    not_null& operator=(not_null&& other) noexcept = default;
    not_null& operator=(const not_null& other) noexcept = default;

    not_null& operator=(std::nullptr_t) = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying pointer
    ///
    /// \return the underlying pointer
    constexpr pointer get() const noexcept;

    /// \brief Dereferences the underlying pointer
    ///
    /// \return the underlying pointer
    constexpr pointer operator->() const noexcept;

    /// \brief Dereferences the underlying pointer
    ///
    /// \return reference to the underlying pointer
    constexpr reference operator*() const noexcept;

    //--------------------------------------------------------------------------
    // Deleted Operators
    //--------------------------------------------------------------------------
  private:

    not_null& operator++() = delete;
    not_null& operator--() = delete;
    not_null operator++(int) = delete;
    not_null operator--(int) = delete;
    not_null& operator+=(std::ptrdiff_t) = delete;
    not_null& operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
  private:

    pointer m_pointer;
  };

  //============================================================================
  // non-member functions : class : not_null
  //============================================================================

  // NOTE(bitwizeshift):
  //   not_null is not given equality operators, since it is expected as a
  //   wrapper to uphold invariants on APIs

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  template<typename T>
  constexpr not_null<std::decay_t<T>> make_not_null(T&& t) noexcept;

} // namespace alloy::core

//==============================================================================
// class : not_null
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::core::not_null<T>::not_null(T pointer)
  noexcept
  : m_pointer{std::move(pointer)}
{
  ALLOY_ASSERT( m_pointer!= nullptr );
}

template<typename T>
template<typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(U&& u)
  noexcept
  : m_pointer{std::forward<U>(u)}
{
  ALLOY_ASSERT( m_pointer!= nullptr );
}

template<typename T>
template<typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(not_null<U>&& other)
  noexcept
  : m_pointer{std::move(other.m_pointer)}
{

}

template<typename T>
template<typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(const not_null<U>& other)
  noexcept
  : m_pointer{std::move(other.m_pointer)}
{

}

//------------------------------------------------------------------------------

template<typename T>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(T other)
  noexcept
{
  m_pointer = std::move(other);

  ALLOY_ASSERT(m_pointer!=nullptr);
  return (*this);
}

template<typename T>
template<typename U, typename>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(U&& other)
  noexcept
{
  m_pointer = std::forward<U>(other);

  ALLOY_ASSERT(m_pointer!=nullptr);
  return (*this);
}

template<typename T>
template<typename U, typename>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(not_null<U>&& other)
  noexcept
{
  m_pointer = std::move(other.get());

  return (*this);
}

template<typename T>
template<typename U, typename>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(const not_null<U>& other)
  noexcept
{
  m_pointer = other.get();

  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::not_null<T>::pointer
  alloy::core::not_null<T>::get()
  const noexcept
{
  return m_pointer;
}

template<typename T>
inline constexpr typename alloy::core::not_null<T>::pointer
  alloy::core::not_null<T>::operator->()
  const noexcept
{
  return m_pointer;
}

template<typename T>
inline constexpr typename alloy::core::not_null<T>::reference
  alloy::core::not_null<T>::operator*()
  const noexcept
{
  return (*m_pointer);
}

//==============================================================================
// non-member functions : class : not_null
//==============================================================================

template<typename T>
inline constexpr alloy::core::not_null<std::decay_t<T>>
  alloy::core::make_not_null(T&& t)
  noexcept
{
  return not_null<std::decay_t<T>>{std::forward<T>(t)};
}

#endif /* ALLOY_CORE_UTILITIES_NOT_NULL_HPP */