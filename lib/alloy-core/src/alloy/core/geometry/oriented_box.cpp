// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/oriented_box.hpp"

#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/point/point3.hpp"

#include "alloy/core/math/basis/basis3.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector3_constants.hpp"
#include "alloy/core/precision/real.hpp"

#include <array> // std::array

namespace {

  constexpr auto world_orientation = alloy::core::basis3::make_unchecked(
    alloy::core::vector3_constants::unit_y,
    alloy::core::vector3_constants::unit_x,
    alloy::core::vector3_constants::unit_z
  );

} // namespace

//==============================================================================
// class : oriented_box
//==============================================================================

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

auto alloy::core::oriented_box::from_axis_aligned_box(
  const axis_aligned_box& box
) noexcept -> oriented_box
{
  const auto& min = box.bottom_left_point();
  const auto& max = box.top_right_point();
  const auto half_extents = (max - min) * real{0.5};

  return oriented_box{min + half_extents, half_extents, world_orientation};
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::oriented_box::corners()
  const noexcept -> std::array<point3, 8>
{
  const auto [x, y, z] = axes();
  const auto dx = x * m_half_extents.x();
  const auto dy = y * m_half_extents.y();
  const auto dz = z * m_half_extents.z();

  return {
    m_center - dx - dy - dz,
    m_center + dx - dy - dz,
    m_center - dx + dy - dz,
    m_center + dx + dy - dz,
    m_center - dx - dy + dz,
    m_center + dx - dy + dz,
    m_center - dx + dy + dz,
    m_center + dx + dy + dz
  };
}

auto alloy::core::oriented_box::bounding_box()
  const noexcept -> axis_aligned_box
{
  // The extent of the box along a world axis is the sum of the extents of
  // each of its own axes projected onto that world axis.
  const auto box_axes = axes();
  auto extent = vector3{};
  for (auto i = 0; i < 3; ++i) {
    for (auto j = 0; j < 3; ++j) {
      extent[i] += abs(box_axes[j][i]) * m_half_extents[j];
    }
  }

  return axis_aligned_box{m_center - extent, m_center + extent};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::oriented_box::nearest_point(const point3& p)
  const noexcept -> point3
{
  const auto offset = (p - m_center);
  const auto box_axes = axes();

  auto result = m_center;
  for (auto i = 0; i < 3; ++i) {
    const auto extent = m_half_extents[i];
    const auto distance = clamp(offset.dot(box_axes[i]), -extent, extent);
    result += box_axes[i] * distance;
  }
  return result;
}

auto alloy::core::oriented_box::contains(const point3& p) const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::oriented_box::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto offset = (p - m_center);
  const auto box_axes = axes();

  for (auto i = 0; i < 3; ++i) {
    const auto extent = m_half_extents[i];
    if (abs(offset.dot(box_axes[i])) > (extent + tolerance)) {
      return false;
    }
  }
  return true;
}
