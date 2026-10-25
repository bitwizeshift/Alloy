// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file texture_dimension.hpp
///
/// \brief This header defines the shapes that a texture may take
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_TEXTURE_TEXTURE_DIMENSION_HPP
#define ALLOY_RENDER_TEXTURE_TEXTURE_DIMENSION_HPP

#ifdef _MSC_VER
# pragma once
#endif

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The shape of a texture, and how it is addressed when sampled
  //////////////////////////////////////////////////////////////////////////////
  enum class texture_dimension {
    /// A row of texels, addressed by one coordinate
    d1,

    /// A sequence of `d1` layers, addressed by one coordinate and a layer
    d1_array,

    /// A grid of texels, addressed by two coordinates
    d2,

    /// A sequence of `d2` layers, addressed by two coordinates and a layer
    d2_array,

    /// A volume of texels, addressed by three coordinates
    d3,

    /// Six square `d2` faces, addressed by a direction
    cube,

    /// A sequence of `cube`s, addressed by a direction and a cube index
    cube_array,
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_TEXTURE_TEXTURE_DIMENSION_HPP */
