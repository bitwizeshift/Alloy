// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file global_light.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_RENDER_LIGHT_GLOBAL_LIGHT_HPP
#define ALLOY_RENDER_LIGHT_GLOBAL_LIGHT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/light/light.hpp"

#include "alloy/core/model/color.hpp"

namespace alloy::render {

  //============================================================================
  // struct : global_light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A light that illuminates from everywhere; effectively behaves
  ///        like sunlight
  //////////////////////////////////////////////////////////////////////////////
  struct global_light
  {
    core::color color;
    light::attenuation attenuation;
    light::intensity intensity;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_LIGHT_GLOBAL_LIGHT_HPP */
