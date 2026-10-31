// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file shader_stage.hpp
///
/// \brief This header defines the conversion of a shader stage into its
///        OpenGL shader type
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SHADER_SHADER_STAGE_HPP
#define ALLOY_RENDER_GL_SHADER_SHADER_STAGE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"

#include "alloy/render/shader/shader_stage.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  constexpr auto to_opengl_type(shader_stage s) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline constexpr
auto alloy::render::gl::to_opengl_type(shader_stage s) noexcept -> ::GLenum
{
  switch (s) {
    case shader_stage::vertex: {
      return GL_VERTEX_SHADER;
    }
    case shader_stage::tessellation_control: {
      return GL_TESS_CONTROL_SHADER;
    }
    case shader_stage::tessellation_evaluation: {
      return GL_TESS_EVALUATION_SHADER;
    }
    case shader_stage::geometry: {
      return GL_GEOMETRY_SHADER;
    }
    case shader_stage::fragment: {
      return GL_FRAGMENT_SHADER;
    }
    case shader_stage::compute: {
      return GL_COMPUTE_SHADER;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_SHADER_SHADER_STAGE_HPP */
