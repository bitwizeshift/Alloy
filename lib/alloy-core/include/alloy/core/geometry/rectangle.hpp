// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file rectangle.hpp
 *
 * \brief This header contains an implementation of a 2d axis-aligned
 *        rectangle geometric object
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_RECTANGLE_HPP
#define ALLOY_CORE_GEOMETRY_RECTANGLE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/geometry/point/point2.hpp"

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/precision.hpp"

namespace alloy::core {

  //============================================================================
  // class : rectangle
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A representation of a 2d rectangle that is aligned with the x and
  ///        y axes
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API rectangle
  {
    //--------------------------------------------------------------------------
    // Static Factory Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a rectangle from a starting point, and a vector
    ///        indicating the size
    ///
    /// \param p the point
    /// \param size the vector that determines the x and y size
    /// \return the created rectangle
    static auto from_point_and_size(const point2& p, const vector2& size)
      noexcept -> rectangle;

    /// \brief Constructs a rectangle constrained between the two points
    ///        \p p0 and \p p1
    ///
    /// \note Unlike the constructor for this class, this factory does not have
    ///       any preconditions on the points passed to this
    ///
    /// \param p0 the first point
    /// \param p1 the second point
    /// \return the created rectangle
    static auto from_points(const point2& p0, const point2& p1)
      noexcept -> rectangle;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a rectangle at the origin with no size
    constexpr rectangle() noexcept;

    /// \brief Constructs a rectangle from points from two opposite corners
    ///
    /// \pre `bottom_left.x() <= top_right.x()`
    /// \pre `bottom_left.y() <= top_right.y()`
    ///
    /// \param bottom_left the corner with the least coordinates
    /// \param top_right the corner with the greatest coordinates
    constexpr rectangle(const point2& bottom_left, const point2& top_right)
      noexcept;

    /// \brief Constructs a rectangle by copying an existing instance
    ///
    /// \param other the other rectangle to copy
    rectangle(const rectangle& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing rectangle
    ///
    /// \param other the other rectangle to copy
    /// \return reference to `(*this)`
    auto operator=(const rectangle& other) -> rectangle& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the corner of this rectangle with the least coordinates
    ///
    /// \return the bottom-left point
    [[nodiscard]]
    constexpr auto bottom_left_point() const noexcept -> const point2&;

    /// \brief Gets the corner of this rectangle with the greatest coordinates
    ///
    /// \return the top-right point
    [[nodiscard]]
    constexpr auto top_right_point() const noexcept -> const point2&;

    /// \brief Gets the width and height of this rectangle
    ///
    /// \return the size
    [[nodiscard]]
    constexpr auto size() const noexcept -> vector2;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the distance between this rectangle and the point \p p
    ///
    /// \param p the point to get the distance to
    /// \return the distance, which is 0 if \p p is in this rectangle
    [[nodiscard]]
    auto distance_to(const point2& p) const noexcept -> real;

    /// \brief Gets the squared distance between this rectangle and the point
    ///        \p p
    ///
    /// \param p the point to get the distance to
    /// \return the squared distance
    [[nodiscard]]
    auto square_distance_to(const point2& p) const noexcept -> real;

    /// \brief Gets the point in this rectangle nearest to the point \p p
    ///
    /// \param p the point to find the nearest point to
    /// \return the nearest point
    [[nodiscard]]
    auto nearest_point(const point2& p) const noexcept -> point2;

    /// \brief Checks if this rectangle contains the given point \p p
    ///
    /// \param p the point to check
    /// \return `true` if \p p is in the rectangle
    [[nodiscard]]
    auto contains(const point2& p) const noexcept -> bool;

    /// \brief Checks if this rectangle contains the given point \p p
    ///        relative to the given \p tolerance
    ///
    /// \param p the point to check
    /// \param tolerance the tolerance for accepting the containment
    /// \return `true` if \p p is in the rectangle
    [[nodiscard]]
    auto contains(const point2& p, real tolerance) const noexcept -> bool;

    /// \brief Checks if this rectangle intersects another rectangle
    ///
    /// \param other the other rectangle
    /// \return `true` if the rectangles overlap or touch
    [[nodiscard]]
    auto intersects(const rectangle& other) const noexcept -> bool;

    /// \brief Checks if this rectangle completely encloses another rectangle
    ///
    /// \param other the other rectangle
    /// \return `true` if \p other is entirely within this rectangle
    [[nodiscard]]
    auto encloses(const rectangle& other) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point2 m_bottom_left; ///< The corner with the least coordinates
    point2 m_top_right;   ///< The corner with the greatest coordinates
  };

  //============================================================================
  // non-member functions : class : rectangle
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const rectangle& lhs, const rectangle& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const rectangle& lhs, const rectangle& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two rectangles relative to
  ///        \p tolerance
  ///
  /// \param lhs the left rectangle
  /// \param rhs the right rectangle
  /// \param tolerance the tolerance for the comparison
  /// \return `true` if the two rectangles contain almost equal values
  constexpr auto almost_equal(
    const rectangle& lhs,
    const rectangle& rhs,
    real tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : rectangle
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::rectangle::rectangle() noexcept
  : m_bottom_left{},
    m_top_right{}
{

}

inline constexpr
alloy::core::rectangle::rectangle(
  const point2& bottom_left,
  const point2& top_right
) noexcept
  : m_bottom_left{bottom_left},
    m_top_right{top_right}
{
  ALLOY_ASSERT( m_bottom_left.x() <= m_top_right.x() );
  ALLOY_ASSERT( m_bottom_left.y() <= m_top_right.y() );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::rectangle::bottom_left_point() const noexcept -> const point2&
{
  return m_bottom_left;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::rectangle::top_right_point() const noexcept -> const point2&
{
  return m_top_right;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::rectangle::size() const noexcept -> vector2
{
  return m_top_right - m_bottom_left;
}

//==============================================================================
// non-member functions : class : rectangle
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator==(const rectangle& lhs, const rectangle& rhs)
  noexcept -> bool
{
  return lhs.bottom_left_point() == rhs.bottom_left_point() &&
         lhs.top_right_point() == rhs.top_right_point();
}

inline constexpr
auto alloy::core::operator!=(const rectangle& lhs, const rectangle& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(
  const rectangle& lhs,
  const rectangle& rhs,
  real tolerance
) noexcept -> bool
{
  const auto& lhs_min = lhs.bottom_left_point();
  const auto& rhs_min = rhs.bottom_left_point();
  const auto& lhs_max = lhs.top_right_point();
  const auto& rhs_max = rhs.top_right_point();

  return almost_equal(lhs_min, rhs_min, tolerance) &&
         almost_equal(lhs_max, rhs_max, tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_RECTANGLE_HPP */
