// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file unsafe_mesh_builder.hpp
///
/// \brief This header defines the low-level builder that packs vertex
///        attributes directly into a mesh buffer
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP
#define ALLOY_RENDER_MESH_UNSAFE_MESH_BUILDER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/mesh_builder.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/render/mesh/winding_order.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/model/color.hpp"

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t, std::uint32_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A builder for meshes that performs no safety / accuracy checks
  ///        in release builds.
  ///
  /// This builder is essentially a wrapper around the packed_buffer writer
  /// that produces the buffers used in meshes, converting each attribute to
  /// its packed format and padding the end of each vertex. This has a very
  /// weak guarantee that, for each vertex, every attribute present in the
  /// `vertex_components` must be added exactly once, in the following order:
  ///
  /// * position,
  /// * uv,
  /// * uv2,
  /// * normal,
  /// * tangent,
  /// * weights,
  /// * color,
  /// * joints
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

    /// Indices are accepted as 32-bit values, and narrowed to 16 bits when
    /// the built mesh has few enough vertices for them to fit
    using index_type = std::uint32_t;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a builder for vertices made of \p components
    ///
    /// \param components the number of components of each vertex attribute
    /// \param topology the topology of the built mesh
    /// \param winding the winding order of front faces in the built mesh
    /// \param alloc the allocator to build the mesh with
    explicit unsafe_mesh_builder(
      vertex_components components,
      primitive_topology topology = primitive_topology::triangles,
      winding_order winding = winding_order::counter_clockwise,
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

    /// \brief Sets the bounding volume of the built mesh
    ///
    /// The builder does not compute this itself, since the caller generally
    /// knows it without inspecting every position.
    ///
    /// \param box a box that encloses every position
    auto set_bounding_box(const core::axis_aligned_box& box) -> void;

    /// \{
    /// \brief Adds the position of the next vertex
    ///
    /// Only the leading `components.position` coordinates are stored.
    ///
    /// \param p the position
    auto add_position(const core::point2& p) -> void;
    auto add_position(const core::point3& p) -> void;
    /// \}

    /// \{
    /// \brief Adds the UV coordinate of the current vertex
    ///
    /// Only the leading `components.uv` coordinates are stored.
    ///
    /// \param p the UV coordinate
    auto add_uv(float p) -> void;
    auto add_uv(const core::point2& p) -> void;
    auto add_uv(const core::point3& p) -> void;
    /// \}

    /// \{
    /// \brief Adds the secondary UV coordinate of the current vertex
    ///
    /// Only the leading `components.uv2` coordinates are stored.
    ///
    /// \param p the secondary UV coordinate
    auto add_uv2(float p) -> void;
    auto add_uv2(const core::point2& p) -> void;
    auto add_uv2(const core::point3& p) -> void;
    /// \}

    /// \brief Adds the normal of the current vertex
    ///
    /// Only the leading `components.normal` components are stored, and each
    /// is clamped to `[-1, 1]`.
    ///
    /// \param v the normal
    auto add_normal(const core::vector3& v) -> void;

    /// \{
    /// \brief Adds the tangent of the current vertex
    ///
    /// Only the leading `components.tangent` components are stored, and each
    /// is clamped to `[-1, 1]`. A 4-component tangent holds the handedness
    /// of the tangent basis in `w`, as `1` or `-1`.
    ///
    /// \param v the tangent
    auto add_tangent(const core::vector3& v) -> void;
    auto add_tangent(const core::vector4& v) -> void;
    /// \}

    /// \brief Adds the skinning weights of the current vertex
    ///
    /// Only the leading `components.weights` weights are stored, and each is
    /// clamped to `[0, 1]`.
    ///
    /// \param v the weights
    auto add_weights(const core::vector4& v) -> void;

    /// \brief Adds the color of the current vertex
    ///
    /// Only the leading `components.color` channels are stored, in RGBA
    /// order.
    ///
    /// \param c the color
    auto add_color(const core::color& c) -> void;

    /// \brief Adds the skinning joint indices of the current vertex
    ///
    /// Only the leading `components.joints` indices are stored.
    ///
    /// \param joints the joint indices
    auto add_joints(const std::array<std::uint8_t,4>& joints) -> void;

    /// \brief Adds a single index
    ///
    /// \param i the index of the vertex
    auto add_index(index_type i) -> void;

    /// \brief Adds the indices of a triangle face
    ///
    /// \param i0 the index of the first vertex
    /// \param i1 the index of the second vertex
    /// \param i2 the index of the third vertex
    auto add_face(index_type i0, index_type i1, index_type i2) -> void;

    /// \brief Adds the indices of a quad face, as the two triangles
    ///        `(i0, i1, i2)` and `(i2, i3, i0)`
    ///
    /// \param i0 the index of the first vertex
    /// \param i1 the index of the second vertex
    /// \param i2 the index of the third vertex
    /// \param i3 the index of the fourth vertex
    auto add_face(index_type i0, index_type i1, index_type i2, index_type i3)
      -> void;

    /// \brief Starts a new submesh at the current position
    ///
    /// Every index (or vertex, if the mesh has no indices) added after this
    /// call, up until the next call, belongs to the new submesh. Anything
    /// added before the first call forms an initial submesh of its own, if
    /// there is any. If this is never called, the mesh has no submeshes.
    auto begin_submesh() -> void;

    //--------------------------------------------------------------------------
    // Building
    //--------------------------------------------------------------------------
  public:

    /// \brief Builds the mesh by consuming this builder, without copying the
    ///        vertex data
    ///
    /// \return the mesh object
    auto release() && -> mesh;

    /// \brief Builds a copy of the mesh using memory from \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the mesh object
    auto build_with(core::allocator alloc) const -> mesh override;

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    /// The vertex attributes, in the order they are interleaved
    enum class attribute : std::uint8_t {
      position,
      uv,
      uv2,
      normal,
      tangent,
      weights,
      color,
      joints,
    };

    /// The position at which a submesh begins in both the index and vertex
    /// streams, since which of them it is measured in is only known once the
    /// mesh is built
    struct submesh_start
    {
      std::uint32_t index;
      std::uint32_t vertex;
    };

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::allocator m_allocator;
    core::packed_buffer m_buffer;
    core::packed_buffer_writer m_writer;
    core::packed_buffer m_index_data;
    core::packed_buffer_writer m_index_writer;
    core::vector<submesh_start> m_submesh_starts;
    primitive_topology m_topology;
    winding_order m_winding;
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

    /// \brief Gets the number of whole vertices that have been added
    ///
    /// \return the number of vertices
    auto vertex_count() const noexcept -> std::size_t;

    /// \brief Resolves the started submeshes into ranges, allocated from
    ///        \p alloc
    ///
    /// \param alloc the allocator to use
    /// \return the submeshes
    auto make_submeshes(core::allocator alloc) const -> core::vector<submesh>;

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
