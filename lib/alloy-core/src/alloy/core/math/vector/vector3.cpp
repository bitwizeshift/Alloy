#include "alloy/core/math/vector/vector3.hpp"

#include <array> // std::array

//=============================================================================
// class : vector3
//=============================================================================

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector3::magnitude()
  const noexcept -> real
{
  return sqrt((x()*x()) + (y()*y()) + (z()*z()));
}

auto alloy::core::vector3::perpendicular()
  const noexcept -> vector3
{
  static constexpr auto unit_x = vector3{
    real{1}, real{0}, real{0},
  };
  static constexpr auto unit_y = vector3{
    real{0}, real{1}, real{0},
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

  if (almost_equal(mag_product, real{0})){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp(f, real{-1.0}, real{1.0});
  return trigonometry::arccos(f);
}

auto alloy::core::vector3::angle_to(const vector3& other)
  const noexcept -> radian
{
  const auto angle = angle_between(other);

  // if (cross(other) < real{0}) {
  //   return radian_constants::revolution - angle;
  // }

  return angle;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

auto alloy::core::vector3::normalize()
  noexcept -> vector3&
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, real{1})) {
    return (*this);
  }

  if (square_mag > real{0}){
    const auto mag_inv = real{1} / sqrt(square_mag);

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

auto alloy::core::are_linearly_independent(const vector3& v1,
                                           const vector3& v2,
                                           const vector3& v3)
  noexcept -> bool
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

  return !almost_equal(real{0}, determinant);
}
