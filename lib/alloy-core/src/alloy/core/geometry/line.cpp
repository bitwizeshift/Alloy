// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/geometry/line.hpp"

#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::line::point_at_distance(float dt) const noexcept -> point3
{
  return m_origin + (m_direction * dt);
}

auto alloy::core::line::contains(const point3& p) const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::line::contains(const point3& p, float tolerance)
  const noexcept -> bool
{
  // 'm_direction' is a unit vector, so the magnitude of the cross product is
  // the distance from 'p' to this line.
  const auto offset = (p - m_origin).cross(m_direction);

  return offset.square_magnitude() <= (tolerance * tolerance);
}

//==============================================================================
// non-member functions : class : line
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

auto alloy::core::operator==(const line& lhs, const line& rhs) noexcept -> bool
{
  const auto& direction = lhs.direction();
  const auto parallel = direction.cross(rhs.direction());
  const auto offset = (rhs.origin() - lhs.origin()).cross(direction);

  return parallel.square_magnitude() == 0.0f &&
         offset.square_magnitude() == 0.0f;
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

auto alloy::core::almost_equal(
  const line& lhs,
  const line& rhs,
  float tolerance
) noexcept -> bool
{
  const auto parallel = lhs.direction().cross(rhs.direction());

  return parallel.square_magnitude() <= (tolerance * tolerance) &&
         lhs.contains(rhs.origin(), tolerance);
}
