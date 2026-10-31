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

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"

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
    static auto raw_noise(float x, float y) noexcept -> float;

    /// \brief Generates 3-dimensional raw simplex noise
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    /// \return the result of the raw noise
    static auto raw_noise(float x, float y, float z) noexcept -> float;

    //--------------------------------------------------------------------------

    /// \brief Generates 2-dimensional raw simplex noise
    ///
    /// \param pos the 2-dimensional vector position
    /// \return the result of the raw noise
    static auto raw_noise(const vector2& pos) noexcept -> float;

    /// \brief Generates 3-dimensional raw simplex noise
    ///
    /// \param pos the 3-dimensional vector position
    /// \return the result of the raw noise
    static auto raw_noise(const vector3& pos) noexcept -> float;

    //--------------------------------------------------------------------------
    // Raw Scaled Noise
    //--------------------------------------------------------------------------

    static auto scaled_raw_noise(float low, float high, float x, float y)
      noexcept -> float;

    static auto scaled_raw_noise(float low, float high, float x, float y, float z)
      noexcept -> float;

    static auto scaled_raw_noise(float low, float high, const vector2& pos)
      noexcept -> float;

    static auto scaled_raw_noise(float low, float high, const vector3& pos)
      noexcept -> float;

    //----------------------------------------------------------------------
    // Octave Noise
    //----------------------------------------------------------------------

    /// \brief Generates simplex noise as the sum of \p octaves layers of raw
    ///        noise
    ///
    /// Each octave doubles the frequency of the last, and scales its
    /// amplitude by \p persistence. The sum is normalized back into [-1, 1].
    ///
    /// \pre \p octaves is at least 1
    ///
    /// \param octaves the number of layers of noise
    /// \param persistence the amplitude of each octave relative to the last
    /// \param scale the frequency of the first octave
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \return the result of the octave noise
    static auto octave_noise(
      float octaves,
      float persistence,
      float scale,
      float x,
      float y
    ) noexcept -> float;

    /// \brief Generates simplex noise as the sum of \p octaves layers of raw
    ///        noise
    ///
    /// Each octave doubles the frequency of the last, and scales its
    /// amplitude by \p persistence. The sum is normalized back into [-1, 1].
    ///
    /// \pre \p octaves is at least 1
    ///
    /// \param octaves the number of layers of noise
    /// \param persistence the amplitude of each octave relative to the last
    /// \param scale the frequency of the first octave
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    /// \return the result of the octave noise
    static auto octave_noise(
      float octaves,
      float persistence,
      float scale,
      float x,
      float y,
      float z
    ) noexcept -> float;

    //----------------------------------------------------------------------

    static auto octave_noise(
      float octaves,
      float persistence,
      float scale,
      const vector2& pos
    ) noexcept -> float;

    static auto octave_noise(
      float octaves,
      float persistence,
      float scale,
      const vector3& pos
    ) noexcept -> float;

    //----------------------------------------------------------------------
    // Scaled Octave Noise
    //----------------------------------------------------------------------

    static auto scaled_octave_noise(
      float octaves,
      float persistence,
      float scale,
      float low,
      float high,
      float x,
      float y
    ) noexcept -> float;

    static auto scaled_octave_noise(
      float octaves,
      float persistence,
      float scale,
      float low,
      float high,
      float x,
      float y,
      float z
    ) noexcept -> float;

    //------------------------------------------------------------------------

    static auto scaled_octave_noise(
      float octaves,
      float persistence,
      float scale,
      float low,
      float high,
      const vector2& pos
    ) noexcept -> float;

    static auto scaled_octave_noise(
      float octaves,
      float persistence,
      float scale,
      float low,
      float high,
      const vector3& pos
    ) noexcept -> float;
  };

} // namespace alloy::core


//----------------------------------------------------------------------------
// Raw Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::raw_noise(const vector2& pos) noexcept -> float
{
  return raw_noise( pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::raw_noise(const vector3& pos) noexcept -> float
{
  return raw_noise( pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Raw Scaled Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(float low, float high, float x, float y)
  noexcept -> float
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * 0.5f);
  const auto c2 = ((high + low) * 0.5f);
  const auto result = c1 * raw_noise(x, y) + c2;
  return static_cast<float>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(
  float low,
  float high,
  float x,
  float y,
  float z
) noexcept -> float
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * 0.5f);
  const auto c2 = ((high + low) * 0.5f);
  const auto result = c1 * raw_noise(x, y, z) + c2;
  return static_cast<float>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(
  float low,
  float high,
  const vector2& pos
) noexcept -> float
{
  return scaled_raw_noise( low, high, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_raw_noise(
  float low,
  float high,
  const vector3& pos
) noexcept -> float
{
  return scaled_raw_noise( low, high, pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Octave Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::octave_noise(
  float octaves,
  float persistence,
  float scale,
  const vector2& pos
) noexcept -> float
{
  return octave_noise( octaves, persistence, scale, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::octave_noise(
  float octaves,
  float persistence,
  float scale,
  const vector3& pos
) noexcept -> float
{
  return octave_noise( octaves, persistence, scale, pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Scaled Octave Noise
//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  float octaves,
  float persistence,
  float scale,
  float low,
  float high,
  float x,
  float y
) noexcept -> float
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * 0.5f);
  const auto c2 = ((high + low) * 0.5f);
  const auto result = c1 * octave_noise( octaves, persistence, scale, x, y ) + c2;
  return static_cast<float>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  float octaves,
  float persistence,
  float scale,
  float low,
  float high,
  float x,
  float y,
  float z
) noexcept -> float
{
  ALLOY_ASSERT(low < high, "Low must be less than high");

  const auto c1 = ((high - low) * 0.5f);
  const auto c2 = ((high + low) * 0.5f);
  const auto result = c1 * octave_noise( octaves, persistence, scale, x, y, z ) + c2;
  return static_cast<float>(result);
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  float octaves,
  float persistence,
  float scale,
  float low,
  float high,
  const vector2& pos
) noexcept -> float
{
  return scaled_octave_noise( octaves, persistence, scale,
                              low, high, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline
auto alloy::core::simplex::scaled_octave_noise(
  float octaves,
  float persistence,
  float scale,
  float low,
  float high,
  const vector3& pos
) noexcept -> float
{
  return scaled_octave_noise( octaves, persistence, scale,
                              low, high, pos.x(), pos.y(), pos.z() );
}

#endif /* ALLOY_CORE_MATH_SIMPLEX_HPP */
