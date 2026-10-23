// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file mesh.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_MESH_HPP
#define ALLOY_RENDER_MESH_MESH_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/mesh/index_format.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/render/mesh/winding_order.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef> // std::byte, std::size_t
#include <cstdint> // std::uint8_t, std::uint16_t, std::uint32_t
#include <cstring> // std::memcpy
#include <span>    // std::span

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The number of components of each attribute in a mesh vertex
  ///
  /// The members are declared in the order in which the attributes are
  /// interleaved within a vertex. A count of `0` means that the attribute is
  /// absent.
  //////////////////////////////////////////////////////////////////////////////
  struct vertex_components
  {
    /// The number of 32-bit float components per position, in `[0, 4]`
    std::uint8_t position = 0u;

    /// The number of 32-bit float components per UV, in `[0, 3]`
    std::uint8_t uv = 0u;

    /// The number of 32-bit float components per secondary UV, such as for
    /// lightmaps, in `[0, 3]`
    std::uint8_t uv2 = 0u;

    /// The number of snorm16 components per normal, in `[0, 4]`
    std::uint8_t normal = 0u;

    /// The number of snorm16 components per tangent, in `[0, 4]`. A fourth
    /// component holds the handedness of the tangent basis, as `1` or `-1`,
    /// from which the bitangent is reconstructed as
    /// `cross(normal, tangent.xyz) * tangent.w`.
    std::uint8_t tangent = 0u;

    /// The number of unorm16 skinning weights per vertex, in `[0, 4]`
    std::uint8_t weights = 0u;

    /// The number of unorm8 components per color, in `[0, 4]`
    std::uint8_t color = 0u;

    /// The number of 8-bit unsigned integer skinning joint indices per
    /// vertex, in `[0, 4]`
    std::uint8_t joints = 0u;

    /// \brief Computes the number of bytes occupied by the attributes of a
    ///        single vertex, excluding any padding
    ///
    /// \return the unpadded size, in bytes
    constexpr auto unpadded_size() const noexcept -> core::bytes;

    /// \brief Computes the number of bytes between consecutive vertices
    ///
    /// Attributes are packed without any padding between them. The end of
    /// the vertex is padded only as far as is needed for the next vertex to
    /// retain the alignment of its most-aligned attribute.
    ///
    /// \return the stride, in bytes
    constexpr auto stride() const noexcept -> core::bytes;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A contiguous range of a mesh that is drawn as a unit, such as the
  ///        part of a mesh that uses a single material
  ///
  /// The range is measured in indices for meshes that have indices, and in
  /// vertices for meshes that do not.
  //////////////////////////////////////////////////////////////////////////////
  struct submesh
  {
    /// The first index (or vertex) of the range
    std::uint32_t first;

    /// The number of indices (or vertices) in the range
    std::uint32_t count;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Configuration data to use for constructing mesh objects.
  //////////////////////////////////////////////////////////////////////////////
  struct mesh_config
  {
    core::packed_buffer vertex_data;
    core::packed_buffer index_data;
    render::index_format index_format;
    primitive_topology topology;
    winding_order winding;
    vertex_components components;
    core::vector<submesh> submeshes;
    core::axis_aligned_box bounding_volume;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A mesh is a buffer of vertex data where each vertex element is
  ///        interlaced, packed, and strided rather than contiguous.
  ///
  /// In a standard mesh, the layout of vertex data is typically:
  /// * vertex: [v][v][v] ... [v][v][v]
  /// * normal: [n][n][n] ... [n][n][n]
  /// * uvs:    [u][u][u] ... [u][u][u]
  /// * etc., for each attribute
  ///
  /// In an interlaced mesh, the layout of the vertex data is strided instead,
  /// such as:
  ///
  /// * attributes: [v][u][n][v][u][n] ... [v][u][n]
  ///
  /// This is able to be better interpreted by the GPU since it has far better
  /// locality, and can easily fit into a single vertex buffer rather than
  /// requiring several.
  ///
  /// Additionally, this interlaced mesh also *compresses* the underlying data
  /// into an ideal format to minimize GPU memory overhead. Positions and UVs
  /// are 32-bit floats, normals and tangents are snorm16, skinning weights
  /// are unorm16, colors are unorm8, and skinning joints are 8-bit unsigned
  /// integers. Attributes are ordered from most to least aligned, so
  /// that no padding is needed between them; see `vertex_components::stride`.
  //////////////////////////////////////////////////////////////////////////////
  class mesh
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Represents a possible construction error state
    ////////////////////////////////////////////////////////////////////////////
    enum class construct_error {
      /// Supplied indices are invalid for the topology. This may occur if a
      /// mesh contains triangle topology and the number of indices supplied
      /// are not divisible by 3, if the index data is not a whole number of
      /// indices, or if an index refers to a vertex that does not exist.
      invalid_indices,

      /// The vertex data provided is invalid. This may occur if the number of
      /// bytes is not divisible by `stride()`, if vertex data is supplied
      /// without any vertex components, or if a mesh without indices does
      /// not contain a whole number of primitives for its topology.
      invalid_vertex_data,

      /// The specified submeshes are invalid. This can occur if a submesh
      /// extends past the end of the mesh, or does not contain a whole number
      /// of primitives for the topology.
      invalid_submeshes,

      /// The specified number of position components is invalid. This can
      /// occur if `position_components` is not between `0` and `4` (inclusive)
      invalid_position_component,

      /// The specified number of color components is invalid. This can occur if
      /// `color_components` is not between `0` and `4` (inclusive)
      invalid_color_component,

      /// The specified UV components is invalid. This can occur if
      /// `uv_components` is not between `0` and `3` (inclusive)
      invalid_uv_component,

      /// The specified secondary UV components is invalid. This can occur if
      /// `uv2_components` is not between `0` and `3` (inclusive)
      invalid_uv2_component,

      /// The specified normal components is invalid. This can occur if
      /// `normal_components` is not between `0` and `4` (inclusive)
      invalid_normal_component,

      /// The specified tangent components is invalid. This can occur if
      /// `tangent_components` is not between `0` and `4` (inclusive)
      invalid_tangent_component,

      /// The specified weight components is invalid. This can occur if
      /// `weight_components` is not between `0` and `4` (inclusive)
      invalid_weight_component,

      /// The specified joint components is invalid. This can occur if
      /// `joint_components` is not between `0` and `4` (inclusive)
      invalid_joint_component,
    };

    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a mesh from the specified \p config
    ///
    /// \param config the mesh configuration to use
    /// \return the constructed mesh, on success
    static auto from_config(mesh_config config)
      -> core::result<mesh,construct_error>;

    /// \brief Constructs a mesh from the specified \p config without performing
    ///        any checks
    ///
    /// \warning
    /// Only use this factory if you know that the supplied data is 100%
    /// accurate; otherwise this will cause undefined behavior when sent to
    /// the GPU.
    ///
    /// \param config the mesh configuration to use
    /// \return the constructed mesh
    static auto from_config_unchecked(mesh_config config) -> mesh;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    mesh() = delete;
    /// \brief Moves the contents of \p other
    ///
    /// \param other the other mesh to move
    mesh(mesh&& other) = default;

    /// \brief Moves the contents of \p other
    ///
    /// \param other the other mesh to move
    /// \return reference to `(*this)`
    auto operator=(mesh&& other) -> mesh& = default;
    auto operator=(const mesh&) -> mesh& = delete;

    /// \brief Deep copies this mesh
    ///
    /// \return A copy of this mesh
    auto copy() const -> mesh;

    /// \brief Deep copies this mesh to the specified allocator
    ///
    /// \param alloc the allocator to use for the copy
    /// \return A copy of this mesh
    auto copy_with(core::allocator alloc) const -> mesh;

    //--------------------------------------------------------------------------
    // Queries
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Queries whether the specified vertex attribute is present in this
    ///        interlaced mesh
    ///
    /// \return whether the attribute is present
    auto has_positions() const noexcept -> bool;
    auto has_color() const noexcept -> bool;
    auto has_uvs() const noexcept -> bool;
    auto has_uv2() const noexcept -> bool;
    auto has_normals() const noexcept -> bool;
    auto has_tangents() const noexcept -> bool;
    auto has_weights() const noexcept -> bool;
    auto has_joints() const noexcept -> bool;
    auto has_indices() const noexcept -> bool;
    /// \}

    /// \{
    /// \brief Gets the offset to the specified vertex attribute in this mesh
    ///
    /// \return the offset, in bytes, to the specified attribute
    auto offset_to_positions() const noexcept -> core::bytes;
    auto offset_to_colors() const noexcept -> core::bytes;
    auto offset_to_uvs() const noexcept -> core::bytes;
    auto offset_to_uv2() const noexcept -> core::bytes;
    auto offset_to_normals() const noexcept -> core::bytes;
    auto offset_to_tangents() const noexcept -> core::bytes;
    auto offset_to_weights() const noexcept -> core::bytes;
    auto offset_to_joints() const noexcept -> core::bytes;
    /// \}

    /// \{
    /// \brief Gets how many bytes exist per vertex attribute in this mesh
    ///
    /// \return the number of bytes the attribute occupies in each vertex
    auto bytes_per_position() const noexcept -> core::bytes;
    auto bytes_per_colors() const noexcept -> core::bytes;
    auto bytes_per_uvs() const noexcept -> core::bytes;
    auto bytes_per_uv2() const noexcept -> core::bytes;
    auto bytes_per_normals() const noexcept -> core::bytes;
    auto bytes_per_tangents() const noexcept -> core::bytes;
    auto bytes_per_weights() const noexcept -> core::bytes;
    auto bytes_per_joints() const noexcept -> core::bytes;
    /// \}

    /// \{
    /// \brief Gets how many components exist per vertex attribute in this mesh
    ///
    /// \return the number of components in this vertex
    auto position_components() const noexcept -> std::size_t;
    auto color_components() const noexcept -> std::size_t;
    auto uv_components() const noexcept -> std::size_t;
    auto uv2_components() const noexcept -> std::size_t;
    auto normal_components() const noexcept -> std::size_t;
    auto tangent_components() const noexcept -> std::size_t;
    auto weight_components() const noexcept -> std::size_t;
    auto joint_components() const noexcept -> std::size_t;
    /// \}

    /// \brief Gets the number of components of every vertex attribute
    ///
    /// \return the vertex components
    auto components() const noexcept -> const vertex_components&;

    /// \brief Gets the stride for this interlaced mesh
    ///
    /// \return the stride
    auto stride() const noexcept -> core::bytes;

    /// \brief Gets the number of vertices in this mesh
    ///
    /// \return the number of vertices
    auto vertex_count() const noexcept -> std::size_t;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the primitive topology of this mesh
    ///
    /// \return the topology of this mesh
    auto topology() const noexcept -> primitive_topology;

    /// \brief Gets the order in which the vertices of front faces appear
    ///
    /// \return the winding order of this mesh
    auto winding() const noexcept -> winding_order;

    /// \brief Gets the (compressed) data associated with this mesh
    ///
    /// \return the data as a span
    auto data() const noexcept -> std::span<const std::byte>;

    /// \brief Gets the format of the indices of this mesh
    ///
    /// \return the index format
    auto index_format() const noexcept -> render::index_format;

    /// \brief Gets the number of indices in this mesh
    ///
    /// \return the number of indices
    auto index_count() const noexcept -> std::size_t;

    /// \brief Gets the index at position \p i
    ///
    /// \pre `i < index_count()`
    /// \param i the position of the index
    /// \return the index, widened to 32 bits
    auto index_at(std::size_t i) const noexcept -> std::uint32_t;

    /// \brief Gets the raw index data of this mesh, in `index_format()`
    ///
    /// \return the index data as a span
    auto index_data() const noexcept -> std::span<const std::byte>;

    /// \brief Gets the ranges of this mesh that are drawn separately
    ///
    /// An empty list means that the mesh is drawn as a single range.
    ///
    /// \return the submeshes
    auto submeshes() const noexcept -> std::span<const submesh>;

    /// \brief Gets an axis-aligned box that encloses every position in this
    ///        mesh
    ///
    /// \return the bounding volume
    auto bounding_volume() const noexcept -> const core::axis_aligned_box&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    /// The compressed vertex data stream
    core::packed_buffer m_compressed_data;

    /// The indices for this mesh, in `m_index_format`
    core::packed_buffer m_index_data;

    /// The format of each index in `m_index_data`
    render::index_format m_index_format;

    /// The ranges of this mesh that are drawn separately
    core::vector<submesh> m_submeshes;

    /// An axis-aligned box that contains this volume. This is used for
    /// scene-culling estimates to avoid sending models to the renderer that are
    /// off-screen.
    core::axis_aligned_box m_bounding_volume;

    /// The topology required to draw this mesh
    primitive_topology m_topology;

    /// The order in which the vertices of front faces appear
    winding_order m_winding;

    /// The number of components of each vertex attribute
    vertex_components m_components;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    mesh(const mesh& other);
    mesh(const mesh& other, core::allocator alloc);
    mesh(mesh_config config);
  };

  /// \brief Gets the error message from the `construct_error`
  ///
  /// \param e the error
  /// \return the message
  auto error_message(mesh::construct_error e) noexcept -> core::zstring_view;

} // namespace alloy::render

//==============================================================================
// struct : vertex_components
//==============================================================================

inline constexpr
auto alloy::render::vertex_components::unpadded_size()
  const noexcept -> core::bytes
{
  return core::bytes{
    (position * sizeof(float)) +
    (uv * sizeof(float)) +
    (uv2 * sizeof(float)) +
    (normal * sizeof(std::int16_t)) +
    (tangent * sizeof(std::int16_t)) +
    (weights * sizeof(std::uint16_t)) +
    (color * sizeof(std::uint8_t)) +
    (joints * sizeof(std::uint8_t))
  };
}

inline constexpr
auto alloy::render::vertex_components::stride() const noexcept -> core::bytes
{
  const auto has_32_bit = (position != 0u) || (uv != 0u) || (uv2 != 0u);
  const auto has_16_bit = (
    (normal != 0u) || (tangent != 0u) || (weights != 0u)
  );
  const auto alignment = has_32_bit ? 4u : (has_16_bit ? 2u : 1u);
  const auto bytes = unpadded_size().count();

  return core::bytes{((bytes + alignment - 1u) / alignment) * alignment};
}

//==============================================================================
// class : mesh
//==============================================================================

//------------------------------------------------------------------------------
// Queries
//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::has_positions() const noexcept -> bool
{
  return m_components.position != 0u;
}

inline
auto alloy::render::mesh::has_color() const noexcept -> bool
{
  return m_components.color != 0u;
}

inline
auto alloy::render::mesh::has_uvs() const noexcept -> bool
{
  return m_components.uv != 0u;
}

inline
auto alloy::render::mesh::has_uv2() const noexcept -> bool
{
  return m_components.uv2 != 0u;
}

inline
auto alloy::render::mesh::has_normals() const noexcept -> bool
{
  return m_components.normal != 0u;
}

inline
auto alloy::render::mesh::has_tangents() const noexcept -> bool
{
  return m_components.tangent != 0u;
}

inline
auto alloy::render::mesh::has_weights() const noexcept -> bool
{
  return m_components.weights != 0u;
}

inline
auto alloy::render::mesh::has_joints() const noexcept -> bool
{
  return m_components.joints != 0u;
}

inline
auto alloy::render::mesh::has_indices() const noexcept -> bool
{
  return m_index_data.size().count() != 0u;
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::offset_to_positions() const noexcept -> core::bytes
{
  return core::bytes{0u};
}

inline
auto alloy::render::mesh::offset_to_colors() const noexcept -> core::bytes
{
  return offset_to_weights() + bytes_per_weights();
}

inline
auto alloy::render::mesh::offset_to_uvs() const noexcept -> core::bytes
{
  return offset_to_positions() + bytes_per_position();
}

inline
auto alloy::render::mesh::offset_to_uv2() const noexcept -> core::bytes
{
  return offset_to_uvs() + bytes_per_uvs();
}

inline
auto alloy::render::mesh::offset_to_normals() const noexcept -> core::bytes
{
  return offset_to_uv2() + bytes_per_uv2();
}

inline
auto alloy::render::mesh::offset_to_tangents() const noexcept -> core::bytes
{
  return offset_to_normals() + bytes_per_normals();
}

inline
auto alloy::render::mesh::offset_to_weights() const noexcept -> core::bytes
{
  return offset_to_tangents() + bytes_per_tangents();
}

inline
auto alloy::render::mesh::offset_to_joints() const noexcept -> core::bytes
{
  return offset_to_colors() + bytes_per_colors();
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::bytes_per_position() const noexcept -> core::bytes
{
  return core::bytes{m_components.position * sizeof(float)};
}

inline
auto alloy::render::mesh::bytes_per_colors() const noexcept -> core::bytes
{
  return core::bytes{m_components.color * sizeof(std::uint8_t)};
}

inline
auto alloy::render::mesh::bytes_per_uvs() const noexcept -> core::bytes
{
  return core::bytes{m_components.uv * sizeof(float)};
}

inline
auto alloy::render::mesh::bytes_per_uv2() const noexcept -> core::bytes
{
  return core::bytes{m_components.uv2 * sizeof(float)};
}

inline
auto alloy::render::mesh::bytes_per_normals() const noexcept -> core::bytes
{
  return core::bytes{m_components.normal * sizeof(std::int16_t)};
}

inline
auto alloy::render::mesh::bytes_per_tangents() const noexcept -> core::bytes
{
  return core::bytes{m_components.tangent * sizeof(std::int16_t)};
}

inline
auto alloy::render::mesh::bytes_per_weights() const noexcept -> core::bytes
{
  return core::bytes{m_components.weights * sizeof(std::uint16_t)};
}

inline
auto alloy::render::mesh::bytes_per_joints() const noexcept -> core::bytes
{
  return core::bytes{m_components.joints * sizeof(std::uint8_t)};
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::position_components() const noexcept -> std::size_t
{
  return m_components.position;
}

inline
auto alloy::render::mesh::color_components() const noexcept -> std::size_t
{
  return m_components.color;
}

inline
auto alloy::render::mesh::uv_components() const noexcept -> std::size_t
{
  return m_components.uv;
}

inline
auto alloy::render::mesh::uv2_components() const noexcept -> std::size_t
{
  return m_components.uv2;
}

inline
auto alloy::render::mesh::normal_components() const noexcept -> std::size_t
{
  return m_components.normal;
}

inline
auto alloy::render::mesh::tangent_components() const noexcept -> std::size_t
{
  return m_components.tangent;
}

inline
auto alloy::render::mesh::weight_components() const noexcept -> std::size_t
{
  return m_components.weights;
}

inline
auto alloy::render::mesh::joint_components() const noexcept -> std::size_t
{
  return m_components.joints;
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::components()
  const noexcept -> const vertex_components&
{
  return m_components;
}

inline
auto alloy::render::mesh::stride() const noexcept -> core::bytes
{
  return m_components.stride();
}

inline
auto alloy::render::mesh::vertex_count() const noexcept -> std::size_t
{
  const auto bytes_per_vertex = stride().count();
  if (bytes_per_vertex == 0u) {
    return 0u;
  }
  return static_cast<std::size_t>(
    m_compressed_data.size().count() / bytes_per_vertex
  );
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::topology() const noexcept -> primitive_topology
{
  return m_topology;
}

inline
auto alloy::render::mesh::winding() const noexcept -> winding_order
{
  return m_winding;
}

inline
auto alloy::render::mesh::data() const noexcept -> std::span<const std::byte>
{
  return std::span<const std::byte>{
    m_compressed_data.data(),
    m_compressed_data.size().count(),
  };
}

inline
auto alloy::render::mesh::index_format()
  const noexcept -> render::index_format
{
  return m_index_format;
}

inline
auto alloy::render::mesh::index_count() const noexcept -> std::size_t
{
  return static_cast<std::size_t>(
    m_index_data.size().count() / bytes_per_index(m_index_format).count()
  );
}

inline
auto alloy::render::mesh::index_at(std::size_t i)
  const noexcept -> std::uint32_t
{
  ALLOY_ASSERT(i < index_count());

  const auto* const p = m_index_data.data()
                      + (i * bytes_per_index(m_index_format).count());
  switch (m_index_format) {
    case render::index_format::u16: {
      auto result = std::uint16_t{};
      std::memcpy(&result, p, sizeof(result));
      return result;
    }
    case render::index_format::u32: {
      auto result = std::uint32_t{};
      std::memcpy(&result, p, sizeof(result));
      return result;
    }
  }
  core::compiler::unreachable();
}

inline
auto alloy::render::mesh::index_data()
  const noexcept -> std::span<const std::byte>
{
  return std::span<const std::byte>{
    m_index_data.data(),
    m_index_data.size().count(),
  };
}

inline
auto alloy::render::mesh::submeshes()
  const noexcept -> std::span<const submesh>
{
  return std::span<const submesh>{m_submeshes.data(), m_submeshes.size()};
}

inline
auto alloy::render::mesh::bounding_volume()
  const noexcept -> const core::axis_aligned_box&
{
  return m_bounding_volume;
}

#endif /* ALLOY_RENDER_MESH_MESH_HPP */
