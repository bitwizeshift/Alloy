// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/model/camera.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/euler_angles.hpp"
#include "alloy/core/math/interpolation.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/quaternion.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector3_constants.hpp"
#include "alloy/core/math/vector/vector_utilities.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/utilities/not_null.hpp"

//-----------------------------------------------------------------------------
// World Translations
//-----------------------------------------------------------------------------

auto alloy::core::camera::translate_world(
  real delta_x,
  real delta_y,
  real delta_z
) noexcept -> void
{
  translate_world(vector3{delta_x, delta_y, delta_z});
}

auto alloy::core::camera::translate_world(const vector3& delta) noexcept -> void
{
  m_translation += delta;
}

auto alloy::core::camera::translate_world_x(real delta) noexcept -> void
{
  m_translation.x() += delta;
}

auto alloy::core::camera::translate_world_y(real delta) noexcept -> void
{
  m_translation.y() += delta;
}

auto alloy::core::camera::translate_world_z(real delta) noexcept -> void
{
  m_translation.z() += delta;
}

//-----------------------------------------------------------------------------
// Local Translations
//-----------------------------------------------------------------------------

auto alloy::core::camera::translate_local_x(real delta) noexcept -> void
{
  m_translation += m_right * delta;
}

auto alloy::core::camera::translate_local_y(real delta) noexcept -> void
{
  m_translation += m_up * delta;
}

auto alloy::core::camera::translate_local_z(real delta) noexcept -> void
{
  m_translation += m_forward * delta;
}

auto alloy::core::camera::translate_local(const vector3& delta) noexcept -> void
{
  translate_local_x(delta.x());
  translate_local_y(delta.y());
  translate_local_z(delta.z());
}

//-----------------------------------------------------------------------------

auto alloy::core::camera::rotate(const quaternion& orientation) noexcept -> void
{
  m_orientation *= orientation;
  update_axes();
}

auto alloy::core::camera::rotate(const euler_angles& angles) noexcept -> void
{
  rotate(quaternion::from_angles(angles));
}

auto alloy::core::camera::rotate(radian angle, const vector3& axis)
  noexcept -> void
{
  rotate(quaternion::from_angle_axis(angle, axis));
}

auto alloy::core::camera::rotate_roll(radian angle) noexcept -> void
{
  rotate(quaternion::from_angles(radian{0}, radian{0}, angle));
}

auto alloy::core::camera::rotate_pitch(radian angle) noexcept -> void
{
  rotate(quaternion::from_angles(radian{0}, angle, radian{0}));
}

auto alloy::core::camera::rotate_yaw(radian angle) noexcept -> void
{
  rotate(quaternion::from_angles(angle, radian{0}, radian{0}));
}

//-----------------------------------------------------------------------------

auto alloy::core::camera::rotate_world(const quaternion& orientation)
  noexcept -> void
{
  m_orientation = orientation * m_orientation;
  update_axes();
}

auto alloy::core::camera::rotate_world(radian angle, const vector3& axis)
  noexcept -> void
{
  rotate_world(quaternion::from_angle_axis(angle, axis));
}

auto alloy::core::camera::rotate_world_yaw(radian angle) noexcept -> void
{
  rotate_world(quaternion::from_angle_axis(angle, vector3_constants::unit_y));
}

//-----------------------------------------------------------------------------
// Absolute Transformations
//-----------------------------------------------------------------------------

auto alloy::core::camera::set_translation(real x, real y, real z)
  noexcept -> void
{
  m_translation = vector3{x, y, z};
}

auto alloy::core::camera::set_translation(const vector3& translation)
  noexcept -> void
{
  m_translation = translation;
}

auto alloy::core::camera::set_translation_x(real x) noexcept -> void
{
  m_translation.x() = x;
}

auto alloy::core::camera::set_translation_y(real y) noexcept -> void
{
  m_translation.y() = y;
}

auto alloy::core::camera::set_translation_z(real z) noexcept -> void
{
  m_translation.z() = z;
}

//-----------------------------------------------------------------------------

auto alloy::core::camera::set_roll(radian angle) noexcept -> void
{
  const auto angles = m_orientation.to_euler_angles();

  m_orientation = quaternion::from_angles(angles.yaw(), angles.pitch(), angle);
  update_axes();
}

