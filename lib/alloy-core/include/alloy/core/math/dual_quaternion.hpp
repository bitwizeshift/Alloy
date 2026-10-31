// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file dual_quaternion.hpp
///
/// \brief This header contains the dual quaternion, which represents rigid
///        transformations in 3 dimensions
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MATH_DUAL_QUATERNION_HPP
#define ALLOY_CORE_MATH_DUAL_QUATERNION_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/interpolation.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/quaternion.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/utilities/not_null.hpp"

namespace alloy::core {

  //============================================================================
  // class : dual_quaternion
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A dual quaternion, used to represent a rigid transformation in 3
  ///        dimensions
  ///
  /// A dual quaternion is the sum `r + εd` of a real part `r` and a dual part
  /// `d`, which are each quaternions, and where ε^2 = 0.
  ///
  /// A unit dual quaternion, whose real part is a unit quaternion and is
  /// orthogonal to its dual part, is a rotation followed by a translation.
  /// The real part is the rotation, and the dual part is half of the
  /// translation multiplied by the rotation.
  ///
  /// Products follow the same conventions as \ref quaternion, so the product
  /// `a * b` transforms by `b` first and then by `a`.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API dual_quaternion
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type = float; ///< The underlying value type

    //--------------------------------------------------------------------------
    // Static Factory Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a dual quaternion that only rotates by \p rotation
    ///
    /// \param rotation the rotation
    /// \return the constructed dual quaternion
    static auto from_rotation(const quaternion& rotation)
      noexcept -> dual_quaternion;

    /// \brief Constructs a dual quaternion that only translates by
    ///        \p translation
    ///
    /// \param translation the translation
    /// \return the constructed dual quaternion
    static auto from_translation(const vector3& translation)
      noexcept -> dual_quaternion;

    /// \brief Constructs a dual quaternion that rotates by \p rotation, and
    ///        then translates by \p translation
    ///
    /// \pre \p rotation is normalized
    ///
    /// \param rotation the rotation
    /// \param translation the translation
    /// \return the constructed dual quaternion
    static auto from_rotation_translation(const quaternion& rotation,
                                          const vector3& translation)
      noexcept -> dual_quaternion;

    /// \brief Constructs a dual quaternion from a 4x4 rigid transformation
    ///        matrix
    ///
    /// \pre \p transform only rotates and translates
    ///
    /// \param transform the transformation matrix
    /// \return the constructed dual quaternion
    static auto from_transform_matrix(const matrix4& transform)
      noexcept -> dual_quaternion;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a dual quaternion that does not transform
    constexpr dual_quaternion() noexcept;

    /// \brief Constructs a dual quaternion from its \p real_part and
    ///        \p dual_part
    ///
    /// \param real_part the real part
    /// \param dual_part the dual part
    constexpr dual_quaternion(const quaternion& real_part,
                              const quaternion& dual_part) noexcept;

