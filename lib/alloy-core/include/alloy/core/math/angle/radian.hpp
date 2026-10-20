// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file radian.hpp
 *
 * \brief This header contains the definition of a radian angle class
 ******************************************************************************/

#ifndef ALLOY_CORE_MATH_ANGLE_RADIAN_HPP
#define ALLOY_CORE_MATH_ANGLE_RADIAN_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"
#include "alloy/core/math/math_constants.hpp"
#include "alloy/core/math/angle/basic_angle.hpp"

#include <type_traits> // std::enable_if
#include <utility>     // std::swap

namespace alloy::core {

  //============================================================================
  // struct : radian_unit
  //============================================================================

  struct radian_unit
  {
    static constexpr auto revolution() noexcept -> real;
  };

  //============================================================================
  // class : radian
  //============================================================================

  using radian = basic_angle<radian_unit>;

  //============================================================================
  // non-member functions : class : radian
  //============================================================================

  //---------------------------------------------------------------------------
  // Utilities : Casting
  //---------------------------------------------------------------------------

  inline namespace casts {

    /// \brief Converts the given \p angle to a radian
    ///
    /// \param angle the angle to convert
    /// \return the angle in radians
    template<typename AngleUnit>
    constexpr auto to_radian(basic_angle<AngleUnit> angle) noexcept -> radian;

  } // inline namespace casts

  //============================================================================
  // trait : is_radian
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref radian
  ///
  /// The result is aliased as `::value`
  template<typename T> struct is_radian : std::false_type{};

  template<> struct is_radian<radian> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_radian
  template<typename T>
  constexpr bool is_radian_v = is_radian<T>::value;

  //============================================================================
  // struct : radian_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of radian constants
  //////////////////////////////////////////////////////////////////////////////
  using radian_constants = basic_angle_constants<radian_unit>;

  //============================================================================
  // Literals
  //============================================================================

  inline namespace literals {
    constexpr auto operator""_rad(long double x) noexcept -> radian;
  } // inline namespace literals
} // namespace alloy::core


//==============================================================================
// struct : radian_unit
//==============================================================================

inline constexpr
auto alloy::core::radian_unit::revolution()
  noexcept -> real
{
  return static_cast<real>(math_constants::two_pi);
}

//==============================================================================
// non-member functions : class : radian
//==============================================================================

//------------------------------------------------------------------------------
// Utilities : Casting
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr
auto alloy::core::casts::to_radian(basic_angle<AngleUnit> angle)
  noexcept -> radian
{
  return to_angle<radian>(angle);
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr
auto alloy::core::literals::operator""_rad(long double x)
  noexcept -> radian
{
  return radian{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_MATH_ANGLE_RADIAN_HPP */

