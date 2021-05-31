////////////////////////////////////////////////////////////////////////////////
/// \file primitive_topology.hpp
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

#ifndef ALLOY_RENDER_GL_MESH_PRIMITIVE_TOPOLOGY_HPP
#define ALLOY_RENDER_GL_MESH_PRIMITIVE_TOPOLOGY_HPP

#include "alloy/render/mesh/primitive_topology.hpp"

#include "alloy/core/intrinsics.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy primitive_topology type \p t to an OpenGL
  ///        enum
  ///
  /// \param t the type
  /// \return the GLenum for the primitive
  auto to_opengl_type(primitive_topology t) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline
auto alloy::render::gl::to_opengl_type(primitive_topology topology)
  noexcept -> ::GLenum
{
  switch (topology) {
    case primitive_topology::points: {
      return GL_POINTS;
    }
    case primitive_topology::lines: {
      return GL_LINES;
    }
    case primitive_topology::line_strip: {
      return GL_LINE_STRIP;
    }
    case primitive_topology::lines_adjacency: {
      return GL_LINES_ADJACENCY;
    }
    case primitive_topology::line_strip_adjacency: {
      return GL_LINE_STRIP_ADJACENCY;
    }
    case primitive_topology::triangles: {
      return GL_TRIANGLES;
    }
    case primitive_topology::triangle_strip: {
      return GL_TRIANGLE_STRIP;
    }
    case primitive_topology::triangles_adjacency: {
      return GL_TRIANGLES_ADJACENCY;
    }
    case primitive_topology::triangle_strip_adjacency: {
      return GL_TRIANGLE_STRIP_ADJACENCY;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_MESH_PRIMITIVE_TOPOLOGY_HPP */
