// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file polygon_2d_builder.hpp
///
/// \brief This header defines a builder for meshes with 2-dimensional
///        positions
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_2D_POLYGON_2D_BUILDER_HPP
#define ALLOY_RENDER_MESH_2D_POLYGON_2D_BUILDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/render/mesh/winding_order.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/model/color.hpp"

#include <cstddef> // std::size_t
#include <cstdint> // std::uint32_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A builder for 2-dimensional meshes (e.g. a system with 2-component
  ///        positions).
  ///
  /// Each vertex attribute is collected into its own list, either all at once
  /// with the `set_*` functions or one vertex at a time with the `add_*`
  /// functions. Every attribute that is supplied at all must have exactly one
  /// entry per position; attributes that are never supplied are omitted from
  /// the mesh. The attributes are only interleaved and compressed into the
  /// mesh when it is built.
  ///
  /// UVs are always 2-dimensional, while normals remain 3-dimensional.
  //////////////////////////////////////////////////////////////////////////////
  class polygon_2d_builder : public mesh_builder
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using index_type = std::uint32_t;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this mesh_2d_builder with the specified topology
    ///
    /// \param topology the topology for the mesh
    /// \param winding the winding order of front faces in the mesh
    /// \param alloc the allocator to use for intermediate allocations
    explicit polygon_2d_builder(
      primitive_topology topology = primitive_topology::triangles,
      winding_order winding = winding_order::counter_clockwise,
      core::allocator alloc = core::allocator{}
    ) noexcept;
    polygon_2d_builder(polygon_2d_builder&&) = delete;
    polygon_2d_builder(const polygon_2d_builder&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(polygon_2d_builder&&) -> polygon_2d_builder& = delete;
    auto operator=(const polygon_2d_builder&) -> polygon_2d_builder& = delete;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Reserves space for `n` vertices in every attribute
    ///
    /// \param n the number of vertices
    auto reserve(std::size_t n) -> void;

    /// \{
    /// \brief Reserves space for `n` vertex attributes for the specified
    ///        attribute
    ///
    /// \param n the number of entries
    auto reserve_positions(std::size_t n) -> void;
    auto reserve_colors(std::size_t n) -> void;
    auto reserve_uvs(std::size_t n) -> void;
    auto reserve_normals(std::size_t n) -> void;
    /// \}

    /// \brief Reserves space for the indices of \p n triangle faces
    ///
    /// \param n the number of faces
    auto reserve_indices(std::size_t n) -> void;

    //--------------------------------------------------------------------------
    // Setters
    //--------------------------------------------------------------------------
  public:

    /// \brief Sets the positions for this mesh
    ///
    /// \param positions the positions
    auto set_positions(core::vector<core::point2> positions) -> void;

    /// \brief Sets the colors for this mesh
    ///
    /// \param colors the colors
    auto set_colors(core::vector<core::color> colors) -> void;

    /// \brief Sets the UV coordinates for this mesh
    ///
    /// \param uvs the uv coordinates
    auto set_uvs(core::vector<core::point2> uvs) -> void;

    /// \brief Sets the normals for this mesh
    ///
    /// \param normals the normals for this mesh
    auto set_normals(core::vector<core::vector3> normals) -> void;

    /// \brief Sets the indices for this mesh
    ///
    /// \param indices the indices for this mesh
    auto set_indices(core::vector<index_type> indices) -> void;

    //--------------------------------------------------------------------------
    // Incremental Building
    //--------------------------------------------------------------------------
  public:

    /// \brief Adds a position to the mesh
    ///
    /// \param p the position
    auto add_position(const core::point2& p) -> void;

    /// \brief Adds a color to the mesh
    ///
    /// \param c the color
    auto add_color(const core::color& c) -> void;

    /// \brief Adds a UV coordinate to the mesh
    ///
    /// \param p the uv coordinate
    auto add_uv(const core::point2& p) -> void;

    /// \brief Adds a normal vector to the mesh
    ///
    /// \param v the vector
    auto add_normal(const core::vector3& v) -> void;

    /// \brief Adds a single index to the mesh
    ///
    /// \param v0 the index of the vertex
    auto add_index(index_type v0) -> void;

    /// \brief Adds a triangle face for the mesh from 3 indices
    ///
    /// \param v0 the first index
    /// \param v1 the second index
    /// \param v2 the third index
    auto add_face(index_type v0, index_type v1, index_type v2) -> void;

    /// \brief Adds a quad face for the mesh from 4 indices, as the two
    ///        triangles `(v0, v1, v2)` and `(v2, v3, v0)`
    ///
    /// \param v0 the first index
    /// \param v1 the second index
    /// \param v2 the third index
    /// \param v3 the fourth index
    auto add_face(index_type v0, index_type v1, index_type v2, index_type v3)
      -> void;

    /// \brief Starts a new submesh at the current position
    ///
    /// Every vertex and index added after this call, up until the next call,
    /// belongs to the new submesh. Anything added before the first call forms
    /// an initial submesh of its own, if there is any. If this is never
    /// called, the mesh has a single submesh that covers all of it.
    ///
    /// Submeshes are only meaningful when attributes are added incrementally,
    /// since the `set_*` functions replace every entry.
    auto begin_submesh() -> void;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    /// \brief Builds this polygon using memory from the specified allocator
    ///        \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the mesh object
    auto build_with(core::allocator alloc) const -> mesh override;

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    /// The number of vertices and indices preceding a submesh
    struct submesh_start
    {
      std::size_t vertex;
      std::size_t index;
    };

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::vector<core::point2> m_positions;
    core::vector<core::color> m_colors;
    core::vector<core::point2> m_uvs;
    core::vector<core::vector3> m_normals;
    core::vector<index_type> m_indices;
    core::vector<submesh_start> m_submesh_starts;
    primitive_topology m_topology;
    winding_order m_winding;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_2D_POLYGON_2D_BUILDER_HPP */
