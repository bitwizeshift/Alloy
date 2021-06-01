////////////////////////////////////////////////////////////////////////////////
/// \file mesh.hpp
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

#ifndef ALLOY_RENDER_MESH_MESH_2D_BUILDER_HPP
#define ALLOY_RENDER_MESH_MESH_2D_BUILDER_HPP

#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"

#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/model/color.hpp"

#include <cstdint> // std::uint16_t
#include <cstddef> // std::size_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A builder for 2-dimensional meshes (e.g. a system with 2-component
  ///        positions).
  ///
  /// Normals still exist as a possibility in 3-dimensions.
  ///
  /// Currently this only works for 2d textures, and simply pretends that 3d and
  /// 1d textures don't exist.
  /// This may be slated for a future release if ever this is deemed a useful
  /// feature
  //////////////////////////////////////////////////////////////////////////////
  class polygon_2d_builder : public mesh_builder
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

    /// \brief Constructs this mesh_2d_builder with the specified topology
    ///
    /// \param topology the topology for the mesh
    /// \param alloc the allocator to use for intermediate allocations
    explicit polygon_2d_builder(
      primitive_topology topology = primitive_topology::triangles,
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
    auto reserve_normals(std::size_t n) -> void;
    auto reserve_indices(std::size_t n) -> void;
    /// \}

    //--------------------------------------------------------------------------
    // Setters
    //--------------------------------------------------------------------------
  public:

    /// \brief Sets the positions for this mesh
    ///
    /// \param positions the positions
    auto set_positions(core::vector<core::point2> positions) -> void;

    /// \brief Sets the color coordinates for this mesh
    ///
    /// \param uvs the color coordinates
    auto set_colors(core::vector<core::color> uvs) -> void;

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
    auto set_indices(core::vector<std::uint16_t> indices) -> void;

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
    /// \param p the position
    auto add_uv(const core::point2& p) -> void;

    /// \brief Adds a normal vector to the mesh
    ///
    /// \param v the vector
    auto add_normal(const core::vector3& v) -> void;

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
    auto add_face(index_type v0, index_type v1, index_type v2, index_type v3) -> void;

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

    core::vector<core::point2> m_positions;
    core::vector<core::color> m_colors;
    core::vector<core::point2> m_uvs;
    core::vector<core::vector3> m_normals;
    core::vector<std::uint16_t> m_indices;
    core::point3 m_lower_left;
    core::point3 m_upper_right;
    primitive_topology m_topology;

    auto construct_data(core::packed_buffer& buffer) const -> void;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_MESH_2D_BUILDER_HPP */
