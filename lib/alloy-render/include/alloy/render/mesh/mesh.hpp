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

#ifndef ALLOY_RENDER_MESH_MESH_HPP
#define ALLOY_RENDER_MESH_MESH_HPP

#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/core/containers/span.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/core/intrinsics.hpp"

#include <cstddef> // std::byte
#include <cstdint> // std::uint8_t, std::uint16_t, etc

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Configuration data to use for constructing mesh objects.
  //////////////////////////////////////////////////////////////////////////////
  struct mesh_config
  {
    // TODO(bitwizeshift): Determine whether mesh objects should contain the
    //   winding-order. Logically, a mesh knows what direction its "face"
    //   objects should be -- not the material. But the material needs this
    //   information to render.

    using index_type = std::uint16_t;

    core::packed_buffer vertex_data;
    core::vector<index_type> indices;
    primitive_topology topology;

    std::uint8_t position_components;
    std::uint8_t color_components;
    std::uint8_t uv_components;
    std::uint8_t normal_components;
    std::uint8_t tangent_components;
    std::uint8_t bitangent_components;

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
  /// * attributes: [v][n][u][v][n][u] ... [v][n][u]
  ///
  /// This is able to be better interpreted by the GPU since it has far better
  /// locality, and can easily fit into a single vertex buffer rather than
  /// requiring several.
  ///
  /// Additionally, this interlaced mesh also *compresses* the underlying data
  /// into an ideal format to minimize GPU memory overhead, while also taking
  /// care of appropriate alignment requirements.
  ///
  /// In particular, all `normal`, `tangent`, and `bitangent` attributes are
  /// condensed to `uint16` types, since the exact precision is not necessary.
  /// Padding is added, as appropriate
  //////////////////////////////////////////////////////////////////////////////
  class mesh
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using index_type = std::uint16_t;

    ////////////////////////////////////////////////////////////////////////////
    /// \brief Represents a possible construction error state
    ////////////////////////////////////////////////////////////////////////////
    enum class construct_error {
      /// Supplied indices are invalid for the topology. This may occur if a
      /// mesh contains triangle topology and the number of indices supplied
      /// are not divisible by 3.
      invalid_indices,

      /// The vertex data provided is invalid. This may occur if the number of
      /// bytes is not divisible by `stride()`
      invalid_vertex_data,

      /// The specified number of position components is invalid. This can
      /// occur if `position_components` is not between `0` and `4` (inclusive)
      invalid_position_component,

      /// The specified number of color components is invalid. This can occur if
      /// `color_components` is not between `0` and `4` (inclusive)
      invalid_color_component,

      /// The specified UV components is invalid. This can occur if
      /// `uv_components` is not between `0` and `3` (inclusive)
      invalid_uv_component,

      /// The specified normal components is invalid. This can occur if
      /// `normal_components` is not between `0` and `4` (inclusive)
      invalid_normal_component,

      /// The specified tangent components is invalid. This can occur if
      /// `tangent_components` is not between `0` and `4` (inclusive)
      invalid_tangent_component,

      /// The specified bitangent components is invalid. This can occur if
      /// `bitangent_components` is not between `0` and `4` (inclusive)
      invalid_bitangent_component,
    };

    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a mesh from the specified \p config
    ///
    /// \param config the mesh configuration to use
    /// \return the constructed mesh, on success
    static auto from_config(mesh_config config) -> core::result<mesh,construct_error>;

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
    /// \return reference to \c (*this)
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
    auto has_normals() const noexcept -> bool;
    auto has_tangents() const noexcept -> bool;
    auto has_bitangents() const noexcept -> bool;
    auto has_indices() const noexcept -> bool;
    /// \}

    /// \{
    /// \brief Gets the offset to the specified vertex attribute in this mesh
    ///
    /// \return the offset, in bytes, to the specified attribute
    auto offset_to_positions() const noexcept -> core::bytes;
    auto offset_to_colors() const noexcept -> core::bytes;
    auto offset_to_uvs() const noexcept -> core::bytes;
    auto offset_to_normals() const noexcept -> core::bytes;
    auto offset_to_tangents() const noexcept -> core::bytes;
    auto offset_to_bitangents() const noexcept -> core::bytes;
    /// \}

    /// \{
    /// \brief Gets how many bytes exist per vertex attribute in this mesh
    ///
    /// \return the number of bytes to offset into the buffer for the start of
    ///         the attribute
    auto bytes_per_position() const noexcept -> core::bytes;
    auto bytes_per_colors() const noexcept -> core::bytes;
    auto bytes_per_uvs() const noexcept -> core::bytes;
    auto bytes_per_normals() const noexcept -> core::bytes;
    auto bytes_per_tangents() const noexcept -> core::bytes;
    auto bytes_per_bitangents() const noexcept -> core::bytes;
    /// \}

    /// \{
    /// \brief Gets how many components exist per vertex attribute in this mesh
    ///
    /// \return the number of components in this vertex
    auto position_components() const noexcept -> std::size_t;
    auto color_components() const noexcept -> std::size_t;
    auto uv_components() const noexcept -> std::size_t;
    auto normal_components() const noexcept -> std::size_t;
    auto tangent_components() const noexcept -> std::size_t;
    auto bitangent_components() const noexcept -> std::size_t;
    /// \}

    /// \brief Gets the stride for this interlaced mesh
    ///
    /// \return the stride
    auto stride() const noexcept -> core::bytes;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the primitive topology of this mesh
    ///
    /// \return the topology of this mesh
    auto topology() const noexcept -> primitive_topology;

    /// \brief Gets the (compressed) data associated with this mesh
    ///
    /// \return the data as a span
    auto data() const noexcept -> core::span<const std::byte>;

    /// \brief Gets the mesh indices
    ///
    /// \return the mesh indices
    auto indices() const noexcept -> core::span<const index_type>;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    /// The compressed vertex data stream
    core::packed_buffer m_compressed_data;

    /// The indices for this mesh
    core::vector<index_type> m_indices;

    /// An axis-aligned box that contains this volume. This is used for
    /// scene-culling estimates to avoid sending models to the renderer that are
    /// off-screen.
    core::axis_aligned_box m_bounding_volume;

    /// The topology required to draw this mesh
    primitive_topology m_topology;

    /// The number of components per position
    std::uint8_t m_position_components;

    /// The number of components per color
    std::uint8_t m_color_components;

    /// The number of components per UV
    std::uint8_t m_uv_components;

    /// The number of components per normal.
    /// Upper 4 bits store the raw number, (e.g. '3'), the lower 4 bits store
    /// the padded number of components (e.g. '4'). This is needed to preserve
    /// data alignment in vertex buffers.
    std::uint8_t m_normal_components;

    /// The number of components per tangent.
    /// Upper 4 bits store the raw number, (e.g. '3'), the lower 4 bits store
    /// the padded number of components (e.g. '4'). This is needed to preserve
    /// data alignment in vertex buffers.
    std::uint8_t m_tangent_components;

    /// The number of components per bitangent.
    /// Upper 4 bits store the raw number, (e.g. '3'), the lower 4 bits store
    /// the padded number of components (e.g. '4'). This is needed to preserve
    /// data alignment in vertex buffers.
    std::uint8_t m_bitangent_components;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    mesh(const mesh& other);
    mesh(const mesh& other, core::allocator alloc);
    mesh(mesh_config config);

    static auto round_up_power_two(std::size_t n) noexcept -> std::size_t;
  };

  /// \brief Gets the error message from the `construct_error`
  ///
  /// \param e the error
  /// \return the message
  auto error_message(mesh::construct_error e) noexcept -> core::zstring_view;

} // namespace alloy::render

