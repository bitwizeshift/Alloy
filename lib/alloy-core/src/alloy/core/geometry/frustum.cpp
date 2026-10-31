// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file frustum.cpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#include "alloy/core/geometry/frustum.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/vector/vector4.hpp"

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::core::frustum::from_matrix(const matrix4& view_projection)
  noexcept -> frustum
{
  const auto x = view_projection.row(0);
  const auto y = view_projection.row(1);
  const auto z = view_projection.row(2);
  const auto w = view_projection.row(3);

  // A point is unclipped when '-w <= x <= w', and likewise for 'y' and 'z'.
  // Each of those inequalities is a plane whose normal faces the unclipped
  // side.
  const auto to_plane = [](const vector4& v) -> plane {
    return plane{v.x(), v.y(), v.z(), v.w()}.normalized();
  };

  return make_unchecked(
    to_plane(w + x),
    to_plane(w - x),
    to_plane(w - y),
    to_plane(w + y),
    to_plane(w + z),
    to_plane(w - z)
  );
}

auto alloy::core::frustum::make(
  const plane& left,
  const plane& right,
  const plane& top,
  const plane& bottom,
  const plane& near,
  const plane& far
) noexcept -> frustum
{
  ALLOY_ALWAYS_ASSERT(left.normal().dot(right.normal()) < 0);
  ALLOY_ALWAYS_ASSERT(top.normal().dot(bottom.normal()) < 0);
  ALLOY_ALWAYS_ASSERT(near.normal().dot(far.normal()) < 0);

  return make_unchecked(left, right, top, bottom, near, far);
}

//------------------------------------------------------------------------------
// Contains
//------------------------------------------------------------------------------

auto alloy::core::frustum::contains(const point3& p) const noexcept -> bool
{
  for (const auto& plane : m_planes) {
    if (plane.distance_to(p) < 0) {
      return false;
    }
  }
  return true;
}
