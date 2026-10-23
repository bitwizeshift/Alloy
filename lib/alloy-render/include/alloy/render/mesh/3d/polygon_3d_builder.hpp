// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file mesh_3d_builder.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_3D_POLYGON_3D_BUILDER_HPP
#define ALLOY_RENDER_MESH_3D_POLYGON_3D_BUILDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/model/color.hpp"

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <cstdint> // std::uint16_t


namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A builder for 3-dimensional meshes (e.g. a system with 3-component
  ///        positions).
  ///
  /// Currently this only works for 2d textures, and simply pretends that 3d and
  /// 1d textures don't exist.
  /// This may be slated for a future release if ever this is deemed a useful
  /// feature
  //////////////////////////////////////////////////////////////////////////////
  class polygon_3d_builder : public mesh_builder
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using index_type = std::uint16_t;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this mesh_3d_builder with the specified topology
    ///
    /// \param topology the topology for the mesh
    /// \param alloc the allocator to use for intermediate allocations
    explicit polygon_3d_builder(
      primitive_topology topology = primitive_topology::triangles,
      core::allocator alloc = core::allocator{}
    ) noexcept;
    polygon_3d_builder(polygon_3d_builder&&) = delete;
    polygon_3d_builder(const polygon_3d_builder&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(polygon_3d_builder&&) -> polygon_3d_builder& = delete;
    auto operator=(const polygon_3d_builder&) -> polygon_3d_builder& = delete;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Reserves space for `n` vertices for all 2-dimensional attributes
    ///
    /// \param n the size
    auto reserve(std::size_t n) -> void;

    /// \{
    /// \brief Reserves space for `n` vertex attributes for the specified
    ///        attribute
    ///
    /// \param n the number of entries
    auto reserve_positions(std::size_t n) -> void;
    auto reserve_colors(std::size_t n) -> void;
    auto reserve_uvs(std::size_t n) -> void;
    auto reserve_uv2s(std::size_t n) -> void;
    auto reserve_normals(std::size_t n) -> void;
    auto reserve_tangents(std::size_t n) -> void;
    auto reserve_weights(std::size_t n) -> void;
    auto reserve_joints(std::size_t n) -> void;
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
    auto set_positions(core::vector<core::point3> positions) -> void;

    /// \brief Sets the colors for this mesh
    ///
    /// \param colors the colors
    auto set_colors(core::vector<core::color> colors) -> void;

    /// \brief Sets the UV coordinates for this mesh
    ///
    /// \param uvs the uv coordinates
    auto set_uvs(core::vector<core::point2> uvs) -> void;

    /// \brief Sets the secondary UV coordinates for this mesh, such as for
    ///        lightmaps
    ///
    /// \param uv2s the secondary uv coordinates
    auto set_uv2s(core::vector<core::point2> uv2s) -> void;

    /// \brief Sets the normals for this mesh
    ///
    /// \pre `normals % 3 == 0`
    /// \param normals the normals for this mesh
    auto set_normals(core::vector<core::vector3> normals) -> void;

    /// \brief Sets the tangents for this mesh
    ///
    /// The `w` component of each tangent holds the handedness of the tangent
    /// basis, as `1` or `-1`.
    ///
    /// \param tangents the tangents for this mesh
    auto set_tangents(core::vector<core::vector4> tangents) -> void;

    /// \brief Sets the skinning weights for this mesh
    ///
    /// Each weight corresponds to the joint at the same position in the
    /// vertex's joint indices.
    ///
    /// \param weights the skinning weights for this mesh
    auto set_weights(core::vector<core::vector4> weights) -> void;

    /// \brief Sets the skinning joint indices for this mesh
    ///
    /// \param joints the skinning joint indices for this mesh
    auto set_joints(core::vector<std::array<std::uint8_t,4>> joints) -> void;

    /// \brief Sets the indices for this mesh
    ///
    /// \param indices the indices for this mesh
    auto set_indices(core::vector<std::uint16_t> indices) -> void;

    //--------------------------------------------------------------------------
    // Incremental Building
    //--------------------------------------------------------------------------
  public:

    /// \brief Adds a position to the mesh
    ///
    /// \param p the position
    auto add_position(const core::point3& p) -> void;

    /// \brief Adds a color to the mesh
    ///
    /// \param c the color
    auto add_color(const core::color& c) -> void;

    /// \brief Adds a UV coordinate to the mesh
    ///
    /// \param p the position
    auto add_uv(const core::point2& p) -> void;

    /// \brief Adds a secondary UV coordinate to the mesh, such as for
    ///        lightmaps
    ///
    /// \param p the secondary uv coordinate
    auto add_uv2(const core::point2& p) -> void;

    /// \brief Adds a normal vector to the mesh
    ///
    /// \param v the vector
    auto add_normal(const core::vector3& v) -> void;

    /// \brief Adds a tangent to the mesh
    ///
    /// The `w` component of the tangent holds the handedness of the tangent
    /// basis, as `1` or `-1`.
    ///
    /// \param v the tangent
    auto add_tangent(const core::vector4& v) -> void;

    /// \brief Adds the skinning weights of a vertex to the mesh
    ///
    /// \param v the weights
    auto add_weights(const core::vector4& v) -> void;

    /// \brief Adds the skinning joint indices of a vertex to the mesh
    ///
    /// \param joints the joint indices
    auto add_joints(const std::array<std::uint8_t,4>& joints) -> void;

    /// \brief Adds a
    auto add_index(index_type v0) -> void;

    /// \brief Adds a triangle face for the mesh from 3 indices
    ///
    /// \param v0 the first index
    /// \param v1 the second index
    /// \param v2 the third index
    auto add_face(index_type v0, index_type v1, index_type v2) -> void;

    /// \brief Adds a quad face for the mesh from 4 indices
    ///
    /// \param v0 the first index
    /// \param v1 the second index
    /// \param v2 the third index
    /// \param v3 the fourth index
    auto add_face(index_type v0, index_type v1, index_type v2, index_type v3)
      -> void;

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
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::vector<core::point3> m_positions;
    core::vector<core::color> m_colors;
    core::vector<core::point2> m_uvs;
    core::vector<core::point2> m_uv2s;
    core::vector<core::vector3> m_normals;
    core::vector<core::vector4> m_tangents;
    core::vector<core::vector4> m_weights;
    core::vector<std::array<std::uint8_t,4>> m_joints;
    core::vector<std::uint16_t> m_indices;
    primitive_topology m_topology;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_3D_POLYGON_3D_BUILDER_HPP */
