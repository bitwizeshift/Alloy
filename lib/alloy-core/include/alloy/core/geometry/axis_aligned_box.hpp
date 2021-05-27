/*****************************************************************************
 * \file axis_aligned_box.hpp
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
#ifndef ALLOY_CORE_GEOMETRY_AXIS_ALIGNED_BOX_HPP
#define ALLOY_CORE_GEOMETRY_AXIS_ALIGNED_BOX_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"
#include "alloy/core/precision.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/geometry/plane.hpp"

#include <array> // std::array

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A representation of a box that is aligned with the x, y, and z
  ///        axes.
  ///
  /// Such a box is useful for making rough intersection calculations for an
  /// initial check, before performing more in-depth intersection checks.
  /// For example, a large model may contain one of these around the entire
  /// model, and -- if an intersection occurs with this box -- a more
  /// complicated intersection may be performed on all sub-parts of the model.
  //////////////////////////////////////////////////////////////////////////////
  class axis_aligned_box
  {
    //--------------------------------------------------------------------------
    // Static Factory Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an aligned box from a starting point, and a vector
    ///        indicating the size
    ///
    /// \param p the point
    /// \param size the vector that determines the x, y, z size
    /// \return the created box
    ALLOY_CORE_API
    static axis_aligned_box from_point_and_size( const point3& p,
                                                 const vector3& size ) noexcept;

    /// \brief Constructs an aligned box constrainted between the two points
    ///        \p p0 and \p p1
    ///
    /// \note Unlike the constructor for this class, this factory does not have
    ///       any preconditions on the points passed to this
    ///
    /// \param p0 the first point
    /// \param p1 the second point
    /// \return the created box
    ALLOY_CORE_API
    static axis_aligned_box from_points( const point3& p0,
                                         const point3& p1 ) noexcept;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an axis-aligned bounding-box from points from two
    ///        opposite ends
    ///
    /// \pre \c lower_left.x()<=top_right.x()
    /// \pre \c lower_left.y()<=top_right.y()
    /// \pre \c lower_left.z()<=top_right.z()
    ///
    /// \param lower_left The first point
    /// \param top_right The second point
    axis_aligned_box( const point3& lower_left,
                      const point3& top_right ) noexcept;

    /// \brief Constructs an axis-aligned bounding-box by moving the contents
    ///        from an existing one
    ///
    /// \param other the other box to move
    axis_aligned_box( axis_aligned_box&& other ) noexcept = default;

    /// \brief Constructs an axis-aligned bounding-box by copying the contents
    ///        from an existing one
    ///
    /// \param other the other box to copy
    axis_aligned_box( const axis_aligned_box& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of an axis-aligned bounding-box to this one
    ///
    /// \param other the other box to move
    /// \return reference to \c (*this)
    axis_aligned_box& operator=( axis_aligned_box&& other ) noexcept = default;

    /// \brief Assigns the contents of an axis-aligned bounding-box to this one
    ///
    /// \param other the other box to copy
    /// \return reference to \c (*this)
    axis_aligned_box& operator=( const axis_aligned_box& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers : Normals
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Get the normal to the specified side
    ///
    /// \return the normal for the specified side
    vector3 top_normal() const noexcept;
    vector3 bottom_normal() const noexcept;
    vector3 front_normal() const noexcept;
    vector3 back_normal() const noexcept;
    vector3 left_normal() const noexcept;
    vector3 right_normal() const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Observers : Planes
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Get the plane that defines the specified side
    ///
    /// \return the plane that define sthe specified side
    ALLOY_CORE_API plane top_plane() const noexcept;
    ALLOY_CORE_API plane bottom_plane() const noexcept;
    ALLOY_CORE_API plane front_plane() const noexcept;
    ALLOY_CORE_API plane back_plane() const noexcept;
    ALLOY_CORE_API plane left_plane() const noexcept;
    ALLOY_CORE_API plane right_plane() const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Observers : Points
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the point at the bottom left of this box
    ///
    /// \return the point at the bottom-left of this box
    point3 bottom_left_point() const noexcept;

    /// \brief Gets the point at the top-right of this box
    ///
    /// \return the point at the top-right of this box
    point3 top_right_point() const noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets an array containing the 6 normals
    ///
    /// \return the normals
    ALLOY_CORE_API std::array<vector3,6> normals() const noexcept;

    /// \brief Gets an array containing the 6 planes that encompass this box
    ///
    /// \return the planes
    ALLOY_CORE_API std::array<plane,6> planes() const noexcept;

    /// \brief Gets the distance between this box and the specified \p point
    ///
    /// \param p the point to get the distance to
    /// \return the distance
    real distance_to( const point3& p ) const noexcept;

    /// \brief Checks if this box contains the given point \p p
    ///
    /// \param p the point to check for intersection
    /// \return \c true if \p p is in the box
    ALLOY_CORE_API bool contains( const point3& p ) const noexcept;

    /// \brief Checks if this box contains the given point \p p
    ///        relative to the given \p tolerance
    ///
    /// \param p the point to check for intersection
    /// \param tolerance the tolerance for accepting the containment
    /// \return \c true if \p p is in the box
    ALLOY_CORE_API bool contains( const point3& p, real tolerance ) const noexcept;

    /// \brief Checks if this box intersect another box
    ///
    /// \return \c true if this box intersects another box
    ALLOY_CORE_API bool intersects( const axis_aligned_box& other ) const noexcept;

    /// \brief Checks if this box completely encloses another box
    ///
    /// \return \c true if this box completely encloses another box
    ALLOY_CORE_API bool encloses( const axis_aligned_box& other ) const noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    point3 m_bottom_left;
    point3 m_top_right;
  };

  //============================================================================
  // non-member functions : class : axis_aligned_box
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  ALLOY_CORE_API
  bool operator==(const axis_aligned_box& lhs,
                  const axis_aligned_box& rhs) noexcept;
  ALLOY_CORE_API
  bool operator!=(const axis_aligned_box& lhs,
                  const axis_aligned_box& rhs) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two planes relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left plane
  /// \param rhs the right plane
  /// \return \c true if the two planes contain almost equal values
  ALLOY_CORE_API
  bool almost_equal(const axis_aligned_box& lhs,
                    const axis_aligned_box& rhs) noexcept;

  /// \brief Determines equality between two planes relative to \p tolerance
  ///
  /// \param lhs the left plane
  /// \param rhs the right plane
  /// \param tolerance the tolerance for the comparison
  /// \return \c true if the two planes contain almost equal values
  ALLOY_CORE_API
  bool almost_equal(const axis_aligned_box& lhs,
                    const axis_aligned_box& rhs,
                    real tolerance) noexcept;

} // namespace alloy::core

//==============================================================================
// class : axis_aligned_box
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline alloy::core::axis_aligned_box
  ::axis_aligned_box( const point3& lower_left, const point3& top_right )
  noexcept
  : m_bottom_left{lower_left},
    m_top_right{top_right}
{
  ALLOY_ASSERT( m_bottom_left.x() <= m_top_right.x() );
  ALLOY_ASSERT( m_bottom_left.y() <= m_top_right.y() );
  ALLOY_ASSERT( m_bottom_left.z() <= m_top_right.z() );
}

//------------------------------------------------------------------------------
// Observers : Normals
//------------------------------------------------------------------------------

inline alloy::core::vector3 alloy::core::axis_aligned_box::top_normal()
  const noexcept
{
  return top_plane().normal();
}

inline alloy::core::vector3 alloy::core::axis_aligned_box::bottom_normal()
  const noexcept
{
  return bottom_plane().normal();
}

inline alloy::core::vector3 alloy::core::axis_aligned_box::front_normal()
  const noexcept
{
  return front_plane().normal();
}

inline alloy::core::vector3 alloy::core::axis_aligned_box::back_normal()
  const noexcept
{
  return back_plane().normal();
}

inline alloy::core::vector3 alloy::core::axis_aligned_box::left_normal()
  const noexcept
{
  return left_plane().normal();
}

inline alloy::core::vector3 alloy::core::axis_aligned_box::right_normal()
  const noexcept
{
  return right_plane().normal();
}

//------------------------------------------------------------------------------
// Observers : Points
//------------------------------------------------------------------------------

inline alloy::core::point3 alloy::core::axis_aligned_box::bottom_left_point()
  const noexcept
{
  return m_bottom_left;
}

inline alloy::core::point3 alloy::core::axis_aligned_box::top_right_point()
  const noexcept
{
  return m_top_right;
}

#endif /* ALLOY_CORE_GEOMETRY_AXIS_ALIGNED_BOX_HPP */
