/*****************************************************************************
 * \file real.hpp
 *
 * \brief This header defines the default precision used within the rest of
 *        the alloy engine.
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
#ifndef ALLOY_CORE_PRECISION_REAL_HPP
#define ALLOY_CORE_PRECISION_REAL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/config.hpp"

#include <cmath> // std::isnan, std::isfinite, etc

namespace alloy::core {

  /// \brief Defines a real number for floating-point precision.
  ///
  /// Alloy may be compiled with either
#if ALLOY_PRECISION == ALLOY_PRECISION_FLOAT
  using real = float;
#elif ALLOY_PRECISION == ALLOY_PRECISION_DOUBLE
  using real = double;
#elif ALLOY_PRECISION == ALLOY_PRECISION_HALF
# error ALLOY_PRECISION set to unsupported value (ALLOY_PRECISION_HALF)
#elif ALLOY_PRECISION == ALLOY_PRECISION_FIXED
# error ALLOY_PRECISION set to unsupported value (ALLOY_PRECISION_FIXED)
#else
# error ALLOY_PRECISION set to unknown value
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
  /// \return \c true if \p f is a \c nan
  bool is_nan( real f ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is finite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c finite
  bool is_finite( real f ) noexcept;

  /// \brief Determines whether a given real value \p f is infinite
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c infinite
  bool is_infinite( real f ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines whether a given real value \p f is normal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c normal
  bool is_normal( real f ) noexcept;

  /// \brief Determines whether a given real value \p f is subnormal
  ///
  /// \param f the value to check
  /// \return \c true if \p f is \c subnormal
  bool is_subnormal( real f ) noexcept;

  //============================================================================
  // literals
  //============================================================================

  inline namespace literals {
    inline namespace precision_literals {

      constexpr real operator""_r(long double in) noexcept;
      constexpr real operator""_real(long double in) noexcept;

    } // inline namespace precision_literals
  } // inline namespaec literals

} // namespace alloy::core

#include "detail/real.inl"

#endif /* ALLOY_CORE_PRECISION_REAL_HPP */