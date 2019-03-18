/*
 * Note: The mathematical logic in the quaternion class comes primarily
 *       from the OGRE3D Quaternion library. To avoid any accidental
 *       misrepresentation, the corresponding license is attached below:
 *
 * Copyright (c) 2000-2014 Torus Knot Software Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "alloy/core/quaternion.hpp"

#include "alloy/core/math.hpp"
#include "alloy/core/matrix/matrix_utilities.hpp"
#include "alloy/core/trigonometry.hpp"

#include <stdexcept> // std::out_of_range
#include <cassert>   // assert

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

alloy::core::quaternion
  alloy::core::quaternion::from_angle_axis( radian angle,
                                            const vector3& axis )
  noexcept
{
  const auto norm_axis = axis.normalized();

  const auto half_angle = angle * real{0.5};
  const auto result = trigonometry::sin(half_angle);

  const auto w = trigonometry::cos( half_angle );
  const auto x = norm_axis.x() * result;
  const auto y = norm_axis.y() * result;
  const auto z = norm_axis.z() * result;

  return {w,x,y,z};
}

alloy::core::quaternion
  alloy::core::quaternion::from_angles( radian yaw, radian pitch, radian roll )
  noexcept
{
  // Half Angles
  const auto half_yaw   = yaw * real{0.5};
  const auto half_pitch = pitch * real{0.5};
  const auto half_roll  = roll * real{0.5};

  // y-vector
  const auto v0w = trigonometry::cos( half_yaw );
  const auto v0y = trigonometry::sin( half_yaw );

  // x-vector
  const auto v1w = trigonometry::cos( half_pitch );
  const auto v1x = trigonometry::sin( half_pitch );

  // z-vector
  const auto v2w = trigonometry::cos( half_roll );
  const auto v2z = trigonometry::sin( half_roll );

  // y * x vector
  const auto w1 = (v0w * v1w);
  const auto x1 = (v0w * v1x);
  const auto y1 = (v0y * v1w);
  const auto z1 = -(v0y * v1x);

  // (x * y) * z vector
  const auto w = (w1 * v2w) - (z1 * v2z);
  const auto x = (x1 * v2w) + (y1 * v2z);
  const auto y = (y1 * v2w) - (x1 * v2z);
  const auto z = (w1 * v2z) + (z1 * v2w);

  return {w,x,y,z};
}

alloy::core::quaternion
  alloy::core::quaternion::from_rotation_matrix( const matrix3& rot )
  noexcept
{
  // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
  // article "Quaternion Calculus and Fast Animation".

  const auto trace = rot.trace();
  auto root = real{};

  if (trace > real{0}){
    root = sqrt( trace + 1 );
    const auto w = real{0.5} * root;
    root = real{0.5} / root;
    const auto x = (rot.get(2,1) - rot.get(1,2)) * root;
    const auto y = (rot.get(0,2) - rot.get(2,0)) * root;
    const auto z = (rot.get(1,0) - rot.get(0,1)) * root;

    return {w,x,y,z};
  }

  static constexpr int s_next[3] = { 1, 2, 0 };
  core::real apk_quat[3] = {};
  int i, j, k;

  i = 0;
  if ( rot.get(1,1) > rot.get(0,0) ) {
    i = 1;
  }
  if ( rot.get(2,2) > rot.get(i,i) ) {
    i = 2;
  }
  j = s_next[i];
  k = s_next[j];

  root = sqrt(rot.get(i,i) - rot.get(j,j) - rot.get(k,k) + real{1});

  apk_quat[i] = real{0.5} * root;

  root = real{0.5} / root;

  apk_quat[j] = (rot.get(j,i) + rot.get(i,j)) * root;
  apk_quat[k] = (rot.get(k,i) + rot.get(i,k)) * root;

  const auto w = (rot.get(k,j) - rot.get(j,k)) * root;
  const auto x = apk_quat[0];
  const auto y = apk_quat[1];
  const auto z = apk_quat[2];

  return {w,x,y,z};
}

alloy::core::quaternion
  alloy::core::quaternion::from_rotation_matrix( const matrix4& rot )
  noexcept
{
  using namespace alloy::core::casts;

  return from_rotation_matrix( matrix_cast<matrix3>(rot) );
}

alloy::core::quaternion
  alloy::core::quaternion::from_rotation_axes( const vector3& x_axis,
                                               const vector3& y_axis,
                                               const vector3& z_axis )
  noexcept
{
  return from_rotation_matrix( matrix3{x_axis,y_axis,z_axis} );
}

//----------------------------------------------------------------------------
// Element Access
//----------------------------------------------------------------------------

alloy::core::quaternion::reference
  alloy::core::quaternion::at( index_type n )
{
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range{"quaternion::at: index out of range"};
  }

  return m_data[n];
}

alloy::core::quaternion::const_reference
  alloy::core::quaternion::at( index_type n )
  const
{
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range{"quaternion::at: index out of range"};
  }

  return m_data[n];
}

//----------------------------------------------------------------------------

alloy::core::vector3
  alloy::core::quaternion::x_axis()
  const noexcept
{
  const auto ty  = real{2.0} * y();
  const auto tz  = real{2.0} * z();
  const auto twy = ty * w();
  const auto twz = tz * w();
  const auto txy = ty * x();
  const auto txz = tz * x();
  const auto tyy = ty * y();
  const auto tzz = tz * z();

  return vector3{real{1.0} - (tyy + tzz), txy + twz, txz - twy};
}

//----------------------------------------------------------------------------

alloy::core::vector3
  alloy::core::quaternion::y_axis()
  const noexcept
{
  const auto tx  = 2.0f * x();
  const auto ty  = 2.0f * y();
  const auto tz  = 2.0f * z();
  const auto twx = tx * w();
  const auto twz = tz * w();
  const auto txx = tx * x();
  const auto txy = ty * x();
  const auto tyz = tz * y();
  const auto tzz = tz * z();

  return vector3{txy - twz, 1.0f - (txx + tzz), tyz + twx};
}

//----------------------------------------------------------------------------

alloy::core::vector3
  alloy::core::quaternion::z_axis()
  const noexcept
{
  const auto tx  = 2.0f * x();
  const auto ty  = 2.0f * y();
  const auto tz  = 2.0f * z();
  const auto twx = tx * w();
  const auto twy = ty * w();
  const auto txx = tx * x();
  const auto txz = tz * x();
  const auto tyy = ty * y();
  const auto tyz = tz * y();

  return vector3{txz+twy, tyz-twx, 1.0f-(txx+tyy)};
}

//----------------------------------------------------------------------------
// Extraction
//----------------------------------------------------------------------------

void alloy::core::quaternion::extract_rotation_matrix( matrix3* rot )
  const noexcept
{
  const auto tx  = 2 * x();
  const auto ty  = 2 * y();
  const auto tz  = 2 * z();
  const auto twx = tx * w();
  const auto twy = ty * w();
  const auto twz = tz * w();
  const auto txx = tx * x();
  const auto txy = ty * x();
  const auto txz = tz * x();
  const auto tyy = ty * y();
  const auto tyz = tz * y();
  const auto tzz = tz * z();

  //--------------------------------------------------------------------------

  assert( rot != nullptr );

  auto& matrix = (*rot);

  matrix.get(0,0) = real{1} - (tyy + tzz);
  matrix.get(0,1) = txy - twz;
  matrix.get(0,2) = txz + twy;

  matrix.get(1,0) = txy + twz;
  matrix.get(1,1) = real{1} - (txx + tzz);
  matrix.get(1,2) = tyz - twx;

  matrix.get(2,0) = txz - twy;
  matrix.get(2,1) = tyz + twx;
  matrix.get(2,2) = real{1} - (txx + tyy);
}

//----------------------------------------------------------------------------

void alloy::core::quaternion::extract_rotation_matrix( matrix4* rot )
  const noexcept
{
  const auto tx  = real{2} * x();
  const auto ty  = real{2} * y();
  const auto tz  = real{2} * z();
  const auto twx = tx * w();
  const auto twy = ty * w();
  const auto twz = tz * w();
  const auto txx = tx * x();
  const auto txy = ty * x();
  const auto txz = tz * x();
  const auto tyy = ty * y();
  const auto tyz = tz * y();
  const auto tzz = tz * z();

  //--------------------------------------------------------------------------

  assert( rot != nullptr );

  auto& matrix = (*rot);

  matrix.get(0,0) = real{1} - (tyy + tzz);
  matrix.get(0,1) = txy - twz;
  matrix.get(0,2) = txz + twy;
  matrix.get(0,3) = real{0};

  matrix.get(1,0) = txy + twz;
  matrix.get(1,1) = real{1} - (txx + tzz);
  matrix.get(1,2) = tyz - twx;
  matrix.get(1,3) = real{0};

  matrix.get(2,0) = txz - twy;
  matrix.get(2,1) = tyz + twx;
  matrix.get(2,2) = real{1} - (txx + tyy);
  matrix.get(2,3) = real{0};

  matrix.get(3,0) = real{0};
  matrix.get(3,1) = real{0};
  matrix.get(3,2) = real{0};
  matrix.get(3,3) = real{1};
}

//----------------------------------------------------------------------------

void alloy::core::quaternion::extract_angle_axis( radian* angle,
                                                  vector3* axis )
  const noexcept
{
  assert( angle != nullptr );
  assert( axis != nullptr );

  const auto squared_length = x()*x() + y()*y() + z()*z();

  if( squared_length > real{0} ){

    const auto inv_length = real{1} / sqrt(squared_length);
    (*angle) = 2 * trigonometry::arccos( x() );
    axis->x() = x() * inv_length;
    axis->y() = y() * inv_length;
    axis->z() = z() * inv_length;

  }else{

    (*angle)  = radian{0};
    axis->x() = real{1};
    axis->y() = real{0};
    axis->z() = real{0};

  }
}

//----------------------------------------------------------------------------

void alloy::core::quaternion::extract_axes( vector3* x_axis,
                                            vector3* y_axis,
                                            vector3* z_axis )
  const noexcept
{
  assert( x_axis != nullptr );
  assert( y_axis != nullptr );
  assert( z_axis != nullptr );

  auto mat = matrix3{};
  extract_rotation_matrix(&mat);

  (*x_axis) = mat.row(0);
  (*y_axis) = mat.row(1);
  (*z_axis) = mat.row(2);

}

//----------------------------------------------------------------------------
// Modifiers
//----------------------------------------------------------------------------

alloy::core::quaternion& alloy::core::quaternion::normalize()
  noexcept
{
  const auto mag_inv = real{1} / magnitude();

  for (auto i=0; i<4; ++i) {
    m_data[i] *= mag_inv;
  }
  return (*this);
}

alloy::core::quaternion& alloy::core::quaternion::invert()
  noexcept
{
  const auto mag = magnitude();
  if (mag > 0.0){
    const auto mag_inv = real{1} / mag;
    w() *=  mag_inv;
    x() *= -mag_inv;
    y() *= -mag_inv;
    z() *= -mag_inv;
  } else {
    // Return an invalid quaternion to signify that this is not invertable
    for (auto i=0; i<4; ++i) {
      m_data[i] = 0;
    }
  }
  return (*this);
}

//----------------------------------------------------------------------------
// Quantifiers
//----------------------------------------------------------------------------

alloy::core::quaternion::value_type
  alloy::core::quaternion::dot( const quaternion& rhs )
  const noexcept
{
  auto result = quaternion::value_type{};
  for (auto i=0; i<4; ++i) {
    result += m_data[i] * rhs.m_data[i];
  }

  return result;
}

alloy::core::quaternion::value_type alloy::core::quaternion::magnitude()
  const noexcept
{
  return sqrt(dot(*this));
}

//----------------------------------------------------------------------------

alloy::core::radian alloy::core::quaternion::roll( reproject_axis_t reproject )
  const noexcept
{
  const auto ty  = 2 * y();
  const auto tz  = 2 * z();
  const auto twz = tz * w();
  const auto txy = ty * x();
  const auto tyy = ty * y();
  const auto tzz = tz * z();

  const auto f1 = txy+twz;
  const auto f2 = 1 - (tyy + tzz);
  const auto angle = trigonometry::arctan2(f1, f2);
  return angle >= radian_constants::half_revolution
       ? (angle - radian_constants::half_revolution)
       : angle;
}

alloy::core::radian alloy::core::quaternion::roll()
  const noexcept
{
  const auto f1 = 2*(x()*y() + w()*z());
  const auto f2 = w()*w() + x()*x() - y()*y() - z()*z();
  const auto angle = trigonometry::arctan2(f1, f2);
  return angle >= radian_constants::half_revolution
       ? (angle - radian_constants::half_revolution)
       :  angle;
}

//----------------------------------------------------------------------------

alloy::core::radian alloy::core::quaternion::pitch( reproject_axis_t )
  const noexcept
{
  const auto tx  = 2 * x();
  const auto tz  = 2 * z();
  const auto twx = tx * w();
  const auto txx = tx * x();
  const auto tyz = tz * y();
  const auto tzz = tz * z();

  const auto f1 = tyz + twx;
  const auto f2 = 1 - (txx + tzz);
  const auto angle = trigonometry::arctan2(f1, f2);
  return angle >= radian_constants::half_revolution
       ? (angle - radian_constants::half_revolution)
       :  angle;
}

alloy::core::radian alloy::core::quaternion::pitch()
  const noexcept
{
  const auto f1 = 2*(y()*z() + w()*x());
  const auto f2 = w()*w() - x()*x() - y()*y() + z()*z();
  const auto angle = trigonometry::arctan2(f1, f2);
  return angle >= radian_constants::half_revolution
       ? (angle - radian_constants::half_revolution)
       :  angle;
}

//----------------------------------------------------------------------------

alloy::core::radian alloy::core::quaternion::yaw( reproject_axis_t )
  const noexcept
{
  const auto tx  = 2 * x();
  const auto ty  = 2 * y();
  const auto tz  = 2 * z();
  const auto twy = ty * w();
  const auto txx = tx * x();
  const auto txz = tz * x();
  const auto tyy = ty * y();

  const auto f1 = txz + twy;
  const auto f2 = 1 - (txx + tyy);
  const auto angle = trigonometry::arctan2(f1, f2);
  return angle >= radian_constants::half_revolution
       ? (angle - radian_constants::half_revolution)
       :  angle;
}

alloy::core::radian alloy::core::quaternion::yaw()
  const noexcept
{
  const auto angle = trigonometry::arcsin(-2*(x()*z() - w()*y()));
  return angle >= radian_constants::half_revolution
       ? (angle - radian_constants::half_revolution)
       :  angle;
}

//----------------------------------------------------------------------------
// Compound Operators
//----------------------------------------------------------------------------

alloy::core::quaternion&
  alloy::core::quaternion::operator+=( const quaternion& rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    m_data[i] += rhs[i];
  }

  return (*this);
}

alloy::core::quaternion&
  alloy::core::quaternion::operator-=( const quaternion& rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    m_data[i] -= rhs[i];
  }

  return (*this);
}

alloy::core::quaternion&
  alloy::core::quaternion::operator*=( const quaternion& rhs )
  noexcept
{
  const auto tmp0 = w() * rhs.w() - x() * rhs.x() - y() * rhs.y() - z() * rhs.z();
  const auto tmp1 = w() * rhs.x() + x() * rhs.w() + y() * rhs.z() - z() * rhs.y();
  const auto tmp2 = w() * rhs.y() + y() * rhs.w() + z() * rhs.x() - x() * rhs.z();
  const auto tmp3 = w() * rhs.z() + z() * rhs.w() + x() * rhs.y() - y() * rhs.x();

  w() = tmp0;
  x() = tmp1;
  y() = tmp2;
  z() = tmp3;
  return (*this);
}

alloy::core::quaternion&
  alloy::core::quaternion::operator*=( value_type rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    m_data[i] *= rhs;
  }

  return (*this);
}

alloy::core::quaternion&
  alloy::core::quaternion::operator/=( value_type rhs )
  noexcept
{
  const auto inv = real{1} / rhs;

  for (auto i=0; i<4;++i) {
    m_data[i] *= inv;
  }

  return (*this);
}


alloy::core::quaternion&
  alloy::core::quaternion::operator/=( const quaternion& rhs )
  noexcept
{
  (*this) *= rhs.inverse();

  return (*this);
}

//----------------------------------------------------------------------------

alloy::core::vector3
  alloy::core::operator*( const quaternion& lhs,
                          const vector3& rhs )
  noexcept
{
  auto qvec = vector3{lhs.x(), lhs.y(), lhs.z()};

  auto uv  = qvec.cross(rhs);
  auto uuv = qvec.cross(uv);
  uv *= (real{2} * lhs.w());
  uuv *= real{2};

  return (rhs + uv + uuv);
}