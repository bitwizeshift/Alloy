/******************************************************************************
 * \file gradian.hpp
 *
 * \brief This header contains the definition of a gradian angle class
 ******************************************************************************/

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
#ifndef ALLOY_CORE_MATH_ANGLE_GRADIAN_HPP
#define ALLOY_CORE_MATH_ANGLE_GRADIAN_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"
#include "alloy/core/math/angle/basic_angle.hpp"

#include <type_traits> // std::enable_if
#include <utility>     // std::swap

namespace alloy::core {

  //============================================================================
  // struct : gradian_unit
  //============================================================================

  struct gradian_unit
  {
    static constexpr auto revolution() noexcept -> real;
  };

  //============================================================================
  // class : gradian
  //============================================================================

  using gradian = basic_angle<gradian_unit>;

  //============================================================================
  // non-member functions : class : gradian
  //============================================================================

  //---------------------------------------------------------------------------
  // Utilities : Casting
  //---------------------------------------------------------------------------

  inline namespace casts {

    /// \brief Converts the given \p angle to a gradian
    ///
    /// \param angle the angle to convert
    /// \return the angle in gradians
    template<typename AngleUnit>
    constexpr auto to_gradian(basic_angle<AngleUnit> angle) noexcept -> gradian;

  } // inline namespace casts

  //============================================================================
  // trait : is_gradian
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref gradian
  ///
  /// The result is aliased as `::value`
  template<typename T> struct is_gradian : std::false_type{};

  template<> struct is_gradian<gradian> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_gradian
  template<typename T>
  constexpr bool is_gradian_v = is_gradian<T>::value;

  //============================================================================
  // struct : gradian_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of gradian constants
  //////////////////////////////////////////////////////////////////////////////
  using gradian_constants = basic_angle_constants<gradian_unit>;

  //============================================================================
  // Literals
  //============================================================================

  inline namespace literals {
    constexpr auto operator""_grad(long double x) noexcept -> gradian;
  } // inline namespace literals
} // namespace alloy::core


//==============================================================================
// struct : gradian_unit
//==============================================================================

inline constexpr
auto alloy::core::gradian_unit::revolution()
  noexcept -> real
{
  return static_cast<real>(400);
}

//==============================================================================
// non-member functions : class : gradian
//==============================================================================

//------------------------------------------------------------------------------
// Utilities : Casting
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr
auto alloy::core::casts::to_gradian(basic_angle<AngleUnit> angle)
  noexcept -> gradian
{
  return to_angle<gradian>(angle);
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr
auto alloy::core::literals::operator""_grad(long double x)
  noexcept -> gradian
{
  return gradian{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_MATH_ANGLE_GRADIAN_HPP */

