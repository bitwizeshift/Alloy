// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file quaternion_constants.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_QUATERNION_CONSTANTS_HPP
#define ALLOY_CORE_MATH_QUATERNION_CONSTANTS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/math/quaternion.hpp"
#include "alloy/core/precision/real.hpp"

namespace alloy::core {

  //============================================================================
  // struct : quaternion_constants
  //============================================================================

  struct ALLOY_CORE_API quaternion_constants final
  {
    ~quaternion_constants() = delete;

    /// \brief A quaternion with every component set to 0
    inline static constexpr auto zero = quaternion{
      real{0},
      real{0},
      real{0},
      real{0}
    };

    /// \brief The quaternion that represents no rotation
    inline static constexpr auto identity = quaternion{
      real{1},
      real{0},
      real{0},
      real{0}
    };
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_QUATERNION_CONSTANTS_HPP */
