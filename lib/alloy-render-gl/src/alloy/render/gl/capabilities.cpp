// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/capabilities.hpp"

#include <GL/glew.h>

//==============================================================================
// struct : capabilities
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::capabilities::from_current_context()
  noexcept -> capabilities
{
  const auto anisotropy = GLEW_VERSION_4_6 != GL_FALSE
    || GLEW_ARB_texture_filter_anisotropic != GL_FALSE
    || GLEW_EXT_texture_filter_anisotropic != GL_FALSE;

  auto max_anisotropy = ::GLfloat{1.0f};
  if (anisotropy) {
    ::glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy);
  }

  return capabilities{
    .spirv = GLEW_VERSION_4_6 != GL_FALSE || GLEW_ARB_gl_spirv != GL_FALSE,
    .texture_cube_map_array = GLEW_VERSION_4_0 != GL_FALSE
      || GLEW_ARB_texture_cube_map_array != GL_FALSE,
    .texture_multisample = GLEW_VERSION_3_2 != GL_FALSE
      || GLEW_ARB_texture_multisample != GL_FALSE,
    .max_anisotropy = max_anisotropy,
  };
}
