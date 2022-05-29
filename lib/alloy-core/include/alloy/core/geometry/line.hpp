/*****************************************************************************
 * \file line.hpp
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
#ifndef ALLOY_CORE_GEOMETRY_LINE_HPP
#define ALLOY_CORE_GEOMETRY_LINE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/math/vector/vector3.hpp" // core::vector3
#include "alloy/core/geometry/point/point3.hpp" // core::point

namespace alloy::core {

  //============================================================================
  // class : line
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A representation of an unbounded line.
  ///
  /// A line is unbounded, and stretches from negative infinity to positive
  /// infinity.
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API line
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs a line which runs through the origin in the
    ///        direction of the x-axis
    constexpr line() noexcept;

    /// \brief Constructs a line that passes through \p origin and moves in
    ///        \p direction
    ///
    /// \pre \p direction is normalized
    /// \param origin a point to intersect
    /// \param direction the direction the line extends toward
    constexpr line(const point3& origin, const vector3& direction) noexcept;

    /// \brief Constructs a line by moving an existing instance
    ///
    /// \param other the other line to move
    line(line&& other) = default;

    /// \brief Constructs a line by copying an existing instance
    ///
    /// \param other the other line to copy
    line(const line& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns the contents of an existing line
    ///
    /// \param other the other line to move
    /// \return reference to `(*this)`
    auto operator=(line&& other) -> line& = default;

    /// \brief Copy-assigns the contents of an existing line
    ///
    /// \param other the other line to copy
    /// \return reference to `(*this)`
    auto operator=(const line& other) -> line& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------

    /// \brief Gets the origin point of this line
    ///
    /// \return the origin point of the line
    [[nodiscard]] constexpr auto origin() const noexcept -> const point3&;

    /// \brief Gets the direction that this line extends in
    ///
    /// \return the direction
    [[nodiscard]] constexpr auto direction() const noexcept -> const vector3&;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the point at distance \p dt from the origin of this
    ///        line
    ///
    /// \param dt the distance to get this point at
    /// \return the point at the distance
    [[nodiscard]]
    auto point_at_distance(real dt) const noexcept -> point3;

    /// \brief Checks if this line intersects the given point \p p
    ///
    /// \param p the point to check for intersection
    /// \return `true` if \p p is in the line
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks if this line intersects the given point \p p
    ///        relative to the given \p tolerance
    ///
    /// \param p the point to check for intersection
    /// \param tolerance the tolerance for accepting the containment
    /// \return `true` if \p p is in the line
    [[nodiscard]]
    auto contains(const point3& p, real tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3  m_origin;    ///< The origin point
    vector3 m_direction; ///< The direction
  };

  //============================================================================
  // non-member functions : class : line
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  ALLOY_CORE_API
  auto operator==(const line& lhs, const line& rhs) noexcept -> bool;
  auto operator!=(const line& lhs, const line& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two lines relative to
  ///        \p tolerance
  ///
  /// \param lhs the left line
  /// \param rhs the right line
  /// \return `true` if the two lines contain almost equal values
  ALLOY_CORE_API
  auto almost_equal(const line& lhs, const line& rhs, real tolerance)
    noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : line
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::line::line()
  noexcept
  : m_origin{real{0}, real{0}, real{0}},
    m_direction{real{1}, real{0}, real{0}}
{

}

inline constexpr
alloy::core::line::line(const point3& origin, const vector3& direction)
  noexcept
  : m_origin{origin},
    m_direction{direction}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE constexpr
auto alloy::core::line::origin()
  const noexcept -> const point3&
{
  return m_origin;
}

ALLOY_FORCE_INLINE constexpr
auto alloy::core::line::direction()
  const noexcept -> const vector3&
{
  return m_direction;
}

//==============================================================================
// non-member functions : class : line
//==============================================================================

inline
auto alloy::core::operator!=(const line& lhs, const line& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

#endif /* ALLOY_CORE_GEOMETRY_LINE_HPP */
