// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file attribute_locations.hpp
///
/// \brief This header defines the shader input locations that each mesh
///        vertex attribute is bound to
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_ATTRIBUTE_LOCATIONS_HPP
#define ALLOY_RENDER_MESH_ATTRIBUTE_LOCATIONS_HPP

#ifdef _MSC_VER
# pragma once
#endif

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A static class that contains the constant locations for the
  ///        various vertex attributes.
  //////////////////////////////////////////////////////////////////////////////
  struct attribute_locations final
  {
    //--------------------------------------------------------------------------
    // Constructor / Destructor
    //--------------------------------------------------------------------------

    attribute_locations() = delete;
    ~attribute_locations() = delete;

    //--------------------------------------------------------------------------
    // Constants
    //--------------------------------------------------------------------------

    /// The location of `vec{1-4}` positions
    inline static constexpr auto positions  = 0u;

    /// The location of normalized `vec{1-4}` RGBA colors
    inline static constexpr auto colors     = 1u;

    /// The location of `vec{1-3}` UVs
    inline static constexpr auto uvs        = 2u;

    /// The location of normalized `vec{1-4}` normals
    inline static constexpr auto normals    = 3u;

    /// The location of normalized `vec{1-4}` tangents, with the handedness of
    /// the tangent basis in `w`
    inline static constexpr auto tangents   = 4u;

    /// The location of `vec{1-3}` secondary UVs
    inline static constexpr auto uv2        = 5u;

    /// The location of `uvec{1-4}` skinning joint indices
    inline static constexpr auto joints     = 6u;

    /// The location of normalized `vec{1-4}` skinning weights
    inline static constexpr auto weights    = 7u;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_ATTRIBUTE_LOCATIONS_HPP */
