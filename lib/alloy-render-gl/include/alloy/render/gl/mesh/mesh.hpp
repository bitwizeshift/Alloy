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

#include "alloy/core/containers/vector.hpp"
#include "alloy/render/mesh/mesh.hpp"

#include <GL/glew.h>

#include <cstddef>


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

    /// \brief Gets the number of submeshes of this mesh
    ///
    /// This is always at least one.
    ///
    /// \return the number of submeshes
    auto submesh_count() const noexcept -> std::size_t;

    /// \brief Draws the submesh at \p index
    ///
    /// \pre `index < submesh_count()`
    /// \param index the index of the submesh to draw
    auto draw(std::size_t index) -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_vao;
    ::GLuint m_vbo;
    ::GLuint m_ebo;
    ::GLenum m_topology;
    ::GLenum m_front_face;
    ::GLenum m_index_type;

    /// The ranges of the mesh to draw, in indices, or in vertices for meshes
    /// without indices
    core::vector<render::submesh> m_submeshes;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    mesh(
      ::GLuint vao,
      ::GLuint vbo,
      ::GLuint ebo,
      ::GLenum topology,
      ::GLenum front_face,
      ::GLenum index_type,
      core::vector<render::submesh> submeshes
    );

  };

} // namespace alloy::render::gl


#endif /* ALLOY_RENDER_GL_MESH_MESH_HPP */
