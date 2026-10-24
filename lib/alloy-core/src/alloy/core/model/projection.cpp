// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/model/projection.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/math/angle/degree.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/matrix/matrix4_constants.hpp"
#include "alloy/core/math/trigonometry.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <variant>

//------------------------------------------------------------------------------
// Static Factories: Identity
//------------------------------------------------------------------------------

auto alloy::core::projection::identity() noexcept -> projection
{
  return projection{};
}

//------------------------------------------------------------------------------
// Static Factories: Perspective
//------------------------------------------------------------------------------

auto alloy::core::projection::perspective(
  degree fov,
  real aspect_ratio,
  view_volume::depth_bounds depth
) noexcept -> projection
{
  return perspective(to_radian(fov), aspect_ratio, depth);
}

auto alloy::core::projection::perspective(
  radian fov,
  real aspect_ratio,
  view_volume::depth_bounds depth
) noexcept -> projection
{
  ALLOY_ASSERT(aspect_ratio > 0);
  ALLOY_ASSERT(depth.near > 0);
  ALLOY_ASSERT(depth.near < depth.far);
  ALLOY_ASSERT(fov > radian{0});
  ALLOY_ASSERT(fov < radian_constants::half_revolution);

  return projection{
    perspective_data{
      fov,
      aspect_ratio,
      depth
    }
  };
}

//------------------------------------------------------------------------------
// Static Factories: Orthographic
//------------------------------------------------------------------------------

auto alloy::core::projection::orthographic(
  real left,
  real right,
  real bottom,
  real top,
  real near,
  real far
) noexcept -> projection
{
  return orthographic(view_volume::make(left, right, bottom, top, near, far));
}

auto alloy::core::projection::orthographic(const view_volume& volume)
  noexcept -> projection
{
  return projection{orthographic_data{volume}};
}

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

// clang-tidy flags this function for throwing exceptions despite being marked
// noexcept. The only exceptions that can throw are if std::visit throws, and
// this is insured by being an internal invariant.
//
// NOLINTNEXTLINE(bugprone-exception-escape)
auto alloy::core::projection::extract_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  ALLOY_ASSERT_AND_ASSUME(!m_storage.valueless_by_exception());

  std::visit([out](const auto& data) {
    extract_matrix_of(data, out);
  }, m_storage);
}

auto alloy::core::projection::to_matrix4() const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_matrix(&result);
  return result;
}

//------------------------------------------------------------------------------
// Private Static Functions
//------------------------------------------------------------------------------

auto alloy::core::projection::extract_matrix_of(
  const identity_data&,
  not_null<matrix4*> out
) noexcept -> void
{
  (*out) = matrix4_constants::identity;
}

auto alloy::core::projection::extract_matrix_of(
  const perspective_data& data,
  not_null<matrix4*> out
) noexcept -> void
{
  const auto& depth = data.depth;

  const auto half_tan_fov = trigonometry::tan(data.fov * real{0.5});

  const auto dz = (depth.far - depth.near);

  const auto rx = real{1} / (data.aspect_ratio * half_tan_fov);
  const auto ry = real{1} / half_tan_fov;
  const auto rz = -(depth.far + depth.near) / dz;
  const auto factor = - (real{2} * depth.far * depth.near) / dz;

  (*out) = matrix4{
    rx, 0,  0,   0,
    0,  ry, 0,   0,
    0,  0,  rz,  factor,
    0,  0,  -1,  0,
  };
}

auto alloy::core::projection::extract_matrix_of(
  const orthographic_data& data,
  not_null<matrix4*> out
) noexcept -> void
{
  const auto& left = data.horizontal().left;
  const auto& right = data.horizontal().right;
  const auto& bottom = data.vertical().bottom;
  const auto& top = data.vertical().top;
  const auto& near = data.depth().near;
  const auto& far = data.depth().far;

  const auto dx = (right - left);
  const auto dy = (top - bottom);
  const auto dz = (far - near);

  const auto tx = -((right + left) / dx);
  const auto ty = -((top + bottom) / dy);
  const auto tz = -((far + near) / dz);

  const auto rx = real{2} / dx;
  const auto ry = real{2} / dy;
  const auto rz = -real{2} / dz;

  (*out) = matrix4{
    rx, 0,  0,  tx,
    0,  ry, 0,  ty,
    0,  0,  rz, tz,
    0,  0,  0,  1,
  };
}