//------------------------------------------------------------------------------
// Queries
//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::has_positions()
  const noexcept -> bool
{
  return m_position_components != 0u;
}


inline
auto alloy::render::mesh::has_color()
  const noexcept -> bool
{
  return m_color_components != 0u;
}

inline
auto alloy::render::mesh::has_uvs()
  const noexcept -> bool
{
  return m_uv_components != 0u;
}

inline
auto alloy::render::mesh::has_normals()
  const noexcept -> bool
{
  return m_normal_components != 0u;
}

inline
auto alloy::render::mesh::has_tangents()
  const noexcept -> bool
{
  return m_tangent_components != 0u;
}

inline
auto alloy::render::mesh::has_bitangents()
  const noexcept -> bool
{
  return m_bitangent_components != 0u;
}

inline
auto alloy::render::mesh::has_indices()
  const noexcept -> bool
{
  return !m_indices.empty();
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::offset_to_positions()
  const noexcept -> core::bytes
{
  return core::bytes{0u};
}

inline
auto alloy::render::mesh::offset_to_colors()
  const noexcept -> core::bytes
{
  return offset_to_positions() + bytes_per_colors();
}

inline
auto alloy::render::mesh::offset_to_uvs()
  const noexcept -> core::bytes
{
  return offset_to_positions() + bytes_per_position();
}

inline
auto alloy::render::mesh::offset_to_normals()
  const noexcept -> core::bytes
{
  return offset_to_uvs() + bytes_per_uvs();
}

inline
auto alloy::render::mesh::offset_to_tangents()
  const noexcept -> core::bytes
{
  return offset_to_normals() + bytes_per_normals();
}

inline
auto alloy::render::mesh::offset_to_bitangents()
  const noexcept -> core::bytes
{
  return offset_to_tangents() + bytes_per_tangents();
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::bytes_per_position()
  const noexcept -> core::bytes
{
  return core::bytes{
    m_position_components * sizeof(float)
  };
}

inline
auto alloy::render::mesh::bytes_per_colors()
  const noexcept -> core::bytes
{
  return core::bytes{
    (m_color_components & 0x0f)
  };
}

inline
auto alloy::render::mesh::bytes_per_uvs()
  const noexcept -> core::bytes
{
  return core::bytes{
    m_uv_components * sizeof(float)
  };
}

inline
auto alloy::render::mesh::bytes_per_normals()
  const noexcept -> core::bytes
{
  return core::bytes{
    (m_normal_components & 0x0f) * sizeof(std::uint16_t)
  };
}

inline
auto alloy::render::mesh::bytes_per_tangents()
  const noexcept -> core::bytes
{
  return core::bytes{
    (m_tangent_components & 0x0f) * sizeof(std::uint16_t)
  };
}

inline
auto alloy::render::mesh::bytes_per_bitangents()
  const noexcept -> core::bytes
{
  return core::bytes{
    (m_bitangent_components & 0x0f) * sizeof(std::uint16_t)
  };
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::position_components()
  const noexcept -> std::size_t
{
  return m_position_components;
}

inline
auto alloy::render::mesh::color_components()
  const noexcept -> std::size_t
{
  return (m_color_components >> 4u);
}

inline
auto alloy::render::mesh::uv_components()
  const noexcept -> std::size_t
{
  return m_uv_components;
}

inline
auto alloy::render::mesh::normal_components()
  const noexcept -> std::size_t
{
  return (m_normal_components >> 4u);
}

inline
auto alloy::render::mesh::tangent_components()
  const noexcept -> std::size_t
{
  return (m_tangent_components >> 4u);
}

inline
auto alloy::render::mesh::bitangent_components()
  const noexcept -> std::size_t
{
  return (m_bitangent_components >> 4u);
}

//------------------------------------------------------------------------------

inline
auto alloy::render::mesh::stride()
  const noexcept -> core::bytes
{
  return (
    bytes_per_position() +
    bytes_per_uvs() +
    bytes_per_normals() +
    bytes_per_tangents() +
    bytes_per_bitangents()
  );
}

inline
auto alloy::render::mesh::topology()
  const noexcept -> primitive_topology
{
  return m_topology;
}

inline
auto alloy::render::mesh::data()
  const noexcept -> core::span<const std::byte>
{
  return core::span<const std::byte>{
    m_compressed_data.data(),
    m_compressed_data.size().count(),
  };
}

inline
auto alloy::render::mesh::indices()
  const noexcept -> core::span<const index_type>
{
  return core::span<const index_type>{
    m_indices.data(),
    m_indices.size(),
  };
}

inline
auto alloy::render::mesh::round_up_power_two(std::size_t v)
  noexcept -> std::size_t
{
  // From bit-twiddling hacks. Rounds up to nearest power-of-two.
  // This is only done to 2 components, since we only need to round up to
  // '4' (2^2) in the case that 2-byte (uint16_t) values are either 1 or 3
  // component.
  v--;
  v |= (v >> 1);
  v |= (v >> 2);
  v++;
  return v;
}

#endif /* ALLOY_RENDER_MESH_MESH_HPP */
