/*****************************************************************************
 * \file plane.hpp
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
#ifndef ALLOY_CORE_GEOMETRY_PLANE_HPP
#define ALLOY_CORE_GEOMETRY_PLANE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/math/vector/vector3.hpp" // core::vector3
#include "alloy/core/math/math.hpp"           // core::default_tolerance
#include "alloy/core/geometry/point/point3.hpp" // core::point

#include <cstdint> // std::uinr32_t, std::uint64_t
#include <type_traits>

namespace alloy::core {

  //============================================================================
  // class : plane
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A value-type that represents a geometric plane.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API plane
  {
    //--------------------------------------------------------------------------
    // Static Member Functions
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Makes a plane that intersects the provided points
    ///
    /// \param p0 the first point
    /// \param p1 the second point
    /// \param p2 the third point
    /// \return a plane that intersects the 3 points
    static auto from_points_clockwise(
      const point3& p0,
      const point3& p1,
      const point3& p2
    ) noexcept -> plane;
    static auto from_points_counter_clockwise(
      const point3& p0,
      const point3& p1,
      const point3& p2
    ) noexcept -> plane;
    /// \}

    /// \brief Makes a plane that intersects point \p, and contains the
    ///        given \p normal
    ///
    /// \param p the point for the plane to intersect
    /// \param normal the normal vector
    /// \return the plane
    ALLOY_CORE_API
    static auto from_point_and_normal(const point3& p,
                                      const vector3& normal)
      noexcept -> plane;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs the plane with 0-values
    constexpr plane() noexcept;

    /// \brief Constructs a plane through a normal, and a distance to move
    ///        the plane along that normal
    ///
    /// \param normal the normal vector
    /// \param distance the distance to move the plane along the normal
    constexpr explicit plane(const vector3& normal, real distance) noexcept;

    /// \brief Constructs a plane givene the 4 underlying components
    ///
    /// \param a the a component
    /// \param b the b component
    /// \param c the c component
    /// \param d the d component
    constexpr plane(real a, real b, real c, real d) noexcept;

    /// \brief Constructs this plane by copy-constructing from an existing one
    ///
    /// \param other the other plane to copy
    plane(const plane& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to `this` through
    ///        copy-assignment
    ///
    /// \param other the other plane to copy
    /// \return reference to `(*this)`
    auto operator=(const plane& other) -> plane& = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this plane
    ///
    /// \return reference to `(*this)`
    auto normalize() noexcept -> plane&;

    /// \brief Inverts this plane's direction
    ///
    /// \return reference to `(*this)`
    auto invert() noexcept -> plane&;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:
    [[nodiscard]] constexpr auto a() const noexcept -> real;
    [[nodiscard]] constexpr auto b() const noexcept -> real;
    [[nodiscard]] constexpr auto c() const noexcept -> real;
    [[nodiscard]] constexpr auto d() const noexcept -> real;

    /// \brief Gets the normal of this plane
    ///
    /// \return the normal of this plane
    [[nodiscard]] constexpr auto normal() const noexcept -> vector3;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the distance between this plane and the specified \p point
    ///
    /// \param p the point to get the distance to
    /// \return the distance
    [[nodiscard]]
    auto distance_to(const point3& p) const noexcept -> real;

    /// \brief Gets the projection of the specified vector onto this plane
    ///
    /// \param vec the vector to project
    /// \return the projection
    [[nodiscard]]
    auto projection(const vector3& vec) const noexcept -> vector3;

    /// \brief Gets the rejection of the specified vector off of this plane
    ///
    /// The rejection of the vector is the difference of the normal of the
    /// plane and the projection of the vector.
    ///
    /// \param vec the vector to reject
    /// \return the rejection
    [[nodiscard]]
    auto rejection(const vector3& vec) const noexcept -> vector3;

    /// \brief Gets a copy of this plane normalized
    ///
    /// \return the normalized plane
    [[nodiscard]]
    auto normalized() const noexcept -> plane;

    /// \brief Gets a copy of this plane inverted
    ///
    /// \return the inverted plane
    [[nodiscard]]
    auto inverted() const noexcept -> plane;

    /// \brief Checks if this plane intersects the given point \p p
    ///
    /// \param p the point to check for intersection
    /// \return `true` if \p p is in the plane
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks if this plane intersects the given point \p p
    ///        relative to the given \p tolerance
    ///
    /// \param p the point to check for intersection
    /// \param tolerance the tolerance for accepting the containment
    /// \return `true` if \p p is in the plane
    [[nodiscard]]
    auto contains(const point3& p, real tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    auto operator+() const noexcept -> const plane&;
    auto operator-() const noexcept -> plane;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    alignas(16) real m_data[4];
  };

  //============================================================================
  // non-member functions : class : plane
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const plane& lhs, const plane& rhs) noexcept -> bool;
  constexpr auto operator!=(const plane& lhs, const plane& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two planes relative to \p tolerance
  ///
  /// \param lhs the left plane
  /// \param rhs the right plane
  /// \return `true` if the two planes contain almost equal values
  constexpr auto almost_equal(const plane& lhs, const plane& rhs, real tolerance)
    noexcept -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Checks if the given point \p p is over the \p plane
  ///
  /// \param p the point
  /// \param plane the plane
  /// \return `true` if \p p is over the \p plane
  ALLOY_CORE_API
  auto is_point_over_plane(const point3& p, const plane& plane) noexcept -> bool;

  /// \brief Checks if the given point \p p is under the \p plane
  ///
  /// \param p the point
  /// \param plane the plane
  /// \return `true` if \p p is under the \p plane
  ALLOY_CORE_API
  auto is_point_under_plane(const point3& p, const plane& plane) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : plane
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::plane::plane()
  noexcept
  : plane{0,0,0,0}
{

}

inline constexpr
alloy::core::plane::plane(const vector3& normal, real distance)
  noexcept
  : plane{normal.x(), normal.y(), normal.z(), -distance}
{

}

inline constexpr
alloy::core::plane::plane(real a, real b, real c, real d)
  noexcept
  : m_data{a,b,c,d}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::plane::a()
  const noexcept -> real
{
  return m_data[0];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::plane::b()
  const noexcept -> real
{
  return m_data[1];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::plane::c()
  const noexcept -> real
{
  return m_data[2];
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::plane::d()
  const noexcept -> real
{
  return m_data[3];
}

//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::plane::normal()
  const noexcept -> vector3
{
  return {a(),b(),c()};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline
auto alloy::core::plane::rejection(const vector3& vec)
  const noexcept -> vector3
{
  return normal() - projection(vec);
}

inline
auto alloy::core::plane::normalized()
  const noexcept -> plane
{
  auto copy = (*this);
  copy.normalize();
  return copy;
}

inline
auto alloy::core::plane::inverted()
  const noexcept -> plane
{
  auto copy = (*this);
  copy.invert();
  return copy;
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::plane::operator+()
  const noexcept -> const plane&
{
  return (*this);
}

inline
auto alloy::core::plane::operator-()
  const noexcept -> plane
{
  return inverted();
}

//==============================================================================
// non-member functions : class : plane
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const plane& lhs, const plane& rhs)
  noexcept -> bool
{
  return lhs.a() == rhs.a() &&
         lhs.b() == rhs.b() &&
         lhs.c() == rhs.c() &&
         lhs.d() == rhs.d();
}

inline constexpr
auto alloy::core::operator!=(const plane& lhs, const plane& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const plane& lhs, const plane& rhs, real tolerance)
  noexcept -> bool
{
  return almost_equal(lhs.a(), rhs.a(), tolerance) &&
         almost_equal(lhs.b(), rhs.b(), tolerance) &&
         almost_equal(lhs.c(), rhs.c(), tolerance) &&
         almost_equal(lhs.d(), rhs.d(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_PLANE_HPP */
