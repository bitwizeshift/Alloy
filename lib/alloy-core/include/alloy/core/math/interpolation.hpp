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

#include <type_traits> // std::common_type

namespace alloy::core {

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
    template<typename V0, typename V1, typename T, typename Func>
    static constexpr std::common_type_t<V0,V1,T>
      interpolate( const V0& v0, const V1& v1, const T& t, Func func  ) noexcept;

    //------------------------------------------------------------------------

    /// \brief Linearly interpolates a point between \p v0 and \p v1 at
    ///         position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static constexpr std::common_type_t<V0,V1,T>
      linear( const V0& v0, const V1& v1, const T& t ) noexcept;

    /// \brief Quadratically interpolates a point between \p v0 and \p v1 at
    ///        position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static constexpr std::common_type_t<V0,V1,T>
      quadratic( const V0& v0, const V1& v1, const T& t ) noexcept;

    /// \brief Cubically interpolates a point between \p v0 and \p v1 at
    ///        position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static constexpr std::common_type_t<V0,V1,T>
      cubic( const V0& v0, const V0& v1, const T& t ) noexcept;

    /// \brief Quartically interpolates a point between \p v0 and \p v1 at
    ///        position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static constexpr std::common_type_t<V0,V1,T>
      quartic( const V0& v0, const V0& v1, const T& t ) noexcept;

    /// \brief Performs quintic linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static constexpr std::common_type_t<V0,V1,T>
      quintic( const V0& v0, const V0& v1, const T& t ) noexcept;

    //------------------------------------------------------------------------

    /// \brief Performs circular linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static std::common_type_t<V0,V1,T>
      circular( const V0& v0, const V0& v1, const T& t ) noexcept;

    //------------------------------------------------------------------------

    /// \brief Performs half-cosine linear interpolation on a point between
    ///        \p v0 and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static std::common_type_t<V0,V1,T>
      half_cosine( const V0& v0, const V0& v1, const T& t ) noexcept;

    /// \brief Performs cosine linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static std::common_type_t<V0,V1,T>
      cosine( const V0& v0, const V0& v1, const T& t ) noexcept;

    //------------------------------------------------------------------------

    /// \brief Performs half-sine linear interpolation on a point between
    ///        \p v0 and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static std::common_type_t<V0,V1,T>
      half_sine( const V0& v0, const V0& v1, const T& t ) noexcept;

    /// \brief Performs sine linear interpolation on a point between \p v0
    ///        and \p v1 at position \p t
    ///
    /// \param v0 The starting point
    /// \param v1 The ending point
    /// \param t  The area in the interval to interpolate to [0,1]
    /// \return the result of the interpolation
    template<typename V0, typename V1, typename T>
    static std::common_type_t<V0,V1,T>
      sine( const V0& v0, const V0& v1, const T& t ) noexcept;

    //------------------------------------------------------------------------

    template<typename V00, typename V10, typename V01,
             typename V11, typename Tx, typename Ty>
    static constexpr std::common_type_t<V00,V10,V01,V11,Tx,Ty>
      bilinear( const V00& v00, const V10 v10, const V01& v01,
                const V11& v11, const Tx& tx, const Ty& ty ) noexcept;

    template<typename V000, typename V100, typename V010, typename V110,
             typename V001, typename V101, typename V011, typename V111,
             typename Tx, typename Ty, typename Tz>
    static constexpr std::common_type_t<V000,V100,V010,V110,V001,V101,V011,V111>
      trilinear( const V000& v000, const V100& v100, const V010& v010,
                 const V110& v110, const V001& v001, const V101& v101,
                 const V011& v011, const V111& v111,
                 const Tx& tx, const Ty& ty, const Tz& tz ) noexcept;
  };

} // namespace alloy::core

//=============================================================================
// static class : interpolation
//=============================================================================

template<typename V0, typename V1, typename T, typename Func>
inline constexpr std::common_type_t<V0,V1,T>
  alloy::core::interpolation::interpolate( const V0& v0,
                                           const V1& v1,
                                           const T& t,
                                           Func func  )
  noexcept
{
  static_assert( std::is_nothrow_invocable<Func,T>::value, "" );

  auto r = func(t);
  return v0 * (1 - r) + v1 * r;
}

