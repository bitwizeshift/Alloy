/*****************************************************************************
 * \file axis_aligned_box.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019-2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_GEOMETRY_AXIS_ALIGNED_BOX_HPP
#define ALLOY_CORE_GEOMETRY_AXIS_ALIGNED_BOX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/precision.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/geometry/plane.hpp"

#include <array> // std::array

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A representation of a box that is aligned with the x, y, and z
  ///        axes.
  ///
  /// Such a box is useful for making rough intersection calculations for an
  /// initial check, before performing more in-depth intersection checks.
  /// For example, a large model may contain one of these around the entire
  /// model, and -- if an intersection occurs with this box -- a more
  /// complicated intersection may be performed on all sub-parts of the model.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API axis_aligned_box
  {
    //--------------------------------------------------------------------------
    // Static Factory Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an aligned box from a starting point, and a vector
    ///        indicating the size
    ///
    /// \param p the point
    /// \param size the vector that determines the x, y, z size
    /// \return the created box
    static auto from_point_and_size(const point3& p, const vector3& size)
      noexcept -> axis_aligned_box;

    /// \brief Constructs an aligned box constrainted between the two points
    ///        \p p0 and \p p1
    ///
    /// \note Unlike the constructor for this class, this factory does not have
    ///       any preconditions on the points passed to this
    ///
    /// \param p0 the first point
    /// \param p1 the second point
    /// \return the created box
    static auto from_points(const point3& p0, const point3& p1)
      noexcept -> axis_aligned_box;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an axis-aligned bounding-box with default values
    axis_aligned_box() noexcept = default;

    /// \brief Constructs an axis-aligned bounding-box from points from two
    ///        opposite ends
    ///
    /// \pre `lower_left.x() <= top_right.x()`
    /// \pre `lower_left.y() <= top_right.y()`
    /// \pre `lower_left.z() <= top_right.z()`
    ///
    /// \param lower_left The first point
    /// \param top_right The second point
    axis_aligned_box(const point3& lower_left,
                     const point3& top_right) noexcept;

    /// \brief Constructs an axis-aligned bounding-box by copying the contents
    ///        from an existing one
    ///
    /// \param other the other box to copy
    axis_aligned_box(const axis_aligned_box& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of an axis-aligned bounding-box to this one
    ///
    /// \param other the other box to copy
    /// \return reference to `(*this)`
    auto operator=(const axis_aligned_box& other)
      noexcept -> axis_aligned_box& = default;

    //--------------------------------------------------------------------------
    // Observers : Normals
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Get the normal to the specified side
    ///
    /// \return the normal for the specified side
    [[nodiscard]] auto top_normal() const noexcept -> vector3;
    [[nodiscard]] auto bottom_normal() const noexcept -> vector3;
    [[nodiscard]] auto front_normal() const noexcept -> vector3;
    [[nodiscard]] auto back_normal() const noexcept -> vector3;
    [[nodiscard]] auto left_normal() const noexcept -> vector3;
    [[nodiscard]] auto right_normal() const noexcept -> vector3;
    /// \}

    //--------------------------------------------------------------------------
    // Observers : Planes
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Get the plane that defines the specified side
    ///
    /// \return the plane that define sthe specified side
    [[nodiscard]] auto top_plane() const noexcept -> plane;
    [[nodiscard]] auto bottom_plane() const noexcept -> plane;
    [[nodiscard]] auto front_plane() const noexcept -> plane;
    [[nodiscard]] auto back_plane() const noexcept -> plane;
    [[nodiscard]] auto left_plane() const noexcept -> plane;
    [[nodiscard]] auto right_plane() const noexcept -> plane;
    /// \}

    //--------------------------------------------------------------------------
    // Observers : Points
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the point at the bottom left of this box
    ///
    /// \return the point at the bottom-left of this box
    [[nodiscard]]
    auto bottom_left_point() const noexcept -> point3;

    /// \brief Gets the point at the top-right of this box
    ///
    /// \return the point at the top-right of this box
    [[nodiscard]]
    auto top_right_point() const noexcept -> point3;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets an array containing the 6 normals
    ///
    /// \return the normals
    [[nodiscard]]
    auto normals() const noexcept -> std::array<vector3, 6>;

    /// \brief Gets an array containing the 6 planes that encompass this box
    ///
    /// \return the planes
    [[nodiscard]]
    auto planes() const noexcept -> std::array<plane, 6>;

    /// \brief Gets the distance between this box and the specified point \p p
    ///
    /// \param p the point to get the distance to
    /// \return the distance
    [[nodiscard]]
    auto distance_to(const point3& p) const noexcept -> real;

    /// \brief Gets the squared distance between this box and the specified point
    ///        \p p
    ///
    /// \note This function exists for quicker checks that don't rely on the
    ///       `sqrt` operation (as is needed in conventional magnitude
    ///       calculations).
    ///
    /// \param p the point to get the distance to
    /// \return the squared distance to \p p
    [[nodiscard]]
    auto square_distance_to(const point3& p) const noexcept -> real;

    /// \brief Gets the nearest point on this box to the point \p p
    ///
    /// \param p the point to find the nearest point to
    /// \return the point
    [[nodiscard]]
    auto nearest_point(const point3& p) const noexcept -> point3;

    /// \brief Checks if this box contains the given point \p p
    ///
    /// \param p the point to check for intersection
    /// \return `true` if \p p is in the box
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks if this box contains the given point \p p
    ///        relative to the given \p tolerance
    ///
    /// \param p the point to check for intersection
    /// \param tolerance the tolerance for accepting the containment
    /// \return `true` if \p p is in the box
    [[nodiscard]]
    auto contains(const point3& p, real tolerance) const noexcept -> bool;

    /// \brief Checks if this box intersect another box
    ///
    /// \return `true` if this box intersects another box
    [[nodiscard]]
    auto intersects(const axis_aligned_box& other) const noexcept -> bool;

    /// \brief Checks if this box completely encloses another box
    ///
    /// \return `true` if this box completely encloses another box
    [[nodiscard]]
    auto encloses(const axis_aligned_box& other) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3 m_bottom_left;
    point3 m_top_right;
  };

  //============================================================================
  // non-member functions : class : axis_aligned_box
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  ALLOY_CORE_API
  auto operator==(const axis_aligned_box& lhs,
                  const axis_aligned_box& rhs) noexcept -> bool;
  ALLOY_CORE_API
  auto operator!=(const axis_aligned_box& lhs,
                  const axis_aligned_box& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two planes relative to \p tolerance
  ///
  /// \param lhs the left plane
  /// \param rhs the right plane
  /// \param tolerance the tolerance for the comparison
  /// \return `true` if the two planes contain almost equal values
  ALLOY_CORE_API
  auto almost_equal(const axis_aligned_box& lhs,
                    const axis_aligned_box& rhs,
                    real tolerance) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : axis_aligned_box
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline
alloy::core::axis_aligned_box::axis_aligned_box(
  const point3& lower_left,
  const point3& top_right
) noexcept
  : m_bottom_left{lower_left},
    m_top_right{top_right}
{
  ALLOY_ASSERT( m_bottom_left.x() <= m_top_right.x() );
  ALLOY_ASSERT( m_bottom_left.y() <= m_top_right.y() );
  ALLOY_ASSERT( m_bottom_left.z() <= m_top_right.z() );
}

//------------------------------------------------------------------------------
// Observers : Normals
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::axis_aligned_box::top_normal()
  const noexcept ->vector3
{
  return top_plane().normal();
}

ALLOY_FORCE_INLINE
auto alloy::core::axis_aligned_box::bottom_normal()
  const noexcept -> vector3
{
  return bottom_plane().normal();
}

ALLOY_FORCE_INLINE
auto alloy::core::axis_aligned_box::front_normal()
  const noexcept -> vector3
{
  return front_plane().normal();
}

ALLOY_FORCE_INLINE
auto alloy::core::axis_aligned_box::back_normal()
  const noexcept -> vector3
{
  return back_plane().normal();
}

ALLOY_FORCE_INLINE
auto alloy::core::axis_aligned_box::left_normal()
  const noexcept -> vector3
{
  return left_plane().normal();
}

ALLOY_FORCE_INLINE
auto alloy::core::axis_aligned_box::right_normal()
  const noexcept -> vector3
{
  return right_plane().normal();
}

//------------------------------------------------------------------------------
// Observers : Points
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto  alloy::core::axis_aligned_box::bottom_left_point()
  const noexcept -> point3
{
  return m_bottom_left;
}

ALLOY_FORCE_INLINE
auto alloy::core::axis_aligned_box::top_right_point()
  const noexcept -> point3
{
  return m_top_right;
}

#endif /* ALLOY_CORE_GEOMETRY_AXIS_ALIGNED_BOX_HPP */
