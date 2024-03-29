#include "alloy/core/geometry/plane.hpp"

#include "alloy/core/math/matrix/matrix3.hpp"

//==============================================================================
// class : plane
//==============================================================================

//------------------------------------------------------------------------------
// Static Member Functions
//------------------------------------------------------------------------------

auto alloy::core::plane::from_points_clockwise(
  const point3& p0,
  const point3& p1,
  const point3& p2
) noexcept -> plane
{
  const auto normal = (p2 - p0).cross(p1 - p0).normalize();

  return from_point_and_normal(p0, normal);
}

auto alloy::core::plane::from_points_counter_clockwise(
  const point3& p0,
  const point3& p1,
  const point3& p2
) noexcept -> plane
{
  const auto normal = (p1 - p0).cross(p2 - p0).normalize();

  return from_point_and_normal(p0, normal);
}

auto alloy::core::plane::from_point_and_normal(
  const point3& p,
  const vector3& normal
) noexcept -> plane
{
  const auto d = -normal.dot(to_vector(p));

  return {normal.x(), normal.y(), normal.z(), d};
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::core::plane::normalize()
  noexcept -> plane&
{
  const auto normal_magnitude = sqrt( m_data[0] * m_data[0] +
                                      m_data[1] * m_data[1] +
                                      m_data[2] * m_data[2] );

  if (normal_magnitude > 0) {
    const auto reciprocal = real{1} / normal_magnitude;
    for (auto& i : m_data) {
      i *= reciprocal;
    }
  }

  return (*this);
}

auto alloy::core::plane::invert()
  noexcept -> plane&
{
  for (auto& i : m_data) {
    i = -i;
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

auto alloy::core::plane::distance_to(const point3& p)
  const noexcept -> real
{
  auto result = real{0};
  for (auto i = 0; i < 3; ++i) {
    result += m_data[i] * p[i];
  }
  result += d();

  return result;
}

auto alloy::core::plane::projection(const vector3& vec)
  const noexcept -> vector3
{
  const auto norm = normal();
  auto matrix = matrix3{};
  matrix.get(0,0) = real{1} - norm.x() * norm.x();
  matrix.get(0,1) = -norm.x() * norm.y();
  matrix.get(0,2) = -norm.x() * norm.z();
  matrix.get(1,0) = -norm.y() * norm.x();
  matrix.get(1,1) = real{1} - norm.y() * norm.y();
  matrix.get(1,2) = -norm.y() * norm.z();
  matrix.get(2,0) = -norm.z() * norm.x();
  matrix.get(2,1) = -norm.z() * norm.y();
  matrix.get(2,2) = real{1} - norm.z() * norm.z();

  return vec * matrix;
}

auto alloy::core::plane::contains(const point3& p)
  const noexcept -> bool
{
  return contains(p, default_tolerance);
}

auto alloy::core::plane::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  const auto dt = normal().dot(to_vector(p)) - d();

  return almost_equal(dt, real{0}, tolerance);
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

auto alloy::core::is_point_over_plane(const point3& p, const plane& plane)
  noexcept -> bool
{
  return plane.normal().dot(to_vector(p)) > plane.d();
}

auto alloy::core::is_point_under_plane(const point3& p, const plane& plane)
  noexcept -> bool
{
  return plane.normal().dot(to_vector(p)) < plane.d();
}
