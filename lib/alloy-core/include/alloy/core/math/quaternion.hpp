/*****************************************************************************
 * \file quaternion.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef ALLOY_CORE_MATH_QUATERNION_HPP
#define ALLOY_CORE_MATH_QUATERNION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"                 // ALLOY_CORE_API
#include "alloy/core/intrinsics.hpp"          // ALLOY_FORCE_INLINE
#include "alloy/core/precision.hpp"           // core::real
#include "alloy/core/math/vector/vector3.hpp" // vector3
#include "alloy/core/math/matrix/matrix3.hpp" // matrix3
#include "alloy/core/math/matrix/matrix4.hpp" // matrix4
#include "alloy/core/math/angle/radian.hpp"   // radian
#include "alloy/core/math/euler_angles.hpp"   // euler_angles
#include "alloy/core/math/math.hpp"           // almost_equal
#include "alloy/core/utilities/not_null.hpp"  // not_null

#include <cstddef> // std::size_t, std::ptrdiff_t
#include <tuple>   // std::tuple

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
  /// \brief A quaternion is a mathematical construct representing
  ///        4-dimensions of a rotation about the x,y,z axis.
  ///
  /// A quaternion is conventionally represented as q = {x,y,z,w}, where
  /// x,y, and z represent rotations around the respective axes
  /// (representing roll, pitch, and yaw respectively).
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
    /// \param x_axis the vector representing the x-axis
    /// \param y_axis the vector representing the y-axis
    /// \param z_axis the vector representing the z-axis
    /// \return the constructed quaternion
    static auto from_rotation_axes(const vector3& x_axis,
                                   const vector3& y_axis,
                                   const vector3& z_axis)
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
    /// \return reference to \c (*this)
    auto operator=(const quaternion& other) noexcept -> quaternion& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector2
    ///
    /// \return the number of components in the vector2
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
    /// \return the x-axis
    auto x_axis() const noexcept -> vector3;

    /// \brief Retrieves the y-axis from this quaternion
    ///
    /// \return the y-axis
    auto y_axis() const noexcept -> vector3;

    /// \brief Retrieves the z-axis from this quaternion
    ///
    /// \return the z-axis
    auto z_axis() const noexcept -> vector3;

    //--------------------------------------------------------------------------

    /// \brief Calculates and returns the 3x3 rotation matrix
    ///
    /// \return the rotation matrix
    auto rotation_matrix() const noexcept -> matrix3;

    /// \brief Calculates and returns the rotation angle and axis
    ///
    /// \return a tuple containing the angle and the axis
    auto angle_axis() const noexcept -> std::tuple<radian,vector3>;

    /// \brief Calculates and returns the x, y, and z axss as vectors
    ///
    /// \return a tuple containing the x, y, and z vector axes
    auto axes() const noexcept -> std::tuple<vector3,vector3,vector3>;

    //--------------------------------------------------------------------------

    /// \brief Gets the normalized quaternion of \c this
    ///
    /// \return the normalized quaternion of \c this
    auto normalized() const noexcept -> quaternion;

    /// \brief Gets the inverse of \c this quaternion
    ///
    /// \return the inverse of \c this quaternion
    auto inverse() const noexcept -> quaternion;

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
    /// \param angle the angle to extract into
    /// \param axis the vector to extract into
    auto extract_angle_axis(not_null<radian*> angle,
                            not_null<vector3*> axis) const noexcept -> void;

    //--------------------------------------------------------------------------

    /// \brief Extracts the x, y, and z axes
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

    /// \brief Normalizes this quaternion and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    auto normalize() noexcept -> quaternion&;

    /// \brief Inverts this quaternion and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    auto invert() noexcept -> quaternion&;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Computes the dot-product of \p rhs and \c this
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
    /// \param reproject tag used for tag dispatch
    /// \return the roll angle
    auto roll(reproject_axis_t reproject) const noexcept -> radian;

    /// \brief Computes the roll angle by reprojecting the axis
    ///
    /// \return the roll angle
    auto roll() const noexcept -> radian;

    //--------------------------------------------------------------------------

    /// \brief Computes the pitch angle by reprojecting the axis
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the pitch angle
    auto pitch(reproject_axis_t reproject) const noexcept -> radian;

    /// \brief Computes the pitch angle by reprojecting the axis
    ///
    /// \return the pitch angle
    auto pitch() const noexcept -> radian;

    //--------------------------------------------------------------------------

    /// \brief Computes the yaw angle by reprojecting the axis
    ///
    /// \param reproject tag used for tag dispatch
    /// \return the yaw angle
    auto yaw(reproject_axis_t reproject) const noexcept -> radian;

    /// \brief Computes the yaw angle by reprojecting the axis
    ///
    /// \return the yaw angle
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
  auto operator*(const quaternion& lhs, const quaternion& rhs)
    noexcept -> quaternion;
  auto operator/(const quaternion& lhs, const quaternion& rhs)
    noexcept -> quaternion;
  auto operator*(const quaternion& lhs, real rhs) noexcept -> quaternion;
  auto operator*(real lhs, const quaternion& rhs) noexcept -> quaternion;
  auto operator*(const quaternion& lhs, const vector3& rhs) noexcept -> vector3;
  auto operator/(const quaternion& lhs, real rhs) noexcept -> quaternion;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  auto operator==(const quaternion& lhs, const quaternion& rhs) noexcept -> bool;
  auto operator!=(const quaternion& lhs, const quaternion& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines whether two quaternions are almost equal, v to
  ///        \p tolerance
  ///
  /// \param lhs the left quaternion
  /// \param rhs the right quaternion
  /// \return \c true if \p lhs almost equals \p rhs
  auto almost_equal(const quaternion& lhs,
                    const quaternion& rhs,
                    real tolerance) noexcept -> bool;

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
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref quaternion
  ///
  /// The result is aliased as \c ::value
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
alloy::core::quaternion::quaternion()
  noexcept
  : quaternion{
    real{1},
    real{0},
    real{0},
    real{0}
  }
{

}

ALLOY_FORCE_INLINE constexpr
alloy::core::quaternion::quaternion(value_type w)
  noexcept
  : quaternion{w,0,0,0}
{

}

ALLOY_FORCE_INLINE constexpr
alloy::core::quaternion::quaternion(value_type w,
                                    value_type x,
                                    value_type y,
                                    value_type z)
  noexcept
  : m_data{w,x,y,z}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::size()
  const noexcept -> size_type
{
  return 4;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::w()
  noexcept -> reference
{
  return m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::w()
  const noexcept -> const_reference
{
  return m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::x()
  noexcept -> reference
{
  return m_data[1];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::x()
  const noexcept -> const_reference
{
  return m_data[1];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::y()
  noexcept -> reference
{
  return m_data[2];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::y()
  const noexcept -> const_reference
{
  return m_data[2];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::z()
  noexcept -> reference
{
  return m_data[3];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::z()
  const noexcept -> const_reference
{
  return m_data[3];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::data()
  noexcept -> pointer
{
  return &m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::data()
  const noexcept -> const_pointer
{
  return &m_data[0];
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::quaternion::operator[](index_type i)
  noexcept -> reference
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
auto alloy::core::quaternion::rotation_matrix()
  const noexcept -> matrix3
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
auto alloy::core::quaternion::normalized()
  const noexcept -> quaternion
{
  return quaternion{*this}.normalize();
}

inline
auto alloy::core::quaternion::inverse()
  const noexcept -> quaternion
{
  return quaternion{*this}.invert();
}

//----------------------------------------------------------------------------
// Unary Operators
//----------------------------------------------------------------------------

inline
auto alloy::core::quaternion::operator+()
  const noexcept -> quaternion
{
  return (*this);
}

inline
auto alloy::core::quaternion::operator-()
  const noexcept -> quaternion
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
auto alloy::core::magnitude(const quaternion& x)
  noexcept -> real
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

#endif /* ALLOY_CORE_MATH_QUATERNION_HPP */
