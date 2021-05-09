///////////////////////////////////////////////////////////////////////////////
/// \file mesh.hpp
///
/// \todo(Bitwize): Document this
///////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_MODEL_MESH_HPP
#define ALLOY_CORE_MODEL_MESH_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/utilities/quantity.hpp"

#include "alloy/core/model/vertex.hpp"
#include "alloy/core/model/normal.hpp"
#include "alloy/core/model/texture_coordinate.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/model/color_constants.hpp"

#include <cstdint>  // std::uint16_t
#include <iterator> // std::distance

namespace alloy::core {

  //===========================================================================
  // struct : vertex_data
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Collection of vertex data in a mesh
  ///
  /// Vertex data consists of the vertex point itself, the normal at that
  /// vertex, texture-coordinate, and color data.
  /////////////////////////////////////////////////////////////////////////////
  struct vertex_data
  {
    class vertex vertex;
    class normal normal;
    class texture_coordinate texture_coordinate;
    class color color;
  };

  //===========================================================================
  // class : mesh
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A mesh represents a series of vertices and attributes in a static
  ///        configuration.
  ///
  /// All mesh data is compressed and packed into a simple data-format for
  /// convenient use by the underlying graphics engine.
  /// To build a mesh on-the-fly, see `mesh_builder` -- which allows for
  /// specifying vertex information along with the face information from those
  /// vertices.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API mesh
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    // TODO(bitwize): Create a 'vertex_index_buffer' class where the underlying
    //                index type changes dynamically with the number of vertices
    using index_type = std::uint16_t;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Makes a mesh from data buffers
    ///
    /// This function does no checking of the buffers. Ensure that, by calling
    /// this, the input data is valid, and that indices point into a valid point
    /// of data into the buffer.
    ///
    /// \note
    /// At the moment, all meshes contain the same vertex attribute data as
    /// specified in `vertex_data`. In the future, meshes may optionally disable
    /// vertex attribute information.
    ///
    /// \param buffer the packed buffer
    /// \param vertices the number of vertices in the packed buffer
    /// \param indices the number of indices
    /// \return the created mesh
    static auto make_from_buffers(packed_buffer buffer,
                                  quantity<struct vertex_data> vertices,
                                  vector<index_type> indices) noexcept -> mesh;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a mesh without any vertex data
    mesh() noexcept;

    /// \brief Moves the contents of \p other to this mesh
    ///
    /// \param other the other mesh to move
    mesh(mesh&& other) noexcept = default;

    // Delete copy constructor
    mesh(const mesh&) = delete;

    //-------------------------------------------------------------------------

    /// \brief Moves the contents of \p other to this mesh
    ///
    /// \param other the other mesh to move
    /// \return reference to (*this)
    auto operator=(mesh&& other) noexcept -> mesh& = default;

    // Delete copy assignment
    auto operator=(const mesh& other) -> mesh& = delete;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets how many vertices are stored in this mesh
    ///
    /// \return the number of vertices
    auto vertices() const noexcept ->  quantity<struct vertex_data>;

    /// \brief Gets span of the vertex data
    ///
    /// \return a pointer to contiguous interlaced vertex data
    auto vertex_data() const noexcept -> span<const std::byte>;

    /// \brief Gets span of the index data
    ///
    /// \return a pointer to contiguous index data
    auto index_data() const noexcept -> span<const index_type>;

    /// \brief Gets the offset into the vertex data to reach the location
    ///        information, in bytes
    ///
    /// \return the offset
    auto vertex_offset() const noexcept -> bytes;

    /// \brief Gets the offset into the vertex data to reach the normal data,
    ///        in bytes
    ///
    /// \return the offset
    auto normal_offset() const noexcept -> bytes;

    /// \brief Gets the offset into the vertex data to reach the texture
    ///        coordinate data, in bytes
    ///
    /// \return the offset
    auto texture_coordinate_offset() const noexcept -> bytes;

    /// \brief Gets the offset into the vertex data to reach the color data,
    ///        in bytes
    ///
    /// \return the offset
    auto color_offset() const noexcept -> bytes;

