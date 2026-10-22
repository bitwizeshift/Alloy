// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file gradian.hpp
 *
 * \brief This header contains the definition of a gradian angle class
 ******************************************************************************/

#ifndef ALLOY_CORE_MATH_ANGLE_GRADIAN_HPP
#define ALLOY_CORE_MATH_ANGLE_GRADIAN_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/angle/basic_angle.hpp"

#include "alloy/core/precision.hpp"

#include <type_traits> // std::enable_if

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
auto alloy::core::gradian_unit::revolution() noexcept -> real
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
auto alloy::core::literals::operator""_grad(long double x) noexcept -> gradian
{
  return gradian{ static_cast<real>(x) };
}

#endif /* ALLOY_CORE_MATH_ANGLE_GRADIAN_HPP */

