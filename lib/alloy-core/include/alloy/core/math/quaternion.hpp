// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file quaternion.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_CORE_MATH_QUATERNION_HPP
#define ALLOY_CORE_MATH_QUATERNION_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/euler_angles.hpp"
#include "alloy/core/math/interpolation.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/precision.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <tuple>       // std::tuple
#include <type_traits>

namespace alloy::core {

  namespace detail {
    struct reproject_axis{};
  }
  /// \brief Function used for tag dispatch
#ifndef ALLOY_DOXYGEN
  inline void reproject_axis(detail::reproject_axis){}
#else
  auto reproject_axis = implementation defined
#endif

  /// \brief Type used for tag dispatching reprojecting the axis
#ifndef ALLOY_DOXYGEN
  using reproject_axis_t = void(*)(detail::reproject_axis);
#else
  using reproject_axis_t = implementation defined
#endif

  //============================================================================
  // class : quaternion
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A quaternion, used to represent a rotation in 3 dimensions
  ///
  /// The components are stored in the order {w, x, y, z}, where w is the real
  /// part and x, y, and z are the imaginary parts.
  ///
  /// Products follow the Hamilton convention, where i^2 = j^2 = k^2 = ijk =
  /// -1. A unit quaternion q rotates a vector v as q v q^-1, so the product
  /// `a * b` rotates by `b` first and then by `a`, in the same order as the
  /// column-vector matrices.
  ///
  /// Rotations are right-handed: a positive angle about an axis rotates
  /// counter-clockwise when looking down the axis toward the origin, so a
  /// quarter turn about the z-axis takes the x-axis to the y-axis.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API quaternion
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = core::real;        ///< The underlying value type
    using pointer         = value_type*;       ///< The pointer type
    using reference       = value_type&;       ///< The reference type
    using const_pointer   = const value_type*; ///< The const reference type
    using const_reference = const value_type&; ///< The const pointer type

    using size_type  = std::size_t;           ///< The type used for sizes
    using index_type = std::ptrdiff_t;        ///< The type used for indices

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    inline static constexpr auto comparison_tolerance = default_tolerance;

    //--------------------------------------------------------------------------
    // Static Factory Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a quaternion from an \p angle and an \p axis
    ///
    /// \param angle the angle
    /// \param axis the axis
    /// \return the constructed quaternion
    static auto from_angle_axis(radian angle, const vector3& axis)
      noexcept -> quaternion;

    /// \brief Constructs a quaternion from the given a \p yaw, \p pitch, and
    ///        \p roll
    ///
    /// The yaw is about the y-axis, the pitch is about the x-axis, and the
    /// roll is about the z-axis. The result is the product
    /// `yaw * pitch * roll`, which yaws, then pitches about the yawed
    /// x-axis, then rolls about the resulting z-axis.
    ///
    /// \param yaw the yaw angle
    /// \param pitch the pitch angle
    /// \param roll the roll angle
    /// \return the constructed quaternion
    static auto from_angles(radian yaw, radian pitch, radian roll)
      noexcept -> quaternion;

    /// \brief Constructs a quaternion from euler \p angles
    ///
    /// \param angles the angles to construct from
    /// \return the constructed quaternion
    static auto from_angles(const euler_angles& angles)
      noexcept -> quaternion;

    /// \brief Constructs a quaternion from a 3x3 rotation matrix
    ///
    /// \param rot the rotation matrix
    /// \return the constructed quaternion
    static auto from_rotation_matrix(const matrix3& rot) noexcept -> quaternion;

    /// \brief Constructs a quaternion from a 4x4 rotation matrix
    ///
    /// \param rot the rotation matrix
    /// \return the constructed quaternion
    static auto from_rotation_matrix(const matrix4& rot) noexcept -> quaternion;

    /// \brief Constructs a quaternion from 3 axes forming a 3x3 rotation
    ///        matrix
    ///
    /// The axes are the columns of the rotation matrix, which are the
    /// directions that the rotation maps the unit x, y, and z vectors to.
    /// This is the inverse of `extract_axes`.
    ///
    /// \pre the axes are orthonormal and form a right-handed basis
    ///
    /// \param x_axis the vector representing the x-axis
    /// \param y_axis the vector representing the y-axis
    /// \param z_axis the vector representing the z-axis
    /// \return the constructed quaternion
    static auto from_rotation_axes(const vector3& x_axis,
                                   const vector3& y_axis,
                                   const vector3& z_axis)
      noexcept -> quaternion;

