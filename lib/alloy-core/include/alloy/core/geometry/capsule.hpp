// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file capsule.hpp
 *
 * \brief This header contains an implementation of a 3d capsule geometric
 *        object
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_CAPSULE_HPP
#define ALLOY_CORE_GEOMETRY_CAPSULE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/geometry/line_segment.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/math.hpp"

namespace alloy::core {

  //============================================================================
  // class : capsule
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A geometric primitive of a capsule
  ///
  /// A capsule is the set of all points within a radius of a line segment; a
  /// cylinder capped at both ends by hemispheres.
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API capsule
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a capsule at the origin with no length or
    ///        radius
    constexpr capsule() noexcept;

    /// \brief Constructs a capsule around \p segment with the given \p radius
    ///
    /// \pre \p radius cannot be negative
    ///
    /// \param segment the segment at the center of the capsule
    /// \param radius the radius of the capsule
    constexpr capsule(const line_segment& segment, float radius) noexcept;

    /// \brief Constructs a capsule by copying an existing instance
    ///
    /// \param other the other capsule to copy
    capsule(const capsule& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing capsule
    ///
    /// \param other the other capsule to copy
    /// \return reference to `(*this)`
    auto operator=(const capsule& other) -> capsule& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the segment at the center of this capsule
    ///
    /// \return the segment
    [[nodiscard]]
    constexpr auto segment() const noexcept -> const line_segment&;

    /// \brief Gets the radius of this capsule
    ///
    /// \return the radius
    [[nodiscard]]
    constexpr auto radius() const noexcept -> float;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the point in this capsule nearest to the point \p p
    ///
    /// \param p the point to find the nearest point to
    /// \return \p p if it is in this capsule, otherwise the nearest point on
    ///         its surface
    [[nodiscard]]
    auto nearest_point(const point3& p) const noexcept -> point3;

    /// \brief Checks whether a given point \p p is contained within this
    ///        capsule
    ///
    /// \param p the point to check
    /// \return `true` if the point is contained in this capsule
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks whether a given point \p p is contained within this
    ///        capsule within a specified error \p tolerance
    ///
    /// \param p the point to check
    /// \param tolerance the error tolerance to accept
    /// \return `true` if the point is contained in this capsule
    [[nodiscard]]
    auto contains(const point3& p, float tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    line_segment m_segment; ///< The segment at the center of the capsule
    float m_radius;         ///< The radius of the capsule
  };

  //============================================================================
  // non-member functions : class : capsule
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const capsule& lhs, const capsule& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const capsule& lhs, const capsule& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two capsules relative to \p tolerance
  ///
  /// \param lhs the left capsule
  /// \param rhs the right capsule
  /// \param tolerance the tolerance for the comparison
  /// \return `true` if the two capsules contain almost equal values
  constexpr auto almost_equal(
    const capsule& lhs,
    const capsule& rhs,
    float tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : capsule
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::capsule::capsule() noexcept
  : m_segment{},
    m_radius{0}
{

}

inline constexpr
alloy::core::capsule::capsule(const line_segment& segment, float radius)
  noexcept
  : m_segment{segment},
    m_radius{radius}
{
  ALLOY_ASSERT( radius >= 0, "radius must not be negative" );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::capsule::segment() const noexcept -> const line_segment&
{
  return m_segment;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::capsule::radius() const noexcept -> float
{
  return m_radius;
}

//==============================================================================
// non-member functions : class : capsule
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const capsule& lhs, const capsule& rhs)
  noexcept -> bool
{
  return lhs.segment() == rhs.segment() && lhs.radius() == rhs.radius();
}

inline constexpr
auto alloy::core::operator!=(const capsule& lhs, const capsule& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(
  const capsule& lhs,
  const capsule& rhs,
  float tolerance
) noexcept -> bool
{
  return almost_equal(lhs.segment(), rhs.segment(), tolerance) &&
         almost_equal(lhs.radius(), rhs.radius(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_CAPSULE_HPP */
