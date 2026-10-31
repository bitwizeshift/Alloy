// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/vector/vector2.hpp"

#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/trigonometry.hpp"

//=============================================================================
// class : vector2
//=============================================================================

//-----------------------------------------------------------------------------
// Public Static Factories
//-----------------------------------------------------------------------------

auto alloy::core::vector2::from_magnitude_direction(
  float magnitude,
  radian direction
) noexcept -> vector2
{
  return vector2 {
    magnitude * trigonometry::cos(direction),
    magnitude * trigonometry::sin(direction)
  };
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector2::magnitude() const noexcept -> float
{
  return sqrt((x()*x()) + (y()*y()));
}

auto alloy::core::vector2::angle_between(const vector2 &other)
  const noexcept -> radian
{
  auto mag_product = magnitude() * other.magnitude();

  if (almost_equal(mag_product, 0.0f)){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp(f, -1.0f, 1.0f);
  return trigonometry::arccos(f);
}

auto alloy::core::vector2::angle_to(const vector2 &other)
  const noexcept -> radian
{
  auto angle = angle_between(other);

  if (cross(other) < 0.0f) {
    return radian_constants::revolution - angle;
  }

  return angle;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector2::normalize() noexcept -> vector2&
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, 1.0f)) {
    return (*this);
  }

  if (square_mag > 0.0f) {
    const auto mag_inv = 1.0f / sqrt(square_mag);

    x() *= mag_inv;
    y() *= mag_inv;
  }

  return (*this);
}

//=============================================================================
// non-member functions : class : vector2
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

auto alloy::core::are_linearly_independent(const vector2 &v1, const vector2 &v2)
  noexcept -> bool
{
  // linear-independence tested by checking if the determinant of a produced
  // 2x2 matrix is non-zero

  const auto determinant = v1.x() * v2.y() - v1.y() * v2.x();
  return !almost_equal(0.0f, determinant);
}
