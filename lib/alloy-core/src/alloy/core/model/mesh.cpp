#include "alloy/core/model/mesh.hpp"
#include "alloy/core/utilities/casts.hpp"

#include <cstdint>
#include <limits>
#include <utility> // std::move

namespace alloy::core {
  namespace {

    //=========================================================================
    // Free functions
    //=========================================================================

    /// \brief Converts a 'real' value that has been normalized between [-1,1]
    ///        to a value between -2^15 and 2^15
    ///
    /// \param x the real value to normalize
    /// \return the normalized value as an integer
    auto compress(real x) -> std::int16_t
    {
      static constexpr auto max = std::numeric_limits<std::int16_t>::max();

      return static_cast<std::int16_t>(x * max);
    }

#if 0 // TODO(bitwizeshift): Re-enable this when a view of the mesh is added
    auto uncompress(std::int16_t x) -> real
    {
      static constexpr auto max = static_cast<real>(std::numeric_limits<std::int16_t>::max());

      return static_cast<real>(x) / max;
    }
#endif

    //=========================================================================
    // struct : vertex_data_packer
    //=========================================================================

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compresses vertex data into a consistent strided stream by
    ///        storing it all into a packed buffer
    ///////////////////////////////////////////////////////////////////////////
    struct vertex_data_packer
    {
      auto pack_vertex(packed_buffer_writer& writer, const vertex& v) const noexcept -> void
      {
        writer.pack_object(v.x());
        writer.pack_object(v.y());
        writer.pack_object(v.z());
      }

      auto pack_normal(packed_buffer_writer& writer, const normal& n) const noexcept -> void
      {
        static constexpr auto padding = std::int16_t{0};
        writer.pack_object(compress(n.x()));
        writer.pack_object(compress(n.y()));
        writer.pack_object(compress(n.z()));
        writer.pack_object(padding);
      }

      auto pack_texture_coordinate(packed_buffer_writer& writer, const texture_coordinate& tc) const noexcept -> void
      {
        writer.pack_object(tc.u());
        writer.pack_object(tc.v());
      }

      auto pack_color(packed_buffer_writer& writer, const color& c) const noexcept -> void
      {
        auto rgba = static_cast<std::uint32_t>(c.to_rgba32());
        const auto r = static_cast<std::uint8_t>((rgba & 0xff000000) >> 24);
        const auto g = static_cast<std::uint8_t>((rgba & 0x00ff0000) >> 16);
        const auto b = static_cast<std::uint8_t>((rgba & 0x0000ff00) >> 8);
        const auto a = static_cast<std::uint8_t>((rgba & 0x000000ff) >> 0);

        writer.pack_object(r);
        writer.pack_object(g);
        writer.pack_object(b);
        writer.pack_object(a);
      }

      auto pack(packed_buffer_writer& writer, const vertex_data& data) const noexcept -> void
      {
        pack_vertex(writer, data.vertex);
        pack_normal(writer, data.normal);
        pack_texture_coordinate(writer, data.texture_coordinate);
        pack_color(writer, data.color);
      }


#if 0 // TODO(bitwizeshift): Re-enable this when a view of the mesh is added
      auto unpack_vertex(packed_buffer_reader& reader) const noexcept -> result<vertex,std::error_code>
      {
        const auto components = reader.unpack_object<std::array<real,3>>();
        if (!components) {
          return fail(components.error());
        }

        const auto& x = (*components)[0];
        const auto& y = (*components)[1];
        const auto& z = (*components)[2];

        return vertex{x,y,z};
      }

      auto unpack_normal(packed_buffer_reader& reader) const noexcept -> result<normal,std::error_code>
      {
        const auto components = reader.unpack_object<std::array<std::int16_t,4>>();
        if (!components) {
          return fail(components.error());
        }

        const auto& x = (*components)[0];
        const auto& y = (*components)[1];
        const auto& z = (*components)[2];
        const auto& padding = (*components)[3];
        compiler::unused(padding);

        return normal::make_unchecked(
          uncompress(x),
          uncompress(y),
          uncompress(z)
        );
      }

