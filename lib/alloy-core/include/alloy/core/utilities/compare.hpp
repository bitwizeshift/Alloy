/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_UTILITIES_COMPARE_HPP
#define ALLOY_CORE_UTILITIES_COMPARE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/utilities/int_utilities.hpp"
#include <functional>  // std::less, std::greater, etc
#include <utility>     // std::declval
#include <type_traits> // std::is_integral_v

namespace alloy::core {

  // Changes from the C++ standard:
  // * All comparators are functor objects with constexpr constant versions
  // * All comparators are already transparent
  // * Integral comparators

  //===========================================================================
  // class : compare_equal
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator object for comparing equality between two types using
  ///        `operator==`.
  ///
  /// This comparator behaves in the following way:
  /// * For normal object types, this uses `operator==`
  /// * For integral types, this performs a safe comparison between signs
  ///
  /// \note This comparator is transparent
  /////////////////////////////////////////////////////////////////////////////
  class compare_equal
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using is_transparent = void;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    compare_equal() = default;
    compare_equal(const compare_equal& other) = default;

    auto operator=(const compare_equal& other) -> compare_equal& = default;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    template <typename T, typename U,
              typename = decltype(std::declval<const T&>() == std::declval<const U&>())>
    constexpr auto operator()(const T& lhs, const U& rhs)
      const noexcept -> bool;

  };

  inline constexpr auto equal = compare_equal{};

  //===========================================================================
  // class : compare_not_equal
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator object for comparing inequality between two types using
  ///        `operator!=`.
  ///
  /// This comparator behaves in the following way:
  /// * For normal object types, this uses `operator!=`
  /// * For integral types, this performs a safe comparison between signs
  ///
  /// \note This comparator is transparent
  /////////////////////////////////////////////////////////////////////////////
  class compare_not_equal
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using is_transparent = void;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    compare_not_equal() = default;
    compare_not_equal(const compare_not_equal& other) = default;

    auto operator=(const compare_not_equal& other) -> compare_not_equal& = default;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    template <typename T, typename U,
              typename = decltype(std::declval<const T&>() != std::declval<const U&>())>
    constexpr auto operator()(const T& lhs, const U& rhs)
      const noexcept -> bool;

  };

  inline constexpr auto not_equal = compare_not_equal{};

  //===========================================================================
  // class : compare_less
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator object for comparing less-than ordering between two
  ///        types using `operator<`.
  ///
  /// This comparator behaves in the following way:
  /// * For normal object types, this uses `operator<`
  /// * For integral types, this performs a safe comparison between signs
  /// * For pointer types, this provides a total-ordering between pointers from
  ///   different subobjects
  ///
  /// \note This comparator is transparent
  /////////////////////////////////////////////////////////////////////////////
  class compare_less
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using is_transparent = void;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    compare_less() = default;
    compare_less(const compare_less& other) = default;

    auto operator=(const compare_less& other) -> compare_less& = default;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    template <typename T, typename U,
              typename = decltype(std::declval<const T&>() < std::declval<const U&>())>
    constexpr auto operator()(const T& lhs, const U& rhs)
      const noexcept -> bool;

  };

  constexpr inline auto less = compare_less{};

  //===========================================================================
  // class : compare_less
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator object for comparing greater-than ordering between two
  ///        types using `operator>`.
  ///
  /// This comparator behaves in the following way:
  /// * For normal object types, this uses `operator>`
  /// * For integral types, this performs a safe comparison between signs
  /// * For pointer types, this provides a total-ordering between pointers from
  ///   different subobjects
  ///
  /// \note This comparator is transparent
  /////////////////////////////////////////////////////////////////////////////
  class compare_greater
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using is_transparent = void;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    compare_greater() = default;
    compare_greater(const compare_greater& other) = default;

    auto operator=(const compare_greater& other) -> compare_greater& = default;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    template <typename T, typename U,
              typename = decltype(std::declval<const T&>() > std::declval<const U&>())>
    constexpr auto operator()(const T& lhs, const U& rhs)
      const noexcept -> bool;

  };

  constexpr inline auto greater = compare_greater{};


  //===========================================================================
  // class : compare_less
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator object for comparing less-than ordering between two
  ///        types using `operator<`.
  ///
  /// This comparator behaves in the following way:
  /// * For normal object types, this uses `operator<`
  /// * For integral types, this performs a safe comparison between signs
  /// * For pointer types, this provides a total-ordering between pointers from
  ///   different subobjects
  ///
  /// \note This comparator is transparent
  /////////////////////////////////////////////////////////////////////////////
  class compare_less_equal
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using is_transparent = void;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    compare_less_equal() = default;
    compare_less_equal(const compare_less_equal& other) = default;

    auto operator=(const compare_less_equal& other) -> compare_less_equal& = default;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    template <typename T, typename U,
              typename = decltype(std::declval<const T&>() < std::declval<const U&>())>
    constexpr auto operator()(const T& lhs, const U& rhs)
      const noexcept -> bool;

  };

  constexpr inline auto less_equal = compare_less_equal{};

  //===========================================================================
  // class : compare_less
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A comparator object for comparing greater-than ordering between two
  ///        types using `operator>`.
  ///
  /// This comparator behaves in the following way:
  /// * For normal object types, this uses `operator>`
  /// * For integral types, this performs a safe comparison between signs
  /// * For pointer types, this provides a total-ordering between pointers from
  ///   different subobjects
  ///
  /// \note This comparator is transparent
  /////////////////////////////////////////////////////////////////////////////
  class compare_greater_equal
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using is_transparent = void;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    compare_greater_equal() = default;
    compare_greater_equal(const compare_greater_equal& other) = default;

    auto operator=(const compare_greater_equal& other) -> compare_greater_equal& = default;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    template <typename T, typename U,
              typename = decltype(std::declval<const T&>() > std::declval<const U&>())>
    constexpr auto operator()(const T& lhs, const U& rhs)
      const noexcept -> bool;

  };

  constexpr inline auto greater_equal = compare_greater_equal{};

} // namespace alloy::core

