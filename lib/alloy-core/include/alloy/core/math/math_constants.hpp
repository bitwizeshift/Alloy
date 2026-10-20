// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file math_constants.hpp
 *
 * \brief This header defines mathematical constants required elsewhere
 *        within the alloy engine.
 *****************************************************************************/

#ifndef ALLOY_CORE_MATH_MATH_CONSTANTS_HPP
#define ALLOY_CORE_MATH_MATH_CONSTANTS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A pure static-class that contains functions for receiving
  ///        mathematical constants.
  /////////////////////////////////////////////////////////////////////////////
  struct math_constants
  {
    math_constants() = delete;
    ~math_constants() = delete;

    inline static constexpr auto pi = 3.1415926535897932384626433832795028_real;
    inline static constexpr auto half_pi = pi / 2;
    inline static constexpr auto two_pi = 2 * pi;

    inline static constexpr auto tau = two_pi;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_MATH_CONSTANTS_HPP */
