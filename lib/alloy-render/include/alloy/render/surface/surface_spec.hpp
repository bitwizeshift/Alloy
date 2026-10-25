// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file surface_spec.hpp
///
/// \brief This header defines the description of a surface that a render
///        device draws into
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_SURFACE_SURFACE_SPEC_HPP
#define ALLOY_RENDER_SURFACE_SURFACE_SPEC_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/resource/resource_handle.hpp"
#include "alloy/render/texture/texture_spec.hpp"

namespace alloy::render {

  struct surface_tag;

  /// \brief A non-owning handle to a surface that a device draws into
  ///
  /// A surface is either made of textures, as described by a `surface_spec`,
  /// or is a target that a particular device knows how to present to, such
  /// as a window. Drawing treats both alike.
  using surface_handle = resource_handle<surface_tag>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The complete description of a surface that draws into textures
  //////////////////////////////////////////////////////////////////////////////
  struct surface_spec
  {
    /// The texture that colors are drawn into, which must have been created
    /// as a render target and must outlive the surface
    texture_handle color;

    /// The texture that depths are drawn into, or null for a surface without
    /// depth. A depth texture must have been created as a render target with
    /// a depth format and the same extent as `color`, and must outlive the
    /// surface.
    texture_handle depth;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_SURFACE_SURFACE_SPEC_HPP */
