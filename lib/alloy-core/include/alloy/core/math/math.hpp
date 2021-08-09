/****************************************************************************
 * \file math.hpp
 *
 * \brief This header defines the miscellaneous math functions that can't
 *        really be grouped elsewhere.
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
#ifndef ALLOY_CORE_MATH_MATH_HPP
#define ALLOY_CORE_MATH_MATH_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

// TODO(bitwize): Come up with a better group name than 'math' for this.

#include "alloy/core/precision.hpp" // core::real

#include <type_traits> // std::common_type, std::conditional_t, etc
#include <cstddef>     // std::size_t
#include <cmath>       // std::fmod

namespace alloy::core {

  //===========================================================================
  // trait : math_result
  //===========================================================================

  template <typename T>
  using math_result = std::conditional<
    std::is_floating_point<T>::value,
    T,
    core::real
  >;

  template <typename T>
  using math_result_t = typename math_result<T>::type;

  //===========================================================================
  // constants
  //===========================================================================

  /// The tolerance to use for floating-point equality
  inline constexpr auto default_tolerance = real{1e-6f};

  //===========================================================================
  // non-member functions
  //===========================================================================

  //---------------------------------------------------------------------------
  // Products
  //---------------------------------------------------------------------------

  /// \brief Calculates the dot product between two arrays
  ///
  /// \param lhs the left array
  /// \param rhs the right array
  /// \return the dot product of the two arrays
  template<typename T, typename U, std::size_t N>
  constexpr auto dot(T(&lhs)[N], U(&rhs)[N])
    noexcept -> std::common_type_t<T, U>;

  //---------------------------------------------------------------------------
  // Rounding
  //---------------------------------------------------------------------------

  /// \brief Rounds the floating point value \p a
  ///
  /// \param a the floating point value
  /// \return the rounded float value
  template<typename Arithmetic>
  auto round(Arithmetic a) noexcept -> Arithmetic;

  /// \brief Computes the smallest integer value not less than \p a
  ///
  /// \param a the floating point value
  /// \return the ceiling of \p a
  template<typename Arithmetic>
  auto ceil(Arithmetic a) noexcept -> Arithmetic;

  /// \brief Computes the smallest integer value not less than \p a
  ///
  /// \param a the floating point value
  /// \return the ceiling of \p a
  template<typename Arithmetic>
  auto floor(Arithmetic a) noexcept -> Arithmetic;

  /// \brief Truncates the arithmetic value \p a
  ///
  /// \param a the value to truncate
  /// \return the truncated value
  template<typename Arithmetic>
  auto trunc(Arithmetic a) noexcept -> Arithmetic;

  /// \brief Calculates the modulo of \p num by \p den
  ///
  /// This function promotes the return value to the common type of
  /// \p num and \p den
  ///
  /// \param num the numerator
  /// \param den the denominator
  /// \return the modulo of \p num and \p den
  auto mod(real num, real den) noexcept -> real;

  //---------------------------------------------------------------------------
  // Roots
  //---------------------------------------------------------------------------

  /// \brief Computes the square-root of \p a
  ///
  /// \param a the value to square-root
  /// \return the square-root of \p a
  template<typename Arithmetic>
  auto sqrt(Arithmetic a) noexcept -> math_result_t<Arithmetic>;

  //---------------------------------------------------------------------------
  // Logarithms
  //---------------------------------------------------------------------------

  /// \brief Computes the logarithm (base-10) of \p a
  ///
  /// \param a the arithmetic type to determine the logarithm
  /// \return the result of the logarithm
  template<typename Arithmetic>
  auto log(Arithmetic a) noexcept -> math_result_t<Arithmetic>;

  //---------------------------------------------------------------------------

  /// \brief Computes the logarithm (base-2) of \p a
  ///
  /// \param a the arithmetic type to determine the logarithm
  /// \return the result of the logarithm
  template<typename Arithmetic>
  auto log2(Arithmetic a) noexcept -> math_result_t<Arithmetic>;

  //---------------------------------------------------------------------------
  // Absolute Values
  //---------------------------------------------------------------------------

  /// \brief Absolute value function specialization for signed types
  ///
  /// \param x the signed value
  /// \return \p x the absolute value of \p x (\c |x| )
#ifndef ALLOY_DOXYGEN
  template<typename Arithmetic,
           std::enable_if_t<!std::is_unsigned<Arithmetic>::value>* = nullptr>
#else
  template<typename Arithmetic>
#endif
  constexpr auto abs(Arithmetic x) noexcept -> Arithmetic;

  /// \brief Absolute value function specialization for unsigned types
  ///
  /// This function acts as a simple type-safe identity that returns the
  /// input value
  ///
  /// \param x the unsigned value
  /// \return \p x
#ifndef ALLOY_DOXYGEN
  template<typename Arithmetic,
           std::enable_if_t<std::is_unsigned<Arithmetic>::value>* = nullptr>
#else
  template<typename Arithmetic>
#endif
  constexpr auto abs(Arithmetic x) noexcept -> Arithmetic;

  //---------------------------------------------------------------------------
  // Clamping
  //---------------------------------------------------------------------------

  /// \brief Clamps a value between [\p min , \p max ]
  ///
  /// \param val the value to clamp
  /// \param min the min value
  /// \param max the max value
  /// \return the clamped value
  constexpr auto clamp(real val, real min, real max) noexcept -> real;

  /// \brief Clamps a floating value between \c [0,1]
  ///
  /// \param val the value to clamp
  /// \return the clamped value
  constexpr auto saturate(real val) noexcept -> real;

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  namespace detail {

    template <typename T, typename = void>
    struct comparison_tolerance
    {
      static inline constexpr auto tolerance = default_tolerance;
    };

    template <typename T>
    struct comparison_tolerance<T, std::void_t<decltype(T::comparison_tolerance)>>
    {
      static inline constexpr auto tolerance = T::comparison_tolerance;
    };

  } // namespace detail

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Traits to retrieve the tolerance for `almost_equals` comparison.
  ///
  /// By default, this will search `T::comparison_tolerance` first before
  /// falling back to `default_tolerance`.
  ///
  /// For custom types, this can either be specialized, or the class may be
  /// given a `comparison_tolerance` static member.
  ///
  /// \tparam T the type to get the tolerance
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct comparison_traits
  {
    static inline constexpr auto tolerance = detail::comparison_tolerance<T>::tolerance;
  };

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to `comparison_traits<T>::tolerance`
  ///
  /// This function will find `almost_equal(lhs,rhs,tolerance)` by ADL, and
  /// apply the appropriate default tolerance based on `comparison_traits`
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \return \c true if \p lhs is almost equal to \p rhs
  template <typename T, typename U>
  constexpr auto almost_equal(const T& lhs, const U& rhs) noexcept -> bool;

  /// \brief Determines relative equality between \p lhs and \p rhs relative
  ///        to the specified \p tolerance
  ///
  /// \param lhs the value on the left of the equation
  /// \param rhs the value on the right of the equation
  /// \param tolerance the tolerance to use for comparison
  /// \return \c true if \p lhs is almost equal to \p rhs
  constexpr auto almost_equal(
    real lhs,
    real rhs,
    real tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//=============================================================================
// inline definitions : non-member functions
//=============================================================================

//-----------------------------------------------------------------------------
// Products
//-----------------------------------------------------------------------------

template<typename T, typename U, std::size_t N>
inline constexpr
auto alloy::core::dot(T (&lhs)[N], U (&rhs)[N])
  noexcept -> std::common_type_t<T, U>
{
  auto result = std::common_type_t<T,U>{0};
  for( auto i = 0; i < N; ++i ) {
    result += (lhs[i] * rhs[i]);
  }
  return result;
}

//-----------------------------------------------------------------------------
// Rounding
//-----------------------------------------------------------------------------

template<typename Arithmetic>
inline
auto alloy::core::round(Arithmetic a)
  noexcept -> Arithmetic
{
  return std::round(a);
}

template<typename Arithmetic>
inline
auto alloy::core::ceil(Arithmetic a)
  noexcept -> Arithmetic
{
  return std::ceil(a);
}

template<typename Arithmetic>
inline
auto alloy::core::floor(Arithmetic a)
  noexcept -> Arithmetic
{
  return std::floor(a);
}

template<typename Arithmetic>
inline
auto alloy::core::trunc(Arithmetic a)
  noexcept -> Arithmetic
{
  return std::trunc(a);
}

inline
auto alloy::core::mod(real num, real den)
  noexcept -> alloy::core::real
{
  return std::fmod(num,den);
}

//-----------------------------------------------------------------------------
// Roots
//-----------------------------------------------------------------------------

template<typename Arithmetic>
inline
auto alloy::core::sqrt(Arithmetic a)
  noexcept -> math_result_t<Arithmetic>
{
  return std::sqrt(a);
}

//-----------------------------------------------------------------------------
// Logarithms
//-----------------------------------------------------------------------------

template<typename Arithmetic>
inline
auto alloy::core::log(Arithmetic a)
  noexcept -> math_result_t<Arithmetic>
{
  return std::log(a);
}

//-----------------------------------------------------------------------------

template<typename Arithmetic>
inline
auto alloy::core::log2(Arithmetic a)
  noexcept -> math_result_t<Arithmetic>
{
  return std::log2(a);
}

//-----------------------------------------------------------------------------
// Absolute Values
//-----------------------------------------------------------------------------

template<typename Arithmetic,
         std::enable_if_t<!std::is_unsigned<Arithmetic>::value>*>
inline constexpr
auto alloy::core::abs(Arithmetic x)
  noexcept -> Arithmetic
{
  return ((x < 0) ? -x : x);
}

template<typename Arithmetic,
         std::enable_if_t<std::is_unsigned<Arithmetic>::value>*>
inline constexpr
auto alloy::core::abs(Arithmetic x)
  noexcept -> Arithmetic
{
  return x;
}

//-----------------------------------------------------------------------------
// Clamping
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::clamp(real val, real min, real max)
  noexcept -> real
{
  return ((val < min) ? min : ((val > max) ? max : val));
}

inline constexpr
auto alloy::core::saturate(real val)
  noexcept -> real
{
    return clamp( val, real{0}, real{1} );
}

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

template <typename T, typename U>
inline constexpr
auto alloy::core::almost_equal(const T& lhs, const U& rhs)
  noexcept -> bool
{
  using alloy::core::almost_equal;

  using type = std::common_type_t<T,U>;

  // Call via ADL
  return almost_equal(lhs, rhs, comparison_traits<type>::tolerance);
}

inline constexpr
auto alloy::core::almost_equal(real lhs, real rhs, real tolerance)
  noexcept -> bool
{
  const auto tmp = (lhs - rhs);

  return (((tmp < real{0}) ? -tmp : tmp) <= tolerance);
}

#endif /* ALLOY_CORE_MATH_MATH_HPP */