auto alloy::core::camera::set_pitch(radian angle) noexcept -> void
{
  const auto angles = m_orientation.to_euler_angles();

  m_orientation = quaternion::from_angles(angles.yaw(), angle, angles.roll());
  update_axes();
}

auto alloy::core::camera::set_yaw(radian angle) noexcept -> void
{
  const auto angles = m_orientation.to_euler_angles();

  m_orientation = quaternion::from_angles(angle, angles.pitch(), angles.roll());
  update_axes();
}

auto alloy::core::camera::set_orientation(const quaternion& orientation)
  noexcept -> void
{
  m_orientation = orientation;
  update_axes();
}

auto alloy::core::camera::set_orientation(const euler_angles& angles)
  noexcept -> void
{
  m_orientation = quaternion::from_angles(angles);
  update_axes();
}

auto alloy::core::camera::look_at(const vector3& subject, const vector3& up)
  noexcept -> void
{
  const auto direction = (subject - m_translation);

  ALLOY_ASSERT(direction.square_magnitude() > default_tolerance);
  ALLOY_ASSERT(up.cross(direction).square_magnitude() > default_tolerance);

  m_orientation = quaternion::look_rotation(direction, up);
  update_axes();
}

auto alloy::core::camera::look_at(real x, real y, real z, const vector3& up)
  noexcept -> void
{
  look_at(vector3{x,y,z}, up);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::core::camera::translation() const noexcept -> vector3
{
  return m_translation;
}

auto alloy::core::camera::orientation() const noexcept -> quaternion
{
  return m_orientation;
}

auto alloy::core::camera::angles() const noexcept -> euler_angles
{
  return m_orientation.to_euler_angles();
}

//-----------------------------------------------------------------------------

auto alloy::core::camera::right() const noexcept -> vector3
{
  return m_right;
}

auto alloy::core::camera::up() const noexcept -> vector3
{
  return m_up;
}

auto alloy::core::camera::forward() const noexcept -> vector3
{
  return m_forward;
}

//-----------------------------------------------------------------------------
// Conversion
//-----------------------------------------------------------------------------

auto alloy::core::camera::extract_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  // The view rotates from world space into camera space, which is the inverse
  // of the orientation, and the inverse of a rotation is its transpose.
  m_orientation.extract_rotation_matrix(out);
  out->transpose();

  auto& result = *out;

  const auto x_axis = to_vector3(result.row(0));
  const auto y_axis = to_vector3(result.row(1));
  const auto z_axis = to_vector3(result.row(2));

  const auto x_pos = -dot(x_axis, m_translation);
  const auto y_pos = -dot(y_axis, m_translation);
  const auto z_pos = -dot(z_axis, m_translation);

  result[3][0] = x_pos;
  result[3][1] = y_pos;
  result[3][2] = z_pos;
}

auto alloy::core::camera::to_matrix4() const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_matrix(&result);

  return result;
}

auto alloy::core::camera::extract_inverse_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  // The camera's own transform rotates by the orientation, and then
  // translates to the camera's position.
  m_orientation.extract_rotation_matrix(out);

  auto& result = *out;

  result[3][0] = m_translation.x();
  result[3][1] = m_translation.y();
  result[3][2] = m_translation.z();
}

auto alloy::core::camera::to_inverse_matrix4() const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_inverse_matrix(&result);

  return result;
}

//-----------------------------------------------------------------------------
// Private Members
//-----------------------------------------------------------------------------

auto alloy::core::camera::update_axes() noexcept -> void
{
  auto rotation = matrix3{};
  m_orientation.extract_rotation_matrix(&rotation);

  m_right   = rotation.column(0);
  m_up      = rotation.column(1);

  // The camera looks down its negative z-axis.
  m_forward = -rotation.column(2);
}

//-----------------------------------------------------------------------------

auto alloy::core::interpolator<alloy::core::camera>::operator()(
  const camera& v0,
  const camera& v1,
  real alpha
) noexcept -> camera
{
  auto result = camera{};

  result.m_translation = interpolation::linear(v0.m_translation, v1.m_translation, alpha);
  result.m_orientation = interpolation::linear(v0.m_orientation, v1.m_orientation, alpha);
  result.update_axes();

  return result;
}
