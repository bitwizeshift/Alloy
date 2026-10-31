// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/rectangle.hpp"

#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector2.hpp"

#include <algorithm> // std::min, std::max

//==============================================================================
// class : rectangle
//==============================================================================

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

auto alloy::core::rectangle::from_point_and_size(
  const point2& p,
  const vector2& size
) noexcept -> rectangle
{
  return from_points(p, p + size);
}

auto alloy::core::rectangle::from_points(const point2& p0, const point2& p1)
  noexcept -> rectangle
{
  const auto min_point = point2{
    std::min(p0.x(), p1.x()),
    std::min(p0.y(), p1.y())
  };
  const auto max_point = point2{
    std::max(p0.x(), p1.x()),
    std::max(p0.y(), p1.y())
  };
  return rectangle{min_point, max_point};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::rectangle::distance_to(const point2& p)
  const noexcept -> float
{
  return (p - nearest_point(p)).magnitude();
}

auto alloy::core::rectangle::square_distance_to(const point2& p)
  const noexcept -> float
{
  return (p - nearest_point(p)).square_magnitude();
}

auto alloy::core::rectangle::nearest_point(const point2& p)
  const noexcept -> point2
{
  return point2{
    clamp(p.x(), m_bottom_left.x(), m_top_right.x()),
    clamp(p.y(), m_bottom_left.y(), m_top_right.y())
  };
}

auto alloy::core::rectangle::contains(const point2& p) const noexcept -> bool
{
  return (p.x() >= m_bottom_left.x()) && (p.x() <= m_top_right.x()) &&
         (p.y() >= m_bottom_left.y()) && (p.y() <= m_top_right.y());
}

auto alloy::core::rectangle::contains(const point2& p, float tolerance)
  const noexcept -> bool
{
  return (p.x() >= (m_bottom_left.x() - tolerance)) &&
         (p.x() <= (m_top_right.x() + tolerance)) &&
         (p.y() >= (m_bottom_left.y() - tolerance)) &&
         (p.y() <= (m_top_right.y() + tolerance));
}

auto alloy::core::rectangle::intersects(const rectangle& other)
  const noexcept -> bool
{
  return (m_bottom_left.x() <= other.m_top_right.x()) &&
         (m_top_right.x() >= other.m_bottom_left.x()) &&
         (m_bottom_left.y() <= other.m_top_right.y()) &&
         (m_top_right.y() >= other.m_bottom_left.y());
}

auto alloy::core::rectangle::encloses(const rectangle& other)
  const noexcept -> bool
{
  return (m_bottom_left.x() <= other.m_bottom_left.x()) &&
         (m_bottom_left.y() <= other.m_bottom_left.y()) &&
         (m_top_right.x() >= other.m_top_right.x()) &&
         (m_top_right.y() >= other.m_top_right.y());
}
