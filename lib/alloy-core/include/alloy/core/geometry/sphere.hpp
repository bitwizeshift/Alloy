/******************************************************************************
 * \file sphere.hpp
 *
 * \brief This header contains an implementation of a 3d sphere geometric
 *        object
 ******************************************************************************/

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
#ifndef ALLOY_CORE_GEOMETRY_SPHERE_HPP
#define ALLOY_CORE_GEOMETRY_SPHERE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/math/math.hpp" // core::almost_equal
#include "alloy/core/geometry/point/point3.hpp"// core::point

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
    sphere(const point3& center, real radius) noexcept;

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
    constexpr auto radius() const noexcept -> const real&;

    /// \brief Checks whether a given point \p p is contained within this
    ///        sphere
    ///
    /// \param p the point to check
    /// \return `true` if the point is contained in this sphere
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks whether a given point \p p is contained within this
    ///        sphere within a specified error \p tolerance
    ///
    /// \param p the point to check
    /// \param tolerance the error tolerance to accept
    /// \return `true` if the point is contained in this sphere
    [[nodiscard]] ALLOY_CORE_API
    auto contains(const point3& p, real tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3 m_center; ///< The center point of the sphere
    real  m_radius; ///< The radius of the sphere
  };

  //============================================================================
  // non-member functions : class : sphere
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const sphere& lhs, const sphere& rhs) noexcept -> bool;
  constexpr auto operator!=(const sphere& lhs, const sphere& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two spheres relative to \p tolerance
  ///
  /// \param lhs the left sphere
  /// \param rhs the right sphere
  /// \return `true` if the two spheres contain almost equal values
  constexpr auto almost_equal(const sphere& lhs, const sphere& rhs, real tolerance)
    noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : sphere
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::sphere::sphere()
  noexcept
  : m_center{0,0,0},
    m_radius{0}
{

}

inline
alloy::core::sphere::sphere(const point3& center, real radius)
  noexcept
  : m_center{center},
    m_radius{radius}
{
  ALLOY_ASSERT( radius >= 0, "radius must be positive" );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::sphere::center()
  const noexcept -> const point3&
{
  return m_center;
}

inline constexpr
auto alloy::core::sphere::radius()
  const noexcept -> const real&
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
  real tolerance
) noexcept -> bool
{
  return almost_equal(lhs.center(), rhs.center(), tolerance) &&
         almost_equal(lhs.radius(), rhs.radius(), tolerance);
}


#endif /* ALLOY_CORE_GEOMETRY_SPHERE_HPP */
