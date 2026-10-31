// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file blend_equation.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MATERIAL_BLEND_EQUATION_HPP
#define ALLOY_RENDER_GL_MATERIAL_BLEND_EQUATION_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"

#include "alloy/render/material/blend_equation.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy blend_equation \p b to an OpenGL
  ///        enum
  ///
  /// \param b the blend equation
  /// \return the GLenum for the primitive
  constexpr
  auto to_opengl_type(blend_equation b) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline constexpr
auto alloy::render::gl::to_opengl_type(blend_equation b) noexcept -> ::GLenum
{
  switch (b) {
    case blend_equation::add: {
      return GL_FUNC_ADD;
    }
    case blend_equation::subtract: {
      return GL_FUNC_SUBTRACT;
    }
    case blend_equation::reverse_subtract: {
      return GL_FUNC_REVERSE_SUBTRACT;
    }
    case blend_equation::max: {
      return GL_MAX;
    }
    case blend_equation::min: {
      return GL_MIN;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_MATERIAL_BLEND_EQUATION_HPP */
