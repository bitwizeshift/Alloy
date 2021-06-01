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

#include "alloy/render/mesh/3d/polygon_3d_builder.hpp"

#include "../attribute_packer.hpp"

#include <utility> // std::move

namespace {

inline constexpr auto entries_per_position  = 3u;
inline constexpr auto entries_per_color     = 4u;
inline constexpr auto entries_per_uv        = 2u;
inline constexpr auto entries_per_normal    = 3u;
inline constexpr auto entries_per_tangent   = 3u;
inline constexpr auto entries_per_bitangent = 3u;
inline constexpr auto entries_per_indices   = 3u;

} // namespace <anonymous>

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

alloy::render::polygon_3d_builder::polygon_3d_builder(primitive_topology topology,
                                                core::allocator alloc)
  noexcept
  : m_positions{core::stl_allocator_adapter<core::point3>{alloc}},
    m_colors{core::stl_allocator_adapter<core::color>{alloc}},
    m_uvs{core::stl_allocator_adapter<core::point2>{alloc}},
    m_normals{core::stl_allocator_adapter<core::vector3>{alloc}},
    m_tangents{core::stl_allocator_adapter<core::vector3>{alloc}},
    m_bitangents{core::stl_allocator_adapter<core::vector3>{alloc}},
    m_indices{core::stl_allocator_adapter<index_type>{alloc}},
    m_lower_left{0,0,0},
    m_upper_right{0,0,0},
    m_topology{topology}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

auto alloy::render::polygon_3d_builder::reserve(std::size_t n) -> void
{
  reserve_positions(n);
  reserve_colors(n);
  reserve_uvs(n);
  reserve_normals(n);
  reserve_tangents(n);
  reserve_bitangents(n);
}

auto alloy::render::polygon_3d_builder::reserve_positions(std::size_t n)
  -> void
{
  m_positions.reserve(n);
}

auto alloy::render::polygon_3d_builder::reserve_colors(std::size_t n)
  -> void
{
  m_colors.reserve(n);
}

auto alloy::render::polygon_3d_builder::reserve_uvs(std::size_t n)
  -> void
{
  m_uvs.reserve(n);
}

auto alloy::render::polygon_3d_builder::reserve_normals(std::size_t n)
  -> void
{
  m_normals.reserve(n);
}

auto alloy::render::polygon_3d_builder::reserve_tangents(std::size_t n)
  -> void
{
  m_tangents.reserve(n);
}

auto alloy::render::polygon_3d_builder::reserve_bitangents(std::size_t n)
  -> void
{
  m_bitangents.reserve(n);
}

auto alloy::render::polygon_3d_builder::reserve_indices(std::size_t n)
  -> void
{
  m_indices.reserve(n);
}

//------------------------------------------------------------------------------
// Setters
//------------------------------------------------------------------------------

auto alloy::render::polygon_3d_builder::set_positions(core::vector<core::point3> positions)
  -> void
{
  m_positions = std::move(positions);
}

auto alloy::render::polygon_3d_builder::set_colors(core::vector<core::color> colors)
  -> void
{
  m_colors = std::move(colors);
}

auto alloy::render::polygon_3d_builder::set_uvs(core::vector<core::point2> uvs)
  -> void
{
  m_uvs = std::move(uvs);
}

auto alloy::render::polygon_3d_builder::set_normals(core::vector<core::vector3> normals)
  -> void
{
  m_normals = std::move(normals);
}

auto alloy::render::polygon_3d_builder::set_tangents(core::vector<core::vector3> tangents)
  -> void
{
  m_tangents = std::move(tangents);
}

auto alloy::render::polygon_3d_builder::set_bitangents(core::vector<core::vector3> bitangents)
  -> void
{
  m_bitangents = std::move(bitangents);
}

auto alloy::render::polygon_3d_builder::set_indices(core::vector<uint16_t> indices)
  -> void
{
  m_indices = std::move(indices);
}

//------------------------------------------------------------------------------
// Incremental Building
//------------------------------------------------------------------------------

auto alloy::render::polygon_3d_builder::add_position(const core::point3& p)
  -> void
{
  m_positions.push_back(p);

  if (core::piecewise_compare<core::point3>{}(m_lower_left, p)) {
    m_lower_left = p;
  } else if (core::piecewise_compare<core::point3>{}(p, m_upper_right)) {
    m_upper_right = p;
  }
}

auto alloy::render::polygon_3d_builder::add_color(const core::color& c)
  -> void
{
  m_colors.push_back(c);
}