      auto unpack_texture_coordinate(packed_buffer_reader& reader) const noexcept -> result<texture_coordinate,std::error_code>
      {
        const auto components = reader.unpack_object<std::array<real,2>>();
        if (!components) {
          return fail(components.error());
        }

        const auto& u = (*components)[0];
        const auto& v = (*components)[1];

        return texture_coordinate{u, v};
      }

      auto unpack_color(packed_buffer_reader& reader) const noexcept -> result<color,std::error_code>
      {
        const auto components = reader.unpack_object<std::array<std::uint8_t,4>>();
        if (!components) {
          return fail(components.error());
        }
        const auto& r = (*components)[0];
        const auto& g = (*components)[1];
        const auto& b = (*components)[2];
        const auto& a = (*components)[3];

        const auto result = static_cast<rgba32>(
          (r << 24) |
          (g << 16) |
          (b << 8)  |
          (a << 0)
        );

        return color::from_rgba32(result);
      }

      auto unpack(packed_buffer_reader& reader) const noexcept -> result<vertex_data,std::error_code>
      {
        // Rebuild everything piece-by-piece
        const auto vertex = unpack_vertex(reader);
        if (!vertex) {
          return fail(vertex.error());
        }

        const auto normal = unpack_normal(reader);
        if (!normal) {
          return fail(normal.error());
        }

        const auto texture_coordinate = unpack_texture_coordinate(reader);
        if (!texture_coordinate) {
          return fail(texture_coordinate.error());
        }

        const auto color = unpack_color(reader);
        if (!color) {
          return fail(color.error());
        }

        return vertex_data{*vertex, *normal, *texture_coordinate, *color};
      }
#endif
    };

  } // namespace <anonymous>
} // namespace alloy::core

//=============================================================================
// class : mesh
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

