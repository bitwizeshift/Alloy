// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file mesh.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MESH_MESH_HPP
#define ALLOY_RENDER_GL_MESH_MESH_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/mesh.hpp"

#include <GL/glew.h>

#include <cstdint>

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A generic mesh written in terms of OpenGL vertex array objects
  //////////////////////////////////////////////////////////////////////////////
  class mesh
  {
    //--------------------------------------------------------------------------
    // Public Members
    //--------------------------------------------------------------------------
  public:

    static auto make(const render::mesh& m) -> mesh;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    mesh(mesh&& other);
    mesh(const mesh&) = delete;

    //--------------------------------------------------------------------------

    ~mesh();

    //--------------------------------------------------------------------------

    auto operator=(mesh&&) -> mesh& = delete;
    auto operator=(const mesh&) -> mesh& = delete;

    //--------------------------------------------------------------------------
    // Render
    //--------------------------------------------------------------------------
  public:

    /// \brief Draws elements from this mesh
    auto draw() -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_vao;
    ::GLuint m_vbo;
    ::GLuint m_ebo;
    ::GLenum m_topology;
    std::size_t m_indices;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    mesh(
      ::GLuint vao,
      ::GLuint vbo,
      ::GLuint ebo,
      ::GLenum topology,
      std::size_t indices
    );

  };

} // namespace alloy::render::gl


#endif /* ALLOY_RENDER_GL_MESH_MESH_HPP */
