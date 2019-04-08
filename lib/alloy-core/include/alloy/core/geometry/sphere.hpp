/******************************************************************************
 * \file point.hpp
 *
 * \brief This header contains an implementation of a 3d point location
 ******************************************************************************/

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
#ifndef ALLOY_CORE_GEOMETRY_SPHERE_HPP
#define ALLOY_CORE_GEOMETRY_SPHERE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/math/math.hpp" // core::almost_equal
#include "alloy/core/geometry/point.hpp"// core::point

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
    sphere( const point& center, real radius ) noexcept;

    /// \brief Constructs a sphere by moving an existing instance
    ///
    /// \param other the other sphere to move
    constexpr sphere( sphere&& other ) noexcept = default;

    /// \brief Constructs a sphere by copying an existing instance
    ///
    /// \param other the other sphere to copy
    constexpr sphere( const sphere& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns the contents of an existing sphere
    ///
    /// \param other the other sphere to move
    /// \return reference to \c (*this)
    sphere& operator=( sphere&& other ) noexcept = default;

    /// \brief Copy-assigns the contents of an existing sphere
    ///
    /// \param other the other sphere to copy
    /// \return reference to \c (*this)
    sphere& operator=( const sphere& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the center point of this sphere
    ///
    /// \return the center point
    constexpr const point& center() const noexcept;

    /// \brief Gets the radius of this sphere
    ///
    /// \return the radius
    constexpr const real& radius() const noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point m_center; ///< The center point of the sphere
    real  m_radius; ///< The radius of the sphere
  };

  //============================================================================
  // non-member functions : class : sphere
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr bool operator==( const sphere& lhs, const sphere& rhs ) noexcept;
  constexpr bool operator!=( const sphere& lhs, const sphere& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two spheres relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left sphere
  /// \param rhs the right sphere
  /// \return \c true if the two spheres contain almost equal values
  constexpr bool almost_equal( const sphere& lhs, const sphere& rhs ) noexcept;

  /// \brief Determines equality between two spheres relative to \p tolerance
  ///
  /// \param lhs the left sphere
  /// \param rhs the right sphere
  /// \return \c true if the two spheres contain almost equal values
  constexpr bool almost_equal( const sphere& lhs,
                               const sphere& rhs,
                               real tolerance ) noexcept;

} // namespace alloy::core

//==============================================================================
// class : sphere
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::sphere::sphere()
  noexcept
  : m_center{0,0,0},
    m_radius{0}
{

}

inline alloy::core::sphere::sphere( const point& center, real radius )
  noexcept
  : m_center{center},
    m_radius{radius}
{
  ALLOY_ASSERT( radius >= 0, "radius must be positive" );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr const alloy::core::point& alloy::core::sphere::center()
  const noexcept
{
  return m_center;
}

inline constexpr const alloy::core::real& alloy::core::sphere::radius()
  const noexcept
{
  return m_radius;
}

//==============================================================================
// non-member functions : class : sphere
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline constexpr bool alloy::core::operator==( const sphere& lhs,
                                               const sphere& rhs )
  noexcept
{
  return lhs.center() == rhs.center() && lhs.radius() == rhs.radius();
}

inline constexpr bool alloy::core::operator!=( const sphere& lhs,
                                               const sphere& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const sphere& lhs,
                                                 const sphere& rhs )
  noexcept
{
  return almost_equal(lhs.center(), rhs.center()) &&
         almost_equal(lhs.radius(), rhs.radius());
}

inline constexpr bool alloy::core::almost_equal( const sphere& lhs,
                                                 const sphere& rhs,
                                                 real tolerance )
  noexcept
{
  return almost_equal(lhs.center(), rhs.center(), tolerance) &&
         almost_equal(lhs.radius(), rhs.radius(), tolerance);
}


#endif /* ALLOY_CORE_GEOMETRY_SPHERE_HPP */