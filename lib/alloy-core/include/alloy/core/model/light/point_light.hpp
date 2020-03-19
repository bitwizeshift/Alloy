/*****************************************************************************
 * \file point_light .hpp
 *
 * \todo description
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
#ifndef ALLOY_CORE_MODEL_LIGHT_POINT_LIGHT_HPP
#define ALLOY_CORE_MODEL_LIGHT_POINT_LIGHT_HPP

#include "alloy/core/model/color.hpp"
#include "alloy/core/model/light/light.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/geometry/point.hpp"
#include "alloy/core/precision.hpp"

namespace alloy::core {

  //============================================================================
  // struct : point_light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A light emanating from a point in 3D
  ///
  /// Point lights begin from a specific point, and emit radially in all
  /// directions.
  //////////////////////////////////////////////////////////////////////////////
  struct point_light
  {
    point source;
    class color color;
    light::attenuation attenuation;
    light::intensity intensity;
  };

  //============================================================================
  // non-member functions : struct : point_light
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr bool operator==(const point_light& lhs,
                            const point_light& rhs) noexcept;
  constexpr bool operator!=(const point_light& lhs,
                            const point_light& rhs) noexcept;

  constexpr bool almost_equal(const point_light& lhs,
                              const point_light& rhs) noexcept;
  constexpr bool almost_equal(const point_light& lhs,
                              const point_light& rhs,
                              real tolerance) noexcept;
} // namespace alloy::core


//============================================================================
// inline definitions : non-member functions : struct : point_light
//============================================================================

//----------------------------------------------------------------------------
// Equality
//----------------------------------------------------------------------------

#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

inline constexpr bool alloy::core::operator==(const point_light& lhs,
                                              const point_light& rhs)
  noexcept
{
  return lhs.source == rhs.source &&
         lhs.color == rhs.color &&
         lhs.attenuation == rhs.attenuation &&
         lhs.intensity == rhs.intensity;
}


inline constexpr bool alloy::core::operator!=(const point_light& lhs,
                                              const point_light& rhs)
  noexcept
{
  return !(lhs == rhs);
}

#if defined(__clang__)
# pragma clang diagnostic pop
#elif defined(__GNUC__)
# pragma GCC diagnostic pop
#endif

inline constexpr bool alloy::core::almost_equal(const point_light& lhs,
                                                const point_light& rhs)
  noexcept
{
  return almost_equal(lhs, rhs, default_tolerance);
}


inline constexpr bool alloy::core::almost_equal(const point_light& lhs,
                                                const point_light& rhs,
                                                real tolerance)
  noexcept
{
  return almost_equal(lhs.source, rhs.source, tolerance) &&
         almost_equal(lhs.color, rhs.color, tolerance) &&
         almost_equal(lhs.attenuation, rhs.attenuation, tolerance) &&
         almost_equal(lhs.intensity, rhs.intensity, tolerance);
}

#endif /* ALLOY_CORE_MODEL_LIGHT_POINT_LIGHT_HPP */
