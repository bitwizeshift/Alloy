#include "alloy/core/math/vector/vector2.hpp"

//=============================================================================
// class : vector2
//=============================================================================

//-----------------------------------------------------------------------------
// Public Static Factories
//-----------------------------------------------------------------------------

alloy::core::vector2
  alloy::core::vector2::from_magnitude_direction(real magnitude,
                                                 radian direction)
  noexcept
{
  return vector2 {
    magnitude * trigonometry::cos(direction),
    magnitude * trigonometry::sin(direction)
  };
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

alloy::core::real
  alloy::core::vector2::magnitude()
  const noexcept
{
  return sqrt((x()*x()) + (y()*y()));
}

alloy::core::radian
  alloy::core::vector2::angle_between(const vector2& other)
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
  alloy::core::vector2::angle_to(const vector2& other)
  const noexcept
{
  auto angle = angle_between(other);

  if (cross(other) < real{0}) {
    return radian_constants::revolution - angle;
  }

  return angle;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

alloy::core::vector2&
  alloy::core::vector2::normalize()
  noexcept
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, real{1})) {
    return (*this);
  }

  if (square_mag > real{0}) {
    const auto mag_inv = real{1} / sqrt(square_mag);

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

bool alloy::core::are_linearly_independent(const vector2& v1, const vector2& v2)
  noexcept
{
  // linear-independence tested by checking if the determinant of a produced
  // 2x2 matrix is non-zero

  const auto determinant = v1.x() * v2.y() - v1.y() * v2.x();
  return !almost_equal(real{0}, determinant);
}
