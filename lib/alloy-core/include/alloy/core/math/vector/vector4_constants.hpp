// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file vector4_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR4_CONSTANTS_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR4_CONSTANTS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/vector/vector4.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : vector4_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief This static class contains all constants relating to the vector4
  ///        type as inline constexpr values
  /////////////////////////////////////////////////////////////////////////////
  struct vector4_constants
  {
    vector4_constants() = delete;
    ~vector4_constants() = delete;

    //-------------------------------------------------------------------------

    /// \brief A zero-valued vector4
    inline static constexpr auto zero = vector4{
      real{0}, real{0}, real{0}, real{0}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the X unit-vector
    inline static constexpr auto unit_x = vector4{
      real{1}, real{0}, real{0}, real{0}
    };

    /// \brief A vector representing the Y unit-vector
    inline static constexpr auto unit_y = vector4{
      real{0}, real{1}, real{0}, real{0}
    };

    /// \brief A vector representing the Z unit-vector
    inline static constexpr auto unit_z = vector4{
      real{0}, real{0}, real{1}, real{0}
    };

    /// \brief A vector representing the W unit-vector
    inline static constexpr auto unit_w = vector4{
      real{0}, real{0}, real{0}, real{1}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the negative X unit-vector
    inline static constexpr auto neg_unit_x = -unit_x;

    /// \brief A vector representing the negative Y unit-vector
    inline static constexpr auto neg_unit_y = -unit_y;

    /// \brief A vector representing the negative Z unit-vector
    inline static constexpr auto neg_unit_z = -unit_z;

    /// \brief A vector representing the negative W unit-vector
    inline static constexpr auto neg_unit_w = -unit_w;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR4_CONSTANTS_HPP */
