// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file point_light .hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_MODEL_LIGHT_POINT_LIGHT_HPP
#define ALLOY_CORE_MODEL_LIGHT_POINT_LIGHT_HPP

#include "alloy/render/light/light.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/geometry/point/point3.hpp"

namespace alloy::render {

  //============================================================================
  // struct : point_light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A light emanating from a point in 3D
  ///
  /// Point lights begin from a specific point, and emit radially in all
  /// directions.
  //////////////////////////////////////////////////////////////////////////////
  struct point_light
  {
    core::point3 source;
    core::color color;
    light::attenuation attenuation;
    light::intensity intensity;
  };

} // namespace alloy::render

#endif /* ALLOY_CORE_MODEL_LIGHT_POINT_LIGHT_HPP */
