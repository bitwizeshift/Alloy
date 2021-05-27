/*****************************************************************************
 * \file plane.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
  class plane
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
    ALLOY_CORE_API
    static plane from_points_clockwise( const point3& p0,
                                        const point3& p1,
                                        const point3& p2 ) noexcept;
    ALLOY_CORE_API
    static plane from_points_counter_clockwise( const point3& p0,
                                                const point3& p1,
                                                const point3& p2 ) noexcept;
    /// \}

    /// \brief Makes a plane that intersects point \p, and contains the
    ///        given \p normal
    ///
    /// \param p the point for the plane to intersect
    /// \param normal the normal vector
    /// \return the plane
    ALLOY_CORE_API
    static plane from_point_and_normal( const point3& p,
                                        const vector3& normal ) noexcept;

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
    constexpr explicit plane( const vector3& normal, real distance ) noexcept;

    /// \brief Constructs a plane givene the 4 underlying components
    ///
    /// \param a the a component
    /// \param b the b component
    /// \param c the c component
    /// \param d the d component
    constexpr plane( real a, real b, real c, real d ) noexcept;

    /// \brief Constructs this plane by move-constructing from an existing one
    ///
    /// \param other the other plane to move
    constexpr plane( plane&& other ) noexcept = default;

    /// \brief Constructs this plane by copy-constructing from an existing one
    ///
    /// \param other the other plane to copy
    constexpr plane( const plane& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to \c this through
    ///        move-assignment
    ///
    /// \param other the other plane to move
    /// \return reference to \c (*this)
    plane& operator=( plane&& other ) noexcept = default;

    /// \brief Assigns the contents of \p other to \c this through
    ///        copy-assignment
    ///
    /// \param other the other plane to copy
    /// \return reference to \c (*this)
    plane& operator=( const plane& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this plane
    ///
    /// \return reference to \c (*this)
    ALLOY_CORE_API
    plane& normalize() noexcept;

    /// \brief Inverts this plane's direction
    ///
    /// \return reference to \c (*this)
    ALLOY_CORE_API
    plane& invert() noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    constexpr real a() const noexcept;
    constexpr real b() const noexcept;
    constexpr real c() const noexcept;
    constexpr real d() const noexcept;

    /// \brief Gets the normal of this plane
    ///
    /// \return the normal of this plane
    constexpr vector3 normal() const noexcept;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the distance between this plane and the specified \p point
    ///
    /// \param p the point to get the distance to
    /// \return the distance
    ALLOY_CORE_API
    real distance_to( const point3& p ) const noexcept;

    /// \brief Gets the projection of the specified vector onto this plane
    ///
    /// \param vec the vector to project
    /// \return the projection
    ALLOY_CORE_API
    vector3 projection( const vector3& vec ) const noexcept;

    /// \brief Gets the rejection of the specified vector off of this plane
    ///
    /// The rejection of the vector is the difference of the normal of the
    /// plane and the projection of the vector.
    ///
    /// \param vec the vector to reject
    /// \return the rejection
    vector3 rejection( const vector3& vec ) const noexcept;

    /// \brief Gets a copy of this plane normalized
    ///
    /// \return the normalized plane
    plane normalized() const noexcept;

    /// \brief Gets a copy of this plane inverted
    ///
    /// \return the inverted plane
    plane inverted() const noexcept;

    /// \brief Checks if this plane intersects the given point \p p
    ///
    /// \param p the point to check for intersection
    /// \return \c true if \p p is in the plane
    ALLOY_CORE_API
    bool contains( const point3& p ) const noexcept;

    /// \brief Checks if this plane intersects the given point \p p
    ///        relative to the given \p tolerance
    ///
    /// \param p the point to check for intersection
    /// \param tolerance the tolerance for accepting the containment
    /// \return \c true if \p p is in the plane
    ALLOY_CORE_API
    bool contains( const point3& p, real tolerance ) const noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    const plane& operator+() const noexcept;
    plane operator-() const noexcept;

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

  constexpr bool operator==( const plane& lhs, const plane& rhs ) noexcept;
  constexpr bool operator!=( const plane& lhs, const plane& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two planes relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left plane
  /// \param rhs the right plane
  /// \return \c true if the two planes contain almost equal values
  constexpr bool almost_equal( const plane& lhs,
                               const plane& rhs ) noexcept;

  /// \brief Determines equality between two planes relative to \p tolerance
  ///
  /// \param lhs the left plane
  /// \param rhs the right plane
  /// \return \c true if the two planes contain almost equal values
  constexpr bool almost_equal( const plane& lhs,
                               const plane& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Checks if the given point \p p is over the \p plane
  ///
  /// \param p the point
  /// \param plane the plane
  /// \return \c true if \p p is over the \p plane
  ALLOY_CORE_API
  bool is_point_over_plane( const point3& p, const plane& plane ) noexcept;

  /// \brief Checks if the given point \p p is under the \p plane
  ///
  /// \param p the point
  /// \param plane the plane
  /// \return \c true if \p p is under the \p plane
  ALLOY_CORE_API
  bool is_point_under_plane( const point3& p, const plane& plane ) noexcept;

} // namespace alloy::core

//==============================================================================
// class : plane
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::plane::plane()
  noexcept
  : plane{0,0,0,0}
{

}

inline constexpr alloy::core::plane::plane( const vector3& normal,
                                            real distance )
  noexcept
  : plane{normal.x(), normal.y(), normal.z(), -distance}
{

}

inline constexpr alloy::core::plane::plane( real a, real b, real c, real d )
  noexcept
  : m_data{a,b,c,d}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real alloy::core::plane::a()
  const noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::real alloy::core::plane::b()
  const noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::real alloy::core::plane::c()
  const noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::real alloy::core::plane::d()
  const noexcept
{
  return m_data[3];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3 alloy::core::plane::normal()
  const noexcept
{
  return {a(),b(),c()};
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline alloy::core::vector3 alloy::core::plane::rejection( const vector3& vec )
  const noexcept
{
  return normal() - projection(vec);
}

inline alloy::core::plane alloy::core::plane::normalized()
  const noexcept
{
  auto copy = (*this);
  copy.normalize();
  return copy;
}

inline alloy::core::plane alloy::core::plane::inverted()
  const noexcept
{
  auto copy = (*this);
  copy.invert();
  return copy;
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline const alloy::core::plane& alloy::core::plane::operator+()
  const noexcept
{
  return (*this);
}

inline alloy::core::plane alloy::core::plane::operator-()
  const noexcept
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

inline constexpr bool alloy::core::operator==( const plane& lhs,
                                               const plane& rhs )
  noexcept
{
  return lhs.a() == rhs.a() &&
         lhs.b() == rhs.b() &&
         lhs.c() == rhs.c() &&
         lhs.d() == rhs.d();
}

inline constexpr bool alloy::core::operator!=( const plane& lhs,
                                               const plane& rhs )
  noexcept
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const plane& lhs,
                                                 const plane& rhs )
  noexcept
{
  return almost_equal(lhs.a(), rhs.a()) &&
         almost_equal(lhs.b(), rhs.b()) &&
         almost_equal(lhs.c(), rhs.c()) &&
         almost_equal(lhs.d(), rhs.d());
}

inline constexpr bool alloy::core::almost_equal( const plane& lhs,
                                                 const plane& rhs,
                                                 real tolerance )
  noexcept
{
  return almost_equal(lhs.a(), rhs.a(), tolerance) &&
         almost_equal(lhs.b(), rhs.b(), tolerance) &&
         almost_equal(lhs.c(), rhs.c(), tolerance) &&
         almost_equal(lhs.d(), rhs.d(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_PLANE_HPP */
