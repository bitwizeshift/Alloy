// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/mesh/unsafe_mesh_builder.hpp"

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "attribute_packer.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/model/color.hpp"

#include <algorithm> // std::ranges::none_of
#include <array>     // std::array
#include <cstddef>   // std::size_t, std::byte
#include <cstdint>   // std::uint8_t
#include <span>      // std::span
#include <utility>   // std::move

namespace alloy::render {
namespace {

inline constexpr auto indices_per_face = 3u;

} // namespace <anonymous>
} // namespace alloy::render

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

alloy::render::unsafe_mesh_builder::unsafe_mesh_builder(
  vertex_components components,
  primitive_topology topology,
  core::allocator alloc
) noexcept
  : m_buffer{alloc},
    m_writer{&m_buffer},
    m_indices{core::stl_allocator_adapter<index_type>{alloc}},
    m_topology{topology},
    m_bound{},
    m_components{components},
    m_last_attribute{last_attribute_of(components)},
    m_next_attribute{next_attribute_after(m_last_attribute)},
    m_vertex_padding{static_cast<std::uint8_t>(
      (components.stride() - components.unpadded_size()).count()
    )}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::reserve_vertices(std::size_t n) -> void
{
  reserve_bytes(m_components.stride() * n);
}

auto alloy::render::unsafe_mesh_builder::reserve_bytes(core::bytes b) -> void
{
  m_buffer.reserve(b);
}

