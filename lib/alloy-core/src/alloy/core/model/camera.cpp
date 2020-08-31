#include "alloy/core/model/camera.hpp"

#include "alloy/core/math/vector/vector_utilities.hpp"

//-----------------------------------------------------------------------------
// Relative Transformations
//-----------------------------------------------------------------------------

auto alloy::core::camera::translate(real delta_x,
                                    real delta_y,
                                    real delta_z)
  noexcept -> void
{
  translate(vector3{delta_x, delta_y, delta_z});
}

auto alloy::core::camera::translate(const vector3& delta)
  noexcept -> void
{
  m_translation += delta;
}

auto alloy::core::camera::translate_x(real delta)
  noexcept -> void
{
  m_translation.x() += delta;
}

auto alloy::core::camera::translate_y(real delta)
  noexcept -> void
{
  m_translation.y() += delta;
}

auto alloy::core::camera::translate_z(real delta)
  noexcept -> void
{
  m_translation.z() += delta;
}

//-----------------------------------------------------------------------------

auto alloy::core::camera::rotate(const quaternion& orientation)
  noexcept -> void
{
  m_orientation *= orientation;
}

auto alloy::core::camera::rotate(const euler_angles& angles)
  noexcept -> void
{
  rotate(quaternion::from_angles(angles));
}

auto alloy::core::camera::rotate_around_axis(radian angle,
                                             const vector3& axis)
  noexcept -> void
{
  rotate(quaternion::from_angle_axis(angle, axis));
}

auto alloy::core::camera::rotate_roll(radian angle)
  noexcept -> void
{
  rotate(quaternion::from_angles(radian{0}, radian{0}, angle));
}

auto alloy::core::camera::rotate_pitch(radian angle)
  noexcept -> void
{
  rotate(quaternion::from_angles(radian{0}, angle, radian{0}));
}

auto alloy::core::camera::rotate_yaw(radian angle)
  noexcept -> void
{
  rotate(quaternion::from_angles(angle, radian{0}, radian{0}));
}

//-----------------------------------------------------------------------------
// Absolute Transformations
//-----------------------------------------------------------------------------

auto alloy::core::camera::set_origin(real x, real y, real z)
  noexcept -> void
{
  m_translation.x() = x;
  m_translation.y() = y;
  m_translation.z() = z;
}

auto alloy::core::camera::set_origin(const vector3& offset)
  noexcept -> void
{
  m_translation = offset;
}

auto alloy::core::camera::set_x_translation(real x)
  noexcept -> void
{
  m_translation.x() = x;
}

auto alloy::core::camera::set_y_translation(real y)
  noexcept -> void
{
  m_translation.y() = y;
}

auto alloy::core::camera::set_z_translation(real z)
  noexcept -> void
{
  m_translation.z() = z;
}

//-----------------------------------------------------------------------------

auto alloy::core::camera::set_roll(radian angle)
  noexcept -> void
{
  m_orientation *= quaternion::from_angles(radian{0}, radian{0}, angle);
}

auto alloy::core::camera::set_pitch(radian angle)
  noexcept -> void
{
  m_orientation *= quaternion::from_angles(radian{0}, angle, radian{0});
}

auto alloy::core::camera::set_yaw(radian angle)
  noexcept -> void
{
  m_orientation *= quaternion::from_angles(angle, radian{0}, radian{0});
}

auto alloy::core::camera::set_orientation(const quaternion& orientation) 
  noexcept -> void
{
  m_orientation = orientation;
}

auto alloy::core::camera::set_orientation(const euler_angles& angles) 
  noexcept -> void
{
  m_orientation = quaternion::from_angles(angles);
}

auto alloy::core::camera::look_at(const vector3& subject, const vector3& up)
  noexcept -> void
{
  const auto diff = (m_translation - subject);
  const auto z_axis = diff.normalized();
  const auto x_axis = up.cross(z_axis).normalized();
  const auto y_axis = z_axis.cross(x_axis);

  m_orientation = quaternion::from_rotation_axes(x_axis, y_axis, z_axis);
}

auto alloy::core::camera::look_at(real x, real y, real z, const vector3& up)
  noexcept -> void
{
  look_at(vector3{x,y,z}, up);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::core::camera::translation() 
  const noexcept -> vector3
{
  return m_translation;
}

auto alloy::core::camera::orientation()
  const noexcept -> quaternion
{
  return m_orientation;
}

auto alloy::core::camera::angles()
  const noexcept -> euler_angles
{
  return euler_angles{
    m_orientation.yaw(),
    m_orientation.pitch(),
    m_orientation.roll(),
  };
}

//-----------------------------------------------------------------------------
// Conversion
//-----------------------------------------------------------------------------

auto alloy::core::camera::extract_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  m_orientation.extract_rotation_matrix(out);

  auto& result = *out;

  const auto x_axis = to_vector3(result.row(0));
  const auto y_axis = to_vector3(result.row(1));
  const auto z_axis = to_vector3(result.row(2));

  const auto x_pos = -dot(x_axis, m_translation);
  const auto y_pos = -dot(y_axis, m_translation);
  const auto z_pos = -dot(z_axis, m_translation);

  result[0][3] = x_pos;
  result[1][3] = y_pos;
  result[2][3] = z_pos;
}

auto alloy::core::camera::to_matrix4()
  const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_matrix(&result);

  return result;
}
