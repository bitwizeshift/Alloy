////////////////////////////////////////////////////////////////////////////////
/// \file transform.hpp
///
/// \brief This header contains data for transformation objects
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_MODEL_TRANSFORM_HPP
#define ALLOY_CORE_MODEL_TRANSFORM_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/math/quaternion.hpp"
#include "alloy/core/math/euler_angles.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/interpolation.hpp"

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
    auto rotate_around_axis(radian angle, const vector3& axis) noexcept -> void;

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

    /// \brief Sets the origin of the transformation to \p x, \p y, \p z
    ///
    /// \param x the x position
    /// \param y the y position
    /// \param z the z position
    auto set_origin(real x, real y, real z) noexcept -> void;

    /// \brief Sets the origin of this transform from \p offset
    ///
    /// \param offset the amount to offset from the proper origin (0,0,0)
    auto set_origin(const vector3& offset) noexcept -> void;

    /// \brief Sets the \p x translation of this transformation
    ///
    /// \param x the x position
    auto set_x_translation(real x) noexcept -> void;

    /// \brief Sets the \p y translation of this transformation
    ///
    /// \param y the y position
    auto set_y_translation(real y) noexcept -> void;

    /// \brief Sets the \p z translation of this transformation
    ///
    /// \param z the z position
    auto set_z_translation(real z) noexcept -> void;

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

    /// \brief Sets the orientation of this transform by the angle axis
    ///
    /// \param angle the angle to rotate
    /// \param axis the axis to rotate about
    auto set_angle_axis(radian angle, const vector3& axis) noexcept -> void;

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
    auto set_x_scale(real scale) noexcept -> void;

    /// \brief Sets the y scale
    ///
    /// \param scale the y-scale
    auto set_y_scale(real scale) noexcept -> void;

    /// \brief Sets the z scale
    ///
    /// \param scale the z-scale
    auto set_z_scale(real scale) noexcept -> void;

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
    /// \return retrieves the rotation orientation of this transform
    [[nodiscard]]
    auto angles() const noexcept -> euler_angles;

    /// \brief Retrieve the scale of this transform
    ///
    /// \return the 3-component scale
    [[nodiscard]]
    auto scale() const noexcept -> vector3;

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

    quaternion m_rotation;
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
  struct interpolator<transform>
  {
    auto operator()(const transform& v0, const transform& v1, real alpha)
      noexcept -> transform;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MODEL_TRANSFORM_HPP */
