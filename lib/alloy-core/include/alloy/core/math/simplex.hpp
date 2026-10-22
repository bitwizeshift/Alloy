// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file simplex.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_MATH_SIMPLEX_HPP
#define ALLOY_CORE_MATH_SIMPLEX_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/precision.hpp"

#include <cassert>

// source:
// http://webstaff.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf

namespace alloy::core {

  struct ALLOY_CORE_API simplex final
  {
    ~simplex() = delete;

    //--------------------------------------------------------------------------
    // Raw Noise
    //--------------------------------------------------------------------------

    /// \brief Generates 2-dimensional raw simplex noise
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \return the result of the raw noise
    static auto raw_noise(real x, real y) noexcept -> real;

    /// \brief Generates 3-dimensional raw simplex noise
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    /// \return the result of the raw noise
    static auto raw_noise(real x, real y, real z) noexcept -> real;

    //--------------------------------------------------------------------------

    /// \brief Generates 2-dimensional raw simplex noise
    ///
    /// \param pos the 2-dimensional vector position
    /// \return the result of the raw noise
    static auto raw_noise(const vector2& pos) noexcept -> real;

    /// \brief Generates 3-dimensional raw simplex noise
    ///
    /// \param pos the 3-dimensional vector position
    /// \return the result of the raw noise
    static auto raw_noise(const vector3& pos) noexcept -> real;

    //--------------------------------------------------------------------------
    // Raw Scaled Noise
    //--------------------------------------------------------------------------

    static auto scaled_raw_noise(real low, real high, real x, real y)
      noexcept -> real;

    static auto scaled_raw_noise(real low, real high, real x, real y, real z)
      noexcept -> real;

    static auto scaled_raw_noise(real low, real high, const vector2& pos)
      noexcept -> real;

    static auto scaled_raw_noise(real low, real high, const vector3& pos)
      noexcept -> real;

    //----------------------------------------------------------------------
    // Octave Noise
    //----------------------------------------------------------------------

    static auto octave_noise(
      real octaves,
      real persistence,
      real scale,
      real x,
      real y
    ) noexcept -> real;

    static auto octave_noise(
      real octaves,
      real persistence,
      real scale,
      real x,
      real y,
      real z
    ) noexcept -> real;

    //----------------------------------------------------------------------

    static auto octave_noise(
      real octaves,
      real persistence,
      real scale,
      const vector2& pos
    ) noexcept -> real;

    static auto octave_noise(
      real octaves,
      real persistence,
      real scale,
      const vector3& pos
    ) noexcept -> real;

    //----------------------------------------------------------------------
    // Scaled Octave Noise
    //----------------------------------------------------------------------

    static auto scaled_octave_noise(
      real octaves,
      real persistence,
      real scale,
      real low,
      real high,
      real x,
      real y
    ) noexcept -> real;

    static auto scaled_octave_noise(
      real octaves,
      real persistence,
      real scale,
      real low,
      real high,
      real x,
      real y,
      real z
    ) noexcept -> real;

    //------------------------------------------------------------------------

    static auto scaled_octave_noise(
      real octaves,
      real persistence,
      real scale,
      real low,
      real high,
      const vector2& pos
    ) noexcept -> real;

    static auto scaled_octave_noise(
      real octaves,
      real persistence,
      real scale,
      real low,
      real high,
      const vector3& pos
    ) noexcept -> real;
  };

} // namespace alloy::core


//----------------------------------------------------------------------------
// Raw Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::raw_noise(const vector2& pos) noexcept -> real
{
  return raw_noise( pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::raw_noise(const vector3& pos) noexcept -> real
{
  return raw_noise( pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Raw Scaled Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(real low, real high, real x, real y)
  noexcept -> real
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * real{0.5});
  const auto c2 = ((high + low) * real{0.5});
  const auto result = c1 * raw_noise(x, y) + c2;
  return static_cast<real>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(
  real low,
  real high,
  real x,
  real y,
  real z
) noexcept -> real
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * real{0.5});
  const auto c2 = ((high + low) * real{0.5});
  const auto result = c1 * raw_noise(x, y, z) + c2;
  return static_cast<real>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(
  real low,
  real high,
  const vector2& pos
) noexcept -> real
{
  return scaled_raw_noise( low, high, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(
  real low,
  real high,
  const vector3& pos
) noexcept -> real
{
  return scaled_raw_noise( low, high, pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Octave Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::octave_noise(
  real octaves,
  real persistence,
  real scale,
  const vector2& pos
) noexcept -> real
{
  return octave_noise( octaves, persistence, scale, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::octave_noise(
  real octaves,
  real persistence,
  real scale,
  const vector3& pos
) noexcept -> real
{
  return octave_noise( octaves, persistence, scale, pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Scaled Octave Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  real octaves,
  real persistence,
  real scale,
  real low,
  real high,
  real x,
  real y
) noexcept -> real
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * real{0.5});
  const auto c2 = ((high + low) * real{0.5});
  const auto result = c1 * octave_noise( octaves, persistence, scale, x, y ) + c2;
  return static_cast<real>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  real octaves,
  real persistence,
  real scale,
  real low,
  real high,
  real x,
  real y,
  real z
) noexcept -> real
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * real{0.5});
  const auto c2 = ((high + low) * real{0.5});
  const auto result = c1 * octave_noise( octaves, persistence, scale, x, y, z ) + c2;
  return static_cast<real>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  real octaves,
  real persistence,
  real scale,
  real low,
  real high,
  const vector2& pos
) noexcept -> real
{
  return scaled_octave_noise( octaves, persistence, scale,
                              low, high, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  real octaves,
  real persistence,
  real scale,
  real low,
  real high,
  const vector3& pos
) noexcept -> real
{
  return scaled_octave_noise( octaves, persistence, scale,
                              low, high, pos.x(), pos.y(), pos.z() );
}

#endif /* ALLOY_CORE_MATH_SIMPLEX_HPP */
