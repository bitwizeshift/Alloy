// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/model/transform.hpp"

#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/euler_angles.hpp"
#include "alloy/core/math/interpolation.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/utilities/not_null.hpp"

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

alloy::core::transform::transform() noexcept
  : m_orientation{},
    m_translation{},
    m_scale{real{1}, real{1}, real{1}}
{

}

//-----------------------------------------------------------------------------
// Relative Transformations
//-----------------------------------------------------------------------------

auto alloy::core::transform::translate(real delta_x, real delta_y, real delta_z)
  noexcept -> void
{
  m_translation.x() += delta_x;
  m_translation.y() += delta_y;
  m_translation.z() += delta_z;
}

auto alloy::core::transform::translate(const vector3& delta) noexcept -> void
{
  m_translation += delta;
}

auto alloy::core::transform::translate_x(real delta) noexcept -> void
{
  m_translation.x() += delta;
}

auto alloy::core::transform::translate_y(real delta) noexcept -> void
{
  m_translation.y() += delta;
}

auto alloy::core::transform::translate_z(real delta) noexcept -> void
{
  m_translation.z() += delta;
}

//-----------------------------------------------------------------------------

auto alloy::core::transform::rotate(const quaternion& orientation)
  noexcept -> void
{
  m_orientation *= orientation;
}

auto alloy::core::transform::rotate(const euler_angles& angles) noexcept -> void
{
  rotate(quaternion::from_angles(angles));
}

auto alloy::core::transform::rotate(radian angle, const vector3& axis)
  noexcept -> void
{
  rotate(quaternion::from_angle_axis(angle, axis));
}

auto alloy::core::transform::rotate_roll(radian angle) noexcept -> void
{
  rotate(euler_angles{radian{0}, radian{0}, angle});
}

auto alloy::core::transform::rotate_pitch(radian angle) noexcept -> void
{
  rotate(euler_angles{radian{0}, angle, radian{0}});
}

auto alloy::core::transform::rotate_yaw(radian angle) noexcept -> void
{
  rotate(euler_angles{angle, radian{0}, radian{0}});
}

//-----------------------------------------------------------------------------

auto alloy::core::transform::scale(real uniform) noexcept -> void
{
  m_scale *= uniform;
}

auto alloy::core::transform::scale(const vector3& scale) noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_scale[i] *= scale[i];
  }
}

auto alloy::core::transform::scale_x(real scale) noexcept -> void
{
  m_scale.x() *= scale;
}

auto alloy::core::transform::scale_y(real scale) noexcept -> void
{
  m_scale.y() *= scale;
}

auto alloy::core::transform::scale_z(real scale) noexcept -> void
{
  m_scale.z() *= scale;
}

//-----------------------------------------------------------------------------
// Absolute Transformations
//-----------------------------------------------------------------------------

auto alloy::core::transform::set_translation(real x, real y, real z)
  noexcept -> void
{
  m_translation = {x,y,z};
}

auto alloy::core::transform::set_translation(const vector3& translation)
  noexcept -> void
{
  m_translation = translation;
}

auto alloy::core::transform::set_translation_x(real x) noexcept -> void
{
  m_translation.x() = x;
}

auto alloy::core::transform::set_translation_y(real y) noexcept -> void
{
  m_translation.y() = y;
}

auto alloy::core::transform::set_translation_z(real z) noexcept -> void
{
  m_translation.z() = z;
}

auto alloy::core::transform::set_roll(radian angle) noexcept -> void
{
  const auto angles = m_orientation.to_euler_angles();

  m_orientation = quaternion::from_angles(angles.yaw(), angles.pitch(), angle);
}

auto alloy::core::transform::set_pitch(radian angle) noexcept -> void
{
  const auto angles = m_orientation.to_euler_angles();

  m_orientation = quaternion::from_angles(angles.yaw(), angle, angles.roll());
}

auto alloy::core::transform::set_yaw(radian angle) noexcept -> void
{
  const auto angles = m_orientation.to_euler_angles();

  m_orientation = quaternion::from_angles(angle, angles.pitch(), angles.roll());
}

auto alloy::core::transform::set_orientation(const quaternion& q)
  noexcept -> void
{
  m_orientation = q;
}

auto alloy::core::transform::set_orientation(const euler_angles& angles)
  noexcept -> void
{
  m_orientation = quaternion::from_angles(angles);
}

auto alloy::core::transform::set_orientation(radian angle, const vector3& axis)
  noexcept -> void
{
  m_orientation = quaternion::from_angle_axis(angle, axis);
}

auto alloy::core::transform::set_scale(real uniform) noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_scale[i] = uniform;
  }
}

auto alloy::core::transform::set_scale(const vector3& scale) noexcept -> void
{
  m_scale = scale;
}

auto alloy::core::transform::set_scale_x(real scale) noexcept -> void
{
  m_scale.x() = scale;
}

auto alloy::core::transform::set_scale_y(real scale) noexcept -> void
{
  m_scale.y() = scale;
}

auto alloy::core::transform::set_scale_z(real scale) noexcept -> void
{
  m_scale.z() = scale;
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::core::transform::translation() const noexcept -> vector3
{
  return m_translation;
}

auto alloy::core::transform::orientation() const noexcept -> quaternion
{
  return m_orientation;
}

auto alloy::core::transform::angles() const noexcept -> euler_angles
{
  return m_orientation.to_euler_angles();
}

auto alloy::core::transform::scale() const noexcept -> vector3
{
  return m_scale;
}

//-----------------------------------------------------------------------------

auto alloy::core::transform::right() const noexcept -> vector3
{
  return m_orientation.x_axis();
}

auto alloy::core::transform::up() const noexcept -> vector3
{
  return m_orientation.y_axis();
}

auto alloy::core::transform::forward() const noexcept -> vector3
{
  return -m_orientation.z_axis();
}

//-----------------------------------------------------------------------------
// Transformation
//-----------------------------------------------------------------------------

auto alloy::core::transform::transform_point(const vector3& point)
  const noexcept -> vector3
{
  return transform_vector(point) + m_translation;
}

auto alloy::core::transform::transform_vector(const vector3& vector)
  const noexcept -> vector3
{
  const auto scaled = vector3{
    vector.x() * m_scale.x(),
    vector.y() * m_scale.y(),
    vector.z() * m_scale.z()
  };

  return m_orientation * scaled;
}

//-----------------------------------------------------------------------------
// Conversion
//-----------------------------------------------------------------------------

auto alloy::core::transform::extract_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  m_orientation.extract_rotation_matrix(out);

  auto& result = *out;

  // Scaling first scales each column of the rotation by the matching
  // component, and translating last only fills in the final column.
  for (auto c = 0; c < 3; ++c) {
    for (auto r = 0; r < 3; ++r) {
      result[c][r] *= m_scale[c];
    }
    result[3][c] = m_translation[c];
  }
}

auto alloy::core::transform::to_matrix4() const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_matrix(&result);

  return result;
}

auto alloy::core::interpolator<alloy::core::transform>::operator()(
  const transform& v0,
  const transform& v1,
  real alpha
) noexcept -> transform
{
  auto result = transform{};

  result.m_orientation    = interpolation::linear(v0.m_orientation, v1.m_orientation, alpha);
  result.m_translation = interpolation::linear(v0.m_translation, v1.m_translation, alpha);
  result.m_scale       = interpolation::linear(v0.m_scale, v1.m_scale, alpha);

  return result;
}
