#include "alloy/core/model/transform.hpp"

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

alloy::core::transform::transform()
  noexcept
  : m_rotation{},
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

auto alloy::core::transform::translate(const vector3& delta)
  noexcept -> void
{
  m_translation += delta;
}

auto alloy::core::transform::translate_x(real delta) noexcept
  -> void
{
  m_translation.x() += delta;
}

auto alloy::core::transform::translate_y(real delta)
  noexcept -> void
{
  m_translation.y() += delta;
}

auto alloy::core::transform::translate_z(real delta)
  noexcept -> void
{
  m_translation.z() += delta;
}

//-----------------------------------------------------------------------------

auto alloy::core::transform::rotate(const quaternion& orientation)
  noexcept -> void
{
  m_rotation *= orientation;
}

auto alloy::core::transform::rotate(const euler_angles& angles)
  noexcept -> void
{
  rotate(quaternion::from_angles(angles));
}

auto alloy::core::transform::rotate_around_axis(radian angle,
                                                const vector3& axis)
  noexcept -> void
{
  const auto orientation = quaternion::from_angle_axis(angle, axis);

  rotate(orientation);
}

auto alloy::core::transform::rotate_roll(radian angle)
  noexcept -> void
{
  rotate(euler_angles{radian{0}, radian{0}, angle});
}

auto alloy::core::transform::rotate_pitch(radian angle)
  noexcept -> void
{
  rotate(euler_angles{radian{0}, angle, radian{0}});
}

auto alloy::core::transform::rotate_yaw(radian angle)
  noexcept -> void
{
  rotate(euler_angles{angle, radian{0}, radian{0}});
}

//-----------------------------------------------------------------------------

auto alloy::core::transform::scale(real uniform)
  noexcept -> void
{
  m_scale *= uniform;
}

auto alloy::core::transform::scale(const vector3& scale)
  noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_scale[i] *= scale[i];
  }
}

auto alloy::core::transform::scale_x(real scale)
  noexcept -> void
{
  m_scale.x() *= scale;
}

auto alloy::core::transform::scale_y(real scale)
  noexcept -> void
{
  m_scale.y() *= scale;
}

auto alloy::core::transform::scale_z(real scale)
  noexcept -> void
{
  m_scale.z() *= scale;
}

//-----------------------------------------------------------------------------
// Absolute Transformations
//-----------------------------------------------------------------------------

auto alloy::core::transform::set_origin(real x, real y, real z)
  noexcept -> void
{
  m_translation = {x,y,z};
}

auto alloy::core::transform::set_origin(const vector3& offset)
  noexcept -> void
{
  m_translation = offset;
}

auto alloy::core::transform::set_x_translation(real x)
  noexcept -> void
{
  m_translation.x() = x;
}

auto alloy::core::transform::set_y_translation(real y)
  noexcept -> void
{
  m_translation.y() = y;
}

auto alloy::core::transform::set_z_translation(real z)
  noexcept -> void
{
  m_translation.z() = z;
}

auto alloy::core::transform::set_roll(radian angle)
  noexcept -> void
{
  m_rotation = quaternion::from_angles(
    m_rotation.yaw(),
    m_rotation.pitch(),
    angle
  );
}

auto alloy::core::transform::set_pitch(radian angle)
  noexcept -> void
{
  m_rotation = quaternion::from_angles(
    m_rotation.yaw(),
    angle,
    m_rotation.roll()
  );
}

auto alloy::core::transform::set_yaw(radian angle)
  noexcept -> void
{
  m_rotation = quaternion::from_angles(
    angle,
    m_rotation.pitch(),
    m_rotation.roll()
  );
}

auto alloy::core::transform::set_orientation(const quaternion& q)
  noexcept -> void
{
  m_rotation = q;
}

auto alloy::core::transform::set_orientation(const euler_angles& angles)
  noexcept -> void
{
  m_rotation = quaternion::from_angles(angles);
}

auto alloy::core::transform::set_angle_axis(radian angle, const vector3& axis)
  noexcept -> void
{
  m_rotation = quaternion::from_angle_axis(angle, axis);
}

auto alloy::core::transform::set_scale(real uniform)
  noexcept -> void
{
  for (auto i = 0; i < 3; ++i) {
    m_scale[i] = uniform;
  }
}

auto alloy::core::transform::set_scale(const vector3& scale)
  noexcept -> void
{
  m_scale = scale;
}

auto alloy::core::transform::set_x_scale(real scale)
  noexcept -> void
{
  m_scale.x() = scale;
}

auto alloy::core::transform::set_y_scale(real scale)
  noexcept -> void
{
  m_scale.y() = scale;
}

auto alloy::core::transform::set_z_scale(real scale)
  noexcept -> void
{
  m_scale.z() = scale;
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

auto alloy::core::transform::translation()
  const noexcept -> vector3
{
  return m_translation;
}

auto alloy::core::transform::orientation()
  const noexcept -> quaternion
{
  return m_rotation;
}

auto alloy::core::transform::angles()
  const noexcept -> euler_angles
{
  return euler_angles{
    m_rotation.yaw(),
    m_rotation.pitch(),
    m_rotation.roll()
  };
}

auto alloy::core::transform::scale()
  const noexcept -> vector3
{
  return m_scale;
}

//-----------------------------------------------------------------------------
// Conversion
//-----------------------------------------------------------------------------

auto alloy::core::transform::extract_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  const auto scale = matrix4{
    m_scale.x(), real{0},     real{0},     real{0},
    real{0},     m_scale.y(), real{0},     real{0},
    real{0},     real{0},     m_scale.z(), real{0},
    real{0},     real{0},     real{0},     real{1},
  };

  const auto translation = matrix4{
    real{1}, real{0}, real{0}, m_translation.x(),
    real{0}, real{1}, real{0}, m_translation.y(),
    real{0}, real{0}, real{1}, m_translation.z(),
    real{0}, real{0}, real{0}, real{1},
  };

  auto rotation = matrix4{};
  m_rotation.extract_rotation_matrix(&rotation);

  auto& result = *out;

  result = rotation;
  result *= translation;
  result *= scale;
}

auto alloy::core::transform::to_matrix4()
  const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_matrix(&result);

  return result;
}
