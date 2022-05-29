#include "alloy/core/geometry/sphere.hpp"

auto alloy::core::sphere::contains(const point3& p)
  const noexcept -> bool
{
  const auto adjusted = (p - m_center);

  // compare by square magnitude
  return adjusted.dot(adjusted) < (m_radius*m_radius);
}

auto alloy::core::sphere::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto adjusted = (p - m_center);
  const auto adjusted_radius = m_radius + tolerance;

  // compare by square magnitude
  return adjusted.dot(adjusted) < (adjusted_radius)*(adjusted_radius);
}
