// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file mesh.hpp
///
/// \brief This header defines the owner of a mesh uploaded into OpenGL
///        buffers
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_MESH_MESH_HPP
#define ALLOY_RENDER_GL_MESH_MESH_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/containers/vector.hpp"

#include "alloy/render/mesh/mesh.hpp"

#include <GL/glew.h>

#include <cstddef> // std::size_t
#include <cstdint> // std::uint32_t
#include <span>    // std::span

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The unique owner of a mesh uploaded into OpenGL buffers, along
  ///        with the vertex array object that reads them
  ///
  /// Each vertex attribute of the mesh is read from the location given by
  /// `attribute_locations`, converted from its packed format to the floats
  /// (or, for joints, the unsigned integers) that shaders read.
  //////////////////////////////////////////////////////////////////////////////
  class mesh
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Uploads \p m into new OpenGL buffers
    ///
    /// \param m the mesh to upload
    /// \return the uploaded mesh
    static auto make(const render::mesh& m) -> mesh;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    mesh(mesh&& other) noexcept;
    mesh(const mesh&) = delete;

    //--------------------------------------------------------------------------

    ~mesh();

    //--------------------------------------------------------------------------

    auto operator=(mesh&& other) noexcept -> mesh&;
    auto operator=(const mesh&) -> mesh& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components of every vertex attribute
    ///
    /// \return the components of the mesh's vertices
    auto components() const noexcept -> const vertex_components&;

    /// \brief Gets the ranges of this mesh that are drawn separately
    ///
    /// \return the submeshes, of which there is always at least one
    auto submeshes() const noexcept -> std::span<const submesh>;

    //--------------------------------------------------------------------------
    // Drawing
    //--------------------------------------------------------------------------
  public:

    /// \brief Binds the vertex array object of this mesh, and the winding of
    ///        its front faces
    auto bind() const noexcept -> void;

    /// \brief Draws \p instances instances of the submesh at \p index
    ///
    /// \pre this mesh is bound
    /// \pre `index < submeshes().size()`
    /// \param index the index of the submesh to draw
    /// \param instances the number of instances to draw
    auto draw(std::size_t index, std::uint32_t instances)
      const noexcept -> void;

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
      vertex_components components,
      core::vector<submesh> submeshes
    ) noexcept;

    //--------------------------------------------------------------------------
    // Private Modifiers
    //--------------------------------------------------------------------------
  private:

    auto destroy() noexcept -> void;

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
    vertex_components m_components;

    /// The ranges of the mesh to draw, in indices, or in vertices for meshes
    /// without indices
    core::vector<submesh> m_submeshes;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_MESH_MESH_HPP */
