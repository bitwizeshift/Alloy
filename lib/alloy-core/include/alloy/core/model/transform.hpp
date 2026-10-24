// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file transform.hpp
///
/// \brief This header contains data for transformation objects
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_MODEL_TRANSFORM_HPP
#define ALLOY_CORE_MODEL_TRANSFORM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/math/angle/radian.hpp"
#include "alloy/core/math/euler_angles.hpp"
#include "alloy/core/math/interpolation.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/quaternion.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector3_constants.hpp"
#include "alloy/core/precision/real.hpp"
#include "alloy/core/utilities/not_null.hpp"

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An abstraction for wrapping transformations in 3-dimensional space
  ///
  /// This allows for a simple and convenient way to perform the following
  /// transformations:
  /// * translations
  /// * rotations
  /// * scaling
  ///
  /// The transform class does not actually own a matrix directly. Instead, it
  /// acts on the matrix passed into it.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API transform
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a transform translated at the origin with no
    ///        orientation
    transform() noexcept;

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    transform(const transform& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other transform to copy
    /// \return reference to (*this)
    auto operator=(const transform& other) noexcept -> transform& = default;

    //-------------------------------------------------------------------------
    // Relative Transformations
    //-------------------------------------------------------------------------
  public:

    /// \brief Translates this transform by \p delta_x, \p delta_y, \p delta_z
    ///
    /// \param delta_x the change in the x position
    /// \param delta_y the change in the y position
    /// \param delta_z the change in the z position
    auto translate(real delta_x, real delta_y, real delta_z) noexcept -> void;

    /// \brief Translates this transform by the vector \p delta
    ///
    /// \param delta the change in translation
    auto translate(const vector3& delta) noexcept -> void;

    /// \brief Translates this transform by \p delta in the x axis
    ///
    /// \param delta the change in translation for the x axis
    auto translate_x(real delta) noexcept -> void;

    /// \brief Translates this transform by \p delta in the y axis
    ///
    /// \param delta the change in translation for the y axis
    auto translate_y(real delta) noexcept -> void;

    /// \brief Translates this transform by \p delta in the z axis
    ///
    /// \param delta the change in translation for the z axis
    auto translate_z(real delta) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Rotates this transformation by the specified \p orientation
    ///
    /// The rotation is applied before the current orientation, so it rotates
    /// about the transform's own axes.
    ///
    /// \param orientation the orientation
    auto rotate(const quaternion& orientation) noexcept -> void;

    /// \brief Rotates this transformation by the specified euler \p angles
    ///
    /// \param angles the angles
    auto rotate(const euler_angles& angles) noexcept -> void;

    /// \brief Rotates this transformation around the specified \p axis by the
    ///        specified \p angle
    ///
    /// \param angle the angle to rotate
    /// \param axis the axis to rotate about
    auto rotate(radian angle, const vector3& axis) noexcept -> void;

    /// \brief Rotates the roll angle of this transform by \p angle
    ///
    /// \param angle the angle to rotate
    auto rotate_roll(radian angle) noexcept -> void;

    /// \brief Rotates the pitch angle of this transform by \p angle
    ///
    /// \param angle the angle to rotate
    auto rotate_pitch(radian angle) noexcept -> void;

    /// \brief Rotates the yaw angle of this transform by \p angle
    ///
    /// \param angle the angle to rotate
    auto rotate_yaw(radian angle) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Scales this transform by \p uniform in all directions
    ///
    /// \param uniform the direction to scale
    auto scale(real uniform) noexcept -> void;

    /// \brief Scales this transformation by \p scale
    ///
    /// \param scale the amount to scale
    auto scale(const vector3& scale) noexcept -> void;

    /// \brief Scales this transformation by \p scale in the x direction
    ///
    /// \param scale the amount to scale
    auto scale_x(real scale) noexcept -> void;

    /// \brief Scales this transformation by \p scale in the y direction
    ///
    /// \param scale the amount to scale
    auto scale_y(real scale) noexcept -> void;

    /// \brief Scales this transformation by \p scale in the z direction
    ///
    /// \param scale the amount to scale
    auto scale_z(real scale) noexcept -> void;

    //-------------------------------------------------------------------------
    // Absolute Transformations
    //-------------------------------------------------------------------------
  public:

    /// \brief Sets the translation of this transformation to \p x, \p y,
    ///        \p z
    ///
    /// \param x the x position
    /// \param y the y position
    /// \param z the z position
    auto set_translation(real x, real y, real z) noexcept -> void;

    /// \brief Sets the translation of this transform to \p translation
    ///
    /// \param translation the offset from the origin (0,0,0)
    auto set_translation(const vector3& translation) noexcept -> void;

    /// \brief Sets the \p x translation of this transformation
    ///
    /// \param x the x position
    auto set_translation_x(real x) noexcept -> void;

    /// \brief Sets the \p y translation of this transformation
    ///
    /// \param y the y position
    auto set_translation_y(real y) noexcept -> void;

    /// \brief Sets the \p z translation of this transformation
    ///
    /// \param z the z position
    auto set_translation_z(real z) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Sets the roll angle of this transformation
    ///
    /// \param angle the angle to set
    auto set_roll(radian angle) noexcept -> void;

    /// \brief Sets the pitch angle of this transformation
    ///
    /// \param angle the angle to set
    auto set_pitch(radian angle) noexcept -> void;

    /// \brief Sets the yaw angle of this transformation
    ///
    /// \param angle the angle to set
    auto set_yaw(radian angle) noexcept -> void;

    /// \brief Sets the orientation of this transformation from a quaternion
    ///
    /// \param orientation the orientation
    auto set_orientation(const quaternion& orientation) noexcept -> void;

    /// \brief Sets the orientation of this transformation from euler angles
    ///
    /// \param angles the angles
    auto set_orientation(const euler_angles& angles) noexcept -> void;

    /// \brief Sets the orientation of this transform to a rotation of
    ///        \p angle about \p axis
    ///
    /// \param angle the angle to rotate
    /// \param axis the axis to rotate about
    auto set_orientation(radian angle, const vector3& axis) noexcept -> void;

    /// \brief Orients this transform to face \p target
    ///
    /// The transform's forward direction points at \p target, with its up
    /// direction as close to \p up as possible.
    ///
    /// \pre \p target is not the translation of this transform
    /// \pre \p up is not parallel to the direction of \p target
    ///
    /// \param target the position to face
    /// \param up the direction for up
    auto look_at(
      const vector3& target,
      const vector3& up = vector3_constants::unit_y
    ) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Sets the uniform scale of this transformation
    ///
    /// \param uniform the uniform scale
    auto set_scale(real uniform) noexcept -> void;

    /// \brief Sets the scale of this transformation
    ///
    /// \param scale the scale to set
    auto set_scale(const vector3& scale) noexcept -> void;

    /// \brief Sets the x scale
    ///
    /// \param scale the x-scale
    auto set_scale_x(real scale) noexcept -> void;

    /// \brief Sets the y scale
    ///
    /// \param scale the y-scale
    auto set_scale_y(real scale) noexcept -> void;

    /// \brief Sets the z scale
    ///
    /// \param scale the z-scale
    auto set_scale_z(real scale) noexcept -> void;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Retrieves the translation of this transform
    [[nodiscard]]
    auto translation() const noexcept -> vector3;

    /// \brief Retrieves the orientation of this transform as a quaternion
    ///
    /// \return retrieves the rotation orientation of this transform
    [[nodiscard]]
    auto orientation() const noexcept -> quaternion;

    /// \brief Retrieves the orientation of this transform as euler_angles
    ///
    /// These are the angles that `set_orientation` composes the orientation
    /// from, so setting the angles and reading them back is a round trip.
    ///
    /// \return retrieves the rotation orientation of this transform
    [[nodiscard]]
    auto angles() const noexcept -> euler_angles;

    /// \brief Retrieve the scale of this transform
    ///
    /// \return the 3-component scale
    [[nodiscard]]
    auto scale() const noexcept -> vector3;

    //-------------------------------------------------------------------------

    /// \brief Retrieves the direction of this transform's x-axis
    ///
    /// \return the unit direction that the orientation maps the x-axis to
    [[nodiscard]]
    auto right() const noexcept -> vector3;

    /// \brief Retrieves the direction of this transform's y-axis
    ///
    /// \return the unit direction that the orientation maps the y-axis to
    [[nodiscard]]
    auto up() const noexcept -> vector3;

    /// \brief Retrieves the direction that this transform faces
    ///
    /// Following the convention of `quaternion::look_rotation`, a transform
    /// faces down its negative z-axis.
    ///
    /// \return the unit direction that the orientation maps the negative
    ///         z-axis to
    [[nodiscard]]
    auto forward() const noexcept -> vector3;

    //-------------------------------------------------------------------------
    // Transformation
    //-------------------------------------------------------------------------
  public:

    /// \brief Transforms the point \p point by this transform
    ///
    /// The point is scaled, then rotated, then translated, which is the same
    /// as multiplying it by the matrix of this transform.
    ///
    /// \param point the point to transform
    /// \return the transformed point
    [[nodiscard]]
    auto transform_point(const vector3& point) const noexcept -> vector3;

    /// \brief Transforms the vector \p vector by this transform
    ///
    /// The vector is scaled, then rotated, but not translated, since a vector
    /// has no position.
    ///
    /// \note Normals are not preserved by a non-uniform scale, and should be
    ///       transformed by the inverse transpose of the matrix instead.
    ///
    /// \param vector the vector to transform
    /// \return the transformed vector
    [[nodiscard]]
    auto transform_vector(const vector3& vector) const noexcept -> vector3;

    //-------------------------------------------------------------------------
    // Conversion
    //-------------------------------------------------------------------------
  public:

    /// \brief Extracts the transformation matrix from this transform
    ///
    /// \param out the matrix to output
    auto extract_matrix(not_null<matrix4*> out) const noexcept -> void;

    /// \brief Converts this transform to a matrix4
    ///
    /// \return the matrix4
    [[nodiscard]]
    auto to_matrix4() const noexcept -> matrix4;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    quaternion m_orientation;
    vector3 m_translation;
    vector3 m_scale;

    friend interpolator<transform>;
  };

  /// \brief Interpolates the state between \p t0 and \p t1
  ///
  /// \param t0 the start state
  /// \param t1 the end state
  /// \param alpha the state between t0 and t1
  /// \return the interpolated transform
  template <>
  struct ALLOY_CORE_API interpolator<transform>
  {
    auto operator()(const transform& v0, const transform& v1, real alpha)
      noexcept -> transform;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MODEL_TRANSFORM_HPP */
