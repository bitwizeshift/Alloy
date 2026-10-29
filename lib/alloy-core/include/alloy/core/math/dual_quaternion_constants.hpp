// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file dual_quaternion_constants.hpp
///
/// \brief This header contains named constants for dual quaternions
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_DUAL_QUATERNION_CONSTANTS_HPP
#define ALLOY_CORE_MATH_DUAL_QUATERNION_CONSTANTS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/dual_quaternion.hpp"
#include "alloy/core/math/quaternion_constants.hpp"

#include "alloy/core/api.hpp"

namespace alloy::core {

  //============================================================================
  // struct : dual_quaternion_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Named constants for \ref dual_quaternion
  //////////////////////////////////////////////////////////////////////////////
  struct ALLOY_CORE_API dual_quaternion_constants final
  {
    ~dual_quaternion_constants() = delete;

    /// \brief A dual quaternion with every component set to 0
    inline static constexpr auto zero = dual_quaternion{
      quaternion_constants::zero,
      quaternion_constants::zero
    };

    /// \brief The dual quaternion that represents no transformation
    inline static constexpr auto identity = dual_quaternion{
      quaternion_constants::identity,
      quaternion_constants::zero
    };
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MATH_DUAL_QUATERNION_CONSTANTS_HPP */
