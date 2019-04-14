#include "alloy/core/geometry/sphere.hpp"

bool alloy::core::sphere::contains( const point& p )
  const noexcept
{
  const auto adjusted = (p - m_center);

  // compare by square magnitude
  return adjusted.dot(adjusted) < (m_radius*m_radius);
}

bool alloy::core::sphere::contains( const point& p, real tolerance )
  const noexcept
{
  const auto adjusted = (p - m_center);
  const auto adjusted_radius = m_radius + tolerance;

  // compare by square magnitude
  return adjusted.dot(adjusted) < (adjusted_radius)*(adjusted_radius);
}
