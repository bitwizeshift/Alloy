////////////////////////////////////////////////////////////////////////////////
/// \file fill_mode.hpp
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

#ifndef ALLOY_RENDER_GL_MESH_FILL_MODE_HPP
#define ALLOY_RENDER_GL_MESH_FILL_MODE_HPP

#include "alloy/render/mesh/fill_mode.hpp"

#include "alloy/core/intrinsics.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy fill_mode type \p f to an OpenGL
  ///        enum
  ///
  /// \param f the fill mode
  /// \return the GLenum for the primitive
  auto to_opengl_type(fill_mode f) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline
auto alloy::render::gl::to_opengl_type(fill_mode f)
  noexcept -> ::GLenum
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
