// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file unsafe_mesh_builder.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP
#define ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/model/color.hpp"

#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t, std::uint16_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A builder for meshes that performs no safety / accuracy checks.
  ///
  /// This builder is essentially a wrapper around the packed_buffer writer
  /// that produces the buffers used in meshes. This has a very weak guarantee
  /// that, for each vertex, every attribute present in the
  /// `vertex_components` must be added exactly once, in the following order:
  ///
  /// * position,
  /// * uv,
  /// * normal,
  /// * tangent,
  /// * bitangent,
  /// * color
  ///
  /// \warning
  /// This type has a strict requirement on the order of attributes being
  /// stored, which is only checked by assertions in debug builds.
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

    /// \brief Constructs a builder for vertices made of \p components
    ///
    /// \param components the number of components of each vertex attribute
    /// \param topology the topology of the built mesh
    /// \param alloc the allocator to build the mesh with
    explicit unsafe_mesh_builder(
      vertex_components components,
      primitive_topology topology = primitive_topology::triangles,
      core::allocator alloc = core::allocator{}
    ) noexcept;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Reserves space for \p n vertices
    ///
    /// \param n the number of vertices
    auto reserve_vertices(std::size_t n) -> void;

    /// \brief Reserves space for \p b bytes of vertex data
    ///
    /// \param b the number of bytes
    auto reserve_bytes(core::bytes b) -> void;

    /// \brief Reserves space for the indices of \p n triangle faces
    ///
    /// \param n the number of faces
    auto reserve_indices(std::size_t n) -> void;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    auto set_bounding_box(const core::axis_aligned_box& box) -> void;

    auto add_position(const core::point2& p) -> void;
    auto add_position(const core::point3& p) -> void;

    auto add_uv(float p) -> void;
    auto add_uv(const core::point2& p) -> void;
    auto add_uv(const core::point3& p) -> void;

    auto add_normal(const core::vector3& v) -> void;
    auto add_tangent(const core::vector3& v) -> void;
    auto add_bitangent(const core::vector3& v) -> void;

    auto add_color(const core::color& c) -> void;

    auto add_index(index_type i) -> void;
    auto add_face(index_type i0, index_type i1, index_type i2) -> void;
    auto add_face(index_type i0, index_type i1, index_type i2, index_type i3)
      -> void;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    auto release() && -> mesh;

    auto build_with(core::allocator alloc) const -> mesh override;

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    /// The vertex attributes, in the order they are interleaved
    enum class attribute : std::uint8_t {
      position,
      uv,
      normal,
      tangent,
      bitangent,
      color,
    };

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::packed_buffer m_buffer;
    core::packed_buffer_writer m_writer;
    core::vector<index_type> m_indices;
    primitive_topology m_topology;
    core::axis_aligned_box m_bound;
    vertex_components m_components;

    /// The last attribute present in each vertex
    attribute m_last_attribute;

    /// The attribute expected to be added next. This is only advanced in
    /// debug builds, where it is used to check the order of attributes.
    attribute m_next_attribute;

    /// The number of bytes of padding following the last attribute
    std::uint8_t m_vertex_padding;

    //--------------------------------------------------------------------------
    // Private Observers
    //--------------------------------------------------------------------------
  private:

    /// \brief Gets the number of components of attribute \p a
    ///
    /// \param a the attribute
    /// \return the number of components
    auto component_count(attribute a) const noexcept -> std::uint8_t;

    /// \brief Gets the attribute present in each vertex that follows \p a,
    ///        wrapping around to the first attribute of the next vertex
    ///
    /// \param a the attribute
    /// \return the next attribute
    auto next_attribute_after(attribute a) const noexcept -> attribute;

    /// \brief Asserts that attribute \p a is the next attribute in the
    ///        vertex, and that it is being supplied from a value with at
    ///        least as many \p dimensions as it has components
    ///
    /// \param a the attribute being added
    /// \param dimensions the dimensions of the value being added
    auto check_attribute(attribute a, std::size_t dimensions) const noexcept
      -> void;

    /// \brief Asserts that only whole vertices have been added, and that
    ///        every index refers to one of them
    auto check_complete() const noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Modifiers
    //--------------------------------------------------------------------------
  private:

    /// \brief Determines the last attribute present in a vertex made of
    ///        \p components
    ///
    /// \param components the vertex components
    /// \return the last attribute
    static auto last_attribute_of(const vertex_components& components)
      noexcept -> attribute;

    /// \brief Pads the end of the vertex if \p a is its last attribute, and
    ///        advances the expected attribute in debug builds
    ///
    /// \param a the attribute that was just added
    auto complete_attribute(attribute a) -> void;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP */
