/******************************************************************************
 * \file point3.hpp
 *
 * \brief This header contains an implementation of a point in 3 dimensional
 *        space
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
#ifndef ALLOY_CORE_GEOMETRY_POINT_POINT3_HPP
#define ALLOY_CORE_GEOMETRY_POINT_POINT3_HPP

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

    /// \brief Constructs a point at coordinates ``{x,y,z}
    ///
    /// \param x the x-coordinate
    /// \param y the y-coordinate
    /// \param z the z-coordinate
    constexpr point3(real x, real y, real z) noexcept;

    /// \brief Copy-constructs a point from another point
    ///
    /// \param other the other point to copy
    constexpr point3(const point3& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a point from another point
    ///
    /// \param other the other point to copy
    /// \return reference to `(*this)`
    auto operator=(const point3& other) noexcept -> point3& = default;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:
    auto operator+=(const vector3& rhs) noexcept -> point3&;
    auto operator-=(const vector3& rhs) noexcept -> point3&;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the x component of this point
    ///
    /// \return reference to the x component
    constexpr auto x() noexcept -> reference;
    constexpr auto x() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the y component of this point
    ///
    /// \return reference to the y component
    constexpr auto y() noexcept -> reference;
    constexpr auto y() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the z component of this point
    ///
    /// \return reference to the z component
    constexpr auto z() noexcept -> reference;
    constexpr auto z() const noexcept -> const_reference;
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

    real m_data[3]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : point
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr auto operator-(const point3& lhs, const point3& rhs) noexcept -> vector3;
  constexpr auto operator+(const point3& lhs, const vector3& rhs) noexcept -> point3;
  constexpr auto operator-(const point3& lhs, const vector3& rhs) noexcept -> point3;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const point3& lhs, const point3& rhs) noexcept -> bool;
  constexpr auto operator!=(const point3& lhs, const point3& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two point relative to \p tolerance
  ///
  /// \param lhs the left point
  /// \param rhs the right point
  /// \return `true` if the two point contain almost equal values
  constexpr auto almost_equal(const point3& lhs, const point3& rhs, real tolerance)
    noexcept -> bool;

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  /// \brief Converts a point \p p to a \ref vector3
  ///
  /// \param p the point to convert
  /// \return the vector3
  constexpr auto to_vector(const point3& p) noexcept -> vector3;

  //============================================================================
  // struct : piecewise_compare<point>
  //============================================================================

  template<>
  struct piecewise_compare<point3>
  {
    constexpr auto operator()(const point3& lhs, const point3& rhs)
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
alloy::core::point3::point3()
  noexcept
  : m_data{0,0,0}
{

}

inline constexpr
alloy::core::point3::point3(real x, real y, real z)
  noexcept
  : m_data{x,y,z}
{

}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::point3::operator+=(const vector3& rhs)
  noexcept -> point3&
{
  for (auto i = 0; i < 3; ++i) {
    m_data[i] = m_data[i] + rhs[i];
  }
  return (*this);
}

inline auto alloy::core::point3::operator-=(const vector3& rhs)
  noexcept -> point3&
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
auto alloy::core::point3::x()
  noexcept -> reference
{
  return m_data[0];
}

inline constexpr
auto alloy::core::point3::x()
  const noexcept -> const_reference
{
  return m_data[0];
}

inline constexpr
auto alloy::core::point3::y()
  noexcept -> reference
{
  return m_data[1];
}

inline constexpr
auto alloy::core::point3::y()
  const noexcept -> const_reference
{
  return m_data[1];
}

inline constexpr
auto alloy::core::point3::z()
  noexcept -> reference
{
  return m_data[2];
}

inline constexpr
auto alloy::core::point3::z()
  const noexcept -> const_reference
{
  return m_data[2];
}

inline constexpr
auto alloy::core::point3::data()
  noexcept -> pointer
{
  return &m_data[0];
}

inline constexpr
auto alloy::core::point3::data()
  const noexcept -> const_pointer
{
  return &m_data[0];
}

inline constexpr
auto alloy::core::point3::operator[](index_type n)
  noexcept -> reference
{
  return m_data[n];
}

inline constexpr
auto alloy::core::point3::operator[](index_type n)
  const noexcept -> const_reference
{
  return m_data[n];
}

//==============================================================================
// non-member functions : class : point
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator-(const point3& lhs, const point3& rhs)
  noexcept -> vector3
{
  auto result = vector3{};
  for (auto i = 0u; i < 3u; ++i ) {
    result[i] = lhs[i] - rhs[i];
  }
  return result;
}

inline constexpr
auto alloy::core::operator+(const point3& lhs, const vector3& rhs)
  noexcept -> point3
{
  auto result = point3{};
  for (auto i = 0u; i < 3u; ++i ) {
    result[i] = lhs[i] + rhs[i];
  }
  return result;
}

inline constexpr
auto alloy::core::operator-(const point3& lhs, const vector3& rhs)
  noexcept -> point3
{
  auto result = point3{};
  for (auto i = 0u; i < 3u; ++i ) {
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
auto alloy::core::operator==(const point3& lhs, const point3& rhs)
  noexcept -> bool
{
  for (auto i = 0; i < 3; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const point3& lhs, const point3& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const point3& lhs, const point3& rhs, real tolerance)
  noexcept -> bool
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

inline constexpr
auto alloy::core::to_vector(const point3& p)
  noexcept -> vector3
{
  return {p.x(), p.y(), p.z()};
}

//==============================================================================
// struct : piecewise_compare<point>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::piecewise_compare<alloy::core::point3>::operator()(
  const point3& lhs,
  const point3& rhs
) noexcept -> bool
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() == rhs.y()) ?
             (lhs.z() < rhs.z()) :
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_GEOMETRY_POINT_POINT3_HPP */
