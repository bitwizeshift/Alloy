/******************************************************************************
 * \file vector3.hpp
 *
 * \brief This header contains the definition of a 3d vector class
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
#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR3_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR3_HPP

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
  // class : vector3
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief This object represents a 3-component vector in linear algebra.
  ///
  /// Most operations are marked \c constexpr to allow for compile-time
  /// forward-calculations of vector sums and products.
  ///
  /// Operations on this type are able to promote results to reduce loss of
  /// data, depending on what the operands are.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API vector3
  {
    //-------------------------------------------------------------------------
    // Public Types
    //-------------------------------------------------------------------------
  public:

    using value_type      = real;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using size_type  = std::size_t;
    using index_type = std::ptrdiff_t;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructs a vector3 with 0 values
    constexpr vector3() noexcept;

    /// \brief Constructs a vector3 with components \p x and \p y
    ///
    /// \param x the x-component of the vector3
    /// \param y the y-component of the vector3
    /// \param z the z-component of the vector3
    constexpr vector3(real x, real y, real z) noexcept;

    /// \brief Copy-constructs a vector3 with the value of another
    ///        vector3
    ///
    /// \param other the other vector3 to copy
    constexpr vector3(const vector3& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copy-assigns \p other to \c this
    ///
    /// \param other the other vector3 to copy
    /// \return reference to \c (*this)
    auto operator=(const vector3& other) -> vector3& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the number of components in the vector3
    ///
    /// \return the number of components in the vector3
    static constexpr auto size() noexcept -> size_type;
    /// \}

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
    /// \brief Gets the z component of this vector
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

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \throw std::out_of_range if \p n >= 3 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr auto at(index_type n) -> reference;
    constexpr auto at(index_type n) const -> const_reference;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 3 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr auto operator[](index_type n) noexcept -> reference;
    constexpr auto operator[](index_type n) const noexcept -> const_reference;
    /// \}

    //-------------------------------------------------------------------------
    // Quantifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Determines the dot-product of \c this and \p other
    ///
    /// \param other the other vector3 to perform the dot-product with
    /// \return the dot product of \c this and \p other
    constexpr auto dot(const vector3& other) const noexcept -> real;

    /// \brief Calculates the cross-product of \c this and \p other
    ///
    /// \param other the other vector3 to perform the cross-product with
    /// \return the cross product of \c this and \p other
    constexpr auto cross(const vector3& other) const noexcept -> vector3;

    /// \brief Gets the square magnitude of this vector3
    ///
    /// \return the square magnitude of the vector3
    constexpr auto square_magnitude() const noexcept -> real;

    /// \brief Gets the magnitude of this vector3
    ///
    /// \return the magnitude of the vector3
    auto magnitude() const noexcept -> real;

    /// \brief Gets the midpoint between \c this and \p other
    ///
    /// \param other the vector3 to get the midpoint from
    /// \return the midpoint between \c this and \p other
    constexpr auto midpoint(const vector3& other) const noexcept -> vector3;

    /// \brief Gets the reflection vector3 of \c this reflected through the
    ///        \p normal
    ///
    /// \param normal the normal vector3
    /// \return the reflection
    constexpr auto reflection(const vector3& normal) const noexcept -> vector3;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    constexpr auto projection(const vector3& vector) const noexcept -> vector3;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    constexpr auto rejection(const vector3& vector) const noexcept -> vector3;

    /// \brief Gets the perpendicular of this vector3
    ///
    /// \return the perpendicular vector3 to \c this
    auto perpendicular() const noexcept -> vector3;

    /// \brief Gets the normalized vector3 of \c this
    ///
    /// \return the normalized vector3 of \c this
    auto normalized() const noexcept -> vector3;

    /// \brief Gets the inverse of \c this vector3
    ///
    /// \return the inverse of \c this vector3
    constexpr auto inverse() const noexcept -> vector3;

    /// \brief Determines the angle between \c this and \p other
    ///
    /// \param other the other vector to determine the angle between
    /// \return the angle between \c this and \p other
    auto angle_between(const vector3& other) const noexcept -> radian;

    /// \brief Determines the angle from \p to \p other
    ///
    /// \param other the other vector to get the angle to
    /// \return the angle from \c this to \p other
    auto angle_to(const vector3& other) const noexcept -> radian;

    /// \brief Queries whether this vector is normalized
    ///
    /// \return \c true if this vector is normalized
    constexpr auto is_normalized() const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector3 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    auto normalize() noexcept -> vector3&;

    /// \brief Inverts this vector3 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr auto invert() noexcept -> vector3&;

    //-------------------------------------------------------------------------
    // Unary Operators
    //-------------------------------------------------------------------------
  public:

    constexpr auto operator+() const noexcept -> const vector3&;
    constexpr auto operator-() const noexcept -> vector3;

    //-------------------------------------------------------------------------
    // Compound Operators
    //-------------------------------------------------------------------------
  public:

    constexpr auto operator+=(const vector3& rhs) noexcept -> vector3&;
    constexpr auto operator-=(const vector3& rhs) noexcept -> vector3&;
    constexpr auto operator*=(real scalar) noexcept -> vector3&;
    constexpr auto operator/=(real scalar) noexcept -> vector3&;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    real m_data[3]; ///< The storage data
  };

  //===========================================================================
  // trait : is_vector3
  //===========================================================================

  /// \brief Trait to detect whether \p T is a \ref vector3
  ///
  /// The result is aliased as \c ::value
  template<typename T>
  struct is_vector3 : std::false_type{};
  template<>
  struct is_vector3<vector3> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector4::value
  template<typename T>
  constexpr bool is_vector3_v = is_vector3<T>::value;

  //===========================================================================
  // non-member functions : class : vector3
  //===========================================================================

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  constexpr auto operator+(const vector3& lhs,
                           const vector3& rhs) noexcept -> vector3;
  constexpr auto operator-(const vector3& lhs,
                           const vector3& rhs) noexcept -> vector3;
  constexpr auto operator*(const vector3& lhs, real scalar) noexcept -> vector3;
  constexpr auto operator*(real scalar, const vector3& lhs) noexcept -> vector3;
  constexpr auto operator/(const vector3& lhs, real scalar) noexcept -> vector3;

  //---------------------------------------------------------------------------
  // Comparisons
  //---------------------------------------------------------------------------

  constexpr auto operator==(const vector3& lhs,
                            const vector3& rhs) noexcept -> bool;
  constexpr auto operator!=(const vector3& lhs,
                            const vector3& rhs) noexcept -> bool;

  //---------------------------------------------------------------------------

  /// \brief Determines equality between two vector3 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left vector3
  /// \param rhs the right vector3
  /// \return \c true if the two vector3 contain almost equal values
  constexpr auto almost_equal(const vector3& lhs,
                              const vector3& rhs) noexcept -> bool;

  /// \brief Determines equality between two vector3 relative to \p tolerance
  ///
  /// \param lhs the left vector3
  /// \param rhs the right vector3
  /// \return \c true if the two vector3 contain almost equal values
  constexpr auto almost_equal(const vector3& lhs,
                              const vector3& rhs,
                              real tolerance) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Determines if two vector3's are linearly independent
  ///
  /// \param v1 the first vector
  /// \param v2 the second vector
  /// \param v3 the third vector
  /// \return \c true if the three vectors are linearly independent
  auto are_linearly_independent(const vector3& v1,
                                const vector3& v2,
                                const vector3& v3) noexcept -> bool;

  /// \{
  /// \brief Gets the Nth entry from a given vector, used for structure bindings
  ///
  /// \tparam N the index
  /// \param vec the vector to decompose
  /// \return reference to the Nth entry
  template <std::size_t N>
  constexpr auto get(vector3& vec) noexcept -> real&;
  template <std::size_t N>
  constexpr auto get(const vector3& vec) noexcept -> const real&;
  /// \}

  //---------------------------------------------------------------------------
  // Quantifiers
  //---------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector3
  /// \param rhs the right vector3
  /// \return the result of the dot product
  constexpr auto dot(const vector3& lhs,
                     const vector3& rhs) noexcept -> real;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  constexpr auto cross(const vector3& lhs,
                       const vector3& rhs) noexcept -> vector3;

  /// \brief Calculates the magnitude of the vector3 \p vec
  ///
  /// \param vec the vector3 to calculate the magnitude from
  /// \return the magnitude
  auto magnitude(const vector3& vec) noexcept -> real;

  //============================================================================
  // struct : piecewise_compare<vector3>
  //============================================================================

  template<>
  struct piecewise_compare<vector3>
  {
    constexpr auto operator()(const vector3& lhs,
                              const vector3& rhs) noexcept -> bool;
  };

  //===========================================================================
  // aliases
  //===========================================================================

  using vec3 = vector3;

} // namespace alloy::core

namespace std {

  template <>
  struct tuple_size<::alloy::core::vector3> : integral_constant<std::size_t,3>{};

  template <std::size_t N>
  struct tuple_element<N, ::alloy::core::vector3> {
    using type = ::alloy::core::real;
  };

} // namespace std

//=============================================================================
// class : vector3
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline constexpr alloy::core::vector3::vector3()
  noexcept
  : m_data{
      real{0},
      real{0},
      real{0}
    }
{

}

inline constexpr alloy::core::vector3::vector3(real x, real y, real z)
  noexcept
  : m_data{x,y,z}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::size()
  noexcept -> size_type
{
  return 3;
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::x()
  noexcept -> reference
{
  return m_data[0];
}

inline constexpr
auto alloy::core::vector3::x()
  const noexcept -> const_reference
{
  return m_data[0];
}

//----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::y()
  noexcept -> reference
{
  return m_data[1];
}

inline constexpr
auto alloy::core::vector3::y()
  const noexcept -> const_reference
{
  return m_data[1];
}

//----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::z()
  noexcept -> reference
{
  return m_data[2];
}

inline constexpr
auto alloy::core::vector3::z()
  const noexcept -> const_reference
{
  return m_data[2];
}

//----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::data()
  noexcept -> pointer
{
  return m_data;
}

inline constexpr
auto alloy::core::vector3::data()
  const noexcept -> const_pointer
{
  return m_data;
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::at(index_type n)
  -> reference
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 3 || n < 0) {
    throw std::out_of_range("alloy::core::vector3::at: index out of range");
  }
#else
  ALLOY_ASSERT(n < 3 && n >= 0);
#endif
  return m_data[n];
}

inline constexpr
auto alloy::core::vector3::at(index_type n)
  const -> const_reference
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (n >= 3 || n < 0) {
    throw std::out_of_range("alloy::core::vector3::at: index out of range");
  }
#else
  ALLOY_ASSERT(n < 3 && n >= 0);
#endif
  return m_data[n];
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::operator[](index_type n)
  noexcept -> reference
{
  ALLOY_ASSERT(n < 3 && n >= 0);
  return m_data[n];
}

inline constexpr
auto alloy::core::vector3::operator[](index_type n)
  const noexcept -> const_reference
{
  ALLOY_ASSERT(n < 3 && n >= 0);
  return m_data[n];
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::dot(const vector3& other)
  const noexcept -> real
{
  return (x() * other.x()) + (y() * other.y()) + (z() * other.z());
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::cross(const vector3& other)
  const noexcept -> vector3
{
  return vector3{
    (y() * other.z() - z() * other.y()),
    (z() * other.x() - x() * other.z()),
    (x() * other.y() - y() * other.x())
  };
}

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::square_magnitude()
  const noexcept -> real
{
  return dot(*this);
}

inline constexpr
auto alloy::core::vector3::midpoint(const vector3& other)
  const noexcept -> vector3
{
  return vector3{
    ((x() + other.x()) * real{0.5}),
    ((y() + other.y()) * real{0.5}),
    ((z() + other.z()) * real{0.5})
  };
}

inline constexpr
auto alloy::core::vector3::reflection(const vector3& normal)
  const noexcept -> vector3
{
  return (*this) - ((2*dot(normal)) * normal);
}

inline constexpr
auto alloy::core::vector3::projection(const vector3& vector)
  const noexcept -> vector3
{
  const auto multiplier = dot(vector) / dot(*this);

  return vector3{
    multiplier * x(),
    multiplier * y(),
    multiplier * z()
  };
}

inline constexpr
auto alloy::core::vector3::rejection(const vector3& vector)
  const noexcept -> vector3
{
  return (*this) - projection(vector);
}

inline
auto alloy::core::vector3::normalized()
  const noexcept -> vector3
{
  return vector3{*this}.normalize();
}

inline constexpr
auto alloy::core::vector3::inverse()
  const noexcept -> vector3
{
  return vector3{ -x(), -y(), -z() };
}

inline constexpr
auto alloy::core::vector3::is_normalized()
  const noexcept -> bool
{
  return almost_equal(square_magnitude(), real{1});
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::invert()
  noexcept -> vector3&
{
  x() = -x();
  y() = -y();
  z() = -z();

  return (*this);
}

//-----------------------------------------------------------------------------
// Unary Operators
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::operator+()
  const noexcept -> const vector3&
{
  return (*this);
}

inline constexpr
auto alloy::core::vector3::operator-()
  const noexcept -> vector3
{
  return vector3{ -x(), -y(), -z() };
}

//-----------------------------------------------------------------------------
// Compound Operators
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::vector3::operator+=(const vector3& rhs)
  noexcept -> vector3&
{
  x() += rhs.x();
  y() += rhs.y();
  z() += rhs.z();
  return (*this);
}

inline constexpr
auto alloy::core::vector3::operator-=(const vector3& rhs)
  noexcept -> vector3&
{
  x() -= rhs.x();
  y() -= rhs.y();
  z() -= rhs.z();
  return (*this);
}

inline constexpr
auto alloy::core::vector3::operator*=(real scalar)
  noexcept -> vector3&
{
  x() *= scalar;
  y() *= scalar;
  z() *= scalar;
  return (*this);
}

inline constexpr
auto alloy::core::vector3::operator/=(real scalar)
  noexcept -> vector3&
{
  const auto inv = real{1} / scalar;

  x() *= inv;
  y() *= inv;
  z() *= inv;
  return (*this);
}

//=============================================================================
// non-member functions : class : vector3
//=============================================================================

//-----------------------------------------------------------------------------
// Arithmetic Operators
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::operator+(const vector3& lhs, const vector3& rhs)
  noexcept -> vector3
{
  return vector3{lhs}+=rhs;
}

inline constexpr
auto alloy::core::operator-(const vector3& lhs, const vector3& rhs)
  noexcept -> vector3
{
  return vector3{lhs}-=rhs;
}

inline constexpr
auto alloy::core::operator*(const vector3& lhs, real scalar)
  noexcept -> vector3
{
  return vector3{lhs}*=scalar;
}

inline constexpr
auto alloy::core::operator*(real scalar, const vector3& lhs)
  noexcept -> vector3
{
  return vector3{lhs}*=scalar;
}

inline constexpr
auto alloy::core::operator/(const vector3& lhs, real scalar)
  noexcept -> vector3
{
  return vector3{lhs}/=scalar;
}

//-----------------------------------------------------------------------------
// Comparisons
//-----------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const vector3& lhs, const vector3& rhs)
  noexcept -> bool
{
  for (auto i=0;i<3;++i) {
    if (lhs[i]!=rhs[i]) {
      return false;
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const vector3& lhs, const vector3& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::almost_equal(const vector3& lhs, const vector3& rhs)
  noexcept -> bool
{
  return almost_equal(lhs, rhs, default_tolerance);
}

inline constexpr
auto alloy::core::almost_equal(const vector3& lhs,
                               const vector3& rhs,
                               real tolerance)
  noexcept -> bool
{
  for (auto i=0; i<3; ++i) {
    if (!almost_equal(lhs[i],rhs[i],tolerance)) {
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
auto alloy::core::get(vector3& vec)
  noexcept -> real&
{
  static_assert(N < vector3::size());
  return vec[N];
}

template <std::size_t N>
inline constexpr
auto alloy::core::get(const vector3& vec)
  noexcept -> const real&
{
  static_assert(N < vector3::size());
  return vec[N];
}

//-----------------------------------------------------------------------------
// Quantifiers
//-----------------------------------------------------------------------------

inline constexpr
auto alloy::core::dot(const vector3& lhs, const vector3& rhs)
  noexcept -> real
{
  return lhs.dot(rhs);
}

inline constexpr
auto alloy::core::cross(const vector3& lhs, const vector3& rhs)
  noexcept -> vector3
{
  return lhs.cross(rhs);
}

inline auto alloy::core::magnitude(const vector3& vec)
  noexcept -> real
{
  return vec.magnitude();
}

//=============================================================================
// struct : piecewise_compare<vector3>
//=============================================================================

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::piecewise_compare<alloy::core::vector3>
  ::operator()(const vector3& lhs, const vector3& rhs)
  noexcept -> bool
{
  return (lhs.x() == rhs.x()) ?
           (lhs.y() == rhs.y()) ?
             (lhs.z() < rhs.z()) :
           (lhs.y() < rhs.y()) :
         (lhs.x() < rhs.x());
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR3_HPP */
