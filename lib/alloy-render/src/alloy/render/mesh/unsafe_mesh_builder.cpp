// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/mesh/unsafe_mesh_builder.hpp"

#include "alloy/render/mesh/index_format.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "attribute_packer.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/model/color.hpp"

#include <array>   // std::array
#include <cstddef> // std::size_t, std::byte
#include <cstdint> // std::uint8_t, std::uint16_t, std::uint32_t
#include <cstring> // std::memcpy
#include <span>    // std::span
#include <utility> // std::move

namespace alloy::render {
namespace {

inline constexpr auto indices_per_face = 3u;

/// The largest number of vertices that 16-bit indices can address
inline constexpr auto max_u16_vertices = std::size_t{1u} << 16u;

auto index_format_for(std::size_t vertices) noexcept -> index_format
{
  return (vertices <= max_u16_vertices) ? index_format::u16 : index_format::u32;
}

auto read_index(const core::packed_buffer& indices, std::size_t i)
  noexcept -> std::uint32_t
{
  auto result = std::uint32_t{};
  std::memcpy(&result, indices.data() + (i * sizeof(result)), sizeof(result));
  return result;
}

auto count_indices(const core::packed_buffer& indices) noexcept -> std::size_t
{
  return static_cast<std::size_t>(
    indices.size().count() / sizeof(std::uint32_t)
  );
}

/// \brief Narrows the 32-bit \p indices into a buffer of 16-bit indices
///        allocated from \p alloc
auto narrow_indices(const core::packed_buffer& indices, core::allocator alloc)
  -> core::packed_buffer
{
  const auto count = count_indices(indices);

  auto result = core::packed_buffer{alloc};
  result.reserve(core::bytes{count * sizeof(std::uint16_t)});
  auto writer = core::packed_buffer_writer{&result};
  for (auto i = std::size_t{0}; i < count; ++i) {
    writer.pack_object(static_cast<std::uint16_t>(read_index(indices, i)));
  }
  return result;
}

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
  : m_allocator{alloc},
    m_buffer{alloc},
    m_writer{&m_buffer},
    m_index_data{alloc},
    m_index_writer{&m_index_data},
    m_submesh_starts{core::stl_allocator_adapter<submesh_start>{alloc}},
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
  m_index_data.reserve(
    core::bytes{n * indices_per_face * sizeof(index_type)}
  );
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

auto alloy::render::unsafe_mesh_builder::add_uv2(float p) -> void
{
  check_attribute(attribute::uv2, 1u);
  m_writer.pack_object(p);
  complete_attribute(attribute::uv2);
}

auto alloy::render::unsafe_mesh_builder::add_uv2(const core::point2& p) -> void
{
  check_attribute(attribute::uv2, 2u);
  m_writer.pack_object(p, point_packer{m_components.uv2});
  complete_attribute(attribute::uv2);
}

auto alloy::render::unsafe_mesh_builder::add_uv2(const core::point3& p) -> void
{
  check_attribute(attribute::uv2, 3u);
  m_writer.pack_object(p, point_packer{m_components.uv2});
  complete_attribute(attribute::uv2);
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

auto alloy::render::unsafe_mesh_builder::add_weights(const core::vector4& v)
  -> void
{
  check_attribute(attribute::weights, 4u);
  m_writer.pack_object(v, weight_packer{m_components.weights});
  complete_attribute(attribute::weights);
}

auto alloy::render::unsafe_mesh_builder::add_color(const core::color& c)
  -> void
{
  check_attribute(attribute::color, 4u);
  m_writer.pack_object(c, color_packer{m_components.color});
  complete_attribute(attribute::color);
}

auto alloy::render::unsafe_mesh_builder::add_joints(
  const std::array<std::uint8_t,4>& joints
) -> void
{
  check_attribute(attribute::joints, 4u);
  m_writer.pack_object(joints, joint_packer{m_components.joints});
  complete_attribute(attribute::joints);
}

auto alloy::render::unsafe_mesh_builder::add_index(index_type i) -> void
{
  m_index_writer.pack_object(i);
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

auto alloy::render::unsafe_mesh_builder::begin_submesh() -> void
{
  m_submesh_starts.push_back(submesh_start{
    .index  = static_cast<std::uint32_t>(count_indices(m_index_data)),
    .vertex = static_cast<std::uint32_t>(vertex_count()),
  });
}

//------------------------------------------------------------------------------
// Building
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::release() && -> mesh
{
  check_complete();

  const auto format = index_format_for(vertex_count());
  auto submeshes = make_submeshes(m_allocator);

  auto config = mesh_config{};
  config.vertex_data = std::move(m_buffer);
  config.index_data = (format == index_format::u16)
    ? narrow_indices(m_index_data, m_allocator)
    : std::move(m_index_data);
  config.index_format = format;
  config.topology = m_topology;
  config.components = m_components;
  config.submeshes = std::move(submeshes);
  config.bounding_volume = m_bound;

  return mesh::from_config_unchecked(std::move(config));
}

auto alloy::render::unsafe_mesh_builder::build_with(core::allocator alloc)
  const -> mesh
{
  check_complete();

  const auto format = index_format_for(vertex_count());

  auto config = mesh_config{};
  config.vertex_data = m_buffer.copy_with(alloc);
  config.index_data = (format == index_format::u16)
    ? narrow_indices(m_index_data, alloc)
    : m_index_data.copy_with(alloc);
  config.index_format = format;
  config.topology = m_topology;
  config.components = m_components;
  config.submeshes = make_submeshes(alloc);
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
    case attribute::uv2: {
      return m_components.uv2;
    }
    case attribute::normal: {
      return m_components.normal;
    }
    case attribute::tangent: {
      return m_components.tangent;
    }
    case attribute::weights: {
      return m_components.weights;
    }
    case attribute::color: {
      return m_components.color;
    }
    case attribute::joints: {
      return m_components.joints;
    }
  }
  core::compiler::unreachable();
}

auto alloy::render::unsafe_mesh_builder::next_attribute_after(attribute a)
  const noexcept -> attribute
{
  static constexpr auto attributes = static_cast<std::uint8_t>(
    static_cast<std::uint8_t>(attribute::joints) + 1u
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
  ALLOY_ASSERT(
    stride == 0u || (bytes % stride) == 0u,
    "a vertex is missing attributes"
  );

  const auto vertices = vertex_count();
  const auto indices = count_indices(m_index_data);
  for (auto i = std::size_t{0}; i < indices; ++i) {
    ALLOY_ASSERT(
      read_index(m_index_data, i) < vertices,
      "an index refers to a vertex that does not exist"
    );
  }
#endif
}

auto alloy::render::unsafe_mesh_builder::vertex_count()
  const noexcept -> std::size_t
{
  const auto stride = m_components.stride().count();
  if (stride == 0u) {
    return 0u;
  }
  return static_cast<std::size_t>(m_buffer.size().count() / stride);
}

auto alloy::render::unsafe_mesh_builder::make_submeshes(core::allocator alloc)
  const -> core::vector<submesh>
{
  auto result = core::vector<submesh>{
    core::stl_allocator_adapter<submesh>{alloc}
  };
  if (m_submesh_starts.empty()) {
    return result;
  }

  const auto indexed = count_indices(m_index_data) != 0u;
  const auto first_of = [indexed](const submesh_start& start) {
    return indexed ? start.index : start.vertex;
  };
  const auto end = static_cast<std::uint32_t>(
    indexed ? count_indices(m_index_data) : vertex_count()
  );

  result.reserve(m_submesh_starts.size() + 1u);
  const auto initial = first_of(m_submesh_starts.front());
  if (initial != 0u) {
    result.push_back(submesh{.first = 0u, .count = initial});
  }
  for (auto i = std::size_t{0}; i < m_submesh_starts.size(); ++i) {
    const auto first = first_of(m_submesh_starts[i]);
    const auto last = (i + 1u < m_submesh_starts.size())
      ? first_of(m_submesh_starts[i + 1u])
      : end;
    result.push_back(submesh{.first = first, .count = last - first});
  }
  return result;
}

//------------------------------------------------------------------------------
// Private Modifiers
//------------------------------------------------------------------------------

auto alloy::render::unsafe_mesh_builder::last_attribute_of(
  const vertex_components& components
) noexcept -> attribute
{
  if (components.joints != 0u) {
    return attribute::joints;
  }
  if (components.color != 0u) {
    return attribute::color;
  }
  if (components.weights != 0u) {
    return attribute::weights;
  }
  if (components.tangent != 0u) {
    return attribute::tangent;
  }
  if (components.normal != 0u) {
    return attribute::normal;
  }
  if (components.uv2 != 0u) {
    return attribute::uv2;
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
