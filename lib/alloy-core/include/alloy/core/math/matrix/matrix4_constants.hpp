// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file matrix4_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX4_CONSTANTS_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX4_CONSTANTS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/matrix/matrix4.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : matrix4_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class used for storing matrix constants
  /////////////////////////////////////////////////////////////////////////////
  struct matrix4_constants
  {
    matrix4_constants() = delete;
    ~matrix4_constants() = delete;

    inline static constexpr auto identity = matrix4{
      real{1}, real{0}, real{0}, real{0},
      real{0}, real{1}, real{0}, real{0},
      real{0}, real{0}, real{1}, real{0},
      real{0}, real{0}, real{0}, real{1},
    };
  };
} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX4_CONSTANTS_HPP */
