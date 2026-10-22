// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/circle.hpp"

#include "alloy/core/geometry/point/point2.hpp"

#include "alloy/core/math/math.hpp"
#include "alloy/core/precision/real.hpp"

//==============================================================================
// class : circle
//==============================================================================

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::circle::nearest_point(const point2& p)
  const noexcept -> point2
{
  const auto offset = (p - m_center);

  if (offset.square_magnitude() <= (m_radius * m_radius)) {
    return p;
  }
  return m_center + (offset.normalized() * m_radius);
}

auto alloy::core::circle::contains(const point2& p) const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::circle::contains(const point2& p, real tolerance)
  const noexcept -> bool
{
  const auto offset = (p - m_center);
  const auto adjusted_radius = m_radius + tolerance;

  return offset.square_magnitude() <= (adjusted_radius * adjusted_radius);
}