    /// \brief Copy-constructs a dual quaternion from another dual quaternion
    ///
    /// \param other the other dual quaternion to copy
    constexpr dual_quaternion(const dual_quaternion& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the dual quaternion
    ///
    /// \param other the dual quaternion to copy
    /// \return reference to `(*this)`
    auto operator=(const dual_quaternion& other)
      noexcept -> dual_quaternion& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the real part of this dual quaternion
    ///
    /// \return reference to the real part
    constexpr auto real() noexcept -> quaternion&;
    constexpr auto real() const noexcept -> const quaternion&;
    /// \}

    /// \{
    /// \brief Gets the dual part of this dual quaternion
    ///
    /// \return reference to the dual part
    constexpr auto dual() noexcept -> quaternion&;
    constexpr auto dual() const noexcept -> const quaternion&;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Gets the rotation of this dual quaternion
    ///
    /// \pre this dual quaternion is normalized
    ///
    /// \return the rotation
    constexpr auto rotation() const noexcept -> const quaternion&;

    /// \brief Calculates the translation of this dual quaternion
    ///
    /// \pre this dual quaternion is normalized
    ///
    /// \return the translation
    auto translation() const noexcept -> vector3;

    /// \brief Calculates and returns the 4x4 transformation matrix
    ///
    /// The matrix transforms column vectors, so `transform_matrix() * p`
    /// transforms the point `p` the same as `transform_point(p)`.
    ///
    /// \pre this dual quaternion is normalized
    ///
    /// \return the transformation matrix
    auto transform_matrix() const noexcept -> matrix4;

    //--------------------------------------------------------------------------

    /// \brief Gets the normalized dual quaternion of `this`
    ///
    /// \return the normalized dual quaternion of `this`
    auto normalized() const noexcept -> dual_quaternion;

    /// \brief Gets the inverse of `this` dual quaternion
    ///
    /// The inverse is the dual quaternion whose product with `this` is the
    /// identity. A dual quaternion with a zero real part has no inverse, and
    /// inverts to zero.
    ///
    /// \return the inverse of `this` dual quaternion
    auto inverse() const noexcept -> dual_quaternion;

    /// \brief Gets the conjugate of `this` dual quaternion
    ///
    /// The conjugate is the quaternion conjugate of both parts. For a unit
    /// dual quaternion, it is the inverse transformation, and is cheaper to
    /// compute than \ref inverse.
    ///
    /// \return the conjugate of `this` dual quaternion
    auto conjugated() const noexcept -> dual_quaternion;

    //--------------------------------------------------------------------------
    // Extraction
    //--------------------------------------------------------------------------
  public:

    /// \brief Extracts a 4x4 transformation matrix from this dual quaternion
    ///
    /// \pre this dual quaternion is normalized
    ///
    /// \param transform pointer to the transformation matrix to extract into
    auto extract_transform_matrix(not_null<matrix4*> transform)
      const noexcept -> void;

    //--------------------------------------------------------------------------
    // Transformations
    //--------------------------------------------------------------------------
  public:

    /// \brief Transforms the point \p point by this dual quaternion
    ///
    /// The point is rotated, and then translated.
    ///
    /// \pre this dual quaternion is normalized
    ///
    /// \param point the point to transform
    /// \return the transformed point
    auto transform_point(const vector3& point) const noexcept -> vector3;

    /// \brief Transforms the vector \p vector by this dual quaternion
    ///
    /// A vector is a direction, and so is only rotated.
    ///
    /// \pre this dual quaternion is normalized
    ///
    /// \param vector the vector to transform
    /// \return the transformed vector
    auto transform_vector(const vector3& vector) const noexcept -> vector3;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this dual quaternion and returns a reference to
    ///        `(*this)`
    ///
    /// The real part is scaled to unit length, and the dual part is made
    /// orthogonal to it, which corrects the drift that accumulates over many
    /// products. A dual quaternion with a zero real part is left unchanged.
    ///
    /// \return the reference to `(*this)`
    auto normalize() noexcept -> dual_quaternion&;

    /// \brief Inverts this dual quaternion and returns a reference to
    ///        `(*this)`
    ///
    /// A dual quaternion with a zero real part has no inverse, and is set to
    /// zero.
    ///
    /// \return the reference to `(*this)`
    auto invert() noexcept -> dual_quaternion&;

    /// \brief Conjugates this dual quaternion and returns a reference to
    ///        `(*this)`
    ///
    /// \return the reference to `(*this)`
    auto conjugate() noexcept -> dual_quaternion&;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    auto operator+() const noexcept -> dual_quaternion;
    auto operator-() const noexcept -> dual_quaternion;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    auto operator+=(const dual_quaternion& rhs) noexcept -> dual_quaternion&;
    auto operator-=(const dual_quaternion& rhs) noexcept -> dual_quaternion&;
    auto operator*=(const dual_quaternion& rhs) noexcept -> dual_quaternion&;
    auto operator*=(value_type rhs) noexcept -> dual_quaternion&;
    auto operator/=(value_type rhs) noexcept -> dual_quaternion&;
    auto operator/=(const dual_quaternion& rhs) noexcept -> dual_quaternion&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    quaternion m_real; ///< The real part
    quaternion m_dual; ///< The dual part
  };

  //============================================================================
  // non-member functions : class : dual_quaternion
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operations
  //----------------------------------------------------------------------------

  auto operator+(const dual_quaternion& lhs, const dual_quaternion& rhs)
    noexcept -> dual_quaternion;
  auto operator-(const dual_quaternion& lhs, const dual_quaternion& rhs)
    noexcept -> dual_quaternion;

  /// \brief Computes the product of \p lhs and \p rhs
  ///
  /// As a transformation, the product transforms by \p rhs first and then by
  /// \p lhs.
  ///
  /// \param lhs the left dual quaternion
  /// \param rhs the right dual quaternion
  /// \return the product `lhs * rhs`
  auto operator*(const dual_quaternion& lhs, const dual_quaternion& rhs)
    noexcept -> dual_quaternion;

  /// \brief Multiplies \p lhs by the inverse of \p rhs
  ///
  /// \param lhs the left dual quaternion
  /// \param rhs the right dual quaternion
  /// \return the product `lhs * rhs.inverse()`
  auto operator/(const dual_quaternion& lhs, const dual_quaternion& rhs)
    noexcept -> dual_quaternion;

