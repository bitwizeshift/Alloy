/******************************************************************************
 * \file point.hpp
 *
 * \brief This header contains an implementation of a point in 3 dimensional
 *        space
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
#ifndef ALLOY_CORE_GEOMETRY_POINT_POINT_HPP
#define ALLOY_CORE_GEOMETRY_POINT_POINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/utilities/piecewise_compare.hpp" // core::piecewise_compare
#include "alloy/core/math/vector/vector3.hpp" // core::vector3
#include "alloy/core/math/math.hpp"           // core::almost_equal

#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <type_traits> // std::true_type, std::false_type

namespace alloy::core {

  //============================================================================
  // class : point
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This class represents a coordinate in 3d space
  ///
  //////////////////////////////////////////////////////////////////////////////
  class point3
  {
    //--------------------------------------------------------------------------
    // Public Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = real;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using size_type  = std::size_t;
    using index_type = std::ptrdiff_t;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a point at the origin
    constexpr point3() noexcept;

    /// \brief Constructs a point at coordinates \c {x,y,z}
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    constexpr point3( real x, real y, real z ) noexcept;

    /// \brief Move-constructs a point from another point
    ///
    /// \param other the other point to move
    constexpr point3( point3&& other ) noexcept = default;

    /// \brief Copy-constructs a point from another point
    ///
    /// \param other the other point to copy
    constexpr point3( const point3& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns a point from another point
    ///
    /// \param other the other point to move
    /// \return reference to \c (*this)
    point3& operator=( point3&& other ) noexcept = default;

    /// \brief Copy-assigns a point from another point
    ///
    /// \param other the other point to copy
    /// \return reference to \c (*this)
    point3& operator=( const point3& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    point3& operator+=( const vector3& rhs ) noexcept;
    point3& operator-=( const vector3& rhs ) noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the x component of this point
    ///
    /// \return reference to the x component
    constexpr reference x() noexcept;
    constexpr const_reference x() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the y component of this point
    ///
    /// \return reference to the y component
    constexpr reference y() noexcept;
    constexpr const_reference y() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the z component of this point
    ///
    /// \return reference to the z component
    constexpr reference z() noexcept;
    constexpr const_reference z() const noexcept;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets an element to the \p nth element
    ///
    /// \param n the index to get
    /// \return the nth element
    constexpr reference operator[]( index_type n ) noexcept;
    constexpr const_reference operator[]( index_type n ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    real m_data[3]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : point
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr vector3 operator-( const point3& lhs, const point3& rhs ) noexcept;
  constexpr point3 operator+( const point3& lhs, const vector3& rhs ) noexcept;
  constexpr point3 operator-( const point3& lhs, const vector3& rhs ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const point3& lhs, const point3& rhs ) noexcept;
  constexpr bool operator!=( const point3& lhs, const point3& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two point relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left point
  /// \param rhs the right point
  /// \return \c true if the two point contain almost equal values
  constexpr bool almost_equal( const point3& lhs, const point3& rhs ) noexcept;

  /// \brief Determines equality between two point relative to \p tolerance
  ///
  /// \param lhs the left point
  /// \param rhs the right point
  /// \return \c true if the two point contain almost equal values
  constexpr bool almost_equal( const point3& lhs,
                               const point3& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Converts a point \p p to a \ref vector3
  ///
  /// \param p the point to convert
  /// \return the vector3
  constexpr vector3 to_vector( const point3& p ) noexcept;

  //============================================================================
  // struct : piecewise_compare<point>
  //============================================================================

  template<>
  struct piecewise_compare<point3>
  {
    constexpr bool operator()( const point3& lhs,
                               const point3& rhs ) noexcept;
  };

} // namespace alloy::core

//==============================================================================
// class : point
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::point3::point3()
  noexcept
  : m_data{0,0,0}
{

}

inline constexpr alloy::core::point3::point3( real x, real y, real z )
  noexcept
  : m_data{x,y,z}
{

}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline alloy::core::point3& alloy::core::point3::operator+=( const vector3& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] + rhs[i];
  }
  return (*this);
}

inline alloy::core::point3& alloy::core::point3::operator-=( const vector3& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] - rhs[i];
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::point3::reference alloy::core::point3::x()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::point3::const_reference alloy::core::point3::x()
  const noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::point3::reference alloy::core::point3::y()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::point3::const_reference alloy::core::point3::y()
  const noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::point3::reference alloy::core::point3::z()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::point3::const_reference alloy::core::point3::z()
  const noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::point3::pointer alloy::core::point3::data()
  noexcept
{
  return &m_data[0];
}

inline constexpr alloy::core::point3::const_pointer alloy::core::point3::data()
  const noexcept
{
  return &m_data[0];
}

inline constexpr alloy::core::point3::reference
  alloy::core::point3::operator[]( index_type n )
  noexcept
{
  ALLOY_ASSERT( n >= 0 && n < 3 );
  return m_data[n];
}

inline constexpr alloy::core::point3::const_reference
  alloy::core::point3::operator[]( index_type n )
  const noexcept
{
  ALLOY_ASSERT( n >= 0 && n < 3 );
  return m_data[n];
}

//==============================================================================
// non-member functions : class : point
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3
  alloy::core::operator-( const point3& lhs, const point3& rhs )
  noexcept
{
  auto result = vector3{};
  for (auto i = 0; i < 3; ++i ) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

inline constexpr alloy::core::point3
  alloy::core::operator+( const point3& lhs, const vector3& rhs )
  noexcept
{
  auto result = point3{};
  for (auto i = 0; i < 3; ++i ) {
    result[i] = lhs[i] + rhs[i];
  }
  return result;
}

inline constexpr alloy::core::point3
  alloy::core::operator-( const point3& lhs, const vector3& rhs )
  noexcept
{
  auto result = point3{};
  for (auto i = 0; i < 3; ++i ) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==( const point3& lhs,
                                               const point3& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=( const point3& lhs,
                                               const point3& rhs )
  noexcept
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const point3& lhs,
                                                 const point3& rhs )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    if (!almost_equal(lhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::almost_equal( const point3& lhs,
                                                 const point3& rhs,
                                                 real tolerance )
  noexcept
{
  for (auto i = 0; i < 3; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector3 alloy::core::to_vector( const point3& p )
  noexcept
{
  return {p.x(), p.y(), p.z()};
}

//==============================================================================
// struct : piecewise_compare<point>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool
  alloy::core::piecewise_compare<alloy::core::point3>
  ::operator()( const point3& lhs, const point3& rhs )
  noexcept
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() == rhs.y()) ?
             (lhs.z() < rhs.z()) :
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_GEOMETRY_POINT_POINT_HPP */
