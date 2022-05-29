/*****************************************************************************
 * \file trigonometry.hpp
 *
 * \brief This file contains the various standard trig functionalities
 *        that are required in normal math libraries.
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
#ifndef ALLOY_CORE_MATH_TRIGONOMETRY_HPP
#define ALLOY_CORE_MATH_TRIGONOMETRY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/precision.hpp"
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
    static auto cos(radian rad) noexcept -> real;

    /// \brief Calculates the cosine of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `cos(angle)`
    template<typename AngleUnit>
    static auto cos(basic_angle<AngleUnit> angle) noexcept -> real;

    //--------------------------------------------------------------------------

    /// \brief Calculates the sine of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of `sin(rad)`
    static auto sin(radian rad) noexcept -> real;

    /// \brief Calculates the sine of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `sin(angle)`
    template<typename AngleUnit>
    static auto sin(basic_angle<AngleUnit> angle) noexcept -> real;

    //--------------------------------------------------------------------------

    /// \brief Calculates the tangent of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of `tan(rad)`
    static auto tan(radian rad) noexcept -> real;

    /// \brief Calculates the tangent of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `tan(angle)`
    template<typename AngleUnit>
    static auto tan(basic_angle<AngleUnit> angle) noexcept -> real;

    //--------------------------------------------------------------------------

    /// \brief Calculates the secant of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `sec(angle)`
    template<typename AngleUnit>
    static auto sec(basic_angle<AngleUnit> angle) noexcept -> real;

    /// \brief Calculates the cosecant of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `csc(angle)`
    template<typename AngleUnit>
    static auto csc(basic_angle<AngleUnit> angle) noexcept -> real;

    /// \brief Calculates the cotangent of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of `cot(angle)`
    template<typename AngleUnit>
    static auto cot(basic_angle<AngleUnit> angle) noexcept -> real;

    //--------------------------------------------------------------------------
    // Inverse Trig
    //--------------------------------------------------------------------------

    /// \brief Calculates the inverse cosine
    ///
    /// \param f the value
    /// \return the inverse cosine
    static auto arccos(real f) noexcept -> radian;

    /// \brief Calculates the inverse sine
    ///
    /// \param f the value
    /// \return the inverse sine
    static auto arcsin(real f) noexcept -> radian;

    /// \brief Calculates the inverse tangent
    ///
    /// \param f the value
    /// \return the inverse tangent
    static auto arctan(real f) noexcept -> radian;

    /// \brief Calculates the inverse tangent 2
    ///
    /// \param f1 the first value
    /// \param f2 the second value
    /// \return the inverse tangent 2
    static auto arctan2(real f1, real f2) noexcept -> radian;
  };

} // namespace alloy::core

//==============================================================================
// inline definitions : static class : trigonometry
//==============================================================================

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::cos(basic_angle<AngleUnit> angle)
  noexcept -> real
{
  return cos( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::sin(basic_angle<AngleUnit> angle)
  noexcept -> real
{
  return sin( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::tan(basic_angle<AngleUnit> angle)
  noexcept -> real
{
  return tan( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::sec(basic_angle<AngleUnit> angle)
  noexcept -> real
{
  return real{1} / cos( angle );
}

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::csc(basic_angle<AngleUnit> angle)
  noexcept -> real
{
  return real{1} / sin( angle );
}

template<typename AngleUnit>
inline
auto alloy::core::trigonometry::cot(basic_angle<AngleUnit> angle)
  noexcept -> real
{
  return real{1} / tan( angle );
}

#endif /* ALLOY_CORE_MATH_TRIGONOMETRY_HPP */

