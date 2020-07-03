/*****************************************************************************
 * \file light.hpp
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
#ifndef ALLOY_CORE_MODEL_LIGHT_LIGHT_HPP
#define ALLOY_CORE_MODEL_LIGHT_LIGHT_HPP

#include "alloy/core/precision.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/intrinsics.hpp"

namespace alloy::core {

  //============================================================================
  // struct : light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A struct used for storing semantic sub-types used for lighting
  //////////////////////////////////////////////////////////////////////////////
  struct light {

    //==========================================================================
    // struct : light::attenuation
    //==========================================================================

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Attenuation values for light cutoff
    ////////////////////////////////////////////////////////////////////////////
    struct attenuation
    {
      real constant;
      real linear;
      real exponential;
    };

    //==========================================================================
    // struct : light::intensity
    //==========================================================================

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Intensity values for color
    ////////////////////////////////////////////////////////////////////////////
    struct intensity
    {
      real ambient;
      real diffuse;
    };
  };

  //============================================================================
  // non-member functions : struct : light::attenuation
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr bool operator==(const light::attenuation& lhs,
                            const light::attenuation& rhs) noexcept;
  constexpr bool operator!=(const light::attenuation& lhs,
                            const light::attenuation& rhs) noexcept;

  constexpr bool almost_equal(const light::attenuation& lhs,
                              const light::attenuation& rhs) noexcept;
  constexpr bool almost_equal(const light::attenuation& lhs,
                              const light::attenuation& rhs,
                              real tolerance) noexcept;

  //============================================================================
  // non-member functions : struct : light::intensity
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr bool operator==(const light::intensity& lhs,
                            const light::intensity& rhs) noexcept;
  constexpr bool operator!=(const light::intensity& lhs,
                            const light::intensity& rhs) noexcept;

  constexpr bool almost_equal(const light::intensity& lhs,
                              const light::intensity& rhs) noexcept;
  constexpr bool almost_equal(const light::intensity& lhs,
                              const light::intensity& rhs,
                              real tolerance) noexcept;
} // namespace alloy::core

//==============================================================================
// inline definitions : non-member functions : struct : light::attenuation
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==(const light::attenuation& lhs,
                                              const light::attenuation& rhs)
  noexcept
{
  return lhs.constant == rhs.constant &&
         lhs.linear == rhs.linear &&
         lhs.exponential == rhs.exponential;
}


inline constexpr bool alloy::core::operator!=(const light::attenuation& lhs,
                                              const light::attenuation& rhs)
  noexcept
{
  return !(lhs == rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

inline constexpr bool alloy::core::almost_equal(const light::attenuation& lhs,
                                                const light::attenuation& rhs)
  noexcept
{
  return almost_equal(lhs, rhs, default_tolerance);
}


inline constexpr bool alloy::core::almost_equal(const light::attenuation& lhs,
                                                const light::attenuation& rhs,
                                                real tolerance)
  noexcept
{
  return almost_equal(lhs.constant, rhs.constant, tolerance) &&
         almost_equal(lhs.linear, rhs.linear, tolerance) &&
         almost_equal(lhs.exponential, rhs.exponential, tolerance);
}

//==============================================================================
// inline definitions : non-member functions : struct : light::intensity
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==(const light::intensity& lhs,
                                              const light::intensity& rhs)
  noexcept
{
  return lhs.ambient == rhs.ambient && lhs.diffuse == rhs.diffuse;
}


inline constexpr bool alloy::core::operator!=(const light::intensity& lhs,
                                              const light::intensity& rhs)
  noexcept
{
  return !(lhs == rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

inline constexpr bool alloy::core::almost_equal(const light::intensity& lhs,
                                                const light::intensity& rhs)
  noexcept
{
  return almost_equal(lhs, rhs, default_tolerance);
}


inline constexpr bool alloy::core::almost_equal(const light::intensity& lhs,
                                                const light::intensity& rhs,
                                                real tolerance)
  noexcept
{
  return almost_equal(lhs.ambient, rhs.ambient, tolerance) &&
         almost_equal(lhs.diffuse, rhs.diffuse, tolerance);
}

#endif /* ALLOY_CORE_MODEL_LIGHT_LIGHT_HPP */