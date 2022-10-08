/******************************************************************************
 * \file vector2.hpp
 *
 * \brief This header contains the definition of a 2D vector class
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
#include <tuple>       // std::tuple_element, std::tuple_size

namespace alloy::core {

  //===========================================================================
  // class : vector2
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief This object represents a 2-component vector in linear algebra.
  ///
  /// Most operations are marked `constexpr` to allow for compile-time
  /// forward-calculations of vector sums and products.
  ///
  /// Operations on this type are able to promote results to reduce loss of
  /// data, depending on what the operands are.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API vector2 {
    //-------------------------------------------------------------------------
    // Public Types
    //-------------------------------------------------------------------------
  public:

    using value_type = real;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using reference = value_type &;
    using const_reference = const value_type &;

    using size_type = std::size_t;
    using index_type = std::ptrdiff_t;

    //-------------------------------------------------------------------------
    // Public Types
    //-------------------------------------------------------------------------
  public:

    /// \brief The tolerance to use for almost_equal comparison
    static inline constexpr auto comparison_tolerance = default_tolerance;

    //-------------------------------------------------------------------------
    // Public Static Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a vector2 with a given \p magnitude and \p direction
    ///
    /// \param magnitude the distance of the vector
    /// \param direction the angle of the vector
    /// \return a constructed vector
    static auto from_magnitude_direction(real magnitude,
                                         radian direction) noexcept -> vector2;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructs a vector2 with 0 values
    constexpr vector2() noexcept;

    /// \brief Constructs a vector2 with components \p x and \p y
    ///
    /// \param x the x-component of the vector2
    /// \param y the y-component of the vector2
    constexpr vector2(real x, real y) noexcept;

    /// \brief Copy-constructs a vector2 with the value of another
    ///        vector2
    ///
    /// \param other the other vector2 to copy
    constexpr vector2(const vector2& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copy-assigns \p other to `this`
    ///
    /// \param other the other vector2 to copy
    /// \return reference to `(*this)`
    auto operator=(const vector2& other) noexcept -> vector2& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector2
    ///
    /// \return the number of components in the vector2
    static constexpr auto size() noexcept -> size_type;

    /// \{
    /// \brief Gets the x component of this vector
    ///
    /// \return reference to the x component
    constexpr auto x() noexcept -> reference;
    constexpr auto x() const noexcept -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the y component of this vector
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

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \throw std::out_of_range if \p n >= 2 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr auto at(index_type n) -> reference;
    constexpr auto at(index_type n) const -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 2 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr auto operator[](index_type n) noexcept -> reference;
    constexpr auto operator[](index_type n) const noexcept -> const_reference;
    /// \}

    //-------------------------------------------------------------------------
    // Quantifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Determines the dot-product of `this` and \p other
    ///
    /// \param other the other vector2 to perform the dot-product with
    /// \return the dot product of `this` and \p other
    constexpr auto dot(const vector2& other) const noexcept -> real;

    /// \brief Calculates the cross-product of `this` and \p other
    ///
    /// \param other the other vector2 to perform the cross-product with
    /// \return the cross product of `this` and \p other
    constexpr auto cross(const vector2& other) const noexcept -> real;

    /// \brief Gets the square magnitude of this vector2
    ///
    /// \return the square magnitude of the vector2
    constexpr auto square_magnitude() const noexcept -> real;

    /// \brief Gets the magnitude of this vector2
    ///
    /// \return the magnitude of the vector2
    auto magnitude() const noexcept -> real;

    /// \brief Gets the midpoint between `this` and \p other
    ///
    /// \param other the vector2 to get the midpoint from
    /// \return the midpoint between `this` and \p other
    constexpr auto midpoint(const vector2& other) const noexcept -> vector2;

    /// \brief Gets the reflection vector2 of `this` reflected through the
    ///        \p normal
    ///
    /// \param normal the normal vector2
    /// \return the reflection
    constexpr auto reflection(const vector2& normal) const noexcept -> vector2;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    constexpr auto projection(const vector2& vector) const noexcept -> vector2;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    constexpr auto rejection(const vector2& vector) const noexcept -> vector2;

    /// \brief Gets the perpendicular of this vector2
    ///
    /// \return the perpendicular vector2 to `this`
    constexpr auto perpendicular() const noexcept -> vector2;

    /// \brief Gets the normalized vector2 of `this`
    ///
    /// \return the normalized vector2 of `this`
    auto normalized() const noexcept -> vector2;

    /// \brief Gets the inverse of `this` vector2
    ///
    /// \return the inverse of `this` vector2
    constexpr auto inverse() const noexcept -> vector2;

    /// \brief Determines the angle between `this` and \p other
    ///
    /// \param other the other vector to determine the angle between
    /// \return the angle between `this` and \p other
    auto angle_between(const vector2& other) const noexcept -> radian;

    /// \brief Determines the angle from \p to \p other
    ///
    /// \param other the other vector to get the angle to
    /// \return the angle from `this` to \p other
    auto angle_to(const vector2& other) const noexcept -> radian;

    /// \brief Queries whether this vector is normalized
    ///
    /// \return `true` if this vector is normalized
    constexpr auto is_normalized() const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector2 and returns a reference to `(*this)`
    ///
    /// \return the reference to `(*this)`
    auto normalize() noexcept -> vector2&;

    /// \brief Inverts this vector2 and returns a reference to `(*this)`
    ///
    /// \return the reference to `(*this)`
    constexpr auto invert() noexcept -> vector2&;

    //-------------------------------------------------------------------------
    // Unary Operators
    //-------------------------------------------------------------------------
  public:

    constexpr auto operator+() const noexcept -> const vector2&;
    constexpr auto operator-() const noexcept -> vector2;

    //-------------------------------------------------------------------------
    // Compound Operators
    //-------------------------------------------------------------------------
  public:

    constexpr auto operator+=(const vector2&rhs) noexcept -> vector2&;
    constexpr auto operator-=(const vector2&rhs) noexcept -> vector2&;
    constexpr auto operator*=(real scalar) noexcept -> vector2&;
    constexpr auto operator/=(real scalar) noexcept -> vector2&;

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
  private:
    real m_data[2]; ///< The storage data
  };

  //===========================================================================
  // trait : is_vector2
  //===========================================================================

  /// \brief Trait to detect whether \p T is a \ref vector2
  ///
  /// The result is aliased as `::value`
  template<typename T>
  struct is_vector2 : std::false_type{};
  template<>
  struct is_vector2<vector2> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        `is_vector2<T>::value`
  template<typename T>
  inline constexpr bool is_vector2_v = is_vector2<T>::value;

  //===========================================================================
  // non-member functions : class : vector2
  //===========================================================================

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  constexpr auto operator+(const vector2& lhs,
                           const vector2& rhs) noexcept -> vector2;
  constexpr auto operator-(const vector2& lhs,
                           const vector2& rhs) noexcept -> vector2;
  constexpr auto operator*(const vector2& lhs, real scalar) noexcept -> vector2;
  constexpr auto operator*(real scalar, const vector2& lhs) noexcept -> vector2;
  constexpr auto operator/(const vector2& lhs, real scalar) noexcept -> vector2;

  //---------------------------------------------------------------------------
  // Comparisons
  //---------------------------------------------------------------------------

  constexpr auto operator==(const vector2& lhs,
                            const vector2& rhs) noexcept -> bool;
  constexpr auto operator!=(const vector2& lhs,
                            const vector2& rhs) noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines equality between two vector2 relative to \p tolerance
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return `true` if the two vector2 contain almost equal values
  constexpr auto almost_equal(const vector2& lhs, const vector2& rhs,
                              real tolerance) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Determines if two vector2's are linearly independent
  ///
  /// \param v1 the first vector
  /// \param v2 the second vector
  /// \return `true` if the two vectors are linearly independent
  auto are_linearly_independent(const vector2& v1,
                                const vector2& v2) noexcept -> bool;

  /// \{
  /// \brief Gets the Nth entry from a given vector, used for structure bindings
  ///
  /// \tparam N the index
  /// \param vec the vector to decompose
  /// \return reference to the Nth entry
  template <std::size_t N>
  constexpr auto get(vector2& vec) noexcept -> real&;
  template <std::size_t N>
  constexpr auto get(const vector2& vec) noexcept -> const real&;
  /// \}

  //---------------------------------------------------------------------------
  // Quantifiers
  //---------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  constexpr auto dot(const vector2& lhs, const vector2& rhs) noexcept -> real;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  constexpr auto cross(const vector2& lhs, const vector2& rhs) noexcept -> real;

  /// \brief Calculates the magnitude of the vector2 \p vec
  ///
  /// \param vec the vector2 to calculate the magnitude from
  /// \return the magnitude
  auto magnitude(const vector2& vec) noexcept -> real;

  //===========================================================================
  // struct : piecewise_compare<vector2>
  //===========================================================================

  template<>
  struct piecewise_compare<vector2>
  {
    constexpr auto operator()(const vector2& lhs,
                              const vector2& rhs) const noexcept -> bool;
  };

  //===========================================================================
  // aliases
  //===========================================================================

  using vec2 = vector2;

} // namespace alloy::core

namespace std {

  template <>
  struct tuple_size<::alloy::core::vector2> : integral_constant<std::size_t,2>{};

  template <std::size_t N>
  struct tuple_element<N, ::alloy::core::vector2> {
    using type = ::alloy::core::real;
  };

} // namespace std

//=============================================================================
// class : vector2
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline constexpr
alloy::core::vector2::vector2()
  noexcept
  : m_data{
      real{0},
      real{0}
    }
{

}

inline constexpr
alloy::core::vector2::vector2(real x, real y)
  noexcept
  : m_data{x,y}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::size()
  noexcept -> size_type
{
  return 2;
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::x()
  noexcept -> reference
{
  return m_data[0];
}

inline constexpr
auto alloy::core::vector2::x()
  const noexcept -> const_reference
{
  return m_data[0];
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::y()
  noexcept -> reference
{
  return m_data[1];
}

inline constexpr
auto alloy::core::vector2::y()
  const noexcept -> const_reference
{
  return m_data[1];
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::data()
  noexcept -> pointer
{
  return &m_data[0];
}

inline constexpr
auto alloy::core::vector2::data()
  const noexcept -> const_pointer
{
  return &m_data[0];
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::at(index_type n)
    -> reference {
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 2 || n < 0) {
    throw std::out_of_range("alloy::core::vector2::at: index out of range");
  }
#else
  assert(n < 2 && n >= 0);
#endif
  return m_data[n];
}

inline constexpr
auto alloy::core::vector2::at(index_type n)
  const -> const_reference {
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 2 || n < 0) {
    throw std::out_of_range("alloy::core::vector2::at: index out of range");
  }
#else
  ALLOY_ASSERT(n < 2 && n >= 0);
#endif
  return m_data[n];
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::operator[](index_type n)
  noexcept -> reference
{
  ALLOY_ASSERT(n < 2 && n >= 0);
  return m_data[n];
}

inline constexpr
auto alloy::core::vector2::operator[](index_type n)
  const noexcept -> const_reference
{
  ALLOY_ASSERT(n < 2 && n >= 0);
  return m_data[n];
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::dot(const vector2& other)
  const noexcept -> real
{
  return (x() * other.x()) + (y() * other.y());
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::cross(const vector2& other)
  const noexcept -> real
{
  return (x() * other.y()) - (y() * other.x());
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::square_magnitude()
  const noexcept -> real
{
  return dot(*this);
}

inline constexpr
auto alloy::core::vector2::midpoint(const vector2& other)
  const noexcept -> vector2
{
  return vector2{
    ((x() + other.x()) * real{0.5}),
    ((y() + other.y()) * real{0.5})
  };
}

inline constexpr
auto alloy::core::vector2::reflection(const vector2& normal)
  const noexcept -> vector2
{
  return (*this) - (normal * (real{2} * dot(normal)));
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::projection(const vector2& vector)
  const noexcept -> vector2
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector2{
    multiplier * vector.x(),
    multiplier * vector.y()
  };
}

inline constexpr
auto alloy::core::vector2::rejection(const vector2& vector)
  const noexcept -> vector2
{
  return (*this) - projection(vector);
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::perpendicular()
  const noexcept -> vector2
{
  return vector2{ -y(), x() };
}

inline
auto alloy::core::vector2::normalized()
  const noexcept -> vector2
{
  return vector2{*this}.normalize();
}

inline constexpr
auto alloy::core::vector2::inverse()
  const noexcept -> vector2
{
  return vector2{ -x(), -y() };
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::is_normalized()
  const noexcept -> bool
{
  return almost_equal(square_magnitude(), real{1});
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::invert()
  noexcept -> vector2&
{
  x() = -x();
  y() = -y();

  return (*this);
}

//-----------------------------------------------------------------------------
// Unary Operators
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::operator+()
  const noexcept -> const vector2&
{
  return (*this);
}

inline constexpr
auto alloy::core::vector2::operator-()
  const noexcept -> vector2
{
  return vector2{ -x(), -y() };
}

//-----------------------------------------------------------------------------
// Compound Operators
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector2::operator+=(const vector2& rhs)
  noexcept -> vector2&
{
  x() += rhs.x();
  y() += rhs.y();
  return (*this);
}

inline constexpr
auto alloy::core::vector2::operator-=(const vector2& rhs)
  noexcept -> vector2&
{
  x() -= rhs.x();
  y() -= rhs.y();
  return (*this);
}

inline constexpr
auto alloy::core::vector2::operator*=(real scalar)
  noexcept -> vector2&
{
  x() *= scalar;
  y() *= scalar;
  return (*this);
}

inline constexpr
auto alloy::core::vector2::operator/=(real scalar)
  noexcept -> vector2&
{
  const auto inv = real{1} / scalar;

  x() *= inv;
  y() *= inv;
  return (*this);
}

//=============================================================================
// non-member functions : class : vector2
//=============================================================================

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator+(const vector2& lhs, const vector2& rhs)
  noexcept -> vector2
{
  return vector2{lhs}+=rhs;
}

inline constexpr
auto alloy::core::operator-(const vector2& lhs, const vector2& rhs)
  noexcept  -> vector2
{
  return vector2{lhs}-=rhs;
}

inline constexpr
auto alloy::core::operator*(const vector2& lhs, real scalar)
  noexcept -> vector2
{
  return vector2{lhs}*=scalar;
}

inline constexpr
auto alloy::core::operator*(real scalar, const vector2& lhs)
  noexcept -> vector2
{
  return vector2{lhs}*=scalar;
}

inline constexpr
auto alloy::core::operator/(const vector2& lhs, real scalar)
  noexcept -> vector2
{
  return vector2{lhs}/=scalar;
}

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const vector2& lhs, const vector2& rhs)
  noexcept -> bool
{
  for (auto i=0; i<2; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const vector2& lhs, const vector2& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const vector2& lhs,
                               const vector2& rhs,
                               real tolerance)
  noexcept -> bool
{
  for (auto i=0; i<2; ++i) {
    if (!almost_equal(lhs[i], rhs[i], tolerance)) {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <std::size_t N>
inline constexpr
auto alloy::core::get(vector2& vec)
  noexcept -> real&
{
  static_assert(N < vector2::size());
  return vec[N];
}

template <std::size_t N>
inline constexpr
auto alloy::core::get(const vector2& vec)
  noexcept -> const real&
{
  static_assert(N < vector2::size());
  return vec[N];
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::dot(const vector2& lhs, const vector2& rhs)
  noexcept -> real {

  return lhs.dot(rhs);
}

inline constexpr
auto alloy::core::cross(const vector2& lhs, const vector2& rhs)
  noexcept -> real
{
  return lhs.cross(rhs);
}

inline
auto alloy::core::magnitude(const vector2& vec)
  noexcept -> real {

  return vec.magnitude();
}

//=============================================================================
// struct : piecewise_compare<vector2>
//=============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::piecewise_compare<alloy::core::vector2>
  ::operator()(const vector2& lhs, const vector2& rhs)
  const noexcept -> bool
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR2_HPP */

