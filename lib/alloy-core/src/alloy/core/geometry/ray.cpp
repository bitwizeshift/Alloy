#include "alloy/core/geometry/ray.hpp"

//==============================================================================
// class : ray
//==============================================================================

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

alloy::core::ray alloy::core::ray::from_points( const point& origin,
                                                const point& p )
  noexcept
{
  const auto direction = (p - origin).normalized();

  return {origin, direction};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

alloy::core::point
  alloy::core::ray::point_at_distance( real dt )
  const noexcept
{
  return m_origin + (m_direction * dt);
}
