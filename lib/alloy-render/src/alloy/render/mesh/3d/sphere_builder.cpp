// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/mesh/3d/sphere_builder.hpp"

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/render/mesh/unsafe_mesh_builder.hpp"
#include "alloy/render/mesh/winding_order.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/math_constants.hpp"
#include "alloy/core/math/trigonometry.hpp"
#include "alloy/core/memory/allocator.hpp"

#include <cstddef> // std::size_t
#include <limits>  // std::numeric_limits
#include <utility> // std::move

alloy::render::sphere_builder::sphere_builder(
  std::size_t x_segments,
  std::size_t y_segments
) noexcept
  : m_x_segments{x_segments},
    m_y_segments{y_segments}
{

}

auto alloy::render::sphere_builder::build_with(core::allocator alloc)
  const -> mesh
{
  using index_type = unsafe_mesh_builder::index_type;

  ALLOY_ASSERT(m_x_segments >= 3u);
  ALLOY_ASSERT(m_y_segments >= 2u);

  const auto columns = m_x_segments + 1u;
  const auto rows = m_y_segments + 1u;
  ALLOY_ASSERT(
    rows * columns <=
    static_cast<std::size_t>(std::numeric_limits<index_type>::max()) + 1u
  );

  const auto components = vertex_components{
    .position = 3u,
    .uv       = 2u,
    .normal   = 3u,
  };
  auto builder = unsafe_mesh_builder{
    components,
    primitive_topology::triangles,
    winding_order::counter_clockwise,
    alloc
  };

  // The first and last rows each lose one triangle per segment, since one
  // edge of every quad touching a pole collapses to a single point.
  const auto triangles = 2u * m_x_segments * (m_y_segments - 1u);
  builder.reserve_vertices(rows * columns);
  builder.reserve_indices(triangles);

  for (auto y = 0u; y < rows; ++y) {
    for (auto x = 0u; x < columns; ++x) {

      const auto x_segment = core::radian{
        static_cast<float>(x) / static_cast<float>(m_x_segments)
      };
      const auto y_segment = core::radian{
        static_cast<float>(y) / static_cast<float>(m_y_segments)
      };

      const auto x_pos = (
        core::trigonometry::cos(x_segment * core::math_constants::tau) *
        core::trigonometry::sin(y_segment * core::math_constants::pi)
      );
      const auto y_pos = (
        core::trigonometry::cos(y_segment * core::math_constants::pi)
      );
      const auto z_pos = (
        core::trigonometry::sin(x_segment * core::math_constants::tau) *
        core::trigonometry::sin(y_segment * core::math_constants::pi)
      );

      builder.add_position({x_pos, y_pos, z_pos});
      builder.add_uv({x_segment.value(), y_segment.value()});
      builder.add_normal({x_pos, y_pos, z_pos});
    }
  }

  const auto index_of = [columns](std::size_t row, std::size_t column) {
    return static_cast<index_type>((row * columns) + column);
  };
  const auto last_row = m_y_segments - 1u;
  for (auto y = 0u; y < m_y_segments; ++y) {
    for (auto x = 0u; x < m_x_segments; ++x) {
      const auto i0 = index_of(y + 1u, x);
      const auto i1 = index_of(y, x);
      const auto i2 = index_of(y, x + 1u);
      const auto i3 = index_of(y + 1u, x + 1u);

      // i1 and i2 coincide at the north pole; i0 and i3 at the south pole.
      if (y != 0u) {
        builder.add_face(i0, i1, i2);
      }
      if (y != last_row) {
        builder.add_face(i2, i3, i0);
      }
    }
  }

  builder.set_bounding_box(
    core::axis_aligned_box::from_points({-1, -1, -1}, {1, 1, 1})
  );

  return std::move(builder).release();
}
