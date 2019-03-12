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

#include "alloy/math/quaternion.hpp"

#include "alloy/math/math.hpp"
#include "alloy/math/matrix/matrix_utilities.hpp"
#include "alloy/math/trigonometry.hpp"

#include <stdexcept> // std::out_of_range
#include <cassert>   // assert

//------------------------------------------------------------------------------
// Static Factory Functions
//------------------------------------------------------------------------------

alloy::math::quaternion
  alloy::math::quaternion::from_angle_axis( radian angle,
                                            const vector_type& axis )
  noexcept
{
  const auto norm_axis = axis.normalized();

  const auto half_angle = angle * core::real{0.5};
  const auto result = trigonometry::sin(half_angle);

  const auto w = trigonometry::cos( half_angle );
  const auto x = norm_axis.x() * result;
  const auto y = norm_axis.y() * result;
  const auto z = norm_axis.z() * result;

  return {w,x,y,z};
}

alloy::math::quaternion
  alloy::math::quaternion::from_angles( radian yaw, radian pitch, radian roll )
  noexcept
{
  // Half Angles
  const auto half_yaw   = yaw * core::real{0.5};
  const auto half_pitch = pitch * core::real{0.5};
  const auto half_roll  = roll * core::real{0.5};

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

alloy::math::quaternion
  alloy::math::quaternion::from_rotation_matrix( const matrix3_type& rot )
  noexcept
{
  // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
  // article "Quaternion Calculus and Fast Animation".

  const auto trace = rot.trace();
  auto root = core::real{};

  if (trace > core::real{0}){
    root = sqrt( trace + 1 );
    const auto w = core::real{0.5} * root;
    root = core::real{0.5} / root;
    const auto x = (rot(2,1) - rot(1,2)) * root;
    const auto y = (rot(0,2) - rot(2,0)) * root;
    const auto z = (rot(1,0) - rot(0,1)) * root;

    return {w,x,y,z};
  }

  static constexpr int s_next[3] = { 1, 2, 0 };
  core::real apk_quat[3] = {};
  int i, j, k;

  i = 0;
  if ( rot(1,1) > rot(0,0) ) {
    i = 1;
  }
  if ( rot(2,2) > rot(i,i) ) {
    i = 2;
  }
  j = s_next[i];
  k = s_next[j];

  root = sqrt(rot(i,i) - rot(j,j) - rot(k,k) + core::real{1});

  apk_quat[i] = core::real{0.5} * root;

  root = core::real{0.5} / root;

  apk_quat[j] = (rot(j,i) + rot(i,j)) * root;
  apk_quat[k] = (rot(k,i) + rot(i,k)) * root;

  const auto w = (rot(k,j) - rot(j,k)) * root;
  const auto x = apk_quat[0];
  const auto y = apk_quat[1];
  const auto z = apk_quat[2];

  return {w,x,y,z};
}

alloy::math::quaternion
  alloy::math::quaternion::from_rotation_matrix( const matrix4_type& rot )
  noexcept
{
  using namespace alloy::math::casts;

  return from_rotation_matrix( matrix_cast<matrix3_type>(rot) );
}

alloy::math::quaternion
  alloy::math::quaternion::from_rotation_axes( const vector_type& x_axis,
                                               const vector_type& y_axis,
                                               const vector_type& z_axis )
  noexcept
{
  return from_rotation_matrix( matrix3_type{x_axis,y_axis,z_axis} );
}

//----------------------------------------------------------------------------
// Element Access
//----------------------------------------------------------------------------

alloy::math::quaternion::reference
  alloy::math::quaternion::at( index_type n )
{
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range{"quaternion::at: index out of range"};
  }

  return m_data[n];
}

alloy::math::quaternion::const_reference
  alloy::math::quaternion::at( index_type n )
  const
{
  if( n >= 4 || n < 0 )
    throw std::out_of_range{"quaternion::at: index out of range"};

  return m_data[n];
}

//----------------------------------------------------------------------------

alloy::math::vector3<alloy::math::quaternion::value_type>
  alloy::math::quaternion::x_axis()
  const noexcept
{
  const auto ty  = 2.0f * y();
  const auto tz  = 2.0f * z();
  const auto twy = ty * w();
  const auto twz = tz * w();
  const auto txy = ty * x();
  const auto txz = tz * x();
  const auto tyy = ty * y();
  const auto tzz = tz * z();

  return vector_type{1.0f - (tyy + tzz), txy + twz, txz - twy};
}

//----------------------------------------------------------------------------

alloy::math::vector3<alloy::math::quaternion::value_type>
  alloy::math::quaternion::y_axis()
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

  return vector_type{txy - twz, 1.0f - (txx + tzz), tyz + twx};
}

//----------------------------------------------------------------------------

alloy::math::vector3<alloy::math::quaternion::value_type>
  alloy::math::quaternion::z_axis()
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

  return vector_type{txz+twy, tyz-twx, 1.0f-(txx+tyy)};
}

//----------------------------------------------------------------------------
// Extraction
//----------------------------------------------------------------------------

void alloy::math::quaternion::extract_rotation_matrix( matrix3_type* rot )
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

  matrix(0,0) = core::real{1} - (tyy + tzz);
  matrix(0,1) = txy - twz;
  matrix(0,2) = txz + twy;

  matrix(1,0) = txy + twz;
  matrix(1,1) = core::real{1} - (txx + tzz);
  matrix(1,2) = tyz - twx;

  matrix(2,0) = txz - twy;
  matrix(2,1) = tyz + twx;
  matrix(2,2) = core::real{1} - (txx + tyy);
}