    /// \brief Constructs the shortest rotation that turns the direction
    ///        \p from to the direction \p to
    ///
    /// If the directions are opposite, any half turn about an axis that is
    /// perpendicular to both is a shortest rotation, and one is chosen.
    ///
    /// \pre \p from and \p to are not zero vectors
    ///
    /// \param from the direction to rotate from
    /// \param to the direction to rotate to
    /// \return the constructed quaternion
    static auto from_to(const vector3& from, const vector3& to)
      noexcept -> quaternion;

    /// \brief Constructs the rotation that faces the \p forward direction,
    ///        with its y-axis as close to \p up as possible
    ///
    /// Following the convention of OpenGL cameras, the rotation faces down
    /// its negative z-axis, so it maps the negative z-axis to \p forward.
    ///
    /// \pre \p forward is not a zero vector, and is not parallel to \p up
    ///
    /// \param forward the direction to face
    /// \param up the direction that the y-axis is oriented towards
    /// \return the constructed quaternion
    static auto look_rotation(const vector3& forward, const vector3& up)
      noexcept -> quaternion;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a quaternion with an angle of 0
    constexpr quaternion() noexcept;

    /// \brief Constructs a quaternion with only 1 real component
    ///
    /// \param w the real component of the quaternion
    constexpr explicit quaternion(value_type w) noexcept;

    /// \brief Constructs a quaternion with 4 angle components
    ///
    /// \param w the real component of the quaternion
    /// \param x the first imaginary component of the quaternion
    /// \param y the second imaginary component of the quaternion
    /// \param z the final imaginary component of the quaternion
    constexpr quaternion(value_type w,
                         value_type x,
                         value_type y,
                         value_type z) noexcept;

