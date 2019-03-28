/*****************************************************************************
 * \file simplex.hpp
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
#ifndef ALLOY_CORE_MATH_SIMPLEX_HPP
#define ALLOY_CORE_MATH_SIMPLEX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"

#include <cassert>

// source:
// http://webstaff.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf

namespace alloy::core {

  struct simplex final
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
    static core::real raw_noise( core::real x,
                                 core::real y ) noexcept;

    /// \brief Generates 3-dimensional raw simplex noise
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    /// \return the result of the raw noise
    static core::real raw_noise( core::real x,
                                 core::real y,
                                 core::real z ) noexcept;

    //--------------------------------------------------------------------------

    /// \brief Generates 2-dimensional raw simplex noise
    ///
    /// \param pos the 2-dimensional vector position
    /// \return the result of the raw noise
    static core::real raw_noise( const vector2& pos ) noexcept;

    /// \brief Generates 3-dimensional raw simplex noise
    ///
    /// \param pos the 3-dimensional vector position
    /// \return the result of the raw noise
    static core::real raw_noise( const vector3& pos ) noexcept;

    //--------------------------------------------------------------------------
    // Raw Scaled Noise
    //--------------------------------------------------------------------------

    static core::real scaled_raw_noise( core::real low,
                                        core::real high,
                                        core::real x,
                                        core::real y ) noexcept;

    static core::real scaled_raw_noise( core::real low,
                                        core::real high,
                                        core::real x,
                                        core::real y,
                                        core::real z ) noexcept;

    static core::real scaled_raw_noise( core::real low,
                                        core::real high,
                                        const vector2& pos ) noexcept;

    static core::real scaled_raw_noise( core::real low,
                                        core::real high,
                                        const vector3& pos ) noexcept;

    //----------------------------------------------------------------------
    // Octave Noise
    //----------------------------------------------------------------------

    static core::real octave_noise( core::real octaves,
                                    core::real persistence,
                                    core::real scale,
                                    core::real x,
                                    core::real y ) noexcept;

    static core::real octave_noise( core::real octaves,
                                    core::real persistence,
                                    core::real scale,
                                    core::real x,
                                    core::real y,
                                    core::real z ) noexcept;

    //----------------------------------------------------------------------

    static core::real octave_noise( core::real octaves,
                                    core::real persistence,
                                    core::real scale,
                                    const vector2& pos ) noexcept;

    static core::real octave_noise( core::real octaves,
                                    core::real persistence,
                                    core::real scale,
                                    const vector3& pos ) noexcept;

    //----------------------------------------------------------------------
    // Scaled Octave Noise
    //----------------------------------------------------------------------

    static core::real scaled_octave_noise( core::real octaves,
                                           core::real persistence,
                                           core::real scale,
                                           core::real low,
                                           core::real high,
                                           core::real x,
                                           core::real y ) noexcept;

    static core::real scaled_octave_noise( core::real octaves,
                                           core::real persistence,
                                           core::real scale,
                                           core::real low,
                                           core::real high,
                                           core::real x,
                                           core::real y,
                                           core::real z ) noexcept;

    //------------------------------------------------------------------------

    static core::real scaled_octave_noise( core::real octaves,
                                           core::real persistence,
                                           core::real scale,
                                           core::real low,
                                           core::real high,
                                           const vector2& pos ) noexcept;

    static core::real scaled_octave_noise( core::real octaves,
                                           core::real persistence,
                                           core::real scale,
                                           core::real low,
                                           core::real high,
                                           const vector3& pos ) noexcept;

  };

} // namespace alloy::core


//----------------------------------------------------------------------------
// Raw Noise
//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::raw_noise( const vector2& pos )
  noexcept
{
  return raw_noise( pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::raw_noise( const vector3& pos )
  noexcept
{
  return raw_noise( pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Raw Scaled Noise
//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::scaled_raw_noise( core::real low,
                                          core::real high,
                                          core::real x,
                                          core::real y )
  noexcept
{
  assert( low < high && "Low must be less than high" );

  const auto c1 = ((high - low) * 0.5);
  const auto c2 = ((high + low) * 0.5);
  return c1 * raw_noise(x, y) + c2;
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::scaled_raw_noise( core::real low,
                                          core::real high,
                                          core::real x,
                                          core::real y,
                                          core::real z )
  noexcept
{
  assert( low < high && "Low must be less than high" );

  const auto c1 = ((high - low) * 0.5);
  const auto c2 = ((high + low) * 0.5);
  return c1 * raw_noise(x, y, z) + c2;
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::scaled_raw_noise( core::real low,
                                          core::real high,
                                          const vector2& pos )
  noexcept
{
  return scaled_raw_noise( low, high, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::scaled_raw_noise( core::real low,
                                          core::real high,
                                          const vector3& pos )
  noexcept
{
  return scaled_raw_noise( low, high, pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Octave Noise
//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::octave_noise( core::real octaves,
                                      core::real persistence,
                                      core::real scale,
                                      const vector2& pos )
  noexcept
{
  return octave_noise( octaves, persistence, scale, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::octave_noise( core::real octaves,
                                      core::real persistence,
                                      core::real scale,
                                      const vector3& pos )
  noexcept
{
  return octave_noise( octaves, persistence, scale, pos.x(), pos.y(), pos.z() );
}

//----------------------------------------------------------------------------
// Scaled Octave Noise
//----------------------------------------------------------------------------


inline alloy::core::real
  alloy::core::simplex::scaled_octave_noise( core::real octaves,
                                             core::real persistence,
                                             core::real scale,
                                             core::real low,
                                             core::real high,
                                             core::real x,
                                             core::real y )
  noexcept
{
  assert( low < high && "Low must be less than high" );

  const auto c1 = ((high - low) * 0.5);
  const auto c2 = ((high + low) * 0.5);
  return c1 * octave_noise( octaves, persistence, scale, x, y ) + c2;
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::scaled_octave_noise( core::real octaves,
                                             core::real persistence,
                                             core::real scale,
                                             core::real low,
                                             core::real high,
                                             core::real x,
                                             core::real y,
                                             core::real z )
  noexcept
{
  assert( low < high && "Low must be less than high" );

  const auto c1 = ((high - low) * 0.5);
  const auto c2 = ((high + low) * 0.5);
  return c1 * octave_noise( octaves, persistence, scale, x, y, z ) + c2;
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::scaled_octave_noise( core::real octaves,
                                             core::real persistence,
                                             core::real scale,
                                             core::real low,
                                             core::real high,
                                             const vector2& pos )
  noexcept
{
  return scaled_octave_noise( octaves, persistence, scale,
                              low, high, pos.x(), pos.y() );
}

//----------------------------------------------------------------------------

inline alloy::core::real
  alloy::core::simplex::scaled_octave_noise( core::real octaves,
                                             core::real persistence,
                                             core::real scale,
                                             core::real low,
                                             core::real high,
                                             const vector3& pos )
  noexcept
{
  return scaled_octave_noise( octaves, persistence, scale,
                              low, high, pos.x(), pos.y(), pos.z() );
}

#endif /* ALLOY_CORE_MATH_SIMPLEX_HPP */