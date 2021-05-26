#include "alloy/core/geometry/axis_aligned_box.hpp"

#include <algorithm> // std::min, std::max

//==============================================================================
// class : axis_aligned_box
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

alloy::core::axis_aligned_box
  alloy::core::axis_aligned_box::from_point_and_size( const point3& p,
                                                      const vector3& size )
  noexcept
{
  return from_points(p, p + size);
}

alloy::core::axis_aligned_box
  alloy::core::axis_aligned_box::from_points( const point3& p0,
                                              const point3& p1 )
  noexcept
{
  const auto min_point = point3{
    std::min(p0.x(),p1.x()),
    std::min(p0.y(),p1.y()),
    std::min(p0.z(),p1.z())
  };
  const auto max_point = point3{
    std::max(p0.x(),p1.x()),
    std::max(p0.y(),p1.y()),
    std::max(p0.z(),p1.z())
  };
  return axis_aligned_box{min_point, max_point};
}

//------------------------------------------------------------------------------
// Observers : Planes
//------------------------------------------------------------------------------

alloy::core::plane alloy::core::axis_aligned_box::top_plane()
  const noexcept
{
  const auto normal = vector3{0, 1, 0};
  const auto distance = m_top_right.y();

  return plane{normal, distance};
}

alloy::core::plane alloy::core::axis_aligned_box::bottom_plane()
  const noexcept
{
  const auto normal = vector3{0, -1, 0};
  const auto distance = m_bottom_left.y();

  return plane{normal, distance};
}

alloy::core::plane alloy::core::axis_aligned_box::front_plane()
  const noexcept
{
  const auto normal = vector3{0, 0, 1};
  const auto distance = m_top_right.z();

  return plane{normal, distance};
}

alloy::core::plane alloy::core::axis_aligned_box::back_plane()
  const noexcept
{
  const auto normal = vector3{0, 0, -1};
  const auto distance = m_bottom_left.z();

  return plane{normal, distance};
}

alloy::core::plane alloy::core::axis_aligned_box::left_plane()
  const noexcept
{
  const auto normal = vector3{-1, 0, 0};
  const auto distance = m_bottom_left.x();

  return plane{normal, distance};
}

alloy::core::plane alloy::core::axis_aligned_box::right_plane()
  const noexcept
{
  const auto normal = vector3{1, 0, 0};
  const auto distance = m_top_right.x();

  return plane{normal, distance};
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

std::array<alloy::core::vector3,6> alloy::core::axis_aligned_box::normals()
  const noexcept
{
  const auto p = planes();
  auto result = std::array<vector3,6>{};

  for (auto i = 0u; i < result.size(); ++i) {
    result[i] = p[i].normal();
  }

  return result;
}

std::array<alloy::core::plane,6> alloy::core::axis_aligned_box::planes()
  const noexcept
{
  return {
    top_plane(),
    bottom_plane(),
    front_plane(),
    back_plane(),
    left_plane(),
    right_plane()
  };
}

bool alloy::core::axis_aligned_box::contains( const point3& p )
  const noexcept
{
  return (p.x() <= m_top_right.x()) && (p.x() >= m_bottom_left.x()) &&
         (p.y() <= m_top_right.y()) && (p.y() >= m_bottom_left.y()) &&
         (p.z() <= m_top_right.z()) && (p.z() >= m_bottom_left.z());
}

bool alloy::core::axis_aligned_box::contains( const point3& p, real tolerance )
  const noexcept
{
  // Add a buffer of 'tolerance' for fuzzy matching
  return (p.x() <= (m_top_right.x() + tolerance)) &&
         (p.x() >= (m_bottom_left.x() - tolerance)) &&
         (p.y() <= (m_top_right.y() + tolerance)) &&
         (p.y() >= (m_bottom_left.y() - tolerance)) &&
         (p.z() <= (m_top_right.z() + tolerance)) &&
         (p.z() >= (m_bottom_left.z() - tolerance));
}

bool alloy::core::axis_aligned_box::intersects( const axis_aligned_box& other )
  const noexcept
{
  if (m_bottom_left.x() > other.m_top_right.x()) {
    return false;
  }
  if (m_top_right.x() < other.m_bottom_left.x()) {
    return false;
  }

  if (m_bottom_left.y() > other.m_top_right.y()) {
    return false;
  }
  if (m_top_right.y() < other.m_top_right.y()) {
    return false;
  }

  if (m_bottom_left.z() > other.m_top_right.z()) {
    return false;
  }
  if (m_top_right.z() < other.m_bottom_left.z()) {
    return false;
  }

  return true;
}

bool alloy::core::axis_aligned_box::encloses( const axis_aligned_box& other )
  const noexcept
{
  return (m_bottom_left.x() <= other.m_bottom_left.x()) &&
         (m_top_right.x() >= other.m_top_right.x()) &&
         (m_bottom_left.y() <= other.m_bottom_left.y()) &&
         (m_top_right.y() >= other.m_top_right.y()) &&
         (m_bottom_left.z() <= other.m_bottom_left.z()) &&
         (m_top_right.z() >= other.m_top_right.z());
}

//==============================================================================
// non-member functions : class : axis_aligned_box
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

bool alloy::core::operator==( const axis_aligned_box& lhs,
                              const axis_aligned_box& rhs )
  noexcept
{
  return lhs.top_right_point() == rhs.top_right_point() &&
         lhs.bottom_left_point() == rhs.bottom_left_point();
}

bool alloy::core::operator!=( const axis_aligned_box& lhs,
                              const axis_aligned_box& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

bool alloy::core::almost_equal( const axis_aligned_box& lhs,
                                const axis_aligned_box& rhs )
  noexcept
{
  return almost_equal(lhs.top_right_point(), rhs.top_right_point()) &&
         almost_equal(lhs.bottom_left_point(), rhs.bottom_left_point());
}

bool alloy::core::almost_equal( const axis_aligned_box& lhs,
                                const axis_aligned_box& rhs,
                                real tolerance )
  noexcept
{
  return almost_equal(lhs.top_right_point(),
                      rhs.top_right_point(),
                      tolerance) &&
         almost_equal(lhs.bottom_left_point(),
                      rhs.bottom_left_point(),
                      tolerance);
}
