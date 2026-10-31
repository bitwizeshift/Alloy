// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/vector/vector3.hpp"

#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/trigonometry.hpp"

#include <array> // std::array

//=============================================================================
// class : vector3
//=============================================================================

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector3::magnitude() const noexcept -> float
{
  return sqrt((x()*x()) + (y()*y()) + (z()*z()));
}

auto alloy::core::vector3::perpendicular() const noexcept -> vector3
{
  static constexpr auto unit_x = vector3{
    1.0f, 0.0f, 0.0f,
  };
  static constexpr auto unit_y = vector3{
    0.0f, 1.0f, 0.0f,
  };
  // Try perpendicular to x-axis
  // if result is 0, try y-axis
  auto perp = cross(unit_x);

  if (almost_equal(perp.dot(perp), 0)) {
    perp = cross(unit_y);
  }

  return perp.normalize();
}

auto alloy::core::vector3::angle_between(const vector3& other)
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

auto alloy::core::vector3::angle_to(const vector3& other)
  const noexcept -> radian
{
  const auto angle = angle_between(other);

  // if (cross(other) < 0.0f) {
  //   return radian_constants::revolution - angle;
  // }

  return angle;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector3::normalize() noexcept -> vector3&
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, 1.0f)) {
    return (*this);
  }

  if (square_mag > 0.0f){
    const auto mag_inv = 1.0f / sqrt(square_mag);

    x() *= mag_inv;
    y() *= mag_inv;
    z() *= mag_inv;
  }

  return (*this);
}

//=============================================================================
// non-member functions : class : vector3
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

auto alloy::core::are_linearly_independent(
  const vector3& v1,
  const vector3& v2,
  const vector3& v3
) noexcept -> bool
{
  // linear-independence tested by checking if the determinant of a produced
  // 3x3 matrix is non-zero
  const auto matrix = std::array<vector3,3>{v1,v2,v3};

  const auto determinant = (matrix[0][0] * matrix[1][1] * matrix[2][2]) -
                           (matrix[0][0] * matrix[1][2] * matrix[2][1]) -
                           (matrix[0][1] * matrix[1][0] * matrix[2][2]) +
                           (matrix[0][1] * matrix[1][2] * matrix[2][0]) +
                           (matrix[0][2] * matrix[1][0] * matrix[2][1]) -
                           (matrix[0][2] * matrix[1][1] * matrix[2][0]);

  return !almost_equal(0.0f, determinant);
}