auto alloy::core::mesh::make_from_buffers(packed_buffer buffer,
                                          quantity<struct vertex_data> vertices,
                                          vector<index_type> indices)
  noexcept -> mesh
{
  return mesh{std::move(buffer), vertices, std::move(indices)};
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::core::mesh::mesh()
  noexcept
  : m_buffer{},
    m_vertices{0},
    m_indices{}
{

}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

auto alloy::core::mesh::vertices()
  const noexcept -> quantity<struct vertex_data>
{
  return m_vertices;
}

auto alloy::core::mesh::vertex_data()
  const noexcept -> span<const std::byte>
{
  return span<const std::byte>{
    m_buffer.data(),
    narrow_cast<std::size_t>(m_buffer.size().count())
  };
}

auto alloy::core::mesh::index_data()
  const noexcept -> span<const index_type>
{
  return span<const index_type>{
    m_indices.data(),
    m_indices.size()
  };
}

auto alloy::core::mesh::vertex_offset()
  const noexcept -> bytes
{
  return bytes{0};
}

auto alloy::core::mesh::normal_offset()
  const noexcept -> bytes
{
  return bytes{sizeof(real)*3};
}

auto alloy::core::mesh::texture_coordinate_offset()
  const noexcept -> bytes
{
  return bytes{
    sizeof(real) * 3 +       // vertices
    sizeof(std::int16_t) * 4 // normals
  };
}

auto alloy::core::mesh::color_offset()
  const noexcept -> bytes
{
  return bytes{
    sizeof(real) * 3 +         // vertices
    sizeof(std::int16_t) * 4 + // normals
    sizeof(real) * 2           // uv
  };
}

auto alloy::core::mesh::vertex_data_stride()
  const noexcept -> bytes
{
  return bytes{
    sizeof(real) * 3 +         // vertices
    sizeof(std::int16_t) * 4 + // normals
    sizeof(real) * 2 +         // uv
    sizeof(std::uint8_t) * 4   // color
  };
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

alloy::core::mesh::mesh(packed_buffer buffer,
                        quantity<struct vertex_data> vertices,
                        vector<index_type> indices)
  noexcept
  : m_buffer{std::move(buffer)},
    m_vertices{vertices},
    m_indices{std::move(indices)}
{

}

//=============================================================================
// class : mesh_builder
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::core::mesh_builder::mesh_builder()
  noexcept
  : mesh_builder{allocator{}}
{

}

alloy::core::mesh_builder::mesh_builder(allocator alloc)
  noexcept
  : m_buffer{alloc},
    m_writer{&m_buffer},
    m_vertices{0}
{

}

//-----------------------------------------------------------------------------
// Capacity
//-----------------------------------------------------------------------------

auto alloy::core::mesh_builder::vertices()
  const noexcept -> quantity<vertex_data>
{
  return m_vertices;
}

auto alloy::core::mesh_builder::indices()
  const noexcept -> quantity<index_type>
{
  return quantity<index_type>{as_signed(m_indices.size())};
}

auto alloy::core::mesh_builder::reserve(quantity<vertex_data> vertices,
                                        quantity<index_type> indices)
  noexcept -> void
{
  static constexpr auto bytes_per_vertex_data = bytes{
    sizeof(real) * 3 +
    sizeof(std::uint16_t) * 4 +
    sizeof(real) * 2 +
    sizeof(char) * 4
  };
  const auto required_bytes = (
    bytes_per_vertex_data * checked_narrow_cast<std::size_t>(vertices.count())
  );

  m_buffer.reserve(required_bytes);
  m_indices.reserve(checked_narrow_cast<std::size_t>(indices.count()));
}

//-----------------------------------------------------------------------------
// Building
//-----------------------------------------------------------------------------

auto alloy::core::mesh_builder::add_vertex(const vertex& v,
                                           const normal& n,
                                           const texture_coordinate& tc,
                                           const color& c)
  noexcept -> index_type
{
  const auto data = vertex_data{
    v, n, tc, c
  };

  return add_vertex(data);
}

auto alloy::core::mesh_builder::add_vertex(const vertex_data& data)
  noexcept -> index_type
{
  const auto index = checked_narrow_cast<std::uint16_t>(m_vertices.count());

  pack_vertex(data);

  ++m_vertices;
  return index;
}


//-----------------------------------------------------------------------------

auto alloy::core::mesh_builder::add_face(index_type i0,
                                         index_type i1,
                                         index_type i2)
  noexcept -> void
{
  m_indices.push_back(i0);
  m_indices.push_back(i1);
  m_indices.push_back(i2);
}

auto alloy::core::mesh_builder::add_face(index_type i0,
                                         index_type i1,
                                         index_type i2,
                                         index_type i3)
  noexcept -> void
{
  add_face(i0, i1, i2);
  add_face(i2, i3, i0);
}

//-----------------------------------------------------------------------------

auto alloy::core::mesh_builder::build()
  && noexcept -> mesh
{
  return mesh::make_from_buffers(
    std::move(m_buffer),
    m_vertices,
    std::move(m_indices)
  );
}

auto alloy::core::mesh_builder::build()
  const & noexcept -> mesh
{
  return mesh::make_from_buffers(
    m_buffer.copy(),
    m_vertices,
    m_indices.copy()
  );
}

auto alloy::core::mesh_builder::build(allocator alloc)
  const & noexcept -> mesh
{
  return mesh::make_from_buffers(
    m_buffer.copy_with(alloc),
    m_vertices,
    m_indices.copy_with(stl_allocator_adapter<index_type>{alloc})
  );
}

//------------------------------------------------------------------------------
// Private Functions
//------------------------------------------------------------------------------

auto alloy::core::mesh_builder::pack_vertex(const vertex_data& data)
  noexcept -> void
{
  m_writer.pack_object(data, vertex_data_packer{});
}
