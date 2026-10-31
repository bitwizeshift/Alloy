// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file depth_compare.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MATERIAL_DEPTH_COMPARE_HPP
#define ALLOY_RENDER_GL_MATERIAL_DEPTH_COMPARE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"

#include "alloy/render/material/depth_compare.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy depth_compare \p d to an OpenGL
  ///        enum
  ///
  /// \param d the depth compare system
  /// \return the GLenum for the primitive
  constexpr
  auto to_opengl_type(depth_compare d) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline constexpr
auto alloy::render::gl::to_opengl_type(depth_compare d) noexcept -> ::GLenum
{
  switch (d) {
    case depth_compare::never: {
      return GL_NEVER;
    }
    case depth_compare::less: {
      return GL_LESS;
    }
    case depth_compare::equal: {
      return GL_EQUAL;
    }
    case depth_compare::less_equal: {
      return GL_LEQUAL;
    }
    case depth_compare::greater: {
      return GL_GREATER;
    }
    case depth_compare::not_equal: {
      return GL_NOTEQUAL;
    }
    case depth_compare::greater_equal: {
      return GL_GEQUAL;
    }
    case depth_compare::always: {
      return GL_ALWAYS;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_MATERIAL_DEPTH_COMPARE_HPP */
