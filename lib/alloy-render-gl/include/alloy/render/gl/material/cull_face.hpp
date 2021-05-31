////////////////////////////////////////////////////////////////////////////////
/// \file cull_face.hpp
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

#ifndef ALLOY_RENDER_GL_MATERIAL_CULL_FACE_HPP
#define ALLOY_RENDER_GL_MATERIAL_CULL_FACE_HPP

#include "alloy/render/material/cull_face.hpp"

#include "alloy/core/intrinsics.hpp"

#include <optional>
#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy cull_face \p b to an OpenGL
  ///        enum
  ///
  /// \param c the cull face
  /// \return the GLenum for the primitive
  auto to_opengl_type(cull_face c) noexcept -> std::optional<::GLenum>;

} // namespace alloy::render::gl

inline
auto alloy::render::gl::to_opengl_type(cull_face c)
  noexcept -> std::optional<::GLenum>
{
  switch (c) {
    case cull_face::none: {
      return std::nullopt;
    }
    case cull_face::front: {
      return GL_FRONT;
    }
    case cull_face::back: {
      return GL_BACK;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_MATERIAL_CULL_FACE_HPP */
