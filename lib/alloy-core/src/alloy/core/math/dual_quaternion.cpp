// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/math/dual_quaternion.hpp"

#include "alloy/core/math/quaternion.hpp"

#include "alloy/core/precision/real.hpp"

#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/quaternion.hpp"
#include "alloy/core/math/trigonometry.hpp"
#include "alloy/core/math/vector/vector3.hpp"

#include "alloy/core/precision/real.hpp"
#include "alloy/core/utilities/not_null.hpp"

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

auto alloy::core::dual_quaternion::from_rotation_translation(
  const quaternion& rotation,
  const vector3& translation
) noexcept -> dual_quaternion
{
  // The dual part is the product of the pure quaternion {0, t/2} and the
  // rotation, expanded so that the terms of the zero real part drop out.
  const auto half = translation * value_type{0.5};
  const auto real_axis = vector3{rotation.x(), rotation.y(), rotation.z()};
  const auto dual_axis = (half * rotation.w()) + half.cross(real_axis);

  return dual_quaternion{
    rotation,
    quaternion{
      -half.dot(real_axis),
      dual_axis.x(),
      dual_axis.y(),
      dual_axis.z()
    }
  };
}

auto alloy::core::dual_quaternion::from_transform_matrix(
  const matrix4& transform
) noexcept -> dual_quaternion
{
  return from_rotation_translation(
    quaternion::from_rotation_matrix(transform),
    vector3{transform[3][0], transform[3][1], transform[3][2]}
  );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::core::dual_quaternion::translation() const noexcept -> vector3
{
  // The translation is the vector part of 2dr*, whose real part is 0 for a
  // unit dual quaternion. This is that product expanded for the vector part
  // alone.
  const auto real_axis = vector3{m_real.x(), m_real.y(), m_real.z()};
  const auto dual_axis = vector3{m_dual.x(), m_dual.y(), m_dual.z()};

  return (
    (dual_axis * m_real.w()) -
    (real_axis * m_dual.w()) +
    real_axis.cross(dual_axis)
  ) * value_type{2};
}

//------------------------------------------------------------------------------
// Extraction
//------------------------------------------------------------------------------

auto alloy::core::dual_quaternion::extract_transform_matrix(
  not_null<matrix4*> transform
) const noexcept -> void
{
  m_real.extract_rotation_matrix(transform);

  const auto offset = translation();

  auto& matrix = (*transform);
  matrix[3][0] = offset.x();
  matrix[3][1] = offset.y();
  matrix[3][2] = offset.z();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::core::dual_quaternion::normalize() noexcept -> dual_quaternion&
{
  const auto mag = m_real.magnitude();

  if (mag > value_type{0}) {
    const auto mag_inv = value_type{1} / mag;
    m_real *= mag_inv;
    m_dual *= mag_inv;

    // Dividing by the dual-number norm |r| + ε(r.d / |r|) is the same as
    // scaling by 1/|r|, and then removing the projection of d onto r.
    m_dual -= m_real * m_real.dot(m_dual);
  }
  return (*this);
}

auto alloy::core::dual_quaternion::invert() noexcept -> dual_quaternion&
{
  // (r + εd)(r^-1 - εr^-1 d r^-1) = 1, since the ε^2 term vanishes.
  m_real.invert();
  m_dual = -(m_real * m_dual * m_real);
  return (*this);
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

auto alloy::core::dual_quaternion::operator*=(const dual_quaternion& rhs)
  noexcept -> dual_quaternion&
{
  m_dual = (m_real * rhs.m_dual) + (m_dual * rhs.m_real);
  m_real *= rhs.m_real;
  return (*this);
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

auto alloy::core::operator==(
  const dual_quaternion& lhs,
  const dual_quaternion& rhs
) noexcept -> bool
{
  return lhs.real() == rhs.real() && lhs.dual() == rhs.dual();
}

auto alloy::core::almost_equal(
  const dual_quaternion& lhs,
  const dual_quaternion& rhs,
  real tolerance
) noexcept -> bool
{
  return almost_equal(lhs.real(), rhs.real(), tolerance)
      && almost_equal(lhs.dual(), rhs.dual(), tolerance);
}

//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

auto alloy::core::nlerp(
  const dual_quaternion& q0,
  const dual_quaternion& q1,
  real alpha
) noexcept -> dual_quaternion
{
  // q and -q are the same transformation; picking the one whose rotation is
  // closer to q0 takes the shorter of the two paths between them.
  const auto end = (dot(q0.real(), q1.real()) < real{0}) ? -q1 : q1;

  return ((q0 * (real{1} - alpha)) + (end * alpha)).normalized();
}

auto alloy::core::sclerp(
  const dual_quaternion& q0,
  const dual_quaternion& q1,
  real alpha
) noexcept -> dual_quaternion
{
  const auto end = (dot(q0.real(), q1.real()) < real{0}) ? -q1 : q1;
  const auto diff = q0.conjugated() * end;
  const auto& diff_real = diff.real();
  const auto& diff_dual = diff.dual();

  // Nearly parallel rotations would divide by a sine that is close to 0. The
  // screw motion between them is nearly a pure translation, which linear
  // blending interpolates at a constant velocity.
  if (diff_real.w() > real{0.9995}) {
    return nlerp(q0, end, alpha);
  }

  // The difference is the screw motion that rotates by theta about the line
  // with direction l and moment m, while translating by the pitch d along it:
  //   {cos(theta/2), sin(theta/2)l}
  //   + ε{-(d/2)sin(theta/2), sin(theta/2)m + (d/2)cos(theta/2)l}
  // Scaling theta and d by alpha raises it to the power of alpha.
  const auto half_theta = trigonometry::arccos(diff_real.w());
  const auto inv_sin = real{1} / trigonometry::sin(half_theta);
  const auto real_axis = vector3{diff_real.x(), diff_real.y(), diff_real.z()};
  const auto dual_axis = vector3{diff_dual.x(), diff_dual.y(), diff_dual.z()};

  const auto direction = real_axis * inv_sin;
  const auto pitch = real{-2} * diff_dual.w() * inv_sin;
  const auto moment = (
    dual_axis - (direction * (pitch * real{0.5} * diff_real.w()))
  ) * inv_sin;

  const auto scaled_half_theta = half_theta * alpha;
  const auto scaled_half_pitch = pitch * real{0.5} * alpha;
  const auto scaled_sin = trigonometry::sin(scaled_half_theta);
  const auto scaled_cos = trigonometry::cos(scaled_half_theta);

  const auto real_part = direction * scaled_sin;
  const auto dual_part = (moment * scaled_sin)
                       + (direction * (scaled_half_pitch * scaled_cos));

  return q0 * dual_quaternion{
    quaternion{scaled_cos, real_part.x(), real_part.y(), real_part.z()},
    quaternion{
      -scaled_half_pitch * scaled_sin,
      dual_part.x(),
      dual_part.y(),
      dual_part.z()
    }
  };
}
