/*****************************************************************************
 * \file trigonometry.hpp
 *
 * \brief This file contains the various standard trig functionalities
 *        that are required in normal math libraries.
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
#ifndef ALLOY_CORE_MATH_TRIGONOMETRY_HPP
#define ALLOY_CORE_MATH_TRIGONOMETRY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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
  struct trigonometry
  {
    trigonometry() = delete;
    ~trigonometry() = delete;

    //--------------------------------------------------------------------------
    // Trig
    //--------------------------------------------------------------------------

    /// \brief Calculates the cosine of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of \c cos(rad)
    static core::real cos( radian rad ) noexcept;

    /// \brief Calculates the cosine of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of \c cos(angle)
    template<typename AngleUnit>
    static core::real cos( basic_angle<AngleUnit> angle ) noexcept;

    //--------------------------------------------------------------------------

    /// \brief Calculates the sine of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of \c sin(rad)
    static core::real sin( radian rad ) noexcept;

    /// \brief Calculates the sine of the given \p angle
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of \c sin(angle)
    template<typename AngleUnit>
    static core::real sin( basic_angle<AngleUnit> angle ) noexcept;

    //--------------------------------------------------------------------------

    /// \brief Calculates the tangent of the given \ref radian angle, \p rad
    ///
    /// \param rad the angle
    /// \return the result of \c tan(rad)
    static core::real tan( radian rad ) noexcept;

    /// \brief Calculates the tangent of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of \c tan(angle)
    template<typename AngleUnit>
    static core::real tan( basic_angle<AngleUnit> angle ) noexcept;

    //--------------------------------------------------------------------------

    /// \brief Calculates the secant of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of \c sec(angle)
    template<typename AngleUnit>
    static core::real sec( basic_angle<AngleUnit> angle ) noexcept;

    /// \brief Calculates the cosecant of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of \c csc(angle)
    template<typename AngleUnit>
    static core::real csc( basic_angle<AngleUnit> angle ) noexcept;

    /// \brief Calculates the cotangent of the given \ref gradian angle, \p grad
    ///
    /// \note A conversion is performed in order to make this value a
    ///       \ref radian first
    ///
    /// \param angle the angle
    /// \return the result of \c cot(angle)
    template<typename AngleUnit>
    static core::real cot( basic_angle<AngleUnit> angle ) noexcept;

    //--------------------------------------------------------------------------
    // Inverse Trig
    //--------------------------------------------------------------------------

    /// \brief Calculates the inverse cosine of the given
    static radian arccos( core::real f ) noexcept;
    static radian arcsin( core::real f ) noexcept;
    static radian arctan( core::real f ) noexcept;
    static radian arctan2( core::real f1, core::real f2 ) noexcept;

  };

  using trig = trigonometry;

} // namespace alloy::core

//==============================================================================
// inline definitions : static class : trigonometry
//==============================================================================

template<typename AngleUnit>
inline alloy::core::real
  alloy::core::trigonometry::cos( basic_angle<AngleUnit> angle )
  noexcept
{
  return cos( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline alloy::core::real
  alloy::core::trigonometry::sin( basic_angle<AngleUnit> angle )
  noexcept
{
  return sin( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline alloy::core::real
  alloy::core::trigonometry::tan( basic_angle<AngleUnit> angle )
  noexcept
{
  return tan( to_radian(angle) );
}

//------------------------------------------------------------------------------

template<typename AngleUnit>
inline alloy::core::real
  alloy::core::trigonometry::sec( basic_angle<AngleUnit> angle )
  noexcept
{
  return real{1} / cos( angle );
}

template<typename AngleUnit>
inline alloy::core::real
  alloy::core::trigonometry::csc( basic_angle<AngleUnit> angle )
  noexcept
{
  return real{1} / sin( angle );
}

template<typename AngleUnit>
inline alloy::core::real
  alloy::core::trigonometry::cot( basic_angle<AngleUnit> angle )
  noexcept
{
  return real{1} / tan( angle );
}

#endif /* ALLOY_CORE_MATH_TRIGONOMETRY_HPP */