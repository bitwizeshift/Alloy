// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/capsule.hpp"

#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/precision/real.hpp"

//==============================================================================
// class : capsule
//==============================================================================

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::capsule::nearest_point(const point3& p)
  const noexcept -> point3
{
  const auto center = m_segment.nearest_point(p);
  const auto offset = (p - center);

  if (offset.square_magnitude() <= (m_radius * m_radius)) {
    return p;
  }
  return center + (offset.normalized() * m_radius);
}

auto alloy::core::capsule::contains(const point3& p) const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::capsule::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto offset = (p - m_segment.nearest_point(p));
  const auto adjusted_radius = m_radius + tolerance;

  return offset.square_magnitude() <= (adjusted_radius * adjusted_radius);
}
