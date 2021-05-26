////////////////////////////////////////////////////////////////////////////////
/// \file point2.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_GEOMETRY_POINT2_HPP
#define ALLOY_CORE_GEOMETRY_POINT2_HPP

#if defined(_MSC_VER)
# pragma once
#endif // defined(_MSC_VER)

#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"    // ALLOY_ASSERT
#include "alloy/core/utilities/piecewise_compare.hpp" // core::piecewise_compare
#include "alloy/core/math/vector/vector2.hpp" // core::vector2
#include "alloy/core/math/math.hpp"           // core::almost_equal

#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <type_traits> // std::true_type, std::false_type

namespace alloy::core {

  //============================================================================
  // class : point2
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This class represents a cartesian coordinate in 2d space
  //////////////////////////////////////////////////////////////////////////////
  class point2
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
    constexpr point2() noexcept;

    /// \brief Constructs a point at coordinates \c {x,y,z}
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    constexpr point2(real x, real y) noexcept;

    /// \brief Move-constructs a point from another point
    ///
    /// \param other the other point to move
    constexpr point2(point2&& other) noexcept = default;

    /// \brief Copy-constructs a point from another point
    ///
    /// \param other the other point to copy
    constexpr point2(const point2& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns a point from another point
    ///
    /// \param other the other point to move
    /// \return reference to \c (*this)
    auto operator=(point2&& other) noexcept -> point2& = default;

    /// \brief Copy-assigns a point from another point
    ///
    /// \param other the other point to copy
    /// \return reference to \c (*this)
    auto operator=(const point2& other) noexcept -> point2& = default;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    auto operator+=(const vector2& rhs) noexcept -> point2&;
    auto operator-=(const vector2& rhs) noexcept -> point2&;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the x component of this point
    ///
    /// \return reference to the x component
    constexpr auto x() noexcept -> reference;
    constexpr auto x() const noexcept-> const_reference;
    /// \}

    /// \{
    /// \brief Gets the y component of this point
    ///
    /// \return reference to the y component
    constexpr auto y() noexcept -> reference;
    constexpr auto y() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr auto data() noexcept -> pointer;
    constexpr auto data() const noexcept -> const_pointer;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets an element to the \p nth element
    ///
    /// \param n the index to get
    /// \return the nth element
    constexpr auto operator[](index_type n) noexcept -> reference;
    constexpr auto operator[](index_type n) const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    real m_data[2]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : point
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr auto operator-(const point2& lhs, const point2& rhs) noexcept -> vector2;
  constexpr auto operator+(const point2& lhs, const vector2& rhs) noexcept -> point2;
  constexpr auto operator-(const point2& lhs, const vector2& rhs) noexcept -> point2;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const point2& lhs, const point2& rhs) noexcept -> bool;
  constexpr auto operator!=(const point2& lhs, const point2& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two point relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left point
  /// \param rhs the right point
  /// \return \c true if the two point contain almost equal values
  constexpr auto almost_equal(const point2& lhs, const point2& rhs) noexcept -> bool;

  /// \brief Determines equality between two point relative to \p tolerance
  ///
  /// \param lhs the left point
  /// \param rhs the right point
  /// \return \c true if the two point contain almost equal values
  constexpr auto almost_equal(const point2& lhs,
                              const point2& rhs,
                              real tolerance) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Converts a point \p p to a \ref vector2
  ///
  /// \param p the point to convert
  /// \return the vector2
  constexpr auto to_vector(const point2& p) noexcept -> vector2;

  //============================================================================
  // struct : piecewise_compare<point>
  //============================================================================

  template<>
  struct piecewise_compare<point2>
  {
    constexpr auto operator()(const point2& lhs, const point2& rhs)
      noexcept -> bool;
  };

} // namespace alloy::core

//==============================================================================
// class : point
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::point2::point2()
  noexcept
  : m_data{0,0}
{

}

inline constexpr
alloy::core::point2::point2(real x, real y)
  noexcept
  : m_data{x,y}
{

}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::point2::operator+=(const vector2& rhs)
  noexcept -> point2&
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] + rhs[i];
  }
  return (*this);
}

inline
auto alloy::core::point2::operator-=(const vector2& rhs)
  noexcept -> point2&
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] - rhs[i];
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::point2::x()
  noexcept -> reference
{
  return m_data[0];
}

inline constexpr
auto alloy::core::point2::x()
  const noexcept -> const_reference
{
  return m_data[0];
}

inline constexpr
auto alloy::core::point2::y()
  noexcept -> reference
{
  return m_data[1];
}

inline constexpr
auto alloy::core::point2::y()
  const noexcept -> const_reference
{
  return m_data[1];
}

inline constexpr
auto alloy::core::point2::data()
  noexcept -> pointer
{
  return &m_data[0];
}

inline constexpr
auto alloy::core::point2::data()
  const noexcept -> const_pointer
{
  return &m_data[0];
}

inline constexpr
auto alloy::core::point2::operator[](index_type n)
  noexcept -> reference
{
  ALLOY_ASSERT(n >= 0 && n < 3);
  return m_data[n];
}

inline constexpr
auto alloy::core::point2::operator[](index_type n)
  const noexcept -> const_reference
{
  ALLOY_ASSERT(n >= 0 && n < 3);
  return m_data[n];
}

//==============================================================================
// non-member functions : class : point
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator-(const point2& lhs, const point2& rhs)
  noexcept -> vector2
{
  auto result = vector2{};
  for (auto i = 0; i < 2; ++i ) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

inline constexpr
auto alloy::core::operator+(const point2& lhs, const vector2& rhs)
  noexcept -> point2
{
  auto result = point2{};
  for (auto i = 0; i < 2; ++i ) {
    result[i] = lhs[i] + rhs[i];
  }
  return result;
}

inline constexpr
auto alloy::core::operator-(const point2& lhs, const vector2& rhs)
  noexcept -> point2
{
  auto result = point2{};
  for (auto i = 0; i < 2; ++i ) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const point2& lhs, const point2& rhs )
  noexcept -> bool
{
  for (auto i = 0; i < 2; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const point2& lhs, const point2& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const point2& lhs, const point2& rhs)
  noexcept -> bool
{
  for (auto i = 0; i < 2; ++i) {
    if (!almost_equal(lhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr
auto alloy::core::almost_equal(const point2& lhs,
                               const point2& rhs,
                               real tolerance)
  noexcept -> bool
{
  for (auto i = 0; i < 2; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::to_vector(const point2& p)
  noexcept -> vector2
{
  return {p.x(), p.y()};
}

//==============================================================================
// struct : piecewise_compare<point>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::piecewise_compare<alloy::core::point2>
  ::operator()(const point2& lhs, const point2& rhs)
  noexcept -> bool
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_GEOMETRY_POINT2_HPP */
