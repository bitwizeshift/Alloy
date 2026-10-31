// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/line_segment.hpp"

#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/math.hpp" // core::sqrt
#include "alloy/core/precision/real.hpp"

//==============================================================================
// class : line_segment
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::line_segment::magnitude() const noexcept -> real
{
  return sqrt(square_magnitude());
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::line_segment::point_at_percent(real percent)
  const noexcept -> point3
{
  percent = saturate(percent);

  return m_start + (percent * direction());
}

auto alloy::core::line_segment::nearest_point(const point3& p)
  const noexcept -> point3
{
  const auto dir = direction();
  const auto square_length = dir.dot(dir);

  // A degenerate segment is a single point, and has no direction to project
  // onto.
  if (square_length <= real{0}) {
    return m_start;
  }
  const auto percent = saturate((p - m_start).dot(dir) / square_length);

  return m_start + (percent * dir);
}

auto alloy::core::line_segment::contains(const point3& p) const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::line_segment::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto offset = (p - nearest_point(p));

  return offset.square_magnitude() <= (tolerance * tolerance);
}
