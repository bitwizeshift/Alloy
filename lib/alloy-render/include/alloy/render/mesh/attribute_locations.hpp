// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file vertex_attributes.hpp
///
/// \brief TODO(Bitwize): Add description
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

    inline static constexpr auto positions  = 0u;
    inline static constexpr auto colors     = 1u;
    inline static constexpr auto uvs        = 2u;
    inline static constexpr auto normals    = 3u;
    inline static constexpr auto tangents   = 4u;
    inline static constexpr auto uv2        = 5u;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_ATTRIBUTE_LOCATIONS_HPP */
