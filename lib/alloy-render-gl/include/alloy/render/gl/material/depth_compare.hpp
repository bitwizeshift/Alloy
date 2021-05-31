////////////////////////////////////////////////////////////////////////////////
/// \file depth_compare.hpp
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

#ifndef ALLOY_RENDER_GL_MATERIAL_DEPTH_COMPARE_HPP
#define ALLOY_RENDER_GL_MATERIAL_DEPTH_COMPARE_HPP

#include "alloy/render/material/depth_compare.hpp"

#include "alloy/core/intrinsics.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy depth_compare \p d to an OpenGL
  ///        enum
  ///
  /// \param d the depth compare system
  /// \return the GLenum for the primitive
  auto to_opengl_type(depth_compare d) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline
auto alloy::render::gl::to_opengl_type(depth_compare d)
  noexcept -> ::GLenum
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