//----------------------------------------------------------------------------

void alloy::math::quaternion::extract_rotation_matrix( matrix4_type* rot )
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

  matrix(0,0) = core::real{1} - (tyy + tzz);
  matrix(0,1) = txy - twz;
  matrix(0,2) = txz + twy;
  matrix(0,3) = core::real{0};

  matrix(1,0) = txy + twz;
  matrix(1,1) = core::real{1} - (txx + tzz);
  matrix(1,2) = tyz - twx;
  matrix(1,3) = core::real{0};

  matrix(2,0) = txz - twy;
  matrix(2,1) = tyz + twx;
  matrix(2,2) = core::real{1} - (txx + tyy);
  matrix(2,3) = core::real{0};

  matrix(3,0) = core::real{0};
  matrix(3,1) = core::real{0};
  matrix(3,2) = core::real{0};
  matrix(3,3) = core::real{1};
}

//----------------------------------------------------------------------------

void alloy::math::quaternion::extract_angle_axis( radian* angle,
                                                  vector_type* axis )
  const noexcept
{
  assert( angle != nullptr );
  assert( axis != nullptr );

  const auto squared_length = x()*x() + y()*y() + z()*z();

  if( squared_length > core::real{0} ){

    const auto inv_length = core::real{1} / sqrt(squared_length);
    (*angle) = 2 * trigonometry::arccos( x() );
    axis->x() = x() * inv_length;
    axis->y() = y() * inv_length;
    axis->z() = z() * inv_length;

  }else{

    (*angle)  = radian{0};
    axis->x() = core::real{1};
    axis->y() = core::real{0};
    axis->z() = core::real{0};

  }
}

//----------------------------------------------------------------------------

void alloy::math::quaternion::extract_axes( vector_type* x_axis,
                                            vector_type* y_axis,
                                            vector_type* z_axis )
  const noexcept
{
  assert( x_axis != nullptr );
  assert( y_axis != nullptr );
  assert( z_axis != nullptr );

  auto mat = matrix3_type{};
  extract_rotation_matrix(&mat);

  (*x_axis) = mat.row(0);
  (*y_axis) = mat.row(1);
  (*z_axis) = mat.row(2);

}

//----------------------------------------------------------------------------
// Modifiers
//----------------------------------------------------------------------------

alloy::math::quaternion& alloy::math::quaternion::normalize()
  noexcept
{
  const auto mag_inv = core::real{1} / magnitude();

  for (auto i=0; i<4; ++i) {
    m_data[i] *= mag_inv;
  }
  return (*this);
}

alloy::math::quaternion& alloy::math::quaternion::invert()
  noexcept
{
  const auto mag = magnitude();
  if (mag > 0.0){
    const auto mag_inv = core::real{1} / mag;
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

alloy::math::quaternion::value_type
  alloy::math::quaternion::dot( const quaternion& rhs )
  const noexcept
{
  auto result = quaternion::value_type{};
  for (auto i=0; i<4; ++i) {
    result += m_data[i] * rhs.m_data[i];
  }

  return result;
}

alloy::math::quaternion::value_type alloy::math::quaternion::magnitude()
  const noexcept
{
  return sqrt(dot(*this));
}

//----------------------------------------------------------------------------

alloy::math::radian alloy::math::quaternion::roll( reproject_axis_t reproject )
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

alloy::math::radian alloy::math::quaternion::roll()
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

alloy::math::radian alloy::math::quaternion::pitch( reproject_axis_t )
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

alloy::math::radian alloy::math::quaternion::pitch()
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

alloy::math::radian alloy::math::quaternion::yaw( reproject_axis_t )
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

alloy::math::radian alloy::math::quaternion::yaw()
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

alloy::math::quaternion&
  alloy::math::quaternion::operator+=( const quaternion& rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    m_data[i] += rhs[i];
  }

  return (*this);
}

alloy::math::quaternion&
  alloy::math::quaternion::operator-=( const quaternion& rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    m_data[i] -= rhs[i];
  }

  return (*this);
}

alloy::math::quaternion&
  alloy::math::quaternion::operator*=( const quaternion& rhs )
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

alloy::math::quaternion&
  alloy::math::quaternion::operator*=( value_type rhs )
  noexcept
{
  for (auto i=0; i<4;++i) {
    m_data[i] *= rhs;
  }

  return (*this);
}

alloy::math::quaternion&
  alloy::math::quaternion::operator/=( value_type rhs )
  noexcept
{
  const auto inv = core::real{1} / rhs;

  for (auto i=0; i<4;++i) {
    m_data[i] *= inv;
  }

  return (*this);
}


alloy::math::quaternion&
  alloy::math::quaternion::operator/=( const quaternion& rhs )
  noexcept
{
  (*this) *= rhs.inverse();

  return (*this);
}

//----------------------------------------------------------------------------

alloy::math::quaternion::vector_type
  alloy::math::operator*( const quaternion& lhs,
                          const quaternion::vector_type& rhs )
  noexcept
{
  auto qvec = quaternion::vector_type{lhs.x(), lhs.y(), lhs.z()};

  auto uv = qvec.cross(rhs);
  auto uuv = qvec.cross(uv);
  uv *= (core::real{2} * lhs.w());
  uuv *= core::real{2};

  return (rhs + uv + uuv);
}