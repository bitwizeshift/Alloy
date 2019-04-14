#include "alloy/core/geometry/plane.hpp"

#include "alloy/core/math/matrix/matrix3.hpp"

//==============================================================================
// class : plane
//==============================================================================

//------------------------------------------------------------------------------
// Static Member Functions
//------------------------------------------------------------------------------

alloy::core::plane alloy::core::plane::from_points_clockwise( const point& p0,
                                                              const point& p1,
                                                              const point& p2 )
  noexcept
{
  const auto normal = (p2 - p0).cross(p1 - p0).normalize();

  return from_point_and_normal(p0, normal);
}

alloy::core::plane
  alloy::core::plane::from_points_counter_clockwise( const point& p0,
                                                     const point& p1,
                                                     const point& p2 )
  noexcept
{
  const auto normal = (p1 - p0).cross(p2 - p0).normalize();

  return from_point_and_normal(p0, normal);
}


alloy::core::plane
  alloy::core::plane::from_point_and_normal( const point& p,
                                             const vector3& normal )
  noexcept
{
  const auto d = -normal.dot(to_vector(p));

  return {normal.x(), normal.y(), normal.z(), d};
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

alloy::core::plane& alloy::core::plane::normalize()
  noexcept
{
  const auto normal_magnitude = sqrt( m_data[0] * m_data[0] +
                                      m_data[1] * m_data[1] +
                                      m_data[2] * m_data[2] );

  if (normal_magnitude > 0) {
    const auto reciprocal = real{1} / normal_magnitude;
    for (auto i = 0; i < 4; ++i) {
      m_data[i] *= reciprocal;
    }
  }

  return (*this);
}

alloy::core::plane& alloy::core::plane::invert()
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] = -m_data[i];
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

alloy::core::real alloy::core::plane::distance_to( const point& p )
  const noexcept
{
  auto result = real{0};
  for (auto i = 0; i < 3; ++i) {
    result += m_data[i] * p[i];
  }
  result += d();

  return result;
}

alloy::core::vector3 alloy::core::plane::projection( const vector3& vec )
  const noexcept
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

bool alloy::core::plane::contains( const point& p )
  const noexcept
{
  return contains(p, default_tolerance);
}

bool alloy::core::plane::contains( const point& p, real tolerance )
  const noexcept
{
  const auto dt = normal().dot(to_vector(p)) - d();

  return almost_equal(dt, real{0}, tolerance);
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

bool alloy::core::is_point_over_plane( const point& p, const plane& plane )
  noexcept
{
  return plane.normal().dot(to_vector(p)) > plane.d();
}

bool alloy::core::is_point_under_plane( const point& p, const plane& plane )
  noexcept
{
  return plane.normal().dot(to_vector(p)) < plane.d();
}