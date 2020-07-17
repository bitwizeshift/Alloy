#include "alloy/core/math/vector/vector3.hpp"

//=============================================================================
// class : vector3
//=============================================================================

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

alloy::core::real
  alloy::core::vector3::magnitude()
  const noexcept
{
  return sqrt((x()*x()) + (y()*y()) + (z()*z()));
}

alloy::core::vector3
  alloy::core::vector3::perpendicular()
  const noexcept
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

alloy::core::radian
  alloy::core::vector3::angle_between(const vector3& other)
  const noexcept
{
  auto mag_product = magnitude() * other.magnitude();

  if (almost_equal(mag_product, real{0})){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp(f, real{-1.0}, real{1.0});
  return trigonometry::arccos(f);
}

alloy::core::radian
  alloy::core::vector3::angle_to(const vector3& other)
  const noexcept
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

alloy::core::vector3&
  alloy::core::vector3::normalize()
  noexcept
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

bool alloy::core::are_linearly_independent(const vector3& v1,
                                           const vector3& v2,
                                           const vector3& v3)
  noexcept
{
  // linear-independence tested by checking if the determinant of a produced
  // 3x3 matrix is non-zero
  const vector3 matrix[3] = {v1,v2,v3};

  const auto determinant = (matrix[0][0] * matrix[1][1] * matrix[2][2]) -
                           (matrix[0][0] * matrix[1][2] * matrix[2][1]) -
                           (matrix[0][1] * matrix[1][0] * matrix[2][2]) +
                           (matrix[0][1] * matrix[1][2] * matrix[2][0]) +
                           (matrix[0][2] * matrix[1][0] * matrix[2][1]) -
                           (matrix[0][2] * matrix[1][1] * matrix[2][0]);

  return !almost_equal(real{0}, determinant);
}
