/****************************************************************************
 * \file interpolation.hpp
 *
 * \brief This header contains algorithms for various types of interpolation
 ****************************************************************************/

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
#ifndef ALLOY_CORE_MATH_INTERPOLATION_HPP
#define ALLOY_CORE_MATH_INTERPOLATION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/math/math_constants.hpp" // math_constants::half_pi, etc
#include "alloy/core/math/trigonometry.hpp"   // trigonometry::cos, etc
#include "alloy/core/assert.hpp"

#include <type_traits> // std::common_type

namespace alloy::core {

  //============================================================================
  // struct : interpolator
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A template-specializable utility to interpolate between two values
  ///
  /// \tparam T The type to interpolate
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct interpolator
  {
    /// \brief Interpolates the values between `v0` and `v1`
    ///
    /// By default, interpolation will use the `+` and `*` operators for a
    /// standard lerp operation of `v0 * (1 - alpha) + v1 * alpha
    ///
    /// \param v0 the start state
    /// \param v1 the end state
    /// \param alpha the amount to interpolate [0, 1]
    /// \return the interpolated value
    template <typename Alpha>
    constexpr
    auto operator()(const T& v0, const T& v1, const Alpha& alpha) noexcept -> T;
  };

  //===========================================================================
  // static class : interpolation
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A pure static-class that contains interpolation functions.
  ///
  /// All interpolation functions are marked constexpr, and take generic
  /// template parameters in order to allow for the best optimizations and
  /// flexibility.
  /////////////////////////////////////////////////////////////////////////////
  struct interpolation
  {
    interpolation() = delete;
    ~interpolation() = delete;

    /// \brief Interpolates a point between \p v0 and \p v1 at position
    ///        \p t after being mutated by \p func
    ///
    /// \param v0   The starting point
    /// \param v1   The ending point
    /// \param t    The area in the interval to interpolate to [0,1]
    /// \param func Function to operate on \p t
    /// \return the result of the interpolation
    template <typename V, typename T, typename Func>
    static constexpr auto interpolate(const V& v0,
                                      const V& v1,
                                      const T& t, Func func)
      noexcept -> V;

    //------------------------------------------------------------------------

    /// \brief Linearly interpolates a point between \p v0 and \p v1 at
    ///         position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static constexpr auto linear(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    /// \brief Quadratically interpolates a point between \p v0 and \p v1 at
    ///        position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static constexpr auto quadratic(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    /// \brief Cubically interpolates a point between \p v0 and \p v1 at
    ///        position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static constexpr auto cubic(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    /// \brief Quartically interpolates a point between \p v0 and \p v1 at
    ///        position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static constexpr auto quartic(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    /// \brief Performs quintic linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static constexpr auto quintic(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    //------------------------------------------------------------------------

    /// \brief Performs circular linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static auto circular(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    //------------------------------------------------------------------------

    /// \brief Performs half-cosine linear interpolation on a point between
    ///        \p v0 and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static auto half_cosine(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    /// \brief Performs cosine linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static auto cosine(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    //------------------------------------------------------------------------

    /// \brief Performs half-sine linear interpolation on a point between
    ///        \p v0 and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static auto half_sine(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    /// \brief Performs sine linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template <typename V, typename T>
    static auto sine(const V& v0, const V& v1, const T& t)
      noexcept -> V;

    //------------------------------------------------------------------------

    template <typename V, typename T>
    static constexpr auto bilinear(const V& v00,
                                   const V& v10,
                                   const V& v01,
                                   const V& v11,
                                   const T& tx,
                                   const T& ty)
      noexcept -> V;

    template <typename V, typename T>
    static constexpr auto trilinear(const V& v000,
                                    const V& v100,
                                    const V& v010,
                                    const V& v110,
                                    const V& v001,
                                    const V& v101,
                                    const V& v011,
                                    const V& v111,
                                    const T& tx,
                                    const T& ty,
                                    const T& tz)
      noexcept -> V;
  };

} // namespace alloy::core

//=============================================================================
// struct : interpolator
//=============================================================================

template <typename T>
template <typename Alpha>
inline constexpr
auto alloy::core::interpolator<T>::operator()(const T& v0, const T& v1, const Alpha& alpha)
  noexcept -> T
{
  ALLOY_ASSERT((alpha <= Alpha{1}) && (alpha >= Alpha{0}));
  const auto alpha_prime = (Alpha{1} - alpha);

  return (v0 * alpha_prime) + (v1 * alpha);
}

