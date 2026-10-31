// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/******************************************************************************
 * \file triangle.hpp
 *
 * \brief This header contains an implementation of a 3d triangle geometric
 *        object
 ******************************************************************************/

#ifndef ALLOY_CORE_GEOMETRY_TRIANGLE_HPP
#define ALLOY_CORE_GEOMETRY_TRIANGLE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/geometry/plane.hpp"
#include "alloy/core/geometry/point/barycentric.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/precision.hpp"

namespace alloy::core {

  //============================================================================
  // class : triangle
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A geometric primitive of a triangle
  ///
  /// The front face of a triangle is the one from which its points `a`, `b`,
  /// and `c` appear in counter-clockwise order.
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API triangle
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a triangle with all points at the origin
    constexpr triangle() noexcept;

    /// \brief Constructs a triangle from the points \p a, \p b, and \p c
    ///
    /// \param a the first point
    /// \param b the second point
    /// \param c the third point
    constexpr triangle(const point3& a, const point3& b, const point3& c)
      noexcept;

    /// \brief Constructs a triangle by copying an existing instance
    ///
    /// \param other the other triangle to copy
    triangle(const triangle& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing triangle
    ///
    /// \param other the other triangle to copy
    /// \return reference to `(*this)`
    auto operator=(const triangle& other) -> triangle& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the specified point of this triangle
    ///
    /// \return the point
    [[nodiscard]] constexpr auto a() const noexcept -> const point3&;
    [[nodiscard]] constexpr auto b() const noexcept -> const point3&;
    [[nodiscard]] constexpr auto c() const noexcept -> const point3&;
    /// \}

    /// \brief Gets the unit normal of the front face of this triangle
    ///
    /// \pre this triangle is not degenerate
    ///
    /// \return the normal
    [[nodiscard]]
    auto normal() const noexcept -> vector3;

    /// \brief Gets the plane that this triangle lies on, facing the same way
    ///        as its front face
    ///
    /// \pre this triangle is not degenerate
    ///
    /// \return the plane
    [[nodiscard]]
    auto supporting_plane() const noexcept -> plane;

    /// \brief Gets the area of this triangle
    ///
    /// \return the area
    [[nodiscard]]
    auto area() const noexcept -> real;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the barycentric coordinates of the point \p p relative to
    ///        this triangle
    ///
    /// The point is first projected onto the plane of this triangle.
    ///
    /// \pre this triangle is not degenerate
    ///
    /// \param p the point
    /// \return the barycentric coordinates of \p p
    [[nodiscard]]
    auto barycentric_of(const point3& p) const noexcept -> barycentric;

    /// \brief Gets the point on this triangle nearest to the point \p p
    ///
    /// \param p the point to find the nearest point to
    /// \return the nearest point
    [[nodiscard]]
    auto nearest_point(const point3& p) const noexcept -> point3;

    /// \brief Checks whether a given point \p p lies on this triangle
    ///
    /// \param p the point to check
    /// \return `true` if the point is on this triangle
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks whether a given point \p p lies on this triangle within
    ///        a specified error \p tolerance
    ///
    /// \param p the point to check
    /// \param tolerance the error tolerance to accept
    /// \return `true` if the point is on this triangle
    [[nodiscard]]
    auto contains(const point3& p, real tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3 m_a; ///< The first point
    point3 m_b; ///< The second point
    point3 m_c; ///< The third point
  };

  //============================================================================
  // non-member functions : class : triangle
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const triangle& lhs, const triangle& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const triangle& lhs, const triangle& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two triangles relative to
  ///        \p tolerance
  ///
  /// \param lhs the left triangle
  /// \param rhs the right triangle
  /// \param tolerance the tolerance for the comparison
  /// \return `true` if the two triangles contain almost equal values
  constexpr auto almost_equal(
    const triangle& lhs,
    const triangle& rhs,
    real tolerance
  ) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : triangle
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::triangle::triangle() noexcept
  : m_a{},
    m_b{},
    m_c{}
{

}

inline constexpr
alloy::core::triangle::triangle(
  const point3& a,
  const point3& b,
  const point3& c
) noexcept
  : m_a{a},
    m_b{b},
    m_c{c}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::triangle::a() const noexcept -> const point3&
{
  return m_a;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::triangle::b() const noexcept -> const point3&
{
  return m_b;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::triangle::c() const noexcept -> const point3&
{
  return m_c;
}

//==============================================================================
// non-member functions : class : triangle
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator==(const triangle& lhs, const triangle& rhs)
  noexcept -> bool
{
  return lhs.a() == rhs.a() && lhs.b() == rhs.b() && lhs.c() == rhs.c();
}

inline constexpr
auto alloy::core::operator!=(const triangle& lhs, const triangle& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(
  const triangle& lhs,
  const triangle& rhs,
  real tolerance
) noexcept -> bool
{
  return almost_equal(lhs.a(), rhs.a(), tolerance) &&
         almost_equal(lhs.b(), rhs.b(), tolerance) &&
         almost_equal(lhs.c(), rhs.c(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_TRIANGLE_HPP */
