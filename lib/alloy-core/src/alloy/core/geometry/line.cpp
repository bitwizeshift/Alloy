
#include "alloy/core/geometry/line.hpp"
#include "alloy/core/intrinsics.hpp"

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

alloy::core::point alloy::core::line::point_at_distance( real dt )
  const noexcept
{
  return m_origin + (m_direction * dt);
}

bool alloy::core::line::contains( const point& p )
  const noexcept
{
  return contains(p, default_tolerance);
}

bool alloy::core::line::contains( const point& p, real tolerance )
  const noexcept
{
  const auto dx = (p.x() - m_origin.x()) / m_direction.x();
  const auto dy = (p.y() - m_origin.y()) / m_direction.y();
  const auto dz = (p.z() - m_origin.z()) / m_direction.z();

  return almost_equal(dx, dy, tolerance) &&
         almost_equal(dy, dz, tolerance) &&
         almost_equal(dz, dx, tolerance);
}

//==============================================================================
// non-member functions : class : line
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

bool alloy::core::operator==( const line& lhs, const line& rhs )
  noexcept
{
  const auto& lhs_direction = lhs.direction();
  const auto& rhs_direction = rhs.direction();

  // direction should be normalized
  if (lhs_direction != rhs_direction) {
    return false;
  }

  const auto& lhs_origin = lhs.origin();
  const auto& rhs_origin = rhs.origin();

  const auto lhs_factor = lhs_origin.x() / lhs_direction.x();
  const auto rhs_factor = rhs_origin.x() / rhs_direction.x();

  const auto lhs_ty = lhs_direction.y() * lhs_factor;
  const auto lhs_tz = lhs_direction.z() * lhs_factor;
  const auto rhs_ty = rhs_direction.y() * rhs_factor;
  const auto rhs_tz = rhs_direction.z() * rhs_factor;

  return (lhs_origin.y() - lhs_ty) == (rhs_origin.y() - rhs_ty) &&
         (lhs_origin.z() - lhs_tz) == (rhs_origin.z() - rhs_tz);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

bool alloy::core::almost_equal( const line& lhs, const line& rhs )
  noexcept
{
  return almost_equal(lhs, rhs, default_tolerance);
}

bool alloy::core::almost_equal( const line& lhs,
                                const line& rhs,
                                real tolerance )
  noexcept
{
  const auto& lhs_direction = lhs.direction();
  const auto& rhs_direction = rhs.direction();

  // direction should be normalized
  if (almost_equal(lhs_direction, rhs_direction, tolerance)) {
    return false;
  }

  const auto& lhs_origin = lhs.origin();
  const auto& rhs_origin = rhs.origin();

  const auto lhs_factor = lhs_origin.x() / lhs_direction.x();
  const auto rhs_factor = rhs_origin.x() / rhs_direction.x();

  const auto lhs_ty = lhs_direction.y() * lhs_factor;
  const auto lhs_tz = lhs_direction.z() * lhs_factor;
  const auto rhs_ty = rhs_direction.y() * rhs_factor;
  const auto rhs_tz = rhs_direction.z() * rhs_factor;

  return almost_equal((lhs_origin.y() - lhs_ty),
                      (rhs_origin.y() - rhs_ty),
                      tolerance) &&
         almost_equal((lhs_origin.z() - lhs_tz),
                      (rhs_origin.z() - rhs_tz),
                      tolerance);
}