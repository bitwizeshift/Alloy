#include "alloy/core/geometry/line_segment.hpp"
#include "alloy/core/math/math.hpp" // core::sqrt

//==============================================================================
// class : line_segment
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::line_segment::magnitude()
  const noexcept -> real
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

auto alloy::core::line_segment::contains(const point3& p)
  const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::line_segment::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto dir = (m_end - m_start);

  const auto dx = (p.x() - m_start.x()) / dir.x();
  const auto dy = (p.y() - m_start.y()) / dir.y();
  const auto dz = (p.z() - m_start.z()) / dir.z();

  // If the scale in the x, y, or z-direction is negative, the point must
  // preceed the starting point. If any scale is over 1, it must exceed the
  // end point.
  // For a point to be on the line, all 3 scales should be the same -- so we
  // only check 1 to avoid redundant checks.
  if (dx < real{0} || dx > real{1}) {
    return false;
  }

  // The same scale means it must be in the same direction vector

  // NOTE: we check all 3 almost-equality statements to account for relative
  //       loss. Otherwise dx and dz might be off by a factor of up to
  //       2*epsilon, which no longer satisfies the almost-equality.
  return almost_equal(dx, dy, tolerance) &&
         almost_equal(dy, dz, tolerance) &&
         almost_equal(dz, dx, tolerance);
}
