/*****************************************************************************
 * \file type_index.hpp
 *
 * \todo
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
#ifndef ALLOY_CORE_TYPE_INDEX_HPP
#define ALLOY_CORE_TYPE_INDEX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"

#include <type_traits> // std::is_const, std::is_reference, etc
#include <cstdint>     // std::uint32_t

namespace alloy::core {

  //============================================================================
  // class : component_manager
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A non-RTTI-based way of getting type information from a type
  //////////////////////////////////////////////////////////////////////////////
  class type_index
  {
    //-------------------------------------------------------------------------
    // Public Members
    //-------------------------------------------------------------------------
  public:

    using index_type = std::int32_t;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the index for the given type T
    ///
    /// \tparam T the type
    /// \return the index for the type
    template <typename T>
    static type_index get();

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs the type_index with a default value
    constexpr type_index() noexcept = default;

    /// \brief Constructs the type_index by moving the contents of \p other
    ///
    /// \param other the other type_index to move
    constexpr type_index(type_index&& other) noexcept = default;

    /// \brief Constructs the type_index by copying the contents of \p other
    ///
    /// \param other the other type_index to copy
    constexpr type_index(const type_index& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Assigns the content of this type_index by moving the contents
    ///        of \p other
    ///
    /// \param other the other type_index to move
    /// \return reference to `(*this)`
    type_index& operator=(type_index&&) noexcept = default;

    /// \brief Assigns the content of this type_index by copying the contents
    ///        of \p other
    ///
    /// \param other the other type_index to copy
    /// \return reference to `(*this)`
    type_index& operator=(const type_index&) noexcept = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying value of this type_info
    ///
    /// \return the value
    constexpr index_type value() const noexcept;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    /// \brief Constructs this type_index with the given index \p i
    ///
    /// \param i the index to set
    constexpr explicit type_index(index_type i) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    index_type m_index = index_type{-1};

    ALLOY_CORE_API
    static index_type s_index;
  };

  //===========================================================================
  // non-member functions : class : type_index
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison Operators
  //---------------------------------------------------------------------------

  constexpr bool operator==(type_index lhs, type_index rhs) noexcept;
  constexpr bool operator!=(type_index lhs, type_index rhs) noexcept;
  constexpr bool operator<(type_index lhs, type_index rhs) noexcept;
  constexpr bool operator>(type_index lhs, type_index rhs) noexcept;
  constexpr bool operator<=(type_index lhs, type_index rhs) noexcept;
  constexpr bool operator>=(type_index lhs, type_index rhs) noexcept;

} // namespace alloy::core

//=============================================================================
// definitions : class : type_index
//=============================================================================

//-----------------------------------------------------------------------------
// Factories
//-----------------------------------------------------------------------------

template <typename T>
inline alloy::core::type_index alloy::core::type_index::get()
{
  static_assert(!std::is_reference_v<T>);
  static_assert(!std::is_pointer_v<T>);
  static_assert(!std::is_array_v<T>);
  static_assert(!std::is_const_v<T>);
  static_assert(!std::is_reference_v<T>);

  static const auto s_this_index = s_index++;

  return type_index{s_this_index};
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline constexpr alloy::core::type_index::index_type
  alloy::core::type_index::value()
  const noexcept
{
  return m_index;
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

inline constexpr alloy::core::type_index::type_index(index_type i)
  noexcept
  : m_index{i}
{

}

//=============================================================================
// definitions : non-member functions : class : type_index
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==(type_index lhs, type_index rhs)
  noexcept
{
  return lhs.value() == rhs.value();
}

inline constexpr bool alloy::core::operator!=(type_index lhs, type_index rhs)
  noexcept
{
  return lhs.value() != rhs.value();
}

inline constexpr bool alloy::core::operator<(type_index lhs, type_index rhs)
  noexcept
{
  return lhs.value() < rhs.value();
}

inline constexpr bool alloy::core::operator>(type_index lhs, type_index rhs)
  noexcept
{
  return lhs.value() > rhs.value();
}

inline constexpr bool alloy::core::operator<=(type_index lhs, type_index rhs)
  noexcept
{
  return lhs.value() <= rhs.value();
}

inline constexpr bool alloy::core::operator>=(type_index lhs, type_index rhs)
  noexcept
{
  return lhs.value() >= rhs.value();
}

#endif /* ALLOY_CORE_TYPE_INDEX_HPP */

