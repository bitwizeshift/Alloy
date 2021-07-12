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

#include "unsafe_mesh_builder.hpp"

#include "attribute_packer.hpp"

namespace alloy::render {
namespace {

inline
auto round_up_power_two(std::size_t v)
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

} //namespace <anonymous>
} // namespace alloy::render

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

alloy::render::unsafe_mesh_builder::unsafe_mesh_builder(primitive_topology topology,
                                                        core::allocator alloc)
  noexcept
  : m_buffer{alloc},
    m_writer{&m_buffer},
    m_indices{core::stl_allocator_adapter<index_type>{alloc}},
    m_topology{topology},
    m_bound{},
    m_components_per_position{0u},
    m_components_per_color{0u},
    m_components_per_uv{0u},
    m_components_per_normal{0u},
    m_components_per_tangent{0u},
    m_components_per_bitangent{0u}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::set_components_per_position(std::uint8_t n)
  noexcept -> void
{
  m_components_per_position = n;
}

auto alloy::render::unsafe_mesh_builder::set_components_per_color(std::uint8_t n)
  noexcept -> void
{
  m_components_per_color = n;
}

auto alloy::render::unsafe_mesh_builder::set_components_per_uv(std::uint8_t n)
  noexcept -> void
{
  m_components_per_uv = n;
}

auto alloy::render::unsafe_mesh_builder::set_components_per_normal(std::uint8_t n)
  noexcept -> void
{
  m_components_per_normal = n;
}

auto alloy::render::unsafe_mesh_builder::set_components_per_tangent(std::uint8_t n)
  noexcept -> void
{
  m_components_per_tangent = n;
}

auto alloy::render::unsafe_mesh_builder::set_components_per_bitangent(std::uint8_t n)
  noexcept -> void
{
  m_components_per_bitangent = n;
}

//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::reserve_vertices(std::size_t n)
  -> void
{
  const auto buffer_size = (
    m_components_per_position * core::size_of<float>() +
    round_up_power_two(m_components_per_color) * core::size_of<std::byte>() +
    m_components_per_uv * core::size_of<float>() +
    round_up_power_two(m_components_per_normal) * core::size_of<std::int16_t>() +
    round_up_power_two(m_components_per_tangent) * core::size_of<std::int16_t>() +
    round_up_power_two(m_components_per_bitangent) * core::size_of<std::int16_t>()
  ) * n;

  reserve_bytes(buffer_size);
}

auto alloy::render::unsafe_mesh_builder::reserve_bytes(core::bytes b)
  -> void
{
  m_buffer.reserve(b);
}

auto alloy::render::unsafe_mesh_builder::reserve_indices(core::quantity<index_type> indices)
  -> void
{
  m_indices.reserve(indices.count());
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::set_bounding_box(const core::axis_aligned_box& box)
  -> void
{
  m_bound = box;
}

auto alloy::render::unsafe_mesh_builder::add_position(const core::point2& p)
  -> void
{
  m_writer.pack_object(p, point_packer{});
}

auto alloy::render::unsafe_mesh_builder::add_position(const core::point3& p)
  -> void
{
  m_writer.pack_object(p, point_packer{});
}


auto alloy::render::unsafe_mesh_builder::add_color(const alloy::core::color& c)
  -> void
{
  m_writer.pack_object(c, color_packer{});
}

auto alloy::render::unsafe_mesh_builder::add_uv(float p)
  -> void
{
  m_writer.pack_object(p);
}

auto alloy::render::unsafe_mesh_builder::add_uv(const core::point2& p)
  -> void
{
  m_writer.pack_object(p, point_packer{});
}

auto alloy::render::unsafe_mesh_builder::add_uv(const core::point3& p)
  -> void
{
  m_writer.pack_object(p, point_packer{});
}

auto alloy::render::unsafe_mesh_builder::add_normal(const core::vector3& v)
  -> void
{
  m_writer.pack_object(v, vector_packer{});
}

auto alloy::render::unsafe_mesh_builder::add_tangent(const core::vector3& v)
  -> void
{
  m_writer.pack_object(v, vector_packer{});
}

auto alloy::render::unsafe_mesh_builder::add_bitangent(const core::vector3& v)
  -> void
{
  m_writer.pack_object(v, vector_packer{});
}


auto alloy::render::unsafe_mesh_builder::add_index(index_type i)
  -> void
{
  m_indices.push_back(i);
}

auto alloy::render::unsafe_mesh_builder::add_face(index_type i0,
                                                  index_type i1,
                                                  index_type i2)
  -> void
{
  add_index(i0);
  add_index(i1);
  add_index(i2);
}

auto alloy::render::unsafe_mesh_builder::add_face(index_type i0,
                                                  index_type i1,
                                                  index_type i2,
                                                  index_type i3) -> void
{
  add_face(i0, i1, i2);
  add_face(i2, i3, i0);
}


//------------------------------------------------------------------------------
// Building
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::release()
  && -> mesh
{
  auto config = mesh_config{};
  config.vertex_data = std::move(m_buffer);
  config.indices = std::move(m_indices);
  config.topology = m_topology;
  config.position_components = m_components_per_position;
  config.color_components = m_components_per_color;
  config.uv_components = m_components_per_uv;
  config.normal_components = m_components_per_normal;
  config.tangent_components = m_components_per_tangent;
  config.bitangent_components = m_components_per_bitangent;
  config.bounding_volume = m_bound;

  return mesh::from_config_unchecked(std::move(config));
}

auto alloy::render::unsafe_mesh_builder::build_with(core::allocator alloc)
    const -> mesh
{
  auto config = mesh_config{};
  config.vertex_data = m_buffer.copy_with(alloc);
  config.indices = m_indices.copy_with(core::stl_allocator_adapter<index_type>{alloc});
  config.topology = m_topology;
  config.position_components = m_components_per_position;
  config.color_components = m_components_per_color;
  config.uv_components = m_components_per_uv;
  config.normal_components = m_components_per_normal;
  config.tangent_components = m_components_per_tangent;
  config.bitangent_components = m_components_per_bitangent;
  config.bounding_volume = m_bound;

  return mesh::from_config_unchecked(std::move(config));
}
