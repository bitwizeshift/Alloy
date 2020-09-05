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
  class ALLOY_CORE_API vector4
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

    /// \brief Default constructs a vector4 with 0 values
    constexpr vector4() noexcept;

    /// \brief Constructs a vector4 with components \p x and \p y
    ///
    /// \param x the x-component of the vector4
    /// \param y the y-component of the vector4
    /// \param z the z-component of the vector4
    /// \param w the w-component of the vector4
    constexpr vector4(real x, real y, real z, real w) noexcept;

    /// \brief Copy-constructs a vector4 with the value of another
    ///        vector4
    ///
    /// \param other the other vector4 to copy
    constexpr vector4(const vector4& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Copy-assigns \p other to \c this
    ///
    /// \param other the other vector4 to copy
    /// \return reference to \c (*this)
    auto operator=(const vector4& other) -> vector4& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector4
    ///
    /// \return the number of components in the vector4
    static constexpr auto size() noexcept -> size_type;

    /// \{
    /// \brief Gets the x component of this vector
    ///
    /// \return reference to the x component
    constexpr auto x() noexcept -> reference;
    constexpr auto x() const noexcept -> const_reference;
    /// \}xx

    /// \{
    /// \brief Gets the y component of this vector
    ///
    /// \return reference to the y component
    constexpr auto y() noexcept -> reference;
    constexpr auto y() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the z component of this vector
    ///
    /// \return reference to the z component
    constexpr auto z() noexcept -> reference;
    constexpr auto z() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the w component of this vector
    ///
    /// \return reference to the w component
    constexpr auto w() noexcept -> reference;
    constexpr auto w() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr auto data() noexcept -> pointer;
    constexpr auto data() const noexcept -> const_pointer;
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
    constexpr auto at(index_type n) -> reference;
    constexpr auto at(index_type n) const -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 4 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr auto operator[](index_type n) noexcept -> reference;
    constexpr auto operator[](index_type n) const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Determines the dot-product of \c this and \p other
    ///
    /// \param other the other vector4 to perform the dot-product with
    /// \return the dot product of \c this and \p other
    constexpr auto dot(const vector4& other) const noexcept -> real;

    /// \brief Calculates the cross-product of \c this and \p other
    ///
    /// \note This cross-product is not a true 4-dimensional cross-product,
    ///       it is a cross product constrained to 3-dimensions, clearing
    ///       the w-entry
    ///
    /// \param other the other vector4 to perform the cross-product with
    /// \return the cross product of \c this and \p other
    constexpr auto cross(const vector4& other) const noexcept -> vector4;

    /// \brief Gets the square magnitude of this vector4
    ///
    /// \return the square magnitude of the vector4
    constexpr auto square_magnitude() const noexcept -> real;

    /// \brief Gets the magnitude of this vector4
    ///
    /// \return the magnitude of the vector4
    auto magnitude() const noexcept -> real;

    /// \brief Gets the midpoint between \c this and \p other
    ///
    /// \param other the vector4 to get the midpoint from
    /// \return the midpoint between \c this and \p other
    constexpr auto midpoint(const vector4& other) const noexcept -> vector4;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    constexpr auto projection(const vector4& vector) const noexcept -> vector4;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    constexpr auto rejection(const vector4& vector) const noexcept -> vector4;

    /// \brief Gets the normalized vector4 of \c this
    ///
    /// \return the normalized vector4 of \c this
    auto normalized() const noexcept -> vector4;

    /// \brief Gets the inverse of \c this vector4
    ///
    /// \return the inverse of \c this vector4
    constexpr auto inverse() const noexcept -> vector4;

    /// \brief Queries whether this vector is normalized
    ///
    /// \return \c true if this vector is normalized
    constexpr auto is_normalized() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector4 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    auto normalize() noexcept -> vector4&;

    /// \brief Inverts this vector4 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr auto invert() noexcept -> vector4&;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr auto operator+() const noexcept -> const vector4&;
    constexpr auto operator-() const noexcept -> vector4;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    constexpr auto operator+=(const vector4& rhs) noexcept -> vector4&;
    constexpr auto operator-=(const vector4& rhs) noexcept -> vector4&;
    constexpr auto operator*=(real scalar) noexcept -> vector4&;
    constexpr auto operator/=(real scalar) noexcept -> vector4&;

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

  constexpr auto operator+(const vector4& lhs,
                           const vector4& rhs) noexcept -> vector4;
  constexpr auto operator-(const vector4& lhs,
                           const vector4& rhs) noexcept -> vector4;
  constexpr auto operator*(const vector4& lhs, real scalar) noexcept -> vector4;
  constexpr auto operator*(real scalar, const vector4& lhs) noexcept -> vector4;
  constexpr auto operator/(const vector4& lhs, real scalar) noexcept -> vector4;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const vector4& lhs,
                            const vector4& rhs) noexcept -> bool;
  constexpr auto operator!=(const vector4& lhs,
                            const vector4& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two vector4 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return \c true if the two vector4 contain almost equal values
  constexpr auto almost_equal(const vector4& lhs,
                              const vector4& rhs) noexcept -> bool;

  /// \brief Determines equality between two vector4 relative to \p tolerance
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return \c true if the two vector4 contain almost equal values
  constexpr auto almost_equal(const vector4& lhs,
                              const vector4& rhs,
                              real tolerance) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return the result of the dot product
  constexpr auto dot(const vector4& lhs, const vector4& rhs) noexcept -> real;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return the result of the dot product
  constexpr auto cross(const vector4& lhs,
                       const vector4& rhs) noexcept -> vector4;

  /// \brief Calculates the magnitude of the vector4 \p vec
  ///
  /// \param vec the vector4 to calculate the magnitude from
  /// \return the magnitude
  auto magnitude(const vector4& vec) noexcept -> real;

  //============================================================================
  // struct : piecewise_compare<vector4>
  //============================================================================

  template<>
  struct piecewise_compare<vector4>
  {
    constexpr auto
      operator()(const vector4& lhs, const vector4& rhs) noexcept -> bool;
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

inline constexpr 
alloy::core::vector4::vector4()
  noexcept
  : m_data{
      real{0},
      real{0},
      real{0},
      real{0}
    }
{

}

inline constexpr 
alloy::core::vector4::vector4(real x, real y, real z, real w)
  noexcept
  : m_data{x,y,z,w}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::size()
  noexcept -> size_type
{
  return 4;
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::x()
  noexcept -> reference
{
  return m_data[0];
}

inline constexpr
auto alloy::core::vector4::x()
  const noexcept
  -> const_reference
{
  return m_data[0];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::y()
  noexcept -> reference
{
  return m_data[1];
}

inline constexpr
auto alloy::core::vector4::y()
  const noexcept
  -> const_reference
{
  return m_data[1];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::z()
  noexcept -> reference
{
  return m_data[2];
}

inline constexpr
auto alloy::core::vector4::z()
  const noexcept
  -> const_reference
{
  return m_data[2];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::w()
  noexcept -> reference
{
  return m_data[3];
}

inline constexpr
auto alloy::core::vector4::w()
  const noexcept
  -> const_reference
{
  return m_data[3];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::data()
  noexcept -> pointer
{
  return m_data;
}

inline constexpr
auto alloy::core::vector4::data()
  const noexcept
  -> const_pointer
{
  return m_data;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::at(index_type n)
  -> reference
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 4 || n < 0)
  {
    throw std::out_of_range("alloy::core::vector4::at: index out of range");
  }
#else
  ALLOY_ASSERT(n < 4 && n >= 0);
#endif
  return m_data[n];
}

inline constexpr
auto alloy::core::vector4::at(index_type n)
  const
  -> const_reference
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 4 || n < 0)
  {
    throw std::out_of_range("alloy::core::vector4::at: index out of range");
  }
#else
  ALLOY_ASSERT(n < 4 && n >= 0);
#endif
  return m_data[n];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::operator[](index_type n)
  noexcept
  -> reference
{
  ALLOY_ASSERT(n < 4 && n >= 0);
  return m_data[n];
}

inline constexpr
auto alloy::core::vector4::operator[](index_type n)
  const noexcept
  -> const_reference
{
  ALLOY_ASSERT(n < 4 && n >= 0);
  return m_data[n];
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::dot(const vector4& other) const noexcept
  -> real
{
  // Use a loop to suggest vectorized math
  auto sum = real{0};

  for (auto i = 0; i < 4; ++i) {
    sum += m_data[i] * other.m_data[i];
  }

  return sum;
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::cross(const vector4& other) const noexcept
  -> vector4
{
  return vector4{
    (y() * other.z() - z()
  * other.y()),
    (z() * other.x() - x() * other.z()),
    (x() * other.y() - y() * other.x()),
    0
  };
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::square_magnitude()
  const noexcept -> real
{
  return dot(*this);
}

inline constexpr
auto alloy::core::vector4::midpoint(const vector4& other)
  const noexcept
  -> vector4
{
  return vector4{
    ((x() + other.x()) * real{0.5}),
    ((y() + other.y()) * real{0.5}),
    ((z() + other.z()) * real{0.5}),
    ((w() + other.w()) * real{0.5})
  };
}

inline constexpr
auto alloy::core::vector4::projection(const vector4& vector)
  const noexcept
  -> vector4
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector4{
    multiplier * x(),
    multiplier * y(),
    multiplier * z(),
    multiplier * w()
  };
}

inline constexpr
auto alloy::core::vector4::rejection(const vector4& vector)
  const noexcept
  -> vector4
{
  return (*this) - projection(vector);
}

inline
auto alloy::core::vector4::normalized()
  const noexcept -> vector4
{
  return vector4{*this}.normalize();
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::inverse()
  const noexcept -> vector4
{
  return vector4{*this}.invert();
}

inline constexpr
auto alloy::core::vector4::is_normalized()
  const noexcept -> bool
{
  return almost_equal(square_magnitude(), real{1});
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::invert()
  noexcept -> vector4&
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= real{-1};
  }

  return (*this);
}

//------------------------------------------------------------------------------
// Unary Operators
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::operator+()
  const noexcept
  -> const alloy::core::vector4&
{
  return (*this);
}

inline constexpr
auto alloy::core::vector4::operator-()
  const noexcept -> vector4
{
  return inverse();
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector4::operator+=(const vector4& rhs)
  noexcept
  -> vector4&
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] += rhs.m_data[i];
  }
  return (*this);
}

inline constexpr
auto alloy::core::vector4::operator-=(const vector4& rhs)
  noexcept
  -> vector4&
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] -= rhs.m_data[i];
  }

  return (*this);
}

inline constexpr
auto alloy::core::vector4::operator*=(real scalar)
  noexcept
  -> vector4&
{
  for (auto i = 0; i < 4; ++i) {
    m_data[i] *= scalar;
  }

  return (*this);
}

inline constexpr
auto alloy::core::vector4::operator/=(real scalar)
  noexcept
  -> vector4&
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

inline constexpr
auto alloy::core::operator+(const vector4& lhs, const vector4& rhs)
  noexcept
  -> vector4
{
  return vector4(lhs)+=rhs;
}

inline constexpr
auto alloy::core::operator-(const vector4& lhs, const vector4& rhs)
  noexcept
  -> vector4
{
  return vector4(lhs)-=rhs;
}

inline constexpr
auto alloy::core::operator*(const vector4& lhs, real scalar)
  noexcept
  -> vector4
{
  return vector4(lhs)*=scalar;
}

inline constexpr
auto alloy::core::operator*(real scalar, const vector4& lhs)
  noexcept
  -> vector4
{
  return vector4(lhs)*=scalar;
}

inline constexpr
auto alloy::core::operator/(const vector4& lhs, real scalar)
  noexcept
  -> vector4
{
  return vector4(lhs)/=scalar;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const vector4& lhs, const vector4& rhs)
  noexcept
  -> bool
{
  for (auto i=0; i<4; ++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const vector4& lhs, const vector4& rhs)
  noexcept
  -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//----------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const vector4& lhs, const vector4& rhs)
  noexcept
  -> bool
{
  return almost_equal(lhs, rhs, default_tolerance);
}

inline constexpr
auto alloy::core::almost_equal(const vector4& lhs,
                                                const vector4& rhs,
                                                real tolerance)
  noexcept -> bool
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

inline constexpr
auto alloy::core::dot(const vector4& lhs, const vector4& rhs)
  noexcept
  -> real
{
  return lhs.dot(rhs);
}

inline constexpr
auto alloy::core::cross(const vector4& lhs, const vector4& rhs)
  noexcept
  -> vector4
{
  return lhs.cross(rhs);
}

inline
auto alloy::core::magnitude(const vector4& vec)
  noexcept -> real
{
  return vec.magnitude();
}

//==============================================================================
// struct : piecewise_compare<vector4>
//==============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::piecewise_compare<alloy::core::vector4>
  ::operator()(const vector4& lhs,
               const vector4& rhs)
  noexcept -> bool
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
