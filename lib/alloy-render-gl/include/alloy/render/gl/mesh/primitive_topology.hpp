// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file primitive_topology.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MESH_PRIMITIVE_TOPOLOGY_HPP
#define ALLOY_RENDER_GL_MESH_PRIMITIVE_TOPOLOGY_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  /// \brief Converts the Alloy primitive_topology type \p t to an OpenGL
  ///        enum
  ///
  /// \param t the type
  /// \return the GLenum for the primitive
  constexpr
  auto to_opengl_type(primitive_topology t) noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline constexpr
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
