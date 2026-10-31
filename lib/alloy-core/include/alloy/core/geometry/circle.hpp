// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file circle.hpp
 *
 * \brief This header contains an implementation of a 2d circle geometric
 *        object
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_CIRCLE_HPP
#define ALLOY_CORE_GEOMETRY_CIRCLE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/math.hpp"

namespace alloy::core {

  //============================================================================
  // class : circle
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A geometric primitive of a 2d circle
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API circle
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a circle at the origin with no radius
    constexpr circle() noexcept;

    /// \brief Constructs a circle given its center point and the radius
    ///
    /// \pre \p radius cannot be negative
    ///
    /// \param center the center point
    /// \param radius the radius of the circle
    constexpr circle(const point2& center, float radius) noexcept;

    /// \brief Constructs a circle by copying an existing instance
    ///
    /// \param other the other circle to copy
    circle(const circle& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing circle
    ///
    /// \param other the other circle to copy
    /// \return reference to `(*this)`
    auto operator=(const circle& other) -> circle& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the center point of this circle
    ///
    /// \return the center point
    [[nodiscard]]
    constexpr auto center() const noexcept -> const point2&;

    /// \brief Gets the radius of this circle
    ///
    /// \return the radius
    [[nodiscard]]
    constexpr auto radius() const noexcept -> float;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the point in this circle nearest to the point \p p
    ///
    /// \param p the point to find the nearest point to
    /// \return \p p if it is in this circle, otherwise the nearest point on
    ///         its edge
    [[nodiscard]]
    auto nearest_point(const point2& p) const noexcept -> point2;

    /// \brief Checks whether a given point \p p is contained within this
    ///        circle
    ///
    /// \param p the point to check
    /// \return `true` if the point is contained in this circle
    [[nodiscard]]
    auto contains(const point2& p) const noexcept -> bool;

    /// \brief Checks whether a given point \p p is contained within this
    ///        circle within a specified error \p tolerance
    ///
    /// \param p the point to check
    /// \param tolerance the error tolerance to accept
    /// \return `true` if the point is contained in this circle
    [[nodiscard]]
    auto contains(const point2& p, float tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point2 m_center; ///< The center point of the circle
    float m_radius;  ///< The radius of the circle
  };

  //============================================================================
  // non-member functions : class : circle
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const circle& lhs, const circle& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const circle& lhs, const circle& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two circles relative to \p tolerance
  ///
  /// \param lhs the left circle
  /// \param rhs the right circle
  /// \param tolerance the tolerance for the comparison
  /// \return `true` if the two circles contain almost equal values
  constexpr auto almost_equal(
    const circle& lhs,
    const circle& rhs,
    float tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : circle
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::circle::circle() noexcept
  : m_center{},
    m_radius{0}
{

}

inline constexpr
alloy::core::circle::circle(const point2& center, float radius) noexcept
  : m_center{center},
    m_radius{radius}
{
  ALLOY_ASSERT( radius >= 0, "radius must not be negative" );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::circle::center() const noexcept -> const point2&
{
  return m_center;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::circle::radius() const noexcept -> float
{
  return m_radius;
}

//==============================================================================
// non-member functions : class : circle
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const circle& lhs, const circle& rhs)
  noexcept -> bool
{
  return lhs.center() == rhs.center() && lhs.radius() == rhs.radius();
}

inline constexpr
auto alloy::core::operator!=(const circle& lhs, const circle& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(
  const circle& lhs,
  const circle& rhs,
  float tolerance
) noexcept -> bool
{
  return almost_equal(lhs.center(), rhs.center(), tolerance) &&
         almost_equal(lhs.radius(), rhs.radius(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_CIRCLE_HPP */
