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
#include "alloy/core/geometry/point/point3.hpp"

auto alloy::core::frustum::contains(const point3& p) const noexcept -> bool
{
  for (const auto& plane : m_planes) {
    if (plane.distance_to(p) < 0) {
      return false;
    }
  }
  return true;
}

auto alloy::core::frustum::make(
  const plane& left,
  const plane& right,
  const plane& top,
  const plane& bottom,
  const plane& near,
  const plane& far
) -> frustum
{
  ALLOY_ALWAYS_ASSERT(left.normal().dot(right.normal()) < 0);
  ALLOY_ALWAYS_ASSERT(top.normal().dot(bottom.normal()) < 0);
  ALLOY_ALWAYS_ASSERT(near.normal().dot(far.normal()) < 0);

  return make_unchecked(left, right, top, bottom, near, far);
}

