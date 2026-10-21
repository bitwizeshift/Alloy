// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file real.hpp
 *
 * \brief This header defines the default precision used within the rest of
 *        the alloy engine.
 *****************************************************************************/

#ifndef ALLOY_CORE_PRECISION_REAL_HPP
#define ALLOY_CORE_PRECISION_REAL_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/config.hpp"

#include <cmath> // std::isnan, std::isfinite, etc

namespace alloy::core {

  /// \brief Defines a real number for floating-point precision.
  ///
  /// Alloy may be compiled with either
#if ALLOY_CORE_PRECISION == ALLOY_CORE_PRECISION_FLOAT
  using real = float;
#elif ALLOY_CORE_PRECISION == ALLOY_CORE_PRECISION_DOUBLE
  using real = double;
#elif ALLOY_CORE_PRECISION == ALLOY_CORE_PRECISION_HALF
# error ALLOY_CORE_PRECISION set to unsupported value (ALLOY_CORE_PRECISION_HALF)
#elif ALLOY_CORE_PRECISION == ALLOY_CORE_PRECISION_FIXED
# error ALLOY_CORE_PRECISION set to unsupported value (ALLOY_CORE_PRECISION_FIXED)
#else
# error ALLOY_CORE_PRECISION set to unknown value
#endif

  //============================================================================
  // non-member functions : alias : real
  //============================================================================

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is a nan
  ///
  /// \param f the value to check
  /// \return `true` if \p f is a `nan`
  auto is_nan(real f) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is finite
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `finite`
  auto is_finite(real f) noexcept -> bool;

  /// \brief Determines whether a given real value \p f is infinite
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `infinite`
  auto is_infinite(real f) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is normal
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `normal`
  auto is_normal(real f) noexcept -> bool;

  /// \brief Determines whether a given real value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return `true` if \p f is `subnormal`
  auto is_subnormal(real f) noexcept -> bool;

  //============================================================================
  // literals
  //============================================================================

  inline namespace literals {
    inline namespace precision_literals {

      constexpr auto operator""_r(long double in) noexcept -> real;
      constexpr auto operator""_real(long double in) noexcept -> real;

    } // inline namespace precision_literals
  } // inline namespaec literals

} // namespace alloy::core


//==============================================================================
// definitions : non-member functions : alias : real
//==============================================================================

inline
auto alloy::core::is_nan(real f) noexcept -> bool
{
  return std::isnan(f);
}

inline
auto alloy::core::is_finite(real f) noexcept -> bool
{
  return std::isfinite( f );
}

inline
auto alloy::core::is_infinite(real f) noexcept -> bool
{
  return std::isinf( f );
}

inline
auto alloy::core::is_normal(real f) noexcept -> bool
{
  return std::isnormal( f );
}

inline
auto alloy::core::is_subnormal(real f) noexcept -> bool
{
  return std::fpclassify(f) == FP_SUBNORMAL;
}

//==============================================================================
// definitions : literals
//==============================================================================

inline constexpr
auto alloy::core::literals::precision_literals::operator""_r(long double in)
  noexcept -> real
{
  return static_cast<real>(in);
}

inline constexpr
auto alloy::core::literals::precision_literals::operator""_real(long double in)
  noexcept -> real
{
  return static_cast<real>(in);
}

#endif /* ALLOY_CORE_PRECISION_REAL_HPP */

