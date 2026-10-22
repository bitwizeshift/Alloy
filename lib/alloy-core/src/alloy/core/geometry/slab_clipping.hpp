// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file slab_clipping.hpp
 *
 * \brief This private header clips lines to the slabs of a box, for the
 *        intersection and raycast tests of line-like objects against boxes
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_SLAB_CLIPPING_HPP
#define ALLOY_CORE_GEOMETRY_SLAB_CLIPPING_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/geometry/oriented_box.hpp"
#include "alloy/core/geometry/point/point3.hpp"

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/precision/real.hpp"

#include <algorithm> // std::min, std::max, std::swap
#include <optional>  // std::optional

namespace alloy::core::detail {

  /// \brief The range of distances along a line-like object that lie in a
  ///        shape
  struct interval
  {
    real near;
    real far;
  };

  /// \brief Clips \p range of the line `origin + t * direction` to the part
  ///        that lies within the box [\p box_min, \p box_max]
  ///
  /// \param origin the origin of the line
  /// \param direction the direction of the line
  /// \param range the range of `t` to clip
  /// \param box_min the corner of the box with the least coordinates
  /// \param box_max the corner of the box with the greatest coordinates
  /// \return the clipped range, or `std::nullopt` if none of it is in the box
  auto clip_to_slabs(
    const point3& origin,
    const vector3& direction,
    interval range,
    const point3& box_min,
    const point3& box_max
  ) noexcept -> std::optional<interval>;

  /// \brief Clips \p range of the line `origin + t * direction` to the part
  ///        that lies within the box \p b, by clipping it in the space of the
  ///        box
  ///
  /// \param origin the origin of the line
  /// \param direction the direction of the line
  /// \param range the range of `t` to clip
  /// \param b the box
  /// \return the clipped range, or `std::nullopt` if none of it is in the box
  auto clip_to_box(
    const point3& origin,
    const vector3& direction,
    interval range,
    const oriented_box& b
  ) noexcept -> std::optional<interval>;

} // namespace alloy::core::detail

//==============================================================================
// definitions : functions
//==============================================================================

inline
auto alloy::core::detail::clip_to_slabs(
  const point3& origin,
  const vector3& direction,
  interval range,
  const point3& box_min,
  const point3& box_max
) noexcept -> std::optional<interval>
{
  for (auto i = 0; i < 3; ++i) {
    if (abs(direction[i]) <= default_tolerance) {
      // Parallel to this slab: either always inside of it, or never.
      if (origin[i] < box_min[i] || origin[i] > box_max[i]) {
        return std::nullopt;
      }
      continue;
    }
    const auto reciprocal = real{1} / direction[i];
    auto t0 = (box_min[i] - origin[i]) * reciprocal;
    auto t1 = (box_max[i] - origin[i]) * reciprocal;
    if (t0 > t1) {
      std::swap(t0, t1);
    }
    range.near = std::max(range.near, t0);
    range.far = std::min(range.far, t1);
    if (range.near > range.far) {
      return std::nullopt;
    }
  }
  return range;
}

inline
auto alloy::core::detail::clip_to_box(
  const point3& origin,
  const vector3& direction,
  interval range,
  const oriented_box& b
) noexcept -> std::optional<interval>
{
  const auto axes = b.axes();
  const auto offset = (origin - b.center());
  const auto local_origin = point3{
    offset.dot(axes[0]),
    offset.dot(axes[1]),
    offset.dot(axes[2])
  };
  const auto local_direction = vector3{
    direction.dot(axes[0]),
    direction.dot(axes[1]),
    direction.dot(axes[2])
  };
  const auto& extents = b.half_extents();

  return clip_to_slabs(
    local_origin,
    local_direction,
    range,
    point3{-extents.x(), -extents.y(), -extents.z()},
    point3{extents.x(), extents.y(), extents.z()}
  );
}

#endif /* ALLOY_CORE_GEOMETRY_SLAB_CLIPPING_HPP */
