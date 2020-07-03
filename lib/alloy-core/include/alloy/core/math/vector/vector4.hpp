/******************************************************************************
 * \file vector4.hpp
 *
 * \brief This header contains the definition of a 4d vector class
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
#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR4_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR4_HPP

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
  // class : vector4
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This object represents a 4-component vector in linear algebra.
  ///
  /// Most operations are marked \c constexpr to allow for compile-time
  /// forward-calculations of vector sums and products.
  ///
  /// Operations on this type are able to promote results to reduce loss of
  /// data, depending on what the operands are.
  //////////////////////////////////////////////////////////////////////////////
  class vector4
  {
    //--------------------------------------------------------------------------
    // Static Constants
    //--------------------------------------------------------------------------
  public:

    ALLOY_CORE_API static const vector4 zero;
    ALLOY_CORE_API static const vector4 unit_x;
    ALLOY_CORE_API static const vector4 unit_y;
    ALLOY_CORE_API static const vector4 unit_z;
    ALLOY_CORE_API static const vector4 unit_w;
    ALLOY_CORE_API static const vector4 neg_unit_x;
    ALLOY_CORE_API static const vector4 neg_unit_y;
    ALLOY_CORE_API static const vector4 neg_unit_z;
    ALLOY_CORE_API static const vector4 neg_unit_w;

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

    /// \brief Default constructs a vector4 with 0 values
    constexpr vector4() noexcept;

    /// \brief Constructs a vector4 with components \p x and \p y
    ///
    /// \param x the x-component of the vector4
    /// \param y the y-component of the vector4
    /// \param z the z-component of the vector4
    /// \param w the w-component of the vector4
    constexpr vector4( real x, real y, real z, real w ) noexcept;

    /// \brief Copy-constructs a vector4 with the value of another
    ///        vector4
    ///
    /// \param other the other vector4 to copy
    constexpr vector4( const vector4& other ) noexcept = default;

    /// \brief Move-constructs a vector4 with the value of another
    ///        vector4
    ///
    /// \param other the other vector4 to move
    constexpr vector4( vector4&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Copy-assigns \p other to \c this
    ///
    /// \param other the other vector4 to copy
    /// \return reference to \c (*this)
    vector4& operator=( const vector4& other ) = default;

    /// \brief Move-assigns \p other to \c this
    ///
    /// \param other the other vector4 to move
    /// \return reference to \c (*this)
    vector4& operator=( vector4&& other ) = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector4
    ///
    /// \return the number of components in the vector4
    constexpr size_type size() const noexcept;

    /// \{
    /// \brief Gets the x component of this vector
    ///
    /// \return reference to the x component
    constexpr reference x() noexcept;
    constexpr const_reference x() const noexcept;
    /// \}xx

    /// \{
    /// \brief Gets the y component of this vector
    ///
    /// \return reference to the y component
    constexpr reference y() noexcept;
    constexpr const_reference y() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the z component of this vector
    ///
    /// \return reference to the z component
    constexpr reference z() noexcept;
    constexpr const_reference z() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the w component of this vector
    ///
    /// \return reference to the w component
    constexpr reference w() noexcept;
    constexpr const_reference w() const noexcept;
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
    /// \throw std::out_of_range if \p n >= 4 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr reference at( index_type n );
    constexpr const_reference at( index_type n ) const;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 4 or \p n < 0
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
    /// \param rhs the other vector4 to perform the dot-product with
    /// \return the dot product of \c this and \p rhs
    constexpr real dot( const vector4& rhs ) const noexcept;

    /// \brief Calculates the cross-product of \c this and \p rhs
    ///
    /// \note This cross-product is not a true 4-dimensional cross-product,
    ///       it is a cross product constrainted to 3-dimensions, clearing
    ///       the w-entry
    ///
    /// \param rhs the other vector4 to perform the cross-product with
    /// \return the cross product of \c this and \p rhs
    constexpr vector4 cross( const vector4& rhs ) const noexcept;

    /// \brief Gets the square magnitude of this vector4
    ///
    /// \return the square magnitude of the vector4
    constexpr real square_magnitude() const noexcept;

    /// \brief Gets the magnitude of this vector4
    ///
    /// \return the magnitude of the vector4
    real magnitude() const noexcept;

    /// \brief Gets the midpoint between \c this and \p vec
    ///
    /// \param vec the vector4 to get the midpoint from
    /// \return the midpoint between \c this and \p vec
    constexpr vector4 midpoint( const vector4& vec ) const noexcept;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    constexpr vector4 projection( const vector4& vector ) const noexcept;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    constexpr vector4 rejection( const vector4& vector ) const noexcept;

    /// \brief Gets the normalized vector4 of \c this
    ///
    /// \return the normalized vector4 of \c this
    vector4 normalized() const noexcept;

    /// \brief Gets the inverse of \c this vector4
    ///
    /// \return the inverse of \c this vector4
    constexpr vector4 inverse() const noexcept;

    /// \brief Queries whether this vector is normalized
    ///
    /// \return \c true if this vector is normalized
    constexpr bool is_normalized() const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector4 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    vector4& normalize() noexcept;

    /// \brief Inverts this vector4 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr vector4& invert() noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr const vector4& operator+() const noexcept;
    constexpr vector4 operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    constexpr vector4& operator+=( const vector4& rhs ) noexcept;
    constexpr vector4& operator-=( const vector4& rhs ) noexcept;
    constexpr vector4& operator*=( real scalar ) noexcept;
    constexpr vector4& operator/=( real scalar ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    value_type m_data[4]; ///< The storage data
  };

  //============================================================================
  // trait : is_vector4
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref vector4
  ///
  /// The result is aliased as \c ::value
  template<typename T>
  struct is_vector4 : std::false_type{};

  template<>
  struct is_vector4<vector4> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector4::value
  template<typename T>
  constexpr bool is_vector4_v = is_vector4<T>::value;

  //============================================================================
  // non-member functions : class : vector4
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr vector4 operator+( const vector4& lhs,
                               const vector4& rhs ) noexcept;
  constexpr vector4 operator-( const vector4& lhs,
                               const vector4& rhs ) noexcept;
  constexpr vector4 operator*( const vector4& lhs,
                               real scalar ) noexcept;
  constexpr vector4 operator*( real scalar,
                               const vector4& rhs ) noexcept;
  constexpr vector4 operator/( const vector4& lhs,
                               real scalar ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const vector4& lhs,
                             const vector4& rhs ) noexcept;
  constexpr bool operator!=( const vector4& lhs,
                             const vector4& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two vector4 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return \c true if the two vector4 contain almost equal values
  constexpr bool almost_equal( const vector4& lhs,
                               const vector4& rhs ) noexcept;

  /// \brief Determines equality between two vector4 relative to \p tolerance
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return \c true if the two vector4 contain almost equal values
  constexpr bool almost_equal( const vector4& lhs,
                               const vector4& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return the result of the dot product
  constexpr real dot( const vector4& lhs, const vector4& rhs ) noexcept;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return the result of the dot product
  constexpr vector4 cross( const vector4& lhs, const vector4& rhs ) noexcept;

  /// \brief Calculates the magnitude of the vector4 \p vec
  ///
  /// \param vec the vector4 to calculate the magnitude from
  /// \return the magnitude
  real magnitude( const vector4& vec ) noexcept;

  //============================================================================
  // struct : piecewise_compare<vector4>
  //============================================================================

  template<>
  struct piecewise_compare<vector4>
  {
    constexpr bool operator()( const vector4& lhs,
                               const vector4& rhs ) noexcept;
  };

  //============================================================================
  // aliases
  //============================================================================

  using vec4 = vector4;

} // namespace alloy::core


//==============================================================================
// class : vector4
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::vector4()
  noexcept
  : m_data{
      real{0},
      real{0},
      real{0},
      real{0}
    }
{

}

inline constexpr alloy::core::vector4::vector4( real x,
                                                real y,
                                                real z,
                                                real w )
  noexcept
  : m_data{x,y,z,w}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::size_type
  alloy::core::vector4::size()
  const noexcept
{
  return 4;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::x()
  noexcept
{
  return m_data[0];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::x()
  const noexcept
{
  return m_data[0];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::y()
  noexcept
{
  return m_data[1];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::y()
  const noexcept
{
  return m_data[1];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::z()
  noexcept
{
  return m_data[2];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::z()
  const noexcept
{
  return m_data[2];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::w()
  noexcept
{
  return m_data[3];
}

inline constexpr alloy::core::vector4::const_reference
  alloy::core::vector4::w()
  const noexcept
{
  return m_data[3];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::pointer
  alloy::core::vector4::data()
  noexcept
{
  return m_data;
}

inline constexpr alloy::core::vector4::const_pointer
  alloy::core::vector4::data()
  const noexcept
{
  return m_data;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::at( index_type n )
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector4::at: index out of range");
  }
#else
  ALLOY_ASSERT( n < 4 && n >= 0 );
#endif
  return m_data[n];
}

inline constexpr alloy::core::vector4::const_reference
alloy::core::vector4::at( index_type n )
  const
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if( n >= 4 || n < 0 ) {
    throw std::out_of_range("alloy::core::vector4::at: index out of range");
  }
#else
  ALLOY_ASSERT( n < 4 && n >= 0 );
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4::reference
  alloy::core::vector4::operator[]( index_type n )
  noexcept
{
  ALLOY_ASSERT( n < 4 && n >= 0 );
  return m_data[n];
}

inline constexpr alloy::core::vector4::const_reference
alloy::core::vector4::operator[]( index_type n )
  const noexcept
{
  ALLOY_ASSERT( n < 4 && n >= 0 );
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector4::dot( const vector4& other )
  const noexcept
{
  // Use a loop to suggest vectorized math
  auto sum = real{0};

  for (auto i = 0; i < 4; ++i) {
    sum += m_data[i] * other.m_data[i];
  }

  return sum;
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4
  alloy::core::vector4::cross( const vector4& other )
  const noexcept
{
  return vector4{
    (y() * other.z() - z() * other.y()),
    (z() * other.x() - x() * other.z()),
    (x() * other.y() - y() * other.x()),
    0
  };
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::vector4::square_magnitude()
  const noexcept
{
  return dot(*this);
}

inline alloy::core::real
  alloy::core::vector4::magnitude()
  const noexcept
{
  return sqrt( dot(*this) );
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::midpoint( const vector4& rhs )
  const noexcept
{
  return vector4{
    ((x() + rhs.x()) * real{0.5}),
    ((y() + rhs.y()) * real{0.5}),
    ((z() + rhs.z()) * real{0.5}),
    ((w() + rhs.w()) * real{0.5})
  };
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::projection( const vector4& vector )
  const noexcept
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector4{
    multiplier * x(),
    multiplier * y(),
    multiplier * z(),
    multiplier * w()
  };
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::rejection( const vector4& vector )
  const noexcept
{
  return (*this) - projection( vector );
}

inline alloy::core::vector4
  alloy::core::vector4::normalized()
  const noexcept
{
  return vector4{*this}.normalize();
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4
  alloy::core::vector4::inverse()
  const noexcept
{
  return vector4{*this}.invert();
}

inline constexpr bool
  alloy::core::vector4::is_normalized()
  const noexcept
{
  return almost_equal(square_magnitude(), real{1});
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline alloy::core::vector4&
  alloy::core::vector4::normalize()
  noexcept
{
  const auto square_mag = square_magnitude();

  if (almost_equal(square_mag, real{1})) {
    return (*this);
  }

  if (square_mag > real{0}) {
    const auto mag_inv = real{1} / sqrt(square_mag);

    for (auto i = 0; i < 4; ++i) {
      m_data[i] *= mag_inv;
    }
  }

  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::invert()
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= real{-1};
  }

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline constexpr const alloy::core::vector4&
  alloy::core::vector4::operator+()
  const noexcept
{
  return (*this);
}

inline constexpr alloy::core::vector4
  alloy::core::vector4::operator-()
  const noexcept
{
  return inverse();
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator+=( const vector4& rhs )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] += rhs.m_data[i];
  }
  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator-=( const vector4& rhs )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] -= rhs.m_data[i];
  }

  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator*=( real scalar )
  noexcept
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= scalar;
  }

  return (*this);
}

inline constexpr alloy::core::vector4&
  alloy::core::vector4::operator/=( real scalar )
  noexcept
{
  const auto inv = (real{1} / scalar);

  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= inv;
  }

  return (*this);
}

//==============================================================================
// non-member functions : class : vector4
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline constexpr alloy::core::vector4
  alloy::core::operator+( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return vector4(lhs)+=rhs;
}

inline constexpr alloy::core::vector4
  alloy::core::operator-( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return vector4(lhs)-=rhs;
}

inline constexpr alloy::core::vector4
  alloy::core::operator*( const vector4& lhs, real scalar )
  noexcept
{
  return vector4(lhs)*=scalar;
}

inline constexpr alloy::core::vector4
  alloy::core::operator*( real scalar, const vector4& lhs )
  noexcept
{
  return vector4(lhs)*=scalar;
}

inline constexpr alloy::core::vector4
  alloy::core::operator/( const vector4& lhs, real scalar )
  noexcept
{
  return vector4(lhs)/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool alloy::core::operator==( const vector4& lhs,
                                               const vector4& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::operator!=( const vector4& lhs,
                                               const vector4& rhs )
  noexcept
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//----------------------------------------------------------------------------

inline constexpr bool alloy::core::almost_equal( const vector4& lhs,
                                                 const vector4& rhs )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(lhs[i], rhs[i])) {
      return false;
    }
  }
  return true;
}

inline constexpr bool alloy::core::almost_equal( const vector4& lhs,
                                                 const vector4& rhs,
                                                 real tolerance )
  noexcept
{
  for (auto i=0; i<4; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::dot( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return lhs.dot(rhs);
}

inline constexpr alloy::core::vector4
  alloy::core::cross( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return lhs.cross(rhs);
}

inline alloy::core::real
  alloy::core::magnitude( const vector4& vec )
  noexcept
{
  return vec.magnitude();
}

//==============================================================================
// struct : piecewise_compare<vector4>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr bool
  alloy::core::piecewise_compare<alloy::core::vector4>
  ::operator()( const vector4& lhs, const vector4& rhs )
  noexcept
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() == rhs.y()) ?
             (lhs.z() == rhs.z()) ?
               (lhs.w() < rhs.w()) :
             (lhs.z() < rhs.z()) :
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR4_HPP */