//=============================================================================
// static class : interpolation
//=============================================================================

template <typename V, typename T, typename Func>
inline constexpr
auto alloy::core::interpolation::interpolate(const V& v0,
                                             const V& v1,
                                             const T& t,
                                             Func func)
  noexcept -> V
{
  static_assert( std::is_nothrow_invocable<Func,T>::value, "" );

  return linear(v0, v1, func(t));
}

//----------------------------------------------------------------------------

template <typename V, typename T>
inline constexpr
auto alloy::core::interpolation::linear(const V& v0, const V& v1, const T& t)
  noexcept -> V
{
  return interpolator<V>{}(v0, v1, t);
}

template <typename V, typename T>
inline constexpr
auto alloy::core::interpolation::quadratic(const V& v0,
                                           const V& v1,
                                           const T& t)
  noexcept -> V
{
  return linear(v0,v1,t*t);
}

template <typename V, typename T>
inline constexpr
auto alloy::core::interpolation::cubic(const V& v0,
                                       const V& v1,
                                       const T& t)
  noexcept -> V
{
  return linear(v0,v1,t*t*t);
}

template <typename V, typename T>
inline constexpr
auto alloy::core::interpolation::quartic(const V& v0,
                                         const V& v1,
                                         const T& t)
  noexcept -> V
{
  return linear(v0,v1,t*t*t*t);
}

template <typename V, typename T>
inline constexpr
auto alloy::core::interpolation::quintic(const V& v0,
                                         const V& v1,
                                         const T& t)
  noexcept -> V
{
  return linear(v0,v1,t*t*t*t*t);
}

//----------------------------------------------------------------------------

template <typename V, typename T>
inline
auto alloy::core::interpolation::circular(const V& v0,
                                          const V& v1,
                                          const T& t)
  noexcept -> V
{
  return linear(v0, v1, (1.0 - sqrt(1.0 - (t*t))));
}

//----------------------------------------------------------------------------

template <typename V, typename T>
inline
auto alloy::core::interpolation::half_cosine(const V& v0,
                                             const V& v1,
                                             const T& t)
  noexcept -> V
{
  return linear(v0, v1, (0.5 - trigonometry::cos(t * math_constants::half_pi)));
}

template <typename V, typename T>
inline
auto alloy::core::interpolation::cosine(const V& v0,
                                        const V& v1,
                                        const T& t)
  noexcept -> V
{
  return linear(v0, v1, (1.0 - trigonometry::cos(t * math_constants::pi)));
}

//----------------------------------------------------------------------------

template <typename V, typename T>
inline
auto alloy::core::interpolation::half_sine(const V& v0,
                                           const V& v1,
                                           const T& t)
  noexcept -> V
{
  return linear(v0, v1, (0.5 - trigonometry::sin(t * math_constants::half_pi)));
}

template <typename V, typename T>
inline
auto alloy::core::interpolation::sine(const V& v0,
                                      const V& v1,
                                      const T& t)
  noexcept -> V
{
  return linear(v0, v1, (1.0 - trigonometry::sin(t * math_constants::pi)));
}

//-----------------------------------------------------------------------------

template <typename V, typename T>
inline constexpr
auto alloy::core::interpolation::bilinear(const V& v00,
                                          const V& v10,
                                          const V& v01,
                                          const V& v11,
                                          const T& tx,
                                          const T& ty)
  noexcept -> V
{
  return linear( linear(v00,v10,tx), linear(v01,v11,tx), ty );
}

template <typename V, typename T>
inline constexpr
auto alloy::core::interpolation::trilinear(const V& v000,
                                           const V& v100,
                                           const V& v010,
                                           const V& v110,
                                           const V& v001,
                                           const V& v101,
                                           const V& v011,
                                           const V& v111,
                                           const T& tx,
                                           const T& ty,
                                           const T& tz)
  noexcept -> V
{
  return linear(
    bilinear(v000, v100, v010, v110, tx, ty),
    bilinear(v001, v101, v011, v111, tx, ty),
    tz
  );
}

#endif /* ALLOY_CORE_MATH_INTERPOLATION_HPP */
