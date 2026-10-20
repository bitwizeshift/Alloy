// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file degree.hpp
 *
 * \brief This header contains the definition of a degree angle class
 ******************************************************************************/

#ifndef ALLOY_CORE_MATH_ANGLE_DEGREE_HPP
#define ALLOY_CORE_MATH_ANGLE_DEGREE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/precision.hpp"
#include "alloy/core/math/angle/basic_angle.hpp"

#include <type_traits> // std::enable_if
#include <utility>     // std::swap

namespace alloy::core {

  //============================================================================
  // struct : degree_unit
  //============================================================================

  struct degree_unit
  {
    static constexpr auto revolution() noexcept -> core::real;
  };

  //============================================================================
  // class : degree
  //============================================================================

  using degree = basic_angle<degree_unit>;

  //============================================================================
  // non-member functions : class : degree
  //============================================================================

  //---------------------------------------------------------------------------
  // Utilities : Casting
  //---------------------------------------------------------------------------

  inline namespace casts {

    /// \brief Converts the given \p angle to a degree
    ///
    /// \param angle the angle to convert
    /// \return the angle in degrees
    template<typename AngleUnit>
    constexpr auto to_degree(basic_angle<AngleUnit> angle) noexcept -> degree;

  } // inline namespace casts

  //============================================================================
  // trait : is_degree
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref degree
  ///
  /// The result is aliased as `::value`
  template<typename T> struct is_degree : std::false_type{};

  template<> struct is_degree<degree> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_degree
  template<typename T>
  constexpr bool is_degree_v = is_degree<T>::value;

  //============================================================================
  // struct : degree_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of degree constants
  //////////////////////////////////////////////////////////////////////////////
  using degree_constants = basic_angle_constants<degree_unit>;

  //============================================================================
  // Literals
  //============================================================================

  inline namespace literals {
    constexpr auto operator""_deg(long double x) noexcept -> degree;
  } // inline namespace literals
} // namespace alloy::core


//==============================================================================
// struct : degree_unit
//==============================================================================

inline constexpr auto
  alloy::core::degree_unit::revolution() noexcept -> alloy::core::real
{
  return static_cast<real>(360);
}

//==============================================================================
// non-member functions : class : degree
//==============================================================================

//------------------------------------------------------------------------------
// Utilities : Casting
//------------------------------------------------------------------------------

template<typename AngleUnit>
inline constexpr auto
  alloy::core::casts::to_degree(basic_angle<AngleUnit> angle) noexcept
  -> alloy::core::degree
{
  return to_angle<degree>(angle);
}

//==============================================================================
// Literals
//==============================================================================

inline constexpr auto
  alloy::core::literals::operator""_deg(long double x) noexcept
  -> alloy::core::degree
{
  return degree{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_MATH_ANGLE_DEGREE_HPP */

