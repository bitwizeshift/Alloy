/*****************************************************************************
 * \file line_segment.hpp
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
#ifndef ALLOY_CORE_GEOMETRY_LINE_SEGMENT_HPP
#define ALLOY_CORE_GEOMETRY_LINE_SEGMENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/math/vector/vector3.hpp" // core::vector3
#include "alloy/core/geometry/point/point3.hpp" // core::point

namespace alloy::core {

  //============================================================================
  // class : line_segment
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A representation of a line segment enclosed between two points.
  ///
  /// A line segment is completely bounded between the start point, and the
  /// end point.
  ///
  /// \ingroup geometry
  /// \ingroup value_type
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API line_segment
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs a line-segment of length 0
    constexpr line_segment() noexcept;

    /// \brief Constructs a line segment between points \p p0 and \p p1
    ///
    /// \param p0 the start point
    /// \param p1 the end point
    constexpr line_segment(const point3& p0, const point3& p1) noexcept;

    /// \brief Constructs a line segment starting at point \p origin,
    ///        extending out \p distance
    ///
    /// \param origin the start point
    /// \param distance the distance the line segment extends for
    constexpr line_segment(const point3& origin, const vector3& distance) noexcept;

    /// \brief Constructs a line_segment by copying an existing instance
    ///
    /// \param other the other line_segment to copy
    line_segment(const line_segment& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of an existing line_segment
    ///
    /// \param other the other line_segment to copy
    /// \return reference to `(*this)`
    auto operator=(const line_segment& other) -> line_segment& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------

    /// \brief Gets the point that the line segment originates at
    ///
    /// \return the starting point
    [[nodiscard]]
    constexpr auto start_point() const noexcept -> const point3&;

    /// \brief Gets the point that the line segment ends at
    ///
    /// \return the ending point
    [[nodiscard]]
    constexpr auto end_point() const noexcept -> const point3&;

    /// \brief Gets the direction that this line points
    ///
    /// \return the direction
    [[nodiscard]]
    constexpr auto direction() const noexcept -> vector3;

    /// \brief Gets the square magnitude of this length
    ///
    /// \return the square magnitude
    [[nodiscard]]
    constexpr auto square_magnitude() const noexcept -> real;

    /// \brief Gets the magnitude of this line segment
    ///
    /// \return the magnitude of this line segment
    [[nodiscard]]
    auto magnitude() const noexcept -> real;

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the point at distance \p percent from the origin of this
    ///        line_segment
    ///
    /// \pre \p percent must be between 0 and 1
    /// \param percent the distance to get this point at
    /// \return the point at the distance
    [[nodiscard]]
    auto point_at_percent(real percent) const noexcept -> point3;

    /// \brief Checks if this line_segment intersects the given point \p p
    ///
    /// \param p the point to check for intersection
    /// \return `true` if \p p is in the line_segment
    [[nodiscard]]
    auto contains(const point3& p) const noexcept -> bool;

    /// \brief Checks if this line_segment intersects the given point \p p
    ///        relative to the given \p tolerance
    ///
    /// \param p the point to check for intersection
    /// \param tolerance the tolerance for accepting the containment
    /// \return `true` if \p p is in the line_segment
    [[nodiscard]]
    auto contains(const point3& p, real tolerance) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3 m_start; ///< The start point
    point3 m_end;   ///< The end point
  };

  //============================================================================
  // non-member functions : class : line_segment
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  constexpr auto operator==(const line_segment& lhs,
                            const line_segment& rhs) noexcept -> bool;
  constexpr auto operator!=(const line_segment& lhs,
                            const line_segment& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two line_segments relative to
  ///        \p tolerance
  ///
  /// \param lhs the left line_segment
  /// \param rhs the right line_segment
  /// \return `true` if the two line_segments contain almost equal values
  constexpr auto almost_equal(const line_segment& lhs,
                              const line_segment& rhs,
                              real tolerance) noexcept -> bool;

} // namespace alloy::core

//==============================================================================
// class : line_segment
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::line_segment::line_segment()
  noexcept
  : m_start{real{0}, real{0}, real{0}},
    m_end{real{0}, real{0}, real{0}}
{

}

inline constexpr
alloy::core::line_segment::line_segment(const point3& p0, const point3& p1)
  noexcept
  : m_start{p0},
    m_end{p1}
{

}

inline constexpr
alloy::core::line_segment::line_segment(const point3& origin, const vector3& distance)
  noexcept
  : m_start{origin},
    m_end{origin + distance}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr auto alloy::core::line_segment::start_point()
  const noexcept -> const point3&
{
  return m_start;
}

inline constexpr
auto alloy::core::line_segment::end_point()
  const noexcept -> const point3&
{
  return m_end;
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::line_segment::direction()
  const noexcept -> vector3
{
  return (m_end - m_start);
}

inline constexpr
auto alloy::core::line_segment::square_magnitude()
  const noexcept -> real
{
  const auto dir = direction();
  return dir.dot(dir);
}

//==============================================================================
// non-member functions : class : line_segment
//==============================================================================

inline constexpr
auto alloy::core::operator==(const line_segment& lhs, const line_segment& rhs)
  noexcept -> bool
{
  return lhs.start_point() == rhs.start_point() &&
         lhs.end_point() == rhs.end_point();
}

inline constexpr
auto alloy::core::operator!=(const line_segment& lhs, const line_segment& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

//----------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const line_segment& lhs,
                               const line_segment& rhs,
                               real tolerance)
  noexcept -> bool
{
  return almost_equal(lhs.start_point(), rhs.start_point(), tolerance) &&
         almost_equal(lhs.end_point(), rhs.end_point(), tolerance);
}

#endif /* ALLOY_CORE_GEOMETRY_LINE_SEGMENT_HPP */