//----------------------------------------------------------------------------

template<typename V0, typename V1, typename T>
inline constexpr std::common_type_t<V0,V1,T>
  alloy::core::interpolation::linear( const V0& v0, const V1& v1, const T& t )
  noexcept
{
  return v0 * (1 - t) + v1 * t;
}


template<typename V0, typename V1, typename T>
inline constexpr std::common_type_t<V0,V1,T>
  alloy::core::interpolation::quadratic( const V0& v0, const V1& v1, const T& t )
  noexcept
{
  return linear(v0,v1,t*t);
}


template<typename V0, typename V1, typename T>
inline constexpr std::common_type_t<V0,V1,T>
  alloy::core::interpolation::cubic( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0,v1,t*t*t);
}


template<typename V0, typename V1, typename T>
inline constexpr std::common_type_t<V0,V1,T>
  alloy::core::interpolation::quartic( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0,v1,t*t*t*t);
}


template<typename V0, typename V1, typename T>
inline constexpr std::common_type_t<V0,V1,T>
  alloy::core::interpolation::quintic( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0,v1,t*t*t*t*t);
}

//----------------------------------------------------------------------------

template<typename V0, typename V1, typename T>
inline std::common_type_t<V0,V1,T>
  alloy::core::interpolation::circular( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0, v1, (1.0 - sqrt(1.0 - (t*t))));
}

//----------------------------------------------------------------------------

template<typename V0, typename V1, typename T>
inline std::common_type_t<V0,V1,T>
  alloy::core::interpolation::half_cosine( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0, v1, (0.5 - trigonometry::cos(t * math_constants::half_pi)));
}


template<typename V0, typename V1, typename T>
inline std::common_type_t<V0,V1,T>
  alloy::core::interpolation::cosine( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0, v1, (1.0 - trigonometry::cos(t * math_constants::pi)));
}

//----------------------------------------------------------------------------

template<typename V0, typename V1, typename T>
inline std::common_type_t<V0,V1,T>
  alloy::core::interpolation::half_sine( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0, v1, (0.5 - trigonometry::sin(t * math_constants::half_pi)));
}


template<typename V0, typename V1, typename T>
inline std::common_type_t<V0,V1,T>
  alloy::core::interpolation::sine( const V0& v0, const V0& v1, const T& t )
  noexcept
{
  return linear(v0, v1, (1.0 - trigonometry::sin(t * math_constants::pi)));
}

//-----------------------------------------------------------------------------

template<typename V00, typename V10, typename V01,
         typename V11, typename Tx, typename Ty>
inline constexpr std::common_type_t<V00,V10,V01,V11,Tx,Ty>
  alloy::core::interpolation::bilinear( const V00& v00,
                                        const V10 v10,
                                        const V01& v01,
                                        const V11& v11,
                                        const Tx& tx,
                                        const Ty& ty )
  noexcept
{
  return linear( linear(v00,v10,tx), linear(v01,v11,tx), ty );
}

template<typename V000, typename V100, typename V010, typename V110,
         typename V001, typename V101, typename V011, typename V111,
         typename Tx, typename Ty, typename Tz>
inline constexpr std::common_type_t<V000,V100,V010,V110,V001,V101,V011,V111>
  alloy::core::interpolation::trilinear( const V000& v000,
                                         const V100& v100,
                                         const V010& v010,
                                         const V110& v110,
                                         const V001& v001,
                                         const V101& v101,
                                         const V011& v011,
                                         const V111& v111,
                                         const Tx& tx,
                                         const Ty& ty,
                                         const Tz& tz )
  noexcept
{
  return linear(
    bilinear(v000, v100, v010, v110, tx, ty),
    bilinear(v001, v101, v011, v111, tx, ty),
    tz
  );
}

#endif /* ALLOY_CORE_MATH_INTERPOLATION_HPP */
