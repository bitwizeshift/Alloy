// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file vector3_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR3_CONSTANTS_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR3_CONSTANTS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/vector/vector3.hpp"

#include "alloy/core/precision/real.hpp"

namespace alloy::core {

  //===========================================================================
  // static class : vector3_constants
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief This static class contains all constants relating to the vector3
  ///        type as inline constexpr values
  /////////////////////////////////////////////////////////////////////////////
  struct vector3_constants
  {
    vector3_constants() = delete;
    ~vector3_constants() = delete;

    //-------------------------------------------------------------------------

    /// \brief A zero-valued vector3
    inline static constexpr auto zero = vector3{
      real{0}, real{0}, real{0}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the X unit-vector
    inline static constexpr auto unit_x = vector3{
      real{1}, real{0}, real{0}
    };

    /// \brief A vector representing the Y unit-vector
    inline static constexpr auto unit_y = vector3{
      real{0}, real{1}, real{0}
    };

    /// \brief A vector representing the Z unit-vector
    inline static constexpr auto unit_z = vector3{
      real{0}, real{0}, real{1}
    };

    //-------------------------------------------------------------------------

    /// \brief A vector representing the negative X unit-vector
    inline static constexpr auto neg_unit_x = -unit_x;

    /// \brief A vector representing the negative Y unit-vector
    inline static constexpr auto neg_unit_y = -unit_y;

    /// \brief A vector representing the negative Z unit-vector
    inline static constexpr auto neg_unit_z = -unit_z;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR3_CONSTANTS_HPP */
