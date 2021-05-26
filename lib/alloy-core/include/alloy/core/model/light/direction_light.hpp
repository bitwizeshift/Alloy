/*****************************************************************************
 * \file direction_light.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew ALLOY_CORE_MODEL_ All rights reserved.

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
#ifndef ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP
#define ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP

#include "alloy/core/model/color.hpp"
#include "alloy/core/model/light/light.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/geometry/point3.hpp"
#include "alloy/core/precision.hpp"
#include "alloy/core/intrinsics.hpp"

namespace alloy::core {

  //============================================================================
  // struct : direction_light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A light that shines in a specific direction, but has no specific
  ///        source location
  //////////////////////////////////////////////////////////////////////////////
  struct direction_light
  {
    vector3 direction;
    class color color;
    light::attenuation attenuation;
    light::intensity intensity;
  };

  //============================================================================
  // non-member functions : struct : direction_light
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr bool operator==(const direction_light& lhs,
                            const direction_light& rhs) noexcept;
  constexpr bool operator!=(const direction_light& lhs,
                            const direction_light& rhs) noexcept;

  constexpr bool almost_equal(const direction_light& lhs,
                              const direction_light& rhs) noexcept;
  constexpr bool almost_equal(const direction_light& lhs,
                              const direction_light& rhs,
                              real tolerance) noexcept;
} // namespace alloy::core


//============================================================================
// inline definitions : non-member functions : struct : direction_light
//============================================================================

//----------------------------------------------------------------------------
// Equality
//----------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==(const direction_light& lhs,
                                              const direction_light& rhs)
  noexcept
{
  return lhs.intensity == rhs.intensity &&
         lhs.attenuation == rhs.attenuation &&
         lhs.color == rhs.color &&
         lhs.direction == rhs.direction;
}


inline constexpr bool alloy::core::operator!=(const direction_light& lhs,
                                              const direction_light& rhs)
  noexcept
{
  return !(lhs == rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

inline constexpr bool alloy::core::almost_equal(const direction_light& lhs,
                                                const direction_light& rhs)
  noexcept
{
  return almost_equal(lhs, rhs, default_tolerance);
}


inline constexpr bool alloy::core::almost_equal(const direction_light& lhs,
                                                const direction_light& rhs,
                                                real tolerance)
  noexcept
{
  return almost_equal(lhs.intensity, rhs.intensity, tolerance) &&
         almost_equal(lhs.attenuation, rhs.attenuation, tolerance) &&
         almost_equal(lhs.color, rhs.color, tolerance) &&
         almost_equal(lhs.direction, rhs.direction, tolerance);
}

#endif /* ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP */
