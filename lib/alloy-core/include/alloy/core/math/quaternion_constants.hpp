// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file quaternion_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef LIB_ALLOY_CORE_INCLUDE_ALLOY_CORE_MATH_QUATERNION_CONSTANTS_HPP
#define LIB_ALLOY_CORE_INCLUDE_ALLOY_CORE_MATH_QUATERNION_CONSTANTS_HPP

#include "alloy/core/math/quaternion.hpp"

namespace alloy::core {

  //============================================================================
  // struct : quaternion_constants
  //============================================================================

  struct ALLOY_CORE_API quaternion_constants final
  {
    ~quaternion_constants() = delete;

    inline static const auto zero = quaternion{};

    inline static const auto identity = quaternion{
      real{1},
      real{0},
      real{0},
      real{0}
    };
  };

} // namespace alloy::core

#endif /* LIB_ALLOY_CORE_INCLUDE_ALLOY_CORE_MATH_QUATERNION_CONSTANTS_HPP */
