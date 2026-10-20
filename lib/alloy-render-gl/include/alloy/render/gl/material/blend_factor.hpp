// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file blend_factor.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MATERIAL_BLEND_FACTOR_HPP
#define ALLOY_RENDER_GL_MATERIAL_BLEND_FACTOR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/material/blend_factor.hpp"

#include "alloy/core/intrinsics.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy blend_factor \p b to an OpenGL
  ///        enum
  ///
  /// \param b the blend factor
  /// \return the GLenum for the primitive
  constexpr
  auto to_opengl_type(blend_factor b) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline constexpr
auto alloy::render::gl::to_opengl_type(blend_factor b)
  noexcept -> ::GLenum
{
  switch (b) {
    case blend_factor::one: {
      return GL_ONE;
    }
    case blend_factor::zero: {
      return GL_ZERO;
    }
    case blend_factor::source_color: {
      return GL_SRC_COLOR;
    }
    case blend_factor::source_alpha: {
      return GL_SRC_ALPHA;
    }
    case blend_factor::inverse_source_color: {
      return GL_ONE_MINUS_SRC_COLOR;
    }
    case blend_factor::inverse_source_alpha: {
      return GL_ONE_MINUS_SRC_ALPHA;
    }
    case blend_factor::destination_color: {
      return GL_DST_COLOR;
    }
    case blend_factor::destination_alpha: {
      return GL_DST_ALPHA;
    }
    case blend_factor::inverse_destination_color: {
      return GL_ONE_MINUS_DST_COLOR;
    }
    case blend_factor::inverse_destination_alpha: {
      return GL_ONE_MINUS_DST_ALPHA;
    }
    case blend_factor::constant_color: {
      return GL_CONSTANT_COLOR;
    }
    case blend_factor::inverse_constant_color: {
      return GL_ONE_MINUS_CONSTANT_COLOR;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_MATERIAL_BLEND_FACTOR_HPP */