    /// \brief Gets the stride of the vertex data in the mesh
    ///
    /// \return the stride
    auto vertex_data_stride() const noexcept -> bytes;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    /// \brief Constructs a mesh from the packed_buffer of data, the count of
    ///        vertices, and a vector of the indices
    ///
    /// \param buffer the packed buffer
    /// \param vertices the number of vertices in the packed buffer
    /// \param indices the indices
    mesh(packed_buffer buffer,
         quantity<struct vertex_data> vertices,
         vector<index_type> indices) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    packed_buffer m_buffer;
    quantity<struct vertex_data> m_vertices;
    vector<index_type> m_indices;
  };

  //===========================================================================
  // class : mesh_builder
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A builder system for meshes
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API mesh_builder
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using index_type = mesh::index_type;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a mesh-builder
    mesh_builder() noexcept;

    /// \brief Constructs a mesh-builder from the given allocator \p alloc
    ///
    /// \param alloc the allocator
    explicit mesh_builder(allocator alloc) noexcept;

    // Deleted copy and move constructor
    mesh_builder(const mesh_builder&) = delete;
    mesh_builder(mesh_builder&&) = delete;

    //-------------------------------------------------------------------------

    // Deleted copy and move assignment
    auto operator=(const mesh_builder&) -> mesh_builder& = delete;
    auto operator=(mesh_builder&&) -> mesh_builder& = delete;

    //-------------------------------------------------------------------------
    // Capacity
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the number of vertices from the mesh being built
    ///
    /// \return the number of vertices
    auto vertices() const noexcept -> quantity<vertex_data>;

    /// \brief Gets the number of indices from the mesh being built
    ///
    /// \return the number of indices
    auto indices() const noexcept -> quantity<index_type>;

    /// \brief Reserves space for the specified \p vertices and \p indices
    ///
    /// \param vertices the number of vertices to reserve
    /// \param indices the number of indices to reserve
    auto reserve(quantity<vertex_data> vertices,
                 quantity<index_type> indices) noexcept -> void;

    //-------------------------------------------------------------------------
    // Building
    //-------------------------------------------------------------------------
  public:

    /// \brief Adds vertex data to this mesh builder
    ///
    /// \param v the vertex location
    /// \param n the normal at this vertex
    /// \param tc the texture coordinate
    /// \param c the color
    /// \return the index of the newly added vertex data
    auto add_vertex(const vertex& v,
                    const normal& n,
                    const texture_coordinate& tc,
                    const color& c = color_constants::white)
      noexcept -> index_type;

    /// \brief Adds vertex data to this mesh builder
    ///
    /// \param vertex the vertex data
    /// \return the index of the newly added vertex data
    auto add_vertex(const vertex_data& vertex) noexcept -> index_type;

    /// \brief Adds a collection of vertex data to this mesh builder
    ///
    /// For systems where the vertices/data are all known up-front in an
    /// unpacked form, this operation will be faster than repeatedly calling
    /// `add_vertex`. Not only is an index not returned each time, the complete
    /// vertex count is also not needed.
    ///
    /// \note
    /// The supplied range *must* be usable with multiple passes, otherwise this
    /// function will experience undefined behavior.
    ///
    /// \param first the start of the range
    /// \param last the end of the range
    template <typename ForwardIt, typename Sentinel>
    auto add_vertices(ForwardIt first, Sentinel last) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Adds a face from three vertex indices
    ///
    /// \param i0 the index of the first vertex
    /// \param i1 the index of the second vertex
    /// \param i2 the index of the third vertex
    auto add_face(index_type i0, index_type i1, index_type i2)
      noexcept -> void;

    /// \brief Adds a face from four vertex indices
    ///
    /// \param i0 the index of the first vertex
    /// \param i1 the index of the second vertex
    /// \param i2 the index of the third vertex
    /// \param i3 the index of the fourth vertex
    auto add_face(index_type i0, index_type i1, index_type i2, index_type i3)
      noexcept -> void;

    /// \brief Adds a collection of face data to this mesh builder
    ///
    /// \param first the start of the range
    /// \param last the end of the range
    template <typename InputIt, typename Sentinel>
    auto add_faces(InputIt first, Sentinel last) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Builds the mesh by moving the underlying packed buffer
    ///
    /// \return the mesh to build
    [[nodiscard]]
    auto build() && noexcept -> mesh;

    /// \brief Builds the mesh by copying the underlying buffer
    ///
    /// \return the mesh to build
    [[nodiscard]]
    auto build() const & noexcept -> mesh;

    /// \brief Builds the mesh by copying the underlying buffer using \p alloc
    ///
    /// \param alloc the allocator to copy
    /// \return the mesh to build
    [[nodiscard]]
    auto build(allocator alloc) const & noexcept -> mesh;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    packed_buffer m_buffer;
    packed_buffer_writer m_writer;
    quantity<vertex_data> m_vertices;
    vector<index_type> m_indices;

    //-------------------------------------------------------------------------
    // Private Building
    //-------------------------------------------------------------------------
  private:

    /// \brief Packs the vertex data \p data into the mesh
    ///
    /// \param data the data to pack
    auto pack_vertex(const vertex_data& data) noexcept -> void;
  };

} // namespace alloy::core

//==============================================================================
// class : mesh_builder
//==============================================================================

//------------------------------------------------------------------------------
// Building
//------------------------------------------------------------------------------

template <typename ForwardIt, typename Sentinel>
inline
auto alloy::core::mesh_builder::add_vertices(ForwardIt first, Sentinel last)
  noexcept -> void
{
  for (auto it = first; it != last; ++it) {
    pack_vertex(*it);
  }
  m_vertices += std::distance(first, last);
}

//------------------------------------------------------------------------------

template <typename InputIt, typename Sentinel>
inline
auto alloy::core::mesh_builder::add_faces(InputIt first, Sentinel last)
  noexcept -> void
{
  for (auto it = first; it != last; ++it) {
    add_face(*it);
  }
}


#endif /* ALLOY_CORE_MODEL_MESH_HPP */
