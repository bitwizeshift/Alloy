// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file direction_light.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP
#define ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/light/light.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/model/color.hpp"

namespace alloy::render {

  //============================================================================
  // struct : direction_light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A light that shines in a specific direction, but has no specific
  ///        source location
  //////////////////////////////////////////////////////////////////////////////
  struct direction_light
  {
    core::vector3 direction;
    core::color color;
    light::attenuation attenuation;
    light::intensity intensity;
  };

} // namespace alloy::render

#endif /* ALLOY_CORE_MODEL_LIGHT_DIRECTION_LIGHT_HPP */
