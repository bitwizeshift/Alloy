////////////////////////////////////////////////////////////////////////////////
/// \file camera.hpp
///
/// \brief TODO(Bitwize): Add description
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

#ifndef ALLOY_CORE_MODEL_CAMERA_HPP
#define ALLOY_CORE_MODEL_CAMERA_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/math/quaternion.hpp"
#include "alloy/core/math/euler_angles.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector3_constants.hpp"

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An object for representing a viewer of a scene in 3-space
  ///
  /// The camera class acts as the 'view' transformation wrapper to move
  /// a model from world coordinates to camera coordinates.
  ///
  /// A camera may only be rotated or translated, but may not be scaled.
  /// All translations
  ///
  /// TODO(bitwize): Optimize redundant computations by storing the rotated
  ///                axes. Currently the axes is computed as part of look_at,
  ///                which then gets compressed into a quaternion, but
  ///                extracted again later when converted to a matrix.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API camera
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a camera at the origin with no explicit orientation
    camera() noexcept = default;

    /// \brief Constructs a camera from copying \p other
    ///
    /// \param other the other camera to copy
    camera(const camera& other) noexcept = default;

    /// \brief Copies a camera from copying \p other
    ///
    /// \param other the other camera to copy
    /// \return reference to `(*this)`
    auto operator=(const camera& other) noexcept -> camera& = default;

    //-------------------------------------------------------------------------
    // Relative Transformations
    //-------------------------------------------------------------------------
  public:

    /// \brief Translates this camera by \p delta_x, \p delta_y, \p delta_z
    ///
    /// \param delta_x the change in the x position
    /// \param delta_y the change in the y position
    /// \param delta_z the change in the z position
    auto translate(real delta_x, real delta_y, real delta_z) noexcept -> void;

    /// \brief Translates this camera by the vector \p delta
    ///
    /// \param delta the change in translation
    auto translate(const vector3& delta) noexcept -> void;

    /// \brief Translates this camera by \p delta in the x axis
    ///
    /// \param delta the change in translation for the x axis
    auto translate_x(real delta) noexcept -> void;

    /// \brief Translates this camera by \p delta in the y axis
    ///
    /// \param delta the change in translation for the y axis
    auto translate_y(real delta) noexcept -> void;

    /// \brief Translates this camera by \p delta in the z axis
    ///
    /// \param delta the change in translation for the z axis
    auto translate_z(real delta) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Rotates this camera by the specified \p orientation
    ///
    /// \param orientation the orientation
    auto rotate(const quaternion& orientation) noexcept -> void;

    /// \brief Rotates this camera by the specified euler \p angles
    ///
    /// \param angles the angles
    auto rotate(const euler_angles& angles) noexcept -> void;

    /// \brief Rotates this camera around the specified \p axis by the
    ///        specified \p angle
    ///
    /// \param angle the angle to rotate
    /// \param axis the axis to rotate about
    auto rotate_around_axis(radian angle, const vector3& axis) noexcept -> void;

    /// \brief Rotates the roll angle of this camera by \p angle
    ///
    /// \param angle the angle to rotate
    auto rotate_roll(radian angle) noexcept -> void;

    /// \brief Rotates the pitch angle of this camera by \p angle
    ///
    /// \param angle the angle to rotate
    auto rotate_pitch(radian angle) noexcept -> void;

    /// \brief Rotates the yaw angle of this camera by \p angle
    ///
    /// \param angle the angle to rotate
    auto rotate_yaw(radian angle) noexcept -> void;

    //-------------------------------------------------------------------------
    // Absolute Transformations
    //-------------------------------------------------------------------------
  public:

    /// \brief Sets the origin of the camera to \p x, \p y, \p z
    ///
    /// \param x the x position
    /// \param y the y position
    /// \param z the z position
    auto set_origin(real x, real y, real z) noexcept -> void;

    /// \brief Sets the origin of this camera from \p offset
    ///
    /// \param offset the amount to offset from the proper origin (0,0,0)
    auto set_origin(const vector3& offset) noexcept -> void;

    /// \brief Sets the \p x translation of this camera
    ///
    /// \param x the x position
    auto set_x_translation(real x) noexcept -> void;

    /// \brief Sets the \p y translation of this camera
    ///
    /// \param y the y position
    auto set_y_translation(real y) noexcept -> void;

    /// \brief Sets the \p z translation of this camera
    ///
    /// \param z the z position
    auto set_z_translation(real z) noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Sets the roll angle of this camera
    ///
    /// \param angle the angle to set
    auto set_roll(radian angle) noexcept -> void;

    /// \brief Sets the pitch angle of this camera
    ///
    /// \param angle the angle to set
    auto set_pitch(radian angle) noexcept -> void;

    /// \brief Sets the yaw angle of this camera
    ///
    /// \param angle the angle to set
    auto set_yaw(radian angle) noexcept -> void;

    /// \brief Sets the orientation of this camera from a quaternion
    ///
    /// \param orientation the orientation
    auto set_orientation(const quaternion& orientation) noexcept -> void;

    /// \brief Sets the orientation of this camera from euler angles
    ///
    /// \param angles the angles
    auto set_orientation(const euler_angles& angles) noexcept -> void;

    /// \brief Looks at the specified \p subject
    ///
    /// \param subject the subject to look at
    /// \param up the direction for up
    auto look_at(const vector3& subject,
                 const vector3& up = vector3_constants::unit_y) noexcept -> void;

    /// \brief Looks at the specified \p subject
    ///
    /// \param x the x position
    /// \param y the y position
    /// \param z the z position
    /// \param up the direction for up
    auto look_at(real x, real y, real z,
                 const vector3& up = vector3_constants::unit_y) noexcept -> void;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Retrieves the translation of this camera
    ///
    /// \return the translation of this camera
    [[nodiscard]]
    auto translation() const noexcept -> vector3;

    /// \brief Retrieves the orientation of this camera as a quaternion
    ///
    /// \return the rotation orientation of this camera
    [[nodiscard]]
    auto orientation() const noexcept -> quaternion;

    /// \brief Retrieves the orientation of this camera as euler_angles
    ///
    /// \return the euler angles of the orientation of this camera
    [[nodiscard]]
    auto angles() const noexcept -> euler_angles;

    //-------------------------------------------------------------------------
    // Conversion
    //-------------------------------------------------------------------------
  public:

    /// \brief Extracts the camera matrix from this camera
    ///
    /// \param out the matrix to output
    auto extract_matrix(not_null<matrix4*> out) const noexcept -> void;

    /// \brief Converts this camera to a matrix4
    ///
    /// \return the matrix4
    [[nodiscard]]
    auto to_matrix4() const noexcept -> matrix4;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    vector3 m_translation;
    quaternion m_orientation;
  };

} // namespace alloy::core

#endif /* ALLOY_CORE_MODEL_CAMERA_HPP */
