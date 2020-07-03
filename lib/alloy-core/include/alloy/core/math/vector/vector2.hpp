/******************************************************************************
 * \file vector2.hpp
 *
 * \brief This header contains the definition of a 2D vector class
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
#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR2_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR2_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"    // ALLOY_CORE_API
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp" // ALLOY_ASSERT
#include "alloy/core/config.hpp" // ALLOY_CORE_EXCEPTIONS_ENABLED
#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/utilities/piecewise_compare.hpp" // core::piecewise_compare
#include "alloy/core/math/angle/radian.hpp" // core::radian
#include "alloy/core/math/trigonometry.hpp" // core::cos, core::sin, etc
#include "alloy/core/math/math.hpp"         // core::sqrt

#include <type_traits> // std::true_type, std::false_type, std::common_type
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range

namespace alloy::core {

  //============================================================================
  // class : vector2
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This object represents a 2-component vector in linear algebra.
  ///
  /// Most operations are marked \c constexpr to allow for compile-time
  /// forward-calculations of vector sums and products.
  ///
  /// Operations on this type are able to promote results to reduce loss of
  /// data, depending on what the operands are.
  //////////////////////////////////////////////////////////////////////////////
  class vector2
  {
    //--------------------------------------------------------------------------
    // Static Constants
    //--------------------------------------------------------------------------
  public:

    ALLOY_CORE_API static const vector2 zero;
    ALLOY_CORE_API static const vector2 unit_x;
    ALLOY_CORE_API static const vector2 unit_y;
    ALLOY_CORE_API static const vector2 neg_unit_x;
    ALLOY_CORE_API static const vector2 neg_unit_y;

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
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs a vector2 with 0 values
    constexpr vector2() noexcept;

    /// \brief Constructs a vector2 with a given \p magnitude
    ///        and \p direction
    ///
    /// \param magnitude the distance of the vector
    /// \param direction the angle of the vector
    vector2( real magnitude, radian direction ) noexcept;

    /// \brief Constructs a vector2 with components \p x and \p y
    ///
    /// \param x the x-component of the vector2
    /// \param y the y-component of the vector2
    constexpr vector2( real x, real y ) noexcept;

    /// \brief Copy-constructs a vector2 with the value of another
    ///        vector2
    ///
    /// \param other the other vector2 to copy
    constexpr vector2( const vector2& other ) noexcept = default;

    /// \brief Move-constructs a vector2 with the value of another
    ///        vector2
    ///
    /// \param other the other vector2 to move
    constexpr vector2( vector2&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns \p other to \c this
    ///
    /// \param other the other vector2 to copy
    /// \return reference to \c (*this)
    vector2& operator=( const vector2& other ) noexcept = default;

    /// \brief Move-assigns \p other to \c this
    ///
    /// \param other the other vector2 to move
    /// \return reference to \c (*this)
    vector2& operator=( vector2&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector2
    ///
    /// \return the number of components in the vector2
    constexpr size_type size() const noexcept;

    /// \{
    /// \brief Gets the x component of this vector
    ///
    /// \return reference to the x component
    constexpr reference x() noexcept;
    constexpr const_reference x() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the y component of this vector
    ///
    /// \return reference to the y component
    constexpr reference y() noexcept;
    constexpr const_reference y() const noexcept;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \throw std::out_of_range if \p n >= 2 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr reference at( index_type n );
    constexpr const_reference at( index_type n ) const;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 2 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr reference operator[]( index_type n ) noexcept;
    constexpr const_reference operator[]( index_type n ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Determines the dot-product of \c this and \p rhs
    ///
    /// \param rhs the other vector2 to perform the dot-product with
    /// \return the dot product of \c this and \p rhs
    constexpr real dot( const vector2& rhs ) const noexcept;

    /// \brief Calculates the cross-product of \c this and \p rhs
    ///
    /// \param rhs the other vector2 to perform the cross-product with
    /// \return the cross product of \c this and \p rhs
    constexpr real cross( const vector2& rhs ) const noexcept;

    /// \brief Gets the square magnitude of this vector2
    ///
    /// \return the square magnitude of the vector2
    constexpr real square_magnitude() const noexcept;

    /// \brief Gets the magnitude of this vector2
    ///
    /// \return the magnitude of the vector2
    real magnitude() const noexcept;

    /// \brief Gets the midpoint between \c this and \p vec
    ///
    /// \param vec the vector2 to get the midpoint from
    /// \return the midpoint between \c this and \p vec
    constexpr vector2 midpoint( const vector2& vec ) const noexcept;

    /// \brief Gets the reflection vector2 of \c this reflected through the
    ///        \p normal
    ///
    /// \param normal the normal vector2
    /// \return the reflection
    constexpr vector2 reflection( const vector2& normal ) const noexcept;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    constexpr vector2 projection( const vector2& vector ) const noexcept;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    constexpr vector2 rejection( const vector2& vector ) const noexcept;

    /// \brief Gets the perpendicular of this vector2
    ///
    /// \return the perpendicular vector2 to \c this
    constexpr vector2 perpendicular() const noexcept;

    /// \brief Gets the normalized vector2 of \c this
    ///
    /// \return the normalized vector2 of \c this
    vector2 normalized() const noexcept;

    /// \brief Gets the inverse of \c this vector2
    ///
    /// \return the inverse of \c this vector2
    constexpr vector2 inverse() const noexcept;

    /// \brief Determines the angle between \c this and \p other
    ///
    /// \param other the other vector to determine the angle between
    /// \return the angle between \c this and \p other
    radian angle_between( const vector2& other ) const noexcept;

    /// \brief Determines the angle from \p to \p other
    ///
    /// \param other the other vector to get the angle to
    /// \return the angle from \c this to \p other
    radian angle_to( const vector2& other ) const noexcept;

    /// \brief Queries whether this vector is normalized
    ///
    /// \return \c true if this vector is normalized
    constexpr bool is_normalized() const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector2 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    vector2& normalize() noexcept;

    /// \brief Inverts this vector2 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr vector2& invert() noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr const vector2& operator+() const noexcept;
    constexpr vector2 operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    constexpr vector2& operator+=( const vector2& rhs ) noexcept;
    constexpr vector2& operator-=( const vector2& rhs ) noexcept;
    constexpr vector2& operator*=( real scalar ) noexcept;
    constexpr vector2& operator/=( real scalar ) noexcept;

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
  private:

    real m_data[2]; ///< The storage data
  };

  //============================================================================
  // trait : is_vector2
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref vector2
  ///
  /// The result is aliased as \c ::value
  template<typename T>
  struct is_vector2 : std::false_type{};
  template<>
  struct is_vector2<vector2> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector2<T>::value
  template<typename T>
  inline constexpr bool is_vector2_v = is_vector2<T>::value;

  //============================================================================
  // non-member functions : class : vector2
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr vector2 operator+( const vector2& lhs,
                               const vector2& rhs ) noexcept;
  constexpr vector2 operator-( const vector2& lhs,
                               const vector2& rhs ) noexcept;
  constexpr vector2 operator*( const vector2& lhs,
                               real scalar ) noexcept;
  constexpr vector2 operator*( real scalar, const vector2& rhs ) noexcept;
  constexpr vector2 operator/( const vector2& lhs,
                               real scalar ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const vector2& lhs,
                             const vector2& rhs ) noexcept;
  constexpr bool operator!=( const vector2& lhs,
                             const vector2& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two vector2 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return \c true if the two vector2 contain almost equal values
  constexpr bool almost_equal( const vector2& lhs,
                               const vector2& rhs ) noexcept;

  /// \brief Determines equality between two vector2 relative to \p tolerance
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return \c true if the two vector2 contain almost equal values
  constexpr bool almost_equal( const vector2& lhs,
                               const vector2& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines if two vector2's are linearly independent
  ///
  /// \param v1 the first vector
  /// \param v2 the second vector
  /// \return \c true if the two vectors are linearly independent
  constexpr bool are_linearly_independent( const vector2& v1,
                                           const vector2& v2 ) noexcept;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  constexpr real dot( const vector2& lhs, const vector2& rhs ) noexcept;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  constexpr real cross( const vector2& lhs, const vector2& rhs ) noexcept;

  /// \brief Calculates the magnitude of the vector2 \p vec
  ///
  /// \param vec the vector2 to calculate the magnitude from
  /// \return the magnitude
  real magnitude( const vector2& vec ) noexcept;

  //============================================================================
  // struct : piecewise_compare<vector2>
  //============================================================================

  template<>
  struct piecewise_compare<vector2>
  {
    constexpr bool operator()( const vector2& lhs,
                               const vector2& rhs ) noexcept;
  };

  //============================================================================
  // aliases
  //============================================================================

  using vec2 = vector2;

} // namespace alloy::core

//==============================================================================
// class : vector2
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::vector2()
  noexcept
  : m_data{
      real{0},
      real{0}
    }
{

}

inline alloy::core::vector2::vector2( real magnitude, radian direction )
  noexcept
  : m_data{
      magnitude * trigonometry::cos(direction),
      magnitude * trigonometry::sin(direction)
    }
{

}

inline constexpr alloy::core::vector2::vector2( real x, real y )
  noexcept
  : m_data{x,y}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::size_type
  alloy::core::vector2::size()
  const noexcept
{
  return 2;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::x()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::x()
  const noexcept
{
  return m_data[0];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::y()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::y()
  const noexcept
{
  return m_data[1];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::pointer
  alloy::core::vector2::data()
  noexcept
{
  return &m_data[0];
}

inline constexpr alloy::core::vector2::const_pointer
  alloy::core::vector2::data()
  const noexcept
{
  return &m_data[0];
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::at( index_type n )
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 2 || n < 0) {
    throw std::out_of_range("alloy::core::vector2::at: index out of range");
  }
#else
  assert( n < 2 && n >= 0 );
#endif
  return m_data[n];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::at( index_type n )
  const
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 2 || n < 0) {
    throw std::out_of_range("alloy::core::vector2::at: index out of range");
  }
#else
  ALLOY_ASSERT( n < 2 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2::reference
  alloy::core::vector2::operator[]( index_type n )
  noexcept
{
  ALLOY_ASSERT( n < 2 && n >= 0 );
  return m_data[n];
}

inline constexpr alloy::core::vector2::const_reference
  alloy::core::vector2::operator[]( index_type n )
  const noexcept
{
  ALLOY_ASSERT( n < 2 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector2::dot( const vector2& other )
  const noexcept
{
  return (x() * other.x()) + (y() * other.y());
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector2::cross( const vector2& other )
  const noexcept
{
  return (x() * other.y()) - (y() * other.x());
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector2::square_magnitude()
  const noexcept
{
  return dot(*this);
}

inline alloy::core::real
  alloy::core::vector2::magnitude()
  const noexcept
{
  return sqrt( (x()*x()) + (y()*y()) );
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::midpoint( const vector2& rhs )
  const noexcept
{
  return vector2{
    ((x() + rhs.x()) * real{0.5}),
    ((y() + rhs.y()) * real{0.5})
  };
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::reflection( const vector2& normal )
  const noexcept
{
  return (*this) - (normal * (real{2} * dot(normal)));
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::vector2::projection( const vector2& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector2{
    multiplier * vector.x(),
    multiplier * vector.y()
  };
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::rejection( const vector2& vector )
  const noexcept
{
  return (*this) - projection(vector);
}


//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::vector2::perpendicular()
  const noexcept
{
  return vector2{ -y(), x() };
}

inline alloy::core::vector2
  alloy::core::vector2::normalized()
  const noexcept
{
  return vector2{*this}.normalize();
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::inverse()
  const noexcept
{
  return vector2{ -x(), -y() };
}

//------------------------------------------------------------------------------

inline alloy::core::radian
  alloy::core::vector2::angle_between( const vector2& other )
  const noexcept
{
  auto mag_product = magnitude() * other.magnitude();

  if( almost_equal( mag_product, real{0} ) ){
    mag_product = default_tolerance;
  }

  auto f = dot(other) / mag_product;

  f = clamp( f, real{-1.0}, real{1.0} );
  return trigonometry::arccos( f );
}

inline alloy::core::radian
  alloy::core::vector2::angle_to( const vector2& other )
  const noexcept
{
  auto angle = angle_between( other );

  if( cross(other) < real{0} ) {
    return radian_constants::revolution - angle;
  }

  return angle;
}

inline constexpr bool
  alloy::core::vector2::is_normalized()
  const noexcept
{
  return almost_equal(square_magnitude(), real{1});
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline alloy::core::vector2&
  alloy::core::vector2::normalize()
  noexcept
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, real{1})) {
    return (*this);
  }

  if (square_mag > real{0}) {
    const auto mag_inv = real{1} / sqrt(square_mag);

    x() *= mag_inv;
    y() *= mag_inv;
  }

  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::invert()
  noexcept
{
  x() = -x();
  y() = -y();

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline constexpr const alloy::core::vector2&
  alloy::core::vector2::operator+()
  const noexcept
{
  return (*this);
}

inline constexpr alloy::core::vector2
  alloy::core::vector2::operator-()
  const noexcept
{
  return vector2{ -x(), -y() };
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator+=( const vector2& rhs )
  noexcept
{
  x() += rhs.x();
  y() += rhs.y();
  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator-=( const vector2& rhs )
  noexcept
{
  x() -= rhs.x();
  y() -= rhs.y();
  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator*=( real scalar )
  noexcept
{
  x() *= scalar;
  y() *= scalar;
  return (*this);
}

inline constexpr alloy::core::vector2&
  alloy::core::vector2::operator/=( real scalar )
  noexcept
{
  const auto inv = real{1} / scalar;

  x() *= inv;
  y() *= inv;
  return (*this);
}

//==============================================================================
// non-member functions : class : vector2
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::operator+( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return vector2{lhs}+=rhs;
}

inline constexpr alloy::core::vector2
  alloy::core::operator-( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return vector2{lhs}-=rhs;
}

inline constexpr alloy::core::vector2
  alloy::core::operator*( const vector2& lhs, real scalar )
  noexcept
{
  return vector2{lhs}*=scalar;
}

inline constexpr alloy::core::vector2
  alloy::core::operator*( real scalar, const vector2& lhs )
  noexcept
{
  return vector2{lhs}*=scalar;
}

inline constexpr alloy::core::vector2
  alloy::core::operator/( const vector2& lhs, real scalar )
  noexcept
{
  return vector2{lhs}/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool
  alloy::core::operator==( const vector2& lhs, const vector2& rhs )
  noexcept
{
  for (auto i=0; i<2; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::operator!=( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::almost_equal( const vector2& lhs, const vector2& rhs )
  noexcept
{
  for (auto i=0; i<2; ++i) {
    if (!almost_equal(lhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::almost_equal( const vector2& lhs,
                             const vector2& rhs,
                             real tolerance )
  noexcept
{
  for (auto i=0; i<2; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::are_linearly_independent( const vector2& v1,
                                         const vector2& v2 )
  noexcept
{
  // linear-independence tested by checking if the determinant of a produced
  // 2x2 matrix is non-zero

  const auto determinant = v1.x() * v2.y() - v1.y() * v2.x();
  return !almost_equal( real{0}, determinant );
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::dot( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

inline constexpr alloy::core::real
  alloy::core::cross( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

inline alloy::core::real
  alloy::core::magnitude( const vector2& vec )
  noexcept
{
  return vec.magnitude();
}

//==============================================================================
// struct : piecewise_compare<vector2>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool
  alloy::core::piecewise_compare<alloy::core::vector2>
  ::operator()( const vector2& lhs, const vector2& rhs )
  noexcept
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR2_HPP */
