#include "alloy/core/geometry/ray.hpp"

//==============================================================================
// class : ray
//==============================================================================

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

alloy::core::ray alloy::core::ray::from_points( const point3& origin,
                                                const point3& p )
  noexcept
{
  const auto direction = (p - origin).normalized();

  return {origin, direction};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

alloy::core::point3
  alloy::core::ray::point_at_distance( real dt )
  const noexcept
{
  return m_origin + (m_direction * dt);
}

bool alloy::core::ray::contains( const point3& p )
  const noexcept
{
  return contains(p, default_tolerance);
}

bool alloy::core::ray::contains( const point3& p, real tolerance )
  const noexcept
{
  const auto dx = (p.x() - m_origin.x()) / m_direction.x();
  const auto dy = (p.y() - m_origin.y()) / m_direction.y();
  const auto dz = (p.z() - m_origin.z()) / m_direction.z();

  return dx > 0 &&
         almost_equal(dx, dy, tolerance) &&
         almost_equal(dy, dz, tolerance) &&
         almost_equal(dz, dx, tolerance);
}
