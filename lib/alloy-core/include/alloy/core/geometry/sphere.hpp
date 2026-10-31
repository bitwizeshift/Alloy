// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file sphere.hpp
 *
 * \brief This header contains an implementation of a 3d sphere geometric
 *        object
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_SPHERE_HPP
#define ALLOY_CORE_GEOMETRY_SPHERE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/math.hpp"

namespace alloy::core {

  //============================================================================
  // class : sphere
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A geometric primitive of a sphere
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  /// \ingroup immutable
  //////////////////////////////////////////////////////////////////////////////
  class sphere
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a sphere containing 0-values
    constexpr sphere() noexcept;

    /// \brief Constructs a sphere given its center point and the radius
    ///
    /// \pre \p radius cannot be negative
    ///
    /// \param center the center point
    /// \param radius the radius of the sphere
    constexpr sphere(const point3& center, float radius) noexcept;

    /// \brief Constructs a sphere by copying an existing instance
    ///
    /// \param other the other sphere to copy
    sphere(const sphere& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing sphere
    ///
    /// \param other the other sphere to copy
    /// \return reference to `(*this)`
    auto operator=(const sphere& other) -> sphere& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the center point of this sphere
    ///
    /// \return the center point
    [[nodiscard]]
    constexpr auto center() const noexcept -> const point3&;

    /// \brief Gets the radius of this sphere
    ///
    /// \return the radius
    [[nodiscard]]
    constexpr auto radius() const noexcept -> const float&;

    /// \brief Checks whether a given point \p p is contained within this
    ///        sphere
    ///
    /// \param p the point to check
    /// \return `true` if the point is contained in this sphere
    [[nodiscard]] ALLOY_CORE_API
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks whether a given point \p p is contained within this
    ///        sphere within a specified error \p tolerance
    ///
    /// \param p the point to check
    /// \param tolerance the error tolerance to accept
    /// \return `true` if the point is contained in this sphere
    [[nodiscard]] ALLOY_CORE_API
    auto contains(const point3& p, float tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3 m_center; ///< The center point of the sphere
    float m_radius;  ///< The radius of the sphere
  };

  //============================================================================
  // non-member functions : class : sphere
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const sphere& lhs, const sphere& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const sphere& lhs, const sphere& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two spheres relative to \p tolerance
  ///
  /// \param lhs the left sphere
  /// \param rhs the right sphere
  /// \return `true` if the two spheres contain almost equal values
  constexpr auto almost_equal(
    const sphere& lhs,
    const sphere& rhs,
    float tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : sphere
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::sphere::sphere() noexcept
  : m_center{0,0,0},
    m_radius{0}
{

}

inline constexpr
alloy::core::sphere::sphere(const point3& center, float radius) noexcept
  : m_center{center},
    m_radius{radius}
{
  ALLOY_ASSERT( radius >= 0, "radius must be positive" );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::sphere::center() const noexcept -> const point3&
{
  return m_center;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::sphere::radius() const noexcept -> const float&
{
  return m_radius;
}

//==============================================================================
// non-member functions : class : sphere
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const sphere& lhs, const sphere& rhs)
  noexcept -> bool
{
  return lhs.center() == rhs.center() && lhs.radius() == rhs.radius();
}

inline constexpr
auto alloy::core::operator!=(const sphere& lhs, const sphere& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(
  const sphere& lhs,
  const sphere& rhs,
  float tolerance
) noexcept -> bool
{
  return almost_equal(lhs.center(), rhs.center(), tolerance) &&
         almost_equal(lhs.radius(), rhs.radius(), tolerance);
}


#endif /* ALLOY_CORE_GEOMETRY_SPHERE_HPP */
