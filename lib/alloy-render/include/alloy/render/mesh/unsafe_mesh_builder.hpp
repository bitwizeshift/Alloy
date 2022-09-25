////////////////////////////////////////////////////////////////////////////////
/// \file unsafe_mesh_builder.hpp .hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2022 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP
#define ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP

#include "alloy/render/mesh/mesh_builder.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"

#include "alloy/core/utilities/quantity.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/model/color.hpp"

#include <cstddef>
#include <cstdint>

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A builder for meshes that performs no safety / accuracy checks.
  ///
  /// This builder is essentially a wrapper around the packed_buffer writer
  /// that produces the buffers used in meshes. This has a very weak guarantee
  /// that all entries must be performed in the following interlaced order:
  ///
  /// * position,
  /// * uv,
  /// * normal,
  /// * tangent,
  /// * bitangent
  ///
  /// \warning
  /// This type has a strict requirement on the order of attributes being
  /// stored, and no automatic checking to ensure it's being used correctly.
  /// This can be dangerous if misused.
  /// Use at your own risk.
  /// In general, this exists to be used in the creation of other
  /// `mesh_builders`
  //////////////////////////////////////////////////////////////////////////////
  class unsafe_mesh_builder : public mesh_builder
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using index_type = std::uint16_t;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    unsafe_mesh_builder(
      primitive_topology topology = primitive_topology::triangles,
      core::allocator alloc = core::allocator{}
    ) noexcept;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    auto set_components_per_position(std::uint8_t n) noexcept-> void;
    auto set_components_per_color(std::uint8_t n) noexcept -> void;
    auto set_components_per_uv(std::uint8_t n) noexcept-> void;
    auto set_components_per_normal(std::uint8_t n) noexcept-> void;
    auto set_components_per_tangent(std::uint8_t n) noexcept-> void;
    auto set_components_per_bitangent(std::uint8_t n) noexcept-> void;

    auto reserve_vertices(std::size_t n) -> void;
    auto reserve_bytes(core::bytes b) -> void;
    auto reserve_indices(core::quantity<index_type> indices) -> void;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    auto set_bounding_box(const core::axis_aligned_box& box) -> void;

    auto add_position(const core::point2& p) -> void;
    auto add_position(const core::point3& p) -> void;

    auto add_color(const core::color& c) -> void;

    auto add_uv(float p) -> void;
    auto add_uv(const core::point2& p) -> void;
    auto add_uv(const core::point3& p) -> void;

    auto add_normal(const core::vector3& v) -> void;
    auto add_tangent(const core::vector3& v) -> void;
    auto add_bitangent(const core::vector3& v) -> void;

    auto add_index(index_type i) -> void;
    auto add_face(index_type i0, index_type i1, index_type i2) -> void;
    auto add_face(index_type i0, index_type i1, index_type i2, index_type i3) -> void;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    auto release() && -> mesh;

    auto build_with(core::allocator alloc) const -> mesh override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::packed_buffer m_buffer;
    core::packed_buffer_writer m_writer;
    core::vector<index_type> m_indices;
    primitive_topology m_topology;
    core::axis_aligned_box m_bound;
    std::uint8_t m_components_per_position;
    std::uint8_t m_components_per_color;
    std::uint8_t m_components_per_uv;
    std::uint8_t m_components_per_normal;
    std::uint8_t m_components_per_tangent;
    std::uint8_t m_components_per_bitangent;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP */
