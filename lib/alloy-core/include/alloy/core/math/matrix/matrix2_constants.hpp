// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file matrix2_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX2_CONSTANTS_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX2_CONSTANTS_HPP

#ifdef _MSC_VER
# pragma once
#endif


#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/precision/real.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : matrix2_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A static class used for storing matrix constants
  /////////////////////////////////////////////////////////////////////////////
  struct matrix2_constants
  {
    matrix2_constants() = delete;
    ~matrix2_constants() = delete;

    inline static constexpr auto identity = matrix2{
      real{1}, real{0},
      real{0}, real{1},
    };
  };
} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX2_CONSTANTS_HPP */
