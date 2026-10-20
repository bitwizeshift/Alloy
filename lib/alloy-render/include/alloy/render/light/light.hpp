// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file light.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_MODEL_LIGHT_LIGHT_HPP
#define ALLOY_CORE_MODEL_LIGHT_LIGHT_HPP

#ifdef _MSC_VER
# pragma once
#endif

namespace alloy::render {

  //============================================================================
  // struct : light
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A struct used for storing semantic sub-types used for lighting
  //////////////////////////////////////////////////////////////////////////////
  struct light {

    //==========================================================================
    // struct : light::attenuation
    //==========================================================================

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Attenuation values for light cutoff
    ////////////////////////////////////////////////////////////////////////////
    struct attenuation
    {
      float constant;
      float linear;
      float exponential;
    };

    //==========================================================================
    // struct : light::intensity
    //==========================================================================

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Intensity values for color
    ////////////////////////////////////////////////////////////////////////////
    struct intensity
    {
      float ambient;
      float diffuse;
    };
  };
} // namespace alloy::render


#endif /* ALLOY_CORE_MODEL_LIGHT_LIGHT_HPP */
