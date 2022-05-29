#include "alloy/core/geometry/axis_aligned_box.hpp"

#include <algorithm> // std::min, std::max

//==============================================================================
// class : axis_aligned_box
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::core::axis_aligned_box::from_point_and_size(
  const point3& p,
  const vector3& size
) noexcept -> axis_aligned_box
{
  return from_points(p, p + size);
}

auto alloy::core::axis_aligned_box::from_points(const point3& p0, const point3& p1)
  noexcept -> axis_aligned_box
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

auto alloy::core::axis_aligned_box::top_plane()
  const noexcept -> plane
{
  const auto normal = vector3{0, 1, 0};
  const auto distance = m_top_right.y();

  return plane{normal, distance};
}

auto alloy::core::axis_aligned_box::bottom_plane()
  const noexcept -> plane
{
  const auto normal = vector3{0, -1, 0};
  const auto distance = m_bottom_left.y();

  return plane{normal, distance};
}

auto alloy::core::axis_aligned_box::front_plane()
  const noexcept -> plane
{
  const auto normal = vector3{0, 0, 1};
  const auto distance = m_top_right.z();

  return plane{normal, distance};
}

auto alloy::core::axis_aligned_box::back_plane()
  const noexcept -> plane
{
  const auto normal = vector3{0, 0, -1};
  const auto distance = m_bottom_left.z();

  return plane{normal, distance};
}

auto alloy::core::axis_aligned_box::left_plane()
  const noexcept -> plane
{
  const auto normal = vector3{-1, 0, 0};
  const auto distance = m_bottom_left.x();

  return plane{normal, distance};
}

auto alloy::core::axis_aligned_box::right_plane()
  const noexcept -> plane
{
  const auto normal = vector3{1, 0, 0};
  const auto distance = m_top_right.x();

  return plane{normal, distance};
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::axis_aligned_box::normals()
  const noexcept -> std::array<vector3, 6>
{
  const auto p = planes();
  auto result = std::array<vector3,6>{};

  for (auto i = 0u; i < result.size(); ++i) {
    result[i] = p[i].normal();
  }

  return result;
}

auto alloy::core::axis_aligned_box::planes()
  const noexcept -> std::array<plane, 6>
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

auto alloy::core::axis_aligned_box::contains(const point3& p)
  const noexcept -> bool
{
  return (p.x() <= m_top_right.x()) && (p.x() >= m_bottom_left.x()) &&
         (p.y() <= m_top_right.y()) && (p.y() >= m_bottom_left.y()) &&
         (p.z() <= m_top_right.z()) && (p.z() >= m_bottom_left.z());
}

auto alloy::core::axis_aligned_box::contains(const point3& p, real tolerance)
  const noexcept -> bool
{
  // Add a buffer of 'tolerance' for fuzzy matching
  return (p.x() <= (m_top_right.x() + tolerance)) &&
         (p.x() >= (m_bottom_left.x() - tolerance)) &&
         (p.y() <= (m_top_right.y() + tolerance)) &&
         (p.y() >= (m_bottom_left.y() - tolerance)) &&
         (p.z() <= (m_top_right.z() + tolerance)) &&
         (p.z() >= (m_bottom_left.z() - tolerance));
}

auto alloy::core::axis_aligned_box::intersects(const axis_aligned_box& other)
  const noexcept -> bool
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

auto alloy::core::axis_aligned_box::encloses(const axis_aligned_box& other)
  const noexcept -> bool
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

auto alloy::core::operator==(const axis_aligned_box& lhs, const axis_aligned_box& rhs)
  noexcept -> bool
{
  return lhs.top_right_point() == rhs.top_right_point() &&
         lhs.bottom_left_point() == rhs.bottom_left_point();
}

auto alloy::core::operator!=(const axis_aligned_box& lhs, const axis_aligned_box& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

auto alloy::core::almost_equal(const axis_aligned_box& lhs,
                               const axis_aligned_box& rhs,
                               real tolerance)
  noexcept -> bool
{
  return almost_equal(lhs.top_right_point(),
                      rhs.top_right_point(),
                      tolerance) &&
         almost_equal(lhs.bottom_left_point(),
                      rhs.bottom_left_point(),
                      tolerance);
}
