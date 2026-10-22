// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file fill_mode.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_FILL_MODE_HPP
#define ALLOY_RENDER_MESH_FILL_MODE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/string/zstring_view.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The fill mode for a given mesh
  //////////////////////////////////////////////////////////////////////////////
  enum class fill_mode {
    /// Renders meshes solid (default)
    ///
    /// Corresponds to GL_FILL in OpenGL, D3D11_FILL_SOLID in DX11, etc.
    solid,

    /// Renders meshes as a wireframe without any fill
    ///
    /// Corresponds to GL_LINE in OpenGL, D3D11_FILL_WIREFRAME in DX11, etc.
    wireframe,
  };

  /// \brief Converts this fill mode \p m into a string
  ///
  /// \warning
  /// If \p m is not a valid enumerator for `fill_mode`, the behavior is
  /// undefined!
  ///
  /// \param m the fill_mode to convert to string
  /// \return a null-terminated string
  auto to_string(fill_mode m) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(fill_mode m) noexcept -> core::zstring_view
{
  switch (m) {
    case fill_mode::solid: {
      return "solid";
    }
    case fill_mode::wireframe: {
      return "wireframe";
    }
  }
  core::compiler::unreachable();
}
#endif /* ALLOY_RENDER_MESH_FILL_MODE_HPP */
