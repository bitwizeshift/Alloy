// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file vector2_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR2_CONSTANTS_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR2_CONSTANTS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/precision/real.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : vector2_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief This static class contains all constants relating to the vector2
  ///        type as inline constexpr values
  /////////////////////////////////////////////////////////////////////////////
  struct vector2_constants
  {
    vector2_constants() = delete;
    ~vector2_constants() = delete;

    //-------------------------------------------------------------------------

    /// \brief A zero-valued vector2
    inline static constexpr auto zero = vector2{
      real{0}, real{0}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the X unit-vector
    inline static constexpr auto unit_x = vector2{
      real{1}, real{0}
    };

    /// \brief A vector representing the X unit-vector
    inline static constexpr auto unit_y = vector2{
      real{0}, real{1}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the negative X unit-vector
    inline static constexpr auto neg_unit_x = -unit_x;

    /// \brief A vector representing the negative Y unit-vector
    inline static constexpr auto neg_unit_y = -unit_y;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR2_CONSTANTS_HPP */