    /// \brief Copy-constructs a quaternion from another quaternion
    ///
    /// \param other the other quaternion to copy
    constexpr quaternion(const quaternion& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the quaternion
    ///
    /// \param other the quaternion to copy
    /// \return reference to `(*this)`
    auto operator=(const quaternion& other) noexcept -> quaternion& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the quaternion
    ///
    /// \return the number of components in the quaternion
    constexpr auto size() const noexcept -> size_type;

    /// \{
    /// \brief Gets the w component of this quaternion
    ///
    /// \return reference to the w component
    constexpr auto w() noexcept -> reference;
    constexpr auto w() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the x component of this quaternion
    ///
    /// \return reference to the x component
    constexpr auto x() noexcept -> reference;
    constexpr auto x() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the y component of this quaternion
    ///
    /// \return reference to the y component
    constexpr auto y() noexcept -> reference;
    constexpr auto y() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the z component of this quaternion
    ///
    /// \return reference to the z component
    constexpr auto z() noexcept -> reference;
    constexpr auto z() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr auto data() noexcept -> pointer;
    constexpr auto data() const noexcept -> const_pointer;
    /// \}

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// Retrieves the element of the quaternion at \p n
    ///
    /// \throw std::out_of_range if indx is out of range
    ///
    /// \param n the element index
    /// \return a reference to the entry
    auto at(index_type n) -> reference;
    auto at(index_type n) const -> const_reference;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// Retrieves the element of the quaternion at \p n
    ///
    /// \param n the element index
    /// \return a reference to the entry
    constexpr auto operator[](index_type n) noexcept -> reference;
    constexpr auto operator[](index_type n) const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the x-axis from this quaternion
    ///
    /// The x-axis is the direction that this rotation maps the unit x vector
    /// to, which is the first column of its rotation matrix.
    ///
    /// \return the x-axis
    auto x_axis() const noexcept -> vector3;

    /// \brief Retrieves the y-axis from this quaternion
    ///
    /// The y-axis is the direction that this rotation maps the unit y vector
    /// to, which is the second column of its rotation matrix.
    ///
    /// \return the y-axis
    auto y_axis() const noexcept -> vector3;

    /// \brief Retrieves the z-axis from this quaternion
    ///
    /// The z-axis is the direction that this rotation maps the unit z vector
    /// to, which is the third column of its rotation matrix.
    ///
    /// \return the z-axis
    auto z_axis() const noexcept -> vector3;

    //--------------------------------------------------------------------------

    /// \brief Calculates and returns the 3x3 rotation matrix
    ///
    /// The matrix rotates column vectors, so `rotation_matrix() * v` is the
    /// same as `(*this) * v`.
    ///
    /// \pre this quaternion is normalized
    ///
    /// \return the rotation matrix
    auto rotation_matrix() const noexcept -> matrix3;

    /// \brief Calculates and returns the rotation angle and axis
    ///
    /// \return a tuple containing the angle and the axis
    auto angle_axis() const noexcept -> std::tuple<radian,vector3>;

    /// \brief Calculates and returns the x, y, and z axes as vectors
    ///
    /// \return a tuple containing the x, y, and z vector axes
    auto axes() const noexcept -> std::tuple<vector3,vector3,vector3>;

    /// \brief Decomposes this rotation into the angles of `from_angles`
    ///
    /// `from_angles` applied to the result produces this rotation again. The
    /// pitch is in the range [-pi/2, pi/2], and the yaw and roll are in the
    /// range [-pi, pi]. When the pitch is a quarter turn, the yaw and roll
    /// rotate about the same axis, so the roll is set to 0 and the yaw holds
    /// their combined rotation.
    ///
    /// \pre this quaternion is normalized
    ///
    /// \return the yaw, pitch, and roll of this rotation
    auto to_euler_angles() const noexcept -> euler_angles;

    //--------------------------------------------------------------------------

    /// \brief Gets the normalized quaternion of `this`
    ///
    /// \return the normalized quaternion of `this`
    auto normalized() const noexcept -> quaternion;

    /// \brief Gets the inverse of `this` quaternion
    ///
    /// The inverse is the quaternion whose product with `this` is the
    /// identity. A zero quaternion has no inverse, and inverts to zero.
    ///
    /// \return the inverse of `this` quaternion
    auto inverse() const noexcept -> quaternion;

    /// \brief Gets the conjugate of `this` quaternion
    ///
    /// The conjugate negates the imaginary parts. For a unit quaternion, it
    /// is the inverse rotation.
    ///
    /// \return the conjugate of `this` quaternion
    auto conjugated() const noexcept -> quaternion;

    //--------------------------------------------------------------------------
    // Extraction
    //--------------------------------------------------------------------------
  public:

    /// \brief Extracts a 3x3 rotation matrix from this quaternion
    ///
    /// \param rot pointer to the rotation matrix to extract into
    auto extract_rotation_matrix(not_null<matrix3*> rot) const noexcept -> void;

    /// \brief Extracts a 4x4 rotation matrix from this quaternion
    ///
    /// \param rot pointer to the rotation matrix to extract into
    auto extract_rotation_matrix(not_null<matrix4*> rot) const noexcept -> void;

    //--------------------------------------------------------------------------

    /// \brief Extracts the \p angle and \p axis from this quaternion
    ///
    /// The angle is in the range [0, 2pi]. A quaternion with no rotation
    /// extracts an angle of 0 about the x-axis.
    ///
    /// \pre this quaternion is normalized
    ///
    /// \param angle the angle to extract into
    /// \param axis the vector to extract into
    auto extract_angle_axis(not_null<radian*> angle,
                            not_null<vector3*> axis) const noexcept -> void;

    //--------------------------------------------------------------------------

    /// \brief Extracts the x, y, and z axes
    ///
    /// The axes are the same as those of `x_axis`, `y_axis`, and `z_axis`.
    ///
    /// \param x_axis pointer to the x-axis vector
    /// \param y_axis pointer to the y-axis vector
    /// \param z_axis pointer to the z-axis vector
    auto extract_axes(not_null<vector3*> x_axis,
                      not_null<vector3*> y_axis,
                      not_null<vector3*> z_axis) const noexcept -> void;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this quaternion and returns a reference to `(*this)`
    ///
    /// A zero quaternion has no direction, and is left as zero.
    ///
    /// \return the reference to `(*this)`
    auto normalize() noexcept -> quaternion&;

    /// \brief Inverts this quaternion and returns a reference to `(*this)`
    ///
    /// A zero quaternion has no inverse, and is left as zero.
    ///
    /// \return the reference to `(*this)`
    auto invert() noexcept -> quaternion&;

    /// \brief Conjugates this quaternion and returns a reference to `(*this)`
    ///
    /// \return the reference to `(*this)`
    auto conjugate() noexcept -> quaternion&;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Computes the dot-product of \p rhs and `this`
    ///
    /// \param rhs the quaternion to perform the dot product with
    /// \return the result of the dot product
    auto dot(const quaternion& rhs) const noexcept -> real;

    /// \brief Computes the magnitude of this quaternion
    ///
    /// \return the magnitude
    auto magnitude() const noexcept -> real;

    //--------------------------------------------------------------------------

    /// \brief Computes the roll angle by reprojecting the axis
    ///
    /// The roll is the angle about the z-axis between the x-axis and the
    /// rotated x-axis projected onto the xy-plane.
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the roll angle, in the range [-pi, pi]
    auto roll(reproject_axis_t reproject) const noexcept -> radian;

    /// \brief Computes the roll angle about the z-axis
    ///
    /// The roll, yaw, and pitch are the angles of this quaternion decomposed
    /// into a rotation about the x-axis by the pitch, followed by the y-axis
    /// by the yaw, followed by the z-axis by the roll.
    ///
    /// \return the roll angle, in the range [-pi, pi]
    auto roll() const noexcept -> radian;

    //--------------------------------------------------------------------------

    /// \brief Computes the pitch angle by reprojecting the axis
    ///
    /// The pitch is the angle about the x-axis between the y-axis and the
    /// rotated y-axis projected onto the yz-plane.
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the pitch angle, in the range [-pi, pi]
    auto pitch(reproject_axis_t reproject) const noexcept -> radian;

    /// \brief Computes the pitch angle about the x-axis
    ///
    /// \see roll() for the decomposition that this angle is a part of
    ///
    /// \return the pitch angle, in the range [-pi, pi]
    auto pitch() const noexcept -> radian;

    //--------------------------------------------------------------------------

    /// \brief Computes the yaw angle by reprojecting the axis
    ///
    /// The yaw is the angle about the y-axis between the z-axis and the
    /// rotated z-axis projected onto the zx-plane.
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the yaw angle, in the range [-pi, pi]
    auto yaw(reproject_axis_t reproject) const noexcept -> radian;

    /// \brief Computes the yaw angle about the y-axis
    ///
    /// \see roll() for the decomposition that this angle is a part of
    ///
    /// \return the yaw angle, in the range [-pi/2, pi/2]
    auto yaw() const noexcept -> radian;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    auto operator+() const noexcept -> quaternion;
    auto operator-() const noexcept -> quaternion;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    auto operator+=(const quaternion& rhs) noexcept -> quaternion&;
    auto operator-=(const quaternion& rhs) noexcept -> quaternion&;
    auto operator*=(const quaternion& rhs) noexcept -> quaternion&;
    auto operator*=(value_type rhs) noexcept -> quaternion&;
    auto operator/=(value_type rhs) noexcept -> quaternion&;
    auto operator/=(const quaternion& rhs) noexcept -> quaternion&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    alignas(16) value_type m_data[4]; ///< The data (w + i*x + j*y + k*z)
  };

