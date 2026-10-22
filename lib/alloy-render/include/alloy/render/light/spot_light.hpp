// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file spot_light.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_RENDER_LIGHT_SPOT_LIGHT_HPP
#define ALLOY_RENDER_LIGHT_SPOT_LIGHT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/light/light.hpp"

#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/model/color.hpp"

namespace alloy::render {

  //============================================================================
  // struct : spot_light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A light that emanates from a point, and shines out in a specified
  ///        direction
  ///
  /// The light will extend as far as the specified cutoff distance
  //////////////////////////////////////////////////////////////////////////////
  struct spot_light
  {
    core::point3 source;
    core::vector3 direction;
    float cutoff;
    core::color color;
    light::attenuation attenuation;
    light::intensity intensity;
  };
} // namespace alloy::render

#endif /* ALLOY_RENDER_LIGHT_SPOT_LIGHT_HPP */
