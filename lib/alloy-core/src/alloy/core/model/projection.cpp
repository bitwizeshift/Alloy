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
  view_volume::depth_bounds depth,
  clip_depth clip
) noexcept -> projection
{
  return perspective(to_radian(fov), aspect_ratio, depth, clip);
}

auto alloy::core::projection::perspective(
  radian fov,
  real aspect_ratio,
  view_volume::depth_bounds depth,
  clip_depth clip
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
      depth,
      clip
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
  real far,
  clip_depth clip
) noexcept -> projection
{
  return orthographic(
    view_volume::make(left, right, bottom, top, near, far),
    clip
  );
}

auto alloy::core::projection::orthographic(
  const view_volume& volume,
  clip_depth clip
) noexcept -> projection
{
  return projection{orthographic_data{volume, clip}};
}

//------------------------------------------------------------------------------
// Adjustment
//------------------------------------------------------------------------------

// clang-tidy flags this function for throwing exceptions despite being marked
// noexcept. The only exceptions that can throw are if std::visit throws, and
// this is insured by being an internal invariant.
//
// NOLINTNEXTLINE(bugprone-exception-escape)
auto alloy::core::projection::with_aspect_ratio(real aspect_ratio)
  const noexcept -> projection
{
  ALLOY_ASSERT(aspect_ratio > 0);
  ALLOY_ASSERT_AND_ASSUME(!m_storage.valueless_by_exception());

  return std::visit([aspect_ratio](const auto& data) -> projection {
    return with_aspect_ratio_of(data, aspect_ratio);
  }, m_storage);
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

// clang-tidy flags this function for throwing exceptions despite being marked
// noexcept. The only exceptions that can throw are if std::visit throws, and
// this is insured by being an internal invariant.
//
// NOLINTNEXTLINE(bugprone-exception-escape)
auto alloy::core::projection::extract_inverse_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  ALLOY_ASSERT_AND_ASSUME(!m_storage.valueless_by_exception());

  std::visit([out](const auto& data) {
    extract_inverse_matrix_of(data, out);
  }, m_storage);
}

auto alloy::core::projection::to_inverse_matrix4() const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_inverse_matrix(&result);
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
  const auto rz = (data.clip == clip_depth::zero_to_one)
    ? -depth.far / dz
    : -(depth.far + depth.near) / dz;
  const auto factor = (data.clip == clip_depth::zero_to_one)
    ? -(depth.far * depth.near) / dz
    : -(real{2} * depth.far * depth.near) / dz;

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
  const auto& volume = data.volume;
  const auto& left = volume.horizontal().left;
  const auto& right = volume.horizontal().right;
  const auto& bottom = volume.vertical().bottom;
  const auto& top = volume.vertical().top;
  const auto& near = volume.depth().near;
  const auto& far = volume.depth().far;

  const auto dx = (right - left);
  const auto dy = (top - bottom);
  const auto dz = (far - near);

  const auto tx = -((right + left) / dx);
  const auto ty = -((top + bottom) / dy);
  const auto tz = (data.clip == clip_depth::zero_to_one)
    ? -(near / dz)
    : -((far + near) / dz);

  const auto rx = real{2} / dx;
  const auto ry = real{2} / dy;
  const auto rz = (data.clip == clip_depth::zero_to_one)
    ? -real{1} / dz
    : -real{2} / dz;

  (*out) = matrix4{
    rx, 0,  0,  tx,
    0,  ry, 0,  ty,
    0,  0,  rz, tz,
    0,  0,  0,  1,
  };
}

//------------------------------------------------------------------------------

auto alloy::core::projection::extract_inverse_matrix_of(
  const identity_data& data,
  not_null<matrix4*> out
) noexcept -> void
{
  // The identity projection is its own inverse.
  extract_matrix_of(data, out);
}

auto alloy::core::projection::extract_inverse_matrix_of(
  const perspective_data& data,
  not_null<matrix4*> out
) noexcept -> void
{
  // The perspective matrix only has entries on its diagonal, and in the final
  // row and column of the depth, so its inverse is read from those entries.
  extract_matrix_of(data, out);

  const auto rx = out->get(0, 0);
  const auto ry = out->get(1, 1);
  const auto rz = out->get(2, 2);
  const auto factor = out->get(2, 3);

  (*out) = matrix4{
    real{1} / rx, 0,            0,                0,
    0,            real{1} / ry, 0,                0,
    0,            0,            0,                -1,
    0,            0,            real{1} / factor, rz / factor,
  };
}

auto alloy::core::projection::extract_inverse_matrix_of(
  const orthographic_data& data,
  not_null<matrix4*> out
) noexcept -> void
{
  // The orthographic matrix only scales and then translates each axis, so
  // its inverse is read from those entries.
  extract_matrix_of(data, out);

  const auto rx = out->get(0, 0);
  const auto ry = out->get(1, 1);
  const auto rz = out->get(2, 2);
  const auto tx = out->get(0, 3);
  const auto ty = out->get(1, 3);
  const auto tz = out->get(2, 3);

  (*out) = matrix4{
    real{1} / rx, 0,            0,            -tx / rx,
    0,            real{1} / ry, 0,            -ty / ry,
    0,            0,            real{1} / rz, -tz / rz,
    0,            0,            0,            1,
  };
}

//------------------------------------------------------------------------------

auto alloy::core::projection::with_aspect_ratio_of(
  const identity_data&,
  real
) noexcept -> projection
{
  return projection::identity();
}

auto alloy::core::projection::with_aspect_ratio_of(
  const perspective_data& data,
  real aspect_ratio
) noexcept -> projection
{
  auto result = data;
  result.aspect_ratio = aspect_ratio;

  return projection{result};
}

auto alloy::core::projection::with_aspect_ratio_of(
  const orthographic_data& data,
  real aspect_ratio
) noexcept -> projection
{
  const auto& horizontal = data.volume.horizontal();
  const auto& vertical = data.volume.vertical();
  const auto center = (horizontal.left + horizontal.right) * real{0.5};
  const auto half_width = (vertical.top - vertical.bottom)
                        * aspect_ratio
                        * real{0.5};
  const auto volume = view_volume::make_unchecked(
    view_volume::horizontal_bounds{center - half_width, center + half_width},
    vertical,
    data.volume.depth()
  );

  return projection{orthographic_data{volume, data.clip}};
}
