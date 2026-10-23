// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/mesh/2d/polygon_2d_builder.hpp"

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/render/mesh/unsafe_mesh_builder.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/precision/real.hpp"

#include <cstddef> // std::size_t
#include <cstdint> // std::uint8_t
#include <utility> // std::move

namespace {

inline constexpr auto entries_per_position = std::uint8_t{2};
inline constexpr auto entries_per_color = std::uint8_t{4};
inline constexpr auto entries_per_uv = std::uint8_t{2};
inline constexpr auto entries_per_normal = std::uint8_t{3};
inline constexpr auto entries_per_indices = 3u;

auto to_point3(const alloy::core::point2& p) noexcept -> alloy::core::point3
{
  return alloy::core::point3{p.x(), p.y(), alloy::core::real{0}};
}

template <typename T>
auto components_of(const alloy::core::vector<T>& attribute, std::uint8_t n)
  noexcept -> std::uint8_t
{
  return attribute.empty() ? std::uint8_t{0} : n;
}

} // namespace <anonymous>

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

alloy::render::polygon_2d_builder::polygon_2d_builder(
  primitive_topology topology,
  core::allocator alloc
) noexcept
  : m_positions{core::stl_allocator_adapter<core::point2>{alloc}},
    m_colors{core::stl_allocator_adapter<core::color>{alloc}},
    m_uvs{core::stl_allocator_adapter<core::point2>{alloc}},
    m_normals{core::stl_allocator_adapter<core::vector3>{alloc}},
    m_indices{core::stl_allocator_adapter<index_type>{alloc}},
    m_topology{topology}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

auto alloy::render::polygon_2d_builder::reserve(std::size_t n) -> void
{
  reserve_positions(n);
  reserve_colors(n);
  reserve_uvs(n);
  reserve_normals(n);
}

//------------------------------------------------------------------------------

auto alloy::render::polygon_2d_builder::reserve_positions(std::size_t n) -> void
{
  m_positions.reserve(n);
}

auto alloy::render::polygon_2d_builder::reserve_colors(std::size_t n) -> void
{
  m_colors.reserve(n);
}

auto alloy::render::polygon_2d_builder::reserve_uvs(std::size_t n) -> void
{
  m_uvs.reserve(n);
}

auto alloy::render::polygon_2d_builder::reserve_normals(std::size_t n) -> void
{
  m_normals.reserve(n);
}

auto alloy::render::polygon_2d_builder::reserve_indices(std::size_t n) -> void
{
  m_indices.reserve(n * ::entries_per_indices);
}

//------------------------------------------------------------------------------
// Setters
//------------------------------------------------------------------------------

auto alloy::render::polygon_2d_builder::set_positions(
  core::vector<core::point2> positions
) -> void
{
  m_positions = std::move(positions);
}

auto alloy::render::polygon_2d_builder::set_colors(
  core::vector<core::color> colors
) -> void
{
  m_colors = std::move(colors);
}

auto alloy::render::polygon_2d_builder::set_uvs(core::vector<core::point2> uvs)
  -> void
{
  m_uvs = std::move(uvs);
}

auto alloy::render::polygon_2d_builder::set_normals(
  core::vector<core::vector3> normals
) -> void
{
  m_normals = std::move(normals);
}

auto alloy::render::polygon_2d_builder::set_indices(
  core::vector<std::uint16_t> indices
) -> void
{
  m_indices = std::move(indices);
}

//------------------------------------------------------------------------------
// Incremental Building
//------------------------------------------------------------------------------

auto alloy::render::polygon_2d_builder::add_position(
  const alloy::core::point2& p
) -> void
{
  m_positions.push_back(p);
}

auto alloy::render::polygon_2d_builder::add_color(const core::color& c) -> void
{
  m_colors.push_back(c);
}

auto alloy::render::polygon_2d_builder::add_uv(const core::point2& p) -> void
{
  m_uvs.push_back(p);
}

auto alloy::render::polygon_2d_builder::add_normal(const core::vector3& v)
  -> void
{
  m_normals.push_back(v);
}

//------------------------------------------------------------------------------

auto alloy::render::polygon_2d_builder::add_index(index_type v0) -> void
{
  m_indices.push_back(v0);
}

auto alloy::render::polygon_2d_builder::add_face(
  index_type v0,
  index_type v1,
  index_type v2
) -> void
{
  m_indices.push_back(v0);
  m_indices.push_back(v1);
  m_indices.push_back(v2);
}

auto alloy::render::polygon_2d_builder::add_face(
  index_type v0,
  index_type v1,
  index_type v2,
  index_type v3
) -> void
{
  add_face(v0, v1, v2);
  add_face(v2, v3, v0);
}

//------------------------------------------------------------------------------
// Building
//------------------------------------------------------------------------------

auto alloy::render::polygon_2d_builder::build_with(core::allocator alloc)
  const -> mesh
{
  const auto components = vertex_components{
    .position  = ::entries_per_position,
    .uv        = components_of(m_uvs, ::entries_per_uv),
    .normal    = components_of(m_normals, ::entries_per_normal),
    .color     = components_of(m_colors, ::entries_per_color),
  };
  const auto positions = m_positions.size();
  ALLOY_ASSERT(m_uvs.empty() || m_uvs.size() == positions);
  ALLOY_ASSERT(m_normals.empty() || m_normals.size() == positions);
  ALLOY_ASSERT(m_colors.empty() || m_colors.size() == positions);

  auto builder = unsafe_mesh_builder{components, m_topology, alloc};
  builder.reserve_vertices(positions);
  builder.reserve_indices(
    (m_indices.size() + ::entries_per_indices - 1u) / ::entries_per_indices
  );

  auto bounds = m_positions.empty()
    ? core::axis_aligned_box{}
    : core::axis_aligned_box::from_points(
        to_point3(m_positions[0]),
        to_point3(m_positions[0])
      );
  for (auto i = 0u; i < positions; ++i) {
    const auto position = to_point3(m_positions[i]);
    bounds = core::axis_aligned_box::enclosing(bounds, position);

    builder.add_position(m_positions[i]);
    if (!m_uvs.empty()) {
      builder.add_uv(m_uvs[i]);
    }
    if (!m_normals.empty()) {
      builder.add_normal(m_normals[i]);
    }
    if (!m_colors.empty()) {
      builder.add_color(m_colors[i]);
    }
  }
  for (const auto index : m_indices) {
    builder.add_index(index);
  }
  builder.set_bounding_box(bounds);

  return std::move(builder).release();
}
