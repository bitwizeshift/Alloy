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

#ifndef ALLOY_RENDER_MESH_PRIMITIVE_TOPOLOGY_HPP
#define ALLOY_RENDER_MESH_PRIMITIVE_TOPOLOGY_HPP

#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The topological representation of primitives in a mesh
  ///
  /// This enumeration dictates how a mesh will be rendered
  //////////////////////////////////////////////////////////////////////////////
  enum class primitive_topology {

    /// The primitive is rendered as a series of vertex points.
    ///
    /// Corresponds to GL_POINTS in OpenGL, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
    /// in DX11, etc.
    points,

    /// The primitive is rendered as a single continuous line connected from
    /// the previous vertex.
    ///
    /// Corresponds to GL_LINES in OpenGL, D3D11_PRIMITIVE_TOPOLOGY_LINELIST
    /// in DX11, etc.
    lines,

    /// The primitive is rendered as a line strip
    ///
    /// Corresponds to GL_LINE_STRIP in OpenGL,
    /// D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP in DX11, etc.
    line_strip,

    /// The primitive is rendered as lines by interpreting data as an adjacency
    /// list
    ///
    /// Corresponds to GL_LINES_ADJACENCY in OpenGL,
    /// D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ in DX11, etc.
    lines_adjacency,

    /// The primitive is rendered as a line strip by interpreting data as an
    /// adjacency list
    ///
    /// Corresponds to GL_LINE_STRIP_ADJACENCY in OpenGL,
    /// D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ in DX11, etc.
    line_strip_adjacency,

    /// The primitive is rendered as a series of triangles by interpreting each
    /// triplet of positions as a triangle face
    ///
    /// Corresponds to GL_TRIANGLES in OpenGL,
    /// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST in DX11, etc.
    triangles,

    /// The primitive is rendered as a series of triangles connected as a strip,
    /// using the previous two points as a connecting edge of the triangle.
    ///
    /// Corresponds to GL_TRIANGLE_STRIP
    /// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP in DX11, etc.
    triangle_strip,

    /// The primitive is rendered as a series of triangles by interpreting the
    /// points as adjacency locations for triangles.
    ///
    /// Corresponds to GL_TRIANGLES_ADJACENCY
    /// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ in DX11, etc.
    triangles_adjacency,

    /// The primitive is rendered as a series of triangles as a strip by
    /// interpreting the points as adjacency locations for triangles.
    ///
    /// Corresponds to GL_TRIANGLE_STRIP_ADJACENCY
    /// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ in DX11, etc.
    triangle_strip_adjacency,
  };

  /// \brief Converts this primitive topology \p t into a string
  ///
  /// \warning
  /// If \p c is not a valid enumerator for `primitive_topology`, the behavior is
  /// undefined!
  ///
  /// \param t the primitive_topology to convert to string
  /// \return a null-terminated string
  auto to_string(primitive_topology t) noexcept -> core::zstring_view;

} // namespace alloy::render

inline
auto alloy::render::to_string(primitive_topology t)
  noexcept -> core::zstring_view
{
  switch (t) {
    case primitive_topology::points: {
      return "points";
    }
    case primitive_topology::lines: {
      return "lines";
    }
    case primitive_topology::line_strip: {
      return "line_strip";
    }
    case primitive_topology::lines_adjacency: {
      return "lines_adjacency";
    }
    case primitive_topology::line_strip_adjacency: {
      return "line_strip_adjacency";
    }
    case primitive_topology::triangles: {
      return "triangles";
    }
    case primitive_topology::triangle_strip: {
      return "triangle_strip";
    }
    case primitive_topology::triangles_adjacency: {
      return "triangles_adjacency";
    }
    case primitive_topology::triangle_strip_adjacency: {
      return "triangle_strip_adjacency";
    }
  }
  core::compiler::unreachable();
}
#endif /* ALLOY_RENDER_MESH_PRIMITIVE_TOPOLOGY_HPP */
