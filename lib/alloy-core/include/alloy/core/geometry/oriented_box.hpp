// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file oriented_box.hpp
 *
 * \brief This header contains an implementation of a 3d box geometric object
 *        that may be rotated to any orientation
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_ORIENTED_BOX_HPP
#define ALLOY_CORE_GEOMETRY_ORIENTED_BOX_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/point/point3.hpp"

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/basis/basis3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector3_constants.hpp"
#include "alloy/core/precision.hpp"

#include <array> // std::array

namespace alloy::core {

  //============================================================================
  // class : oriented_box
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A geometric primitive of a box that may be rotated to any
  ///        orientation
  ///
  /// The box is centered on a point, and extends by its half-extents along
  /// the axes of its orientation in both directions. The x, y, and z
  /// half-extents are measured along the `right`, `up`, and `forward` axes of
  /// the orientation respectively.
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API oriented_box
  {
    //--------------------------------------------------------------------------
    // Static Factory Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an oriented box that covers the same space as the
    ///        axis-aligned box \p box
    ///
    /// \param box the axis-aligned box
    /// \return the oriented box
    static auto from_axis_aligned_box(const axis_aligned_box& box)
      noexcept -> oriented_box;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs an oriented box at the origin with no size,
    ///        aligned to the x, y, and z axes
    constexpr oriented_box() noexcept;

    /// \brief Constructs an oriented box centered at \p center, extending by
    ///        \p half_extents along the axes of \p orientation
    ///
    /// \pre every component of \p half_extents is not negative
    ///
    /// \param center the center of the box
    /// \param half_extents the distance from the center to each face
    /// \param orientation the axes that the box is aligned to
    constexpr oriented_box(
      const point3& center,
      const vector3& half_extents,
      const basis3& orientation
    ) noexcept;

    /// \brief Constructs an oriented box by copying an existing instance
    ///
    /// \param other the other oriented box to copy
    oriented_box(const oriented_box& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing oriented box
    ///
    /// \param other the other oriented box to copy
    /// \return reference to `(*this)`
    auto operator=(const oriented_box& other) -> oriented_box& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the center point of this box
    ///
    /// \return the center point
    [[nodiscard]]
    constexpr auto center() const noexcept -> const point3&;

    /// \brief Gets the distances from the center of this box to its faces
    ///
    /// \return the half-extents
    [[nodiscard]]
    constexpr auto half_extents() const noexcept -> const vector3&;

    /// \brief Gets the axes that this box is aligned to
    ///
    /// \return the orientation
    [[nodiscard]]
    constexpr auto orientation() const noexcept -> const basis3&;

    /// \brief Gets the axes that the x, y, and z half-extents are measured
    ///        along, in that order
    ///
    /// \return the axes
    [[nodiscard]]
    constexpr auto axes() const noexcept -> std::array<vector3, 3>;

    /// \brief Gets the 8 corners of this box
    ///
    /// \return the corners
    [[nodiscard]]
    auto corners() const noexcept -> std::array<point3, 8>;

    /// \brief Gets the smallest axis-aligned box that encloses this box
    ///
    /// \return the bounding box
    [[nodiscard]]
    auto bounding_box() const noexcept -> axis_aligned_box;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the point in this box nearest to the point \p p
    ///
    /// \param p the point to find the nearest point to
    /// \return \p p if it is in this box, otherwise the nearest point on its
    ///         surface
    [[nodiscard]]
    auto nearest_point(const point3& p) const noexcept -> point3;

    /// \brief Checks whether a given point \p p is contained within this box
    ///
    /// \param p the point to check
    /// \return `true` if the point is contained in this box
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks whether a given point \p p is contained within this box
    ///        within a specified error \p tolerance
    ///
    /// \param p the point to check
    /// \param tolerance the error tolerance to accept
    /// \return `true` if the point is contained in this box
    [[nodiscard]]
    auto contains(const point3& p, real tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3 m_center;        ///< The center point of the box
    vector3 m_half_extents; ///< The distance from the center to each face
    basis3 m_orientation;   ///< The axes that the box is aligned to
  };

  //============================================================================
  // non-member functions : class : oriented_box
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const oriented_box& lhs, const oriented_box& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const oriented_box& lhs, const oriented_box& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two oriented boxes relative to
  ///        \p tolerance
  ///
  /// \param lhs the left box
  /// \param rhs the right box
  /// \param tolerance the tolerance for the comparison
  /// \return `true` if the two boxes contain almost equal values
  constexpr auto almost_equal(
    const oriented_box& lhs,
    const oriented_box& rhs,
    real tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : oriented_box
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::oriented_box::oriented_box() noexcept
  : m_center{},
    m_half_extents{},
    m_orientation{
      basis3::make_unchecked(
        vector3_constants::unit_y,
        vector3_constants::unit_x,
        vector3_constants::unit_z
      )
    }
{

}

inline constexpr
alloy::core::oriented_box::oriented_box(
  const point3& center,
  const vector3& half_extents,
  const basis3& orientation
) noexcept
  : m_center{center},
    m_half_extents{half_extents},
    m_orientation{orientation}
{
  ALLOY_ASSERT( half_extents.x() >= 0 && half_extents.y() >= 0 &&
                half_extents.z() >= 0, "half-extents must not be negative" );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::oriented_box::center() const noexcept -> const point3&
{
  return m_center;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::oriented_box::half_extents() const noexcept -> const vector3&
{
  return m_half_extents;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::oriented_box::orientation() const noexcept -> const basis3&
{
  return m_orientation;
}

inline constexpr
auto alloy::core::oriented_box::axes() const noexcept -> std::array<vector3, 3>
{
  return {
    m_orientation.right(),
    m_orientation.up(),
    m_orientation.forward()
  };
}

//==============================================================================
// non-member functions : class : oriented_box
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator==(const oriented_box& lhs, const oriented_box& rhs)
  noexcept -> bool
{
  return lhs.center() == rhs.center() &&
         lhs.half_extents() == rhs.half_extents() &&
         lhs.orientation() == rhs.orientation();
}

inline constexpr
auto alloy::core::operator!=(const oriented_box& lhs, const oriented_box& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(
  const oriented_box& lhs,
  const oriented_box& rhs,
  real tolerance
) noexcept -> bool
{
  return almost_equal(lhs.center(), rhs.center(), tolerance) &&
         almost_equal(lhs.half_extents(), rhs.half_extents(), tolerance) &&
         almost_equal(lhs.orientation(), rhs.orientation(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_ORIENTED_BOX_HPP */
