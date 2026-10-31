// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/ray.hpp"

#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/math.hpp"

//==============================================================================
// class : ray
//==============================================================================

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

auto alloy::core::ray::from_points(const point3& origin, const point3& p)
  noexcept -> ray
{
  const auto direction = (p - origin).normalized();

  return {origin, direction};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::ray::point_at_distance(float dt) const noexcept -> point3
{
  return m_origin + (m_direction * dt);
}

auto alloy::core::ray::contains(const point3& p) const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::ray::contains(const point3& p, float tolerance)
  const noexcept -> bool
{
  const auto offset = (p - m_origin);

  // 'm_direction' is a unit vector, so the dot product is the distance along
  // the ray, and the magnitude of the cross product is the distance from it.
  const auto distance_along = offset.dot(m_direction);
  if (distance_along < -tolerance) {
    return false;
  }
  const auto distance_from = offset.cross(m_direction);

  return distance_from.square_magnitude() <= (tolerance * tolerance);
}
