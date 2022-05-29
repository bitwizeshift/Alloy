#include "alloy/core/geometry/ray.hpp"

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

auto alloy::core::ray::point_at_distance(real dt)
  const noexcept -> point3
{
  return m_origin + (m_direction * dt);
}

auto alloy::core::ray::contains(const point3& p)
  const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::ray::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto dx = (p.x() - m_origin.x()) / m_direction.x();
  const auto dy = (p.y() - m_origin.y()) / m_direction.y();
  const auto dz = (p.z() - m_origin.z()) / m_direction.z();

  return (
    dx > 0 &&
    almost_equal(dx, dy, tolerance) &&
    almost_equal(dy, dz, tolerance) &&
    almost_equal(dz, dx, tolerance)
  );
}
