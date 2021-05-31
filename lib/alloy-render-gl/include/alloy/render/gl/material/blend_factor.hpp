////////////////////////////////////////////////////////////////////////////////
/// \file blend_factor.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ALLOY_RENDER_GL_MATERIAL_BLEND_FACTOR_HPP
#define ALLOY_RENDER_GL_MATERIAL_BLEND_FACTOR_HPP

#include "alloy/render/material/blend_factor.hpp"

#include "alloy/core/intrinsics.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy blend_factor \p b to an OpenGL
  ///        enum
  ///
  /// \param b the blend factor
  /// \return the GLenum for the primitive
  auto to_opengl_type(blend_factor b) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline
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
