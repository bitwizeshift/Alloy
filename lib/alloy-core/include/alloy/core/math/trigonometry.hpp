// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file trigonometry.hpp
 *
 * \brief This file contains the various standard trig functionalities
 *        that are required in normal math libraries.
 *****************************************************************************/

#ifndef ALLOY_CORE_MATH_TRIGONOMETRY_HPP
#define ALLOY_CORE_MATH_TRIGONOMETRY_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/math/angle/basic_angle.hpp"
#include "alloy/core/math/angle/radian.hpp"

namespace alloy::core {

  //============================================================================
  // static class : trigonometry
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A static class that contains the standard trigonometric functions
  //////////////////////////////////////////////////////////////////////////////
  struct ALLOY_CORE_API trigonometry
  {
    trigonometry() = delete;
    ~trigonometry() = delete;

    //--------------------------------------------------------------------------
    // Trig
    //--------------------------------------------------------------------------

    /// \brief Calculates the cosine of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of `cos(rad)`
    static auto cos(radian rad) noexcept -> float;

    /// \brief Calculates the cosine of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `cos(angle)`
    template<typename AngleUnit>
    static auto cos(basic_angle<AngleUnit> angle) noexcept -> float;

    //--------------------------------------------------------------------------

    /// \brief Calculates the sine of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of `sin(rad)`
    static auto sin(radian rad) noexcept -> float;

    /// \brief Calculates the sine of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `sin(angle)`
    template<typename AngleUnit>
    static auto sin(basic_angle<AngleUnit> angle) noexcept -> float;

    //--------------------------------------------------------------------------

    /// \brief Calculates the tangent of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of `tan(rad)`
    static auto tan(radian rad) noexcept -> float;

    /// \brief Calculates the tangent of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `tan(angle)`
    template<typename AngleUnit>
    static auto tan(basic_angle<AngleUnit> angle) noexcept -> float;

    //--------------------------------------------------------------------------

    /// \brief Calculates the secant of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `sec(angle)`
    template<typename AngleUnit>
    static auto sec(basic_angle<AngleUnit> angle) noexcept -> float;

    /// \brief Calculates the cosecant of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `csc(angle)`
    template<typename AngleUnit>
    static auto csc(basic_angle<AngleUnit> angle) noexcept -> float;

    /// \brief Calculates the cotangent of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `cot(angle)`
    template<typename AngleUnit>
    static auto cot(basic_angle<AngleUnit> angle) noexcept -> float;

    //--------------------------------------------------------------------------
    // Inverse Trig
    //--------------------------------------------------------------------------

    /// \brief Calculates the inverse cosine
    ///
    /// \param f the value, in [-1, 1]
    /// \return the inverse cosine, in [0, pi]
    static auto arccos(float f) noexcept -> radian;

    /// \brief Calculates the inverse sine
    ///
    /// \param f the value, in [-1, 1]
    /// \return the inverse sine, in [-pi/2, pi/2]
    static auto arcsin(float f) noexcept -> radian;

    /// \brief Calculates the inverse tangent
    ///
    /// \param f the value
    /// \return the inverse tangent, in [-pi/2, pi/2]
    static auto arctan(float f) noexcept -> radian;

    /// \brief Calculates the angle of the point (\p f2, \p f1) from the
    ///        positive x-axis, as if by `std::atan2(f1, f2)`
    ///
    /// The quadrant is determined from the signs of both arguments, so the
    /// result lies in [-pi, pi].
    ///
    /// \param f1 the y-coordinate
    /// \param f2 the x-coordinate
    /// \return the angle of the point
    static auto arctan2(float f1, float f2) noexcept -> radian;
  };

} // namespace alloy::core

//==============================================================================
// inline definitions : static class : trigonometry
//==============================================================================

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::cos(basic_angle<AngleUnit> angle)
  noexcept -> float
{
  return cos( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::sin(basic_angle<AngleUnit> angle)
  noexcept -> float
{
  return sin( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::tan(basic_angle<AngleUnit> angle)
  noexcept -> float
{
  return tan( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::sec(basic_angle<AngleUnit> angle)
  noexcept -> float
{
  return 1.0f / cos( angle );
}

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::csc(basic_angle<AngleUnit> angle)
  noexcept -> float
{
  return 1.0f / sin( angle );
}

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::cot(basic_angle<AngleUnit> angle)
  noexcept -> float
{
  return 1.0f / tan( angle );
}

#endif /* ALLOY_CORE_MATH_TRIGONOMETRY_HPP */