  //============================================================================
  // non-member functions : class : quaternion
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operations
  //----------------------------------------------------------------------------

  auto operator+(const quaternion& lhs, const quaternion& rhs)
    noexcept -> quaternion;
  auto operator-(const quaternion& lhs, const quaternion& rhs)
    noexcept -> quaternion;

  /// \brief Computes the Hamilton product of \p lhs and \p rhs
  ///
  /// As a rotation, the product rotates by \p rhs first and then by \p lhs.
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return the product `lhs * rhs`
  auto operator*(const quaternion& lhs, const quaternion& rhs)
    noexcept -> quaternion;

  /// \brief Multiplies \p lhs by the inverse of \p rhs
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return the product `lhs * rhs.inverse()`
  auto operator/(const quaternion& lhs, const quaternion& rhs)
    noexcept -> quaternion;

  auto operator*(const quaternion& lhs, real rhs) noexcept -> quaternion;
  auto operator*(real lhs, const quaternion& rhs) noexcept -> quaternion;

  /// \brief Rotates the vector \p rhs by \p lhs
  ///
  /// \pre \p lhs is normalized
  ///
  /// \param lhs the rotation
  /// \param rhs the vector to rotate
  /// \return the rotated vector
  ALLOY_CORE_API
  auto operator*(const quaternion& lhs, const vector3& rhs) noexcept -> vector3;

