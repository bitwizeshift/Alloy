////////////////////////////////////////////////////////////////////////////////
/// \file blend_equation.hpp
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

#ifndef ALLOY_RENDER_GL_MATERIAL_BLEND_EQUATION_HPP
#define ALLOY_RENDER_GL_MATERIAL_BLEND_EQUATION_HPP

#include "alloy/render/material/blend_equation.hpp"

#include "alloy/core/intrinsics.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy blend_equation \p b to an OpenGL
  ///        enum
  ///
  /// \param b the blend equation
  /// \return the GLenum for the primitive
  auto to_opengl_type(blend_equation b) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline
auto alloy::render::gl::to_opengl_type(blend_equation b)
  noexcept -> ::GLenum
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
