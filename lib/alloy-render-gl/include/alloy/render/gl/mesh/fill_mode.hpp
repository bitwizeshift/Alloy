// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file fill_mode.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MESH_FILL_MODE_HPP
#define ALLOY_RENDER_GL_MESH_FILL_MODE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/fill_mode.hpp"

#include "alloy/core/intrinsics.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy fill_mode type \p f to an OpenGL
  ///        enum
  ///
  /// \param f the fill mode
  /// \return the GLenum for the primitive
  constexpr
  auto to_opengl_type(fill_mode f) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline constexpr
auto alloy::render::gl::to_opengl_type(fill_mode f) noexcept -> ::GLenum
{
  switch (f) {
    case fill_mode::solid: {
      return GL_FILL;
    }
    case fill_mode::wireframe: {
      return GL_LINE;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_MESH_FILL_MODE_HPP */