  auto operator/(const quaternion& lhs, real rhs) noexcept -> quaternion;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  auto operator==(const quaternion& lhs, const quaternion& rhs)
    noexcept -> bool;
  auto operator!=(const quaternion& lhs, const quaternion& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines whether two quaternions are almost equal, v to
  ///        \p tolerance
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return `true` if \p lhs almost equals \p rhs
  auto almost_equal(
    const quaternion& lhs,
    const quaternion& rhs,
    real tolerance
  ) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return the result of the dot product
  auto dot(const quaternion& lhs, const quaternion& rhs) noexcept -> real;

  /// \brief Computes the magnitude of \p x
  ///
  /// \return the magnitude of \p x
  auto magnitude(const quaternion& x) noexcept -> real;

  //----------------------------------------------------------------------------
  // Interpolation
  //----------------------------------------------------------------------------

  /// \brief Interpolates between \p q0 and \p q1 by normalizing their
  ///        linear interpolation
  ///
  /// The interpolation takes the shortest path between the two rotations.
  /// It is cheaper than \ref slerp, but does not rotate at a constant
  /// angular velocity.
  ///
  /// \pre \p q0 and \p q1 are normalized
  ///
  /// \param q0 the start rotation
  /// \param q1 the end rotation
  /// \param alpha the amount to interpolate [0, 1]
  /// \return the normalized interpolated rotation
  ALLOY_CORE_API
  auto nlerp(const quaternion& q0, const quaternion& q1, real alpha)
    noexcept -> quaternion;

  /// \brief Spherically interpolates between \p q0 and \p q1
  ///
  /// The interpolation takes the shortest path between the two rotations,
  /// at a constant angular velocity.
  ///
  /// \pre \p q0 and \p q1 are normalized
  ///
  /// \param q0 the start rotation
  /// \param q1 the end rotation
  /// \param alpha the amount to interpolate [0, 1]
  /// \return the interpolated rotation
  ALLOY_CORE_API
  auto slerp(const quaternion& q0, const quaternion& q1, real alpha)
    noexcept -> quaternion;

  /// \brief Interpolates rotations with \ref nlerp
  ///
  /// A component-wise linear interpolation of two unit quaternions is not
  /// itself unit length, and so is not a rotation.
  template <>
  struct interpolator<quaternion>
  {
    auto operator()(const quaternion& v0, const quaternion& v1, real alpha)
      noexcept -> quaternion;
  };

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref quaternion
  ///
  /// The result is aliased as `::value`
  template<typename T> struct is_quaternion : std::false_type{};

  template<> struct is_quaternion<quaternion> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_quaternion
  template<typename T>
  constexpr bool is_quaternion_v = is_quaternion<T>::value;

} // namespace alloy::core

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::quaternion::from_angles(const euler_angles& angles)
  noexcept -> quaternion
{
  return from_angles(angles.yaw(), angles.pitch(), angles.roll());
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
alloy::core::quaternion::quaternion() noexcept
  : quaternion{
    real{1},
    real{0},
    real{0},
    real{0}
  }
{

}

ALLOY_FORCE_INLINE constexpr
alloy::core::quaternion::quaternion(value_type w) noexcept
  : quaternion{w,0,0,0}
{

}

ALLOY_FORCE_INLINE constexpr
alloy::core::quaternion::quaternion(
  value_type w,
  value_type x,
  value_type y,
  value_type z
) noexcept
  : m_data{w,x,y,z}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::size() const noexcept -> size_type
{
  return 4;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::w() noexcept -> reference
{
  return m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::w() const noexcept -> const_reference
{
  return m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::x() noexcept -> reference
{
  return m_data[1];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::x() const noexcept -> const_reference
{
  return m_data[1];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::y() noexcept -> reference
{
  return m_data[2];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::y() const noexcept -> const_reference
{
  return m_data[2];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::z() noexcept -> reference
{
  return m_data[3];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::z() const noexcept -> const_reference
{
  return m_data[3];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::data() noexcept -> pointer
{
  return &m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::data() const noexcept -> const_pointer
{
  return &m_data[0];
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::operator[](index_type i) noexcept -> reference
{
  return m_data[i];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::operator[](index_type i)
  const noexcept -> const_reference
{
  return m_data[i];
}

inline
auto alloy::core::quaternion::rotation_matrix() const noexcept -> matrix3
{
  auto mat = matrix3{};
  extract_rotation_matrix(&mat);

  return mat;
}

inline
auto alloy::core::quaternion::angle_axis()
  const noexcept -> std::tuple<radian,vector3>
{
  auto rad = radian{};
  auto vec = vector3{};

  extract_angle_axis(&rad,&vec);
  return std::make_tuple(rad,vec);
}

inline
auto alloy::core::quaternion::axes()
  const noexcept -> std::tuple<vector3, vector3, vector3>
{
  return std::make_tuple(x_axis(), y_axis(), z_axis());
}

//----------------------------------------------------------------------------

inline
auto alloy::core::quaternion::normalized() const noexcept -> quaternion
{
  return quaternion{*this}.normalize();
}

inline
auto alloy::core::quaternion::inverse() const noexcept -> quaternion
{
  return quaternion{*this}.invert();
}

inline
auto alloy::core::quaternion::conjugated() const noexcept -> quaternion
{
  return quaternion{*this}.conjugate();
}

//----------------------------------------------------------------------------
// Unary Operators
//----------------------------------------------------------------------------

inline
auto alloy::core::quaternion::operator+() const noexcept -> quaternion
{
  return (*this);
}

inline
auto alloy::core::quaternion::operator-() const noexcept -> quaternion
{
  return quaternion{ -w(), -x(), -y(), -z() };
}

//==============================================================================
// non-member functions : class : quaternion
//==============================================================================

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline
auto alloy::core::dot(const quaternion& lhs, const quaternion& rhs)
  noexcept -> real
{
  return lhs.dot(rhs);
}

inline
auto alloy::core::magnitude(const quaternion& x) noexcept -> real
{
  return x.magnitude();
}

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::operator+(const quaternion& lhs, const quaternion& rhs)
  noexcept -> quaternion
{
  return quaternion{lhs}+=rhs;
}

inline
auto alloy::core::operator-(const quaternion& lhs, const quaternion& rhs)
  noexcept -> quaternion
{
  return quaternion{lhs}-=rhs;
}

inline
auto alloy::core::operator*(const quaternion& lhs, const quaternion& rhs)
  noexcept -> quaternion
{
  return quaternion{lhs}*=rhs;
}

inline
auto alloy::core::operator/(const quaternion& lhs, const quaternion& rhs)
  noexcept -> quaternion
{
  return quaternion{lhs}/=rhs;
}

inline
auto alloy::core::operator*(const quaternion& lhs, quaternion::value_type rhs)
  noexcept -> quaternion
{
  return quaternion{lhs}*=rhs;
}

inline
auto alloy::core::operator*(quaternion::value_type lhs, const quaternion& rhs)
  noexcept -> quaternion
{
  return quaternion{rhs}*=lhs;
}

inline
auto alloy::core::operator/(const quaternion& lhs, quaternion::value_type rhs)
  noexcept -> quaternion
{
  return quaternion{lhs}/=rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline
auto alloy::core::operator==(const quaternion& lhs, const quaternion& rhs)
  noexcept -> bool
{
  for (auto i=0; i<4;++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline
auto alloy::core::operator!=(const quaternion& lhs, const quaternion& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline
auto alloy::core::almost_equal(const quaternion& lhs,
                               const quaternion& rhs,
                               real tolerance)
  noexcept -> bool
{
  for (auto i=0; i<4;++i) {
    if (!almost_equal(lhs[i],rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//==============================================================================
// struct : interpolator<quaternion>
//==============================================================================

inline
auto alloy::core::interpolator<alloy::core::quaternion>::operator()(
  const quaternion& v0,
  const quaternion& v1,
  real alpha
) noexcept -> quaternion
{
  return nlerp(v0, v1, alpha);
}

#endif /* ALLOY_CORE_MATH_QUATERNION_HPP */