//=============================================================================
// class : compare_equal
//=============================================================================

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

template <typename T, typename U, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_equal::operator()(const T& lhs, const U& rhs)
  const noexcept -> bool
{
  if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
    return int_utilities::equal(lhs, rhs);
  } else {
    return lhs == rhs;
  }
}

//=============================================================================
// class : compare_not_equal
//=============================================================================

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

template <typename T, typename U, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_not_equal::operator()(const T& lhs, const U& rhs)
  const noexcept -> bool
{
  if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
    return int_utilities::not_equal(lhs, rhs);
  } else {
    return lhs != rhs;
  }
}

//=============================================================================
// class : compare_less
//=============================================================================

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

template <typename T, typename U, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_less::operator()(const T& lhs, const U& rhs)
  const noexcept -> bool
{
  if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
    return int_utilities::less(lhs, rhs);
  } else {
    return std::less<void>{}(lhs, rhs);
  }
}

//=============================================================================
// class : compare_greater
//=============================================================================

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

template <typename T, typename U, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_greater::operator()(const T& lhs, const U& rhs)
  const noexcept -> bool
{
  if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
    return int_utilities::greater(lhs, rhs);
  } else {
    return std::greater<void>{}(lhs, rhs);
  }
}

//=============================================================================
// class : compare_less_equal
//=============================================================================

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

template <typename T, typename U, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_less_equal::operator()(const T& lhs, const U& rhs)
  const noexcept -> bool
{
  if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
    return int_utilities::less_equal(lhs, rhs);
  } else {
    return std::less_equal<void>{}(lhs, rhs);
  }
}

//=============================================================================
// class : compare_greater_equal
//=============================================================================

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

template <typename T, typename U, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::compare_greater_equal::operator()(const T& lhs, const U& rhs)
  const noexcept -> bool
{
  if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
    return int_utilities::greater_equal(lhs, rhs);
  } else {
    return std::greater_equal<void>{}(lhs, rhs);
  }

}

#endif /* ALLOY_CORE_UTILITIES_COMPARE_HPP */