auto alloy::render::unsafe_mesh_builder::reserve_indices(std::size_t n) -> void
{
  m_indices.reserve(n * indices_per_face);
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::set_bounding_box(
  const core::axis_aligned_box& box
) -> void
{
  m_bound = box;
}

auto alloy::render::unsafe_mesh_builder::add_position(const core::point2& p)
  -> void
{
  check_attribute(attribute::position, 2u);
  m_writer.pack_object(p, point_packer{m_components.position});
  complete_attribute(attribute::position);
}

auto alloy::render::unsafe_mesh_builder::add_position(const core::point3& p)
  -> void
{
  check_attribute(attribute::position, 3u);
  m_writer.pack_object(p, point_packer{m_components.position});
  complete_attribute(attribute::position);
}

auto alloy::render::unsafe_mesh_builder::add_uv(float p) -> void
{
  check_attribute(attribute::uv, 1u);
  m_writer.pack_object(p);
  complete_attribute(attribute::uv);
}

auto alloy::render::unsafe_mesh_builder::add_uv(const core::point2& p) -> void
{
  check_attribute(attribute::uv, 2u);
  m_writer.pack_object(p, point_packer{m_components.uv});
  complete_attribute(attribute::uv);
}

auto alloy::render::unsafe_mesh_builder::add_uv(const core::point3& p) -> void
{
  check_attribute(attribute::uv, 3u);
  m_writer.pack_object(p, point_packer{m_components.uv});
  complete_attribute(attribute::uv);
}

auto alloy::render::unsafe_mesh_builder::add_normal(const core::vector3& v)
  -> void
{
  check_attribute(attribute::normal, 3u);
  m_writer.pack_object(v, vector_packer{m_components.normal});
  complete_attribute(attribute::normal);
}

auto alloy::render::unsafe_mesh_builder::add_tangent(const core::vector3& v)
  -> void
{
  check_attribute(attribute::tangent, 3u);
  m_writer.pack_object(v, vector_packer{m_components.tangent});
  complete_attribute(attribute::tangent);
}

auto alloy::render::unsafe_mesh_builder::add_tangent(const core::vector4& v)
  -> void
{
  check_attribute(attribute::tangent, 4u);
  m_writer.pack_object(v, vector_packer{m_components.tangent});
  complete_attribute(attribute::tangent);
}

auto alloy::render::unsafe_mesh_builder::add_color(const core::color& c)
  -> void
{
  check_attribute(attribute::color, 4u);
  m_writer.pack_object(c, color_packer{m_components.color});
  complete_attribute(attribute::color);
}

auto alloy::render::unsafe_mesh_builder::add_index(index_type i) -> void
{
  m_indices.push_back(i);
}

auto alloy::render::unsafe_mesh_builder::add_face(
  index_type i0,
  index_type i1,
  index_type i2
) -> void
{
  add_index(i0);
  add_index(i1);
  add_index(i2);
}

auto alloy::render::unsafe_mesh_builder::add_face(
  index_type i0,
  index_type i1,
  index_type i2,
  index_type i3
) -> void
{
  add_face(i0, i1, i2);
  add_face(i2, i3, i0);
}

//------------------------------------------------------------------------------
// Building
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::release() && -> mesh
{
  check_complete();

  auto config = mesh_config{};
  config.vertex_data = std::move(m_buffer);
  config.indices = std::move(m_indices);
  config.topology = m_topology;
  config.components = m_components;
  config.bounding_volume = m_bound;

  return mesh::from_config_unchecked(std::move(config));
}

auto alloy::render::unsafe_mesh_builder::build_with(core::allocator alloc)
  const -> mesh
{
  check_complete();

  auto config = mesh_config{};
  config.vertex_data = m_buffer.copy_with(alloc);
  config.indices = m_indices.copy_with(
    core::stl_allocator_adapter<index_type>{alloc}
  );
  config.topology = m_topology;
  config.components = m_components;
  config.bounding_volume = m_bound;

  return mesh::from_config_unchecked(std::move(config));
}

//------------------------------------------------------------------------------
// Private Observers
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::component_count(attribute a)
  const noexcept -> std::uint8_t
{
  switch (a) {
    case attribute::position: {
      return m_components.position;
    }
    case attribute::uv: {
      return m_components.uv;
    }
    case attribute::normal: {
      return m_components.normal;
    }
    case attribute::tangent: {
      return m_components.tangent;
    }
    case attribute::color: {
      return m_components.color;
    }
  }
  core::compiler::unreachable();
}

auto alloy::render::unsafe_mesh_builder::next_attribute_after(attribute a)
  const noexcept -> attribute
{
  static constexpr auto attributes = static_cast<std::uint8_t>(
    static_cast<std::uint8_t>(attribute::color) + 1u
  );

  const auto index = static_cast<std::uint8_t>(a);
  for (auto i = 1u; i <= attributes; ++i) {
    const auto next = static_cast<attribute>((index + i) % attributes);
    if (component_count(next) != 0u) {
      return next;
    }
  }
  return a;
}

auto alloy::render::unsafe_mesh_builder::check_attribute(
  attribute a,
  std::size_t dimensions
) const noexcept -> void
{
  ALLOY_ASSERT(a == m_next_attribute, "attribute added out of vertex order");
  ALLOY_ASSERT(
    component_count(a) <= dimensions,
    "attribute has fewer dimensions than its configured components"
  );
  core::compiler::unused(a, dimensions);
}

auto alloy::render::unsafe_mesh_builder::check_complete() const noexcept -> void
{
#if !defined(NDEBUG)
  const auto stride = m_components.stride().count();
  const auto bytes = m_buffer.size().count();
  const auto vertices = (stride == 0u) ? 0u : (bytes / stride);
  const auto is_out_of_range = [vertices](index_type i) {
    return i >= vertices;
  };

  ALLOY_ASSERT(
    stride == 0u || (bytes % stride) == 0u,
    "a vertex is missing attributes"
  );
  ALLOY_ASSERT(
    std::ranges::none_of(m_indices, is_out_of_range),
    "an index refers to a vertex that does not exist"
  );
#endif
}

//------------------------------------------------------------------------------
// Private Modifiers
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::last_attribute_of(
  const vertex_components& components
) noexcept -> attribute
{
  if (components.color != 0u) {
    return attribute::color;
  }
  if (components.tangent != 0u) {
    return attribute::tangent;
  }
  if (components.normal != 0u) {
    return attribute::normal;
  }
  if (components.uv != 0u) {
    return attribute::uv;
  }
  return attribute::position;
}

auto alloy::render::unsafe_mesh_builder::complete_attribute(attribute a)
  -> void
{
  static constexpr auto zeros = std::array<std::byte, 3u>{};

  if (a == m_last_attribute) {
    m_writer.write_bytes(std::span{zeros}.first(m_vertex_padding));
  }
#if !defined(NDEBUG)
  m_next_attribute = next_attribute_after(a);
#endif
}