  auto operator*(const dual_quaternion& lhs, float rhs)
    noexcept -> dual_quaternion;
  auto operator*(float lhs, const dual_quaternion& rhs)
    noexcept -> dual_quaternion;
  auto operator/(const dual_quaternion& lhs, float rhs)
    noexcept -> dual_quaternion;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  ALLOY_CORE_API
  auto operator==(const dual_quaternion& lhs, const dual_quaternion& rhs)
    noexcept -> bool;
  auto operator!=(const dual_quaternion& lhs, const dual_quaternion& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines whether two dual quaternions are almost equal, relative
  ///        to \p tolerance
  ///
  /// \param lhs the left dual quaternion
  /// \param rhs the right dual quaternion
  /// \param tolerance the tolerance of each component
  /// \return `true` if \p lhs almost equals \p rhs
  ALLOY_CORE_API
  auto almost_equal(
    const dual_quaternion& lhs,
    const dual_quaternion& rhs,
    float tolerance
  ) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Interpolation
  //----------------------------------------------------------------------------

  /// \brief Interpolates between \p q0 and \p q1 by normalizing their
  ///        linear interpolation
  ///
  /// This is dual quaternion linear blending. The interpolation takes the
  /// shortest path between the two rotations, and always produces a rigid
  /// transformation. It is cheaper than \ref sclerp, but does not move at a
  /// constant velocity.
  ///
  /// \pre \p q0 and \p q1 are normalized
  ///
  /// \param q0 the start transformation
  /// \param q1 the end transformation
  /// \param alpha the amount to interpolate [0, 1]
  /// \return the normalized interpolated transformation
  ALLOY_CORE_API
  auto nlerp(const dual_quaternion& q0, const dual_quaternion& q1, float alpha)
    noexcept -> dual_quaternion;

  /// \brief Interpolates between \p q0 and \p q1 along the screw motion
  ///        between them
  ///
  /// The screw motion rotates about, and translates along, a single axis.
  /// The interpolation takes the shortest path between the two rotations,
  /// at a constant angular and linear velocity.
  ///
  /// \pre \p q0 and \p q1 are normalized
  ///
  /// \param q0 the start transformation
  /// \param q1 the end transformation
  /// \param alpha the amount to interpolate [0, 1]
  /// \return the interpolated transformation
  ALLOY_CORE_API
  auto sclerp(const dual_quaternion& q0, const dual_quaternion& q1, float alpha)
    noexcept -> dual_quaternion;

  //============================================================================
  // struct : interpolator<dual_quaternion>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Interpolates rigid transformations with \ref nlerp
  ///
  /// A component-wise linear interpolation of two unit dual quaternions is
  /// not itself a unit dual quaternion, and so is not a rigid
  /// transformation.
  //////////////////////////////////////////////////////////////////////////////
  template <>
  struct interpolator<dual_quaternion>
  {
    auto operator()(
      const dual_quaternion& v0,
      const dual_quaternion& v1,
      float alpha
    ) noexcept -> dual_quaternion;
  };

} // namespace alloy::core

//==============================================================================
// definitions : class : dual_quaternion
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

inline
auto alloy::core::dual_quaternion::from_rotation(const quaternion& rotation)
  noexcept -> dual_quaternion
{
  return dual_quaternion{
    rotation,
    quaternion{value_type{0}, value_type{0}, value_type{0}, value_type{0}}
  };
}

inline
auto alloy::core::dual_quaternion::from_translation(const vector3& translation)
  noexcept -> dual_quaternion
{
  const auto half = translation * value_type{0.5};

  return dual_quaternion{
    quaternion{value_type{1}, value_type{0}, value_type{0}, value_type{0}},
    quaternion{value_type{0}, half.x(), half.y(), half.z()}
  };
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
alloy::core::dual_quaternion::dual_quaternion() noexcept
  : m_real{value_type{1}, value_type{0}, value_type{0}, value_type{0}},
    m_dual{value_type{0}, value_type{0}, value_type{0}, value_type{0}}
{

}

ALLOY_FORCE_INLINE constexpr
alloy::core::dual_quaternion::dual_quaternion(
  const quaternion& real_part,
  const quaternion& dual_part
) noexcept
  : m_real{real_part},
    m_dual{dual_part}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::dual_quaternion::real() noexcept -> quaternion&
{
  return m_real;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::dual_quaternion::real() const noexcept -> const quaternion&
{
  return m_real;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::dual_quaternion::dual() noexcept -> quaternion&
{
  return m_dual;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::dual_quaternion::dual() const noexcept -> const quaternion&
{
  return m_dual;
}

//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::dual_quaternion::rotation()
  const noexcept -> const quaternion&
{
  return m_real;
}

inline
auto alloy::core::dual_quaternion::transform_matrix() const noexcept -> matrix4
{
  auto mat = matrix4{};
  extract_transform_matrix(&mat);

  return mat;
}

//------------------------------------------------------------------------------

inline
auto alloy::core::dual_quaternion::normalized()
  const noexcept -> dual_quaternion
{
  return dual_quaternion{*this}.normalize();
}

inline
auto alloy::core::dual_quaternion::inverse() const noexcept -> dual_quaternion
{
  return dual_quaternion{*this}.invert();
}

inline
auto alloy::core::dual_quaternion::conjugated()
  const noexcept -> dual_quaternion
{
  return dual_quaternion{*this}.conjugate();
}

//------------------------------------------------------------------------------
// Transformations
//------------------------------------------------------------------------------

inline
auto alloy::core::dual_quaternion::transform_point(const vector3& point)
  const noexcept -> vector3
{
  return (m_real * point) + translation();
}

inline
auto alloy::core::dual_quaternion::transform_vector(const vector3& vector)
  const noexcept -> vector3
{
  return m_real * vector;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline
auto alloy::core::dual_quaternion::conjugate() noexcept -> dual_quaternion&
{
  m_real.conjugate();
  m_dual.conjugate();
  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::dual_quaternion::operator+() const noexcept -> dual_quaternion
{
  return (*this);
}

inline
auto alloy::core::dual_quaternion::operator-() const noexcept -> dual_quaternion
{
  return dual_quaternion{-m_real, -m_dual};
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::dual_quaternion::operator+=(const dual_quaternion& rhs)
  noexcept -> dual_quaternion&
{
  m_real += rhs.m_real;
  m_dual += rhs.m_dual;
  return (*this);
}

inline
auto alloy::core::dual_quaternion::operator-=(const dual_quaternion& rhs)
  noexcept -> dual_quaternion&
{
  m_real -= rhs.m_real;
  m_dual -= rhs.m_dual;
  return (*this);
}

inline
auto alloy::core::dual_quaternion::operator*=(value_type rhs)
  noexcept -> dual_quaternion&
{
  m_real *= rhs;
  m_dual *= rhs;
  return (*this);
}

inline
auto alloy::core::dual_quaternion::operator/=(value_type rhs)
  noexcept -> dual_quaternion&
{
  return (*this) *= (value_type{1} / rhs);
}

inline
auto alloy::core::dual_quaternion::operator/=(const dual_quaternion& rhs)
  noexcept -> dual_quaternion&
{
  return (*this) *= rhs.inverse();
}

//==============================================================================
// non-member functions : class : dual_quaternion
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::operator+(
  const dual_quaternion& lhs,
  const dual_quaternion& rhs
) noexcept -> dual_quaternion
{
  return dual_quaternion{lhs} += rhs;
}

inline
auto alloy::core::operator-(
  const dual_quaternion& lhs,
  const dual_quaternion& rhs
) noexcept -> dual_quaternion
{
  return dual_quaternion{lhs} -= rhs;
}

inline
auto alloy::core::operator*(
  const dual_quaternion& lhs,
  const dual_quaternion& rhs
) noexcept -> dual_quaternion
{
  return dual_quaternion{lhs} *= rhs;
}

inline
auto alloy::core::operator/(
  const dual_quaternion& lhs,
  const dual_quaternion& rhs
) noexcept -> dual_quaternion
{
  return dual_quaternion{lhs} /= rhs;
}

inline
auto alloy::core::operator*(const dual_quaternion& lhs, float rhs)
  noexcept -> dual_quaternion
{
  return dual_quaternion{lhs} *= rhs;
}

inline
auto alloy::core::operator*(float lhs, const dual_quaternion& rhs)
  noexcept -> dual_quaternion
{
  return dual_quaternion{rhs} *= lhs;
}

inline
auto alloy::core::operator/(const dual_quaternion& lhs, float rhs)
  noexcept -> dual_quaternion
{
  return dual_quaternion{lhs} /= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::operator!=(
  const dual_quaternion& lhs,
  const dual_quaternion& rhs
) noexcept -> bool
{
  return !(lhs == rhs);
}

//==============================================================================
// struct : interpolator<dual_quaternion>
//==============================================================================

inline
auto alloy::core::interpolator<alloy::core::dual_quaternion>::operator()(
  const dual_quaternion& v0,
  const dual_quaternion& v1,
  float alpha
) noexcept -> dual_quaternion
{
  return nlerp(v0, v1, alpha);
}

#endif /* ALLOY_CORE_MATH_DUAL_QUATERNION_HPP */
