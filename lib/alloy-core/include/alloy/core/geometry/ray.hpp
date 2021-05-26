/*****************************************************************************
 * \file ray.hpp
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
#ifndef ALLOY_CORE_GEOMETRY_RAY_HPP
#define ALLOY_CORE_GEOMETRY_RAY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/math/vector/vector3.hpp" // core::vector3
#include "alloy/core/geometry/point3.hpp" // core::point

namespace alloy::core {

  //============================================================================
  // class : ray
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A geometric primitive of a ray
  ///
  /// A ray is a segment of line originating at a given point that extends
  /// infinitely in a given direction.
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ray
  {
    //--------------------------------------------------------------------------
    // Static Factory Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a ray that begins at the point \p origin, and crosses
    ///        through the point \p p
    ///
    /// \param origin the start of the ray
    /// \param p a point that this ray passes through
    /// \return a ray that starts at \p origin and travels through \p p
    static ray from_points( const point3& origin,  const point3& p ) noexcept;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a ray which exists at the origin, and casts out in the
    ///        direction of the x-axis
    ///
    /// \note the choice of axis is arbitrary -- this is only done in order to
    ///       maintain the invariants of this class, that \p direction is a
    ///       unit vector
    constexpr ray() noexcept;

    /// \brief Constructs this ray given the \p origin of the ray, and the
    ///        \p direction it faces
    ///
    /// \pre \p direction is normalized (a unit vector)
    ///
    /// \param origin the point where the ray begins
    /// \param direction the direction the ray travels in
    ray( const point3& origin, const vector3& direction ) noexcept;

    /// \brief Constructs a ray by moving an existing instance
    ///
    /// \param other the other ray to move
    constexpr ray( ray&& other ) noexcept = default;

    /// \brief Constructs a ray by copying an existing instance
    ///
    /// \param other the other ray to copy
    constexpr ray( const ray& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns the contents of an existing ray
    ///
    /// \param other the other ray to move
    /// \return reference to \c (*this)
    ray& operator=( ray&& other ) noexcept = default;

    /// \brief Copy-assigns the contents of an existing ray
    ///
    /// \param other the other ray to copy
    /// \return reference to \c (*this)
    ray& operator=( const ray& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts the direction of this ray
    ///
    /// \post \c this->direction() points in the oposite direction
    ///
    /// \return reference to \c (*this)
    ray& invert() noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the origin of this ray
    ///
    /// \return the origin of this ray
    constexpr const point3& origin() const noexcept;

    /// \brief Gets the direction this ray faces
    ///
    /// \return the direction of this ray
    constexpr const vector3& direction() const noexcept;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the inverse of this ray as a copy, leaving this instance
    ///        in-tact
    ///
    /// \return an inverted ray
    ray inverse() const noexcept;

    /// \brief Gets the point at distance \p dt from the origin of this ray
    ///
    /// \pre \p dt must be greater than 0
    /// \param dt the distance to get this point at
    /// \return the point at the distance
    point3 point_at_distance( real dt ) const noexcept;

    /// \brief Checks if this ray intersects the given point \p p
    ///
    /// \param p the point to check for intersection
    /// \return \c true if \p p is in the ray
    bool contains( const point3& p ) const noexcept;

    /// \brief Checks if this ray intersects the given point \p p relative to
    ///        the given \p tolerance
    ///
    /// \param p the point to check for intersection
    /// \param tolerance the tolerance for accepting the containment
    /// \return \c true if \p p is in the ray
    bool contains( const point3& p, real tolerance ) const noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3  m_origin;    ///< Where this point originates from
    vector3 m_direction; ///< The direction this ray moves in
  };

  //============================================================================
  // non-member functions : class : ray
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr bool operator==( const ray& lhs, const ray& rhs ) noexcept;
  constexpr bool operator!=( const ray& lhs, const ray& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two rays relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left ray
  /// \param rhs the right ray
  /// \return \c true if the two rays contain almost equal values
  constexpr bool almost_equal( const ray& lhs, const ray& rhs ) noexcept;

  /// \brief Determines equality between two rays relative to \p tolerance
  ///
  /// \param lhs the left ray
  /// \param rhs the right ray
  /// \return \c true if the two rays contain almost equal values
  constexpr bool almost_equal( const ray& lhs,
                               const ray& rhs,
                               real tolerance ) noexcept;

} // namespace alloy::core

//==============================================================================
// class : ray
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::ray::ray()
  noexcept
  : m_origin{0,0,0},
    m_direction{1,0,0}
{

}

inline alloy::core::ray::ray( const point3& origin,
                              const vector3& direction )
  noexcept
  : m_origin{origin},
    m_direction{direction}
{
  // Unfortunately, due to this assertion, 'ray' cannot be constexpr
  ALLOY_ASSERT( almost_equal(direction.normalized(),direction),
                "'direction' must be a normalized vector" );
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline alloy::core::ray& alloy::core::ray::invert()
  noexcept
{
  m_direction.invert();
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr const alloy::core::point3& alloy::core::ray::origin()
  const noexcept
{
  return m_origin;
}

inline constexpr const alloy::core::vector3& alloy::core::ray::direction()
  const noexcept
{
  return m_direction;
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline alloy::core::ray alloy::core::ray::inverse()
  const noexcept
{
  auto copy = (*this);
  return copy.invert();
}

//==============================================================================
// non-member functions : class : ray
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==( const ray& lhs, const ray& rhs )
  noexcept
{
  return lhs.origin() == rhs.origin() && lhs.direction() == rhs.direction();
}

inline constexpr bool alloy::core::operator!=( const ray& lhs, const ray& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const ray& lhs,
                                                 const ray& rhs )
  noexcept
{
  return almost_equal(lhs.origin(), rhs.origin()) &&
         almost_equal(lhs.direction(), rhs.direction());
}

inline constexpr bool alloy::core::almost_equal( const ray& lhs,
                                                 const ray& rhs,
                                                 real tolerance )
  noexcept
{
  return almost_equal(lhs.origin(), rhs.origin(), tolerance) &&
         almost_equal(lhs.direction(), rhs.direction(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_RAY_HPP */