auto alloy::render::polygon_3d_builder::add_uv(const core::point2& p)
  -> void
{
  m_uvs.push_back(p);
}

auto alloy::render::polygon_3d_builder::add_normal(const core::vector3& v) -> void
{
  m_normals.push_back(v);
}

auto alloy::render::polygon_3d_builder::add_tangent(const core::vector3& v) -> void
{
  m_tangents.push_back(v);
}

auto alloy::render::polygon_3d_builder::add_bitangent(const core::vector3& v) -> void
{
  m_bitangents.push_back(v);
}

//------------------------------------------------------------------------------

auto
alloy::render::polygon_3d_builder::add_index(index_type v0)
  -> void
{
  m_indices.push_back(v0);
}

auto alloy::render::polygon_3d_builder::add_face(index_type v0,
                                              index_type v1,
                                              index_type v2)
  -> void
{
  m_indices.push_back(v0);
  m_indices.push_back(v1);
  m_indices.push_back(v2);
}

auto alloy::render::polygon_3d_builder::add_face(index_type v0,
                                              index_type v1,
                                              index_type v2,
                                              index_type v3)
  -> void
{
  add_face(v0, v1, v2);
  add_face(v2, v3, v0);
}

//------------------------------------------------------------------------------
// Building
//------------------------------------------------------------------------------

auto alloy::render::polygon_3d_builder::build_with(core::allocator alloc)
    const -> mesh
{
  auto config = mesh_config{};
  config.topology             = m_topology;
  config.normal_components    = m_normals.empty() ? 0u : ::entries_per_normal;
  config.color_components     = m_colors.empty() ? 0u : ::entries_per_color;
  config.uv_components        = m_uvs.empty() ? 0u : ::entries_per_uv;
  config.tangent_components   = m_tangents.empty() ? 0u : ::entries_per_tangent;
  config.bitangent_components = m_normals.empty() ? 0u : ::entries_per_bitangent;
  config.indices              = m_indices.copy_with(core::stl_allocator_adapter<index_type>{alloc});
  config.vertex_data          = core::packed_buffer{alloc};
  config.bounding_volume      = core::axis_aligned_box::from_points(m_lower_left, m_upper_right);
  construct_data(config.vertex_data);

  return mesh::from_config_unchecked(std::move(config));
}

auto alloy::render::polygon_3d_builder::construct_data(core::packed_buffer& buffer)
  const -> void
{
  // Compute size. This also accounts for padding on the normals, since 16-bit
  // integers will leave unaligned vertex buffers.
  const auto buffer_size = (
    ::entries_per_position * m_positions.size() * core::size_of<float>() +
    ::entries_per_color * m_colors.size() * core::size_of<std::byte>() +
    ::entries_per_uv * m_uvs.size() * core::size_of<float>() +
    (::entries_per_normal + 1u) * m_normals.size() * core::size_of<std::int16_t>() +
    (::entries_per_tangent + 1u) * m_tangents.size() * core::size_of<std::int16_t>() +
    (::entries_per_bitangent + 1u) * m_bitangents.size() * core::size_of<std::int16_t>()
  );
  buffer.reserve(buffer_size);
  auto writer = core::packed_buffer_writer{&buffer};

  // Pack each entry by [position][uv][normal][tangent][bitangent]
  const auto positions = m_positions.size();
  for (auto i = 0u; i < positions; ++i) {

    writer.pack_object(m_positions[i], point_packer{});

    if (!m_colors.empty()) {
      ALLOY_ASSERT(i < m_colors.size());

      writer.pack_object(m_colors[i], color_packer{});
    }

    if (!m_uvs.empty()) {
      ALLOY_ASSERT(i < m_uvs.size());

      writer.pack_object(m_uvs[i], point_packer{});
    }

    if (!m_normals.empty()) {
      ALLOY_ASSERT(i < m_normals.size());

      writer.pack_object(m_normals[i], vector_packer{});
    }

    if (!m_tangents.empty()) {
      ALLOY_ASSERT(i < m_tangents.size());

      writer.pack_object(m_tangents[i], vector_packer{});
    }

    if (!m_bitangents.empty()) {
      ALLOY_ASSERT(i < m_bitangents.size());

      writer.pack_object(m_bitangents[i], vector_packer{});
    }
  }

  // This should hold provided that the input data was correct.
  ALLOY_ASSERT(buffer.size() == buffer_size);
}
