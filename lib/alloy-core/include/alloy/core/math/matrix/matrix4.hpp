// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/****************************************************************************
 * \file matrix4.hpp
 *
 * \brief This header provides the defintion for a 4x4 matrix class.
 ****************************************************************************/

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX4_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX4_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector4.hpp" // vector4

#include "alloy/core/api.hpp"                 // ALLOY_CORE_API
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp"           // core::real

#include <cassert>     // assert
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range
#include <type_traits> // std::true_type, std::false_type, std::common_type

namespace alloy::core {

  //============================================================================
  // class : matrix4
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Defines a 4x4 Matrix
  ///
  /// The matrix is accessed in column-major format (e.g. mat[col][row]) as
  /// opposed to the native [row][col] way that C++ handles 2d arrays
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API matrix4
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
    // Public Constants
    //--------------------------------------------------------------------------
  public:

    static inline constexpr auto rows    = index_type{4};
    static inline constexpr auto columns = index_type{4};
    static inline constexpr auto comparison_tolerance = default_tolerance;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a matrix4.
    ///
    /// Default constructs a matrix4 with an empty matrix (0 values)
    constexpr matrix4() noexcept;

    /// \brief Constructs a matrix4 from 4 row vectors
    ///
    /// This constructor is to allow a fluid interface for construction:
    /// \code
    /// auto mat = matrix4 { { 1, 0, 0, 0 },
    ///                             { 0, 1, 0, 0 },
    ///                             { 0, 0, 1, 0 },
    ///                             { 0, 0, 0, 1 } };
    /// \endcode
    ///
    /// \param v0 the first row vector
    /// \param v1 the second row vector
    /// \param v2 the third row vector
    /// \param v3 the fourth row vector
    constexpr matrix4(
      const vector4& v0,
      const vector4& v1,
      const vector4& v2,
      const vector4& v3
    ) noexcept;

    /// \brief Constructs a matrix4 from an array of 16 entries
    ///
    /// The entries are read column by column, in the same order that
    /// `data()` exposes them.
    ///
    /// \param array the 1-dimensional array
    constexpr matrix4(const real(&array)[16]) noexcept;

    /// \brief Constructs a matrix4 from a 2-dimensional array
    ///
    /// The array is indexed `array[c][r]`, by column and then by row.
    ///
    /// \param array the 2-dimensional array
    constexpr matrix4(const real(&array)[4][4]) noexcept;

    /// \brief Constructs a matrix4 from value entries
    ///
    /// \param m00 the entry at coordinate (0,0)
    /// \param m01 the entry at coordinate (0,1)
    /// \param m02 the entry at coordinate (0,2)
    /// \param m03 the entry at coordinate (0,3)
    /// \param m10 the entry at coordinate (1,0)
    /// \param m11 the entry at coordinate (1,1)
    /// \param m12 the entry at coordinate (1,2)
    /// \param m13 the entry at coordinate (1,3)
    /// \param m20 the entry at coordinate (2,0)
    /// \param m21 the entry at coordinate (2,1)
    /// \param m22 the entry at coordinate (2,2)
    /// \param m23 the entry at coordinate (2,3)
    /// \param m30 the entry at coordinate (3,0)
    /// \param m31 the entry at coordinate (3,1)
    /// \param m32 the entry at coordinate (3,2)
    /// \param m33 the entry at coordinate (3,3)
    constexpr matrix4(
      real m00,
      real m01,
      real m02,
      real m03,
      real m10,
      real m11,
      real m12,
      real m13,
      real m20,
      real m21,
      real m22,
      real m23,
      real m30,
      real m31,
      real m32,
      real m33
    ) noexcept;

    /// \brief Copy-constructs a matrix4 from another matrix4
    ///
    /// \param other the other matrix4 to copy
    constexpr matrix4(const matrix4& other) noexcept = default;

    /// \brief Move-constructs a matrix4 from another matrix4
    ///
    /// \param other the other matrix4 to move
    constexpr matrix4(matrix4&& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a matrix4 from another matrix4
    ///
    /// \param other the other entry to copy
    /// \return reference to `(*this)`
    auto operator=(const matrix4& other) -> matrix4& = default;

    /// \brief Move-assigns a matrix4 from another matrix4
    ///
    /// \param other the other entry to move
    /// \return reference to `(*this)`
    auto operator=(matrix4&& other) -> matrix4& = default;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Retrieves the matrix entry at column \p c and row \p r
    ///
    /// \throw std::out_of_range if \p c or \p r is greater than 2, or less
    ///        than 0.
    ///
    /// \param r the row to retrieve
    /// \param c the column to retrieve
    /// \return the reference to the entry
    constexpr auto at(index_type r, index_type c) -> reference;
    constexpr auto at(index_type r, index_type c) const -> const_reference;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Retrieves the matrix column at column \p c
    ///
    /// The entries of the column are contiguous, so `mat[c][r]` is the entry
    /// at row `r` and column `c`.
    ///
    /// \param c the column
    /// \return pointer to the column
    constexpr auto operator[](index_type c) noexcept -> pointer;
    constexpr auto operator[](index_type c) const noexcept -> const_pointer;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Retrieves the matrix entry at column \p c, and returns a
    ///        proxy to that row
    ///
    /// \param c the column
    /// \return a proxy to the row
    constexpr auto get(index_type r, index_type c) noexcept -> reference;
    constexpr auto get(index_type r, index_type c)
      const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the vector at row \p r
    ///
    /// \param r the row index to retrieve
    constexpr auto row(index_type r) const noexcept -> vector4;

    /// \brief Retrieves the vector at row \p c
    ///
    /// \param c the column index to retrieve
    constexpr auto column(index_type c) const noexcept -> vector4;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrieves the number of entries in this matrix
    ///
    /// \return the size of the matrix
    constexpr auto size() const noexcept -> size_type;

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr auto data() noexcept -> pointer;
    constexpr auto data() const noexcept -> const_pointer;
    /// \}

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// Calculates the determinant for this matrix4
    ///
    /// \returns the determinant of this matrix
    auto determinant() const noexcept -> real;

    /// Calculates the trace for this matrix4
    ///
    /// \returns the trace of this matrix
    auto trace() const noexcept -> real;

    /// \brief Computes the inverse of this matrix4
    ///
    /// If no inverse is possible, this returns \ref matrix4::identity
    ///
    /// \return the inverse of this matrix4
    auto inverse() const noexcept -> matrix4;

    /// \brief Computes the transpose of this matrix4
    ///
    /// \return the transpose of this matrix4
    auto transposed() const noexcept -> matrix4;

    /// \brief Transforms the column vector \p vec by `(*this)`
    ///
    /// \param vec the vector to transform
    /// \return the product `(*this) * vec`
    auto combine(const vector4& vec) const noexcept -> vector4;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts this current matrix4, and returns a reference to
    ///        `(*this)`
    ///
    /// If there is no inverse possible, this function returns a
    /// reference to \ref matrix4::identity, and the current matrix4
    /// is left unmodified
    ///
    /// \return reference to `(*this)`
    auto invert() noexcept -> matrix4&;

    /// \brief Transposes this current matrix4, returning a reference to
    ///        `(*this)`
    ///
    /// \return reference to `(*this)`
    auto transpose() noexcept -> matrix4&;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    auto operator+=(const matrix4& rhs) noexcept -> matrix4&;
    auto operator-=(const matrix4& rhs) noexcept -> matrix4&;
    auto operator*=(const matrix4& rhs) noexcept -> matrix4&;
    auto operator*=(real scalar) noexcept -> matrix4&;
    auto operator/=(real scalar) noexcept -> matrix4&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    alignas(16) real m_matrix[columns][rows]; ///< The entries, stored column by column

  };

  //============================================================================
  // non-member functions : class : matrix4
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  auto operator+(const matrix4& lhs, const matrix4& rhs) noexcept -> matrix4;
  auto operator-(const matrix4& lhs, const matrix4& rhs) noexcept -> matrix4;

  /// \brief Multiplies \p lhs by \p rhs
  ///
  /// The product transforms a column vector by \p rhs first, and then by
  /// \p lhs.
  ///
  /// \param lhs the left matrix4
  /// \param rhs the right matrix4
  /// \return the product `lhs * rhs`
  auto operator*(const matrix4& lhs, const matrix4& rhs) noexcept -> matrix4;

  /// \brief Transforms the column vector \p rhs by \p lhs
  ///
  /// \param lhs the matrix4
  /// \param rhs the column vector
  /// \return the column vector `lhs * rhs`
  auto operator*(const matrix4& lhs, const vector4& rhs) noexcept -> vector4;

  /// \brief Multiplies the row vector \p lhs by \p rhs
  ///
  /// \param lhs the row vector
  /// \param rhs the matrix4
  /// \return the row vector `lhs * rhs`, which is `transpose(rhs) * lhs`
  auto operator*(const vector4& lhs, const matrix4& rhs) noexcept -> vector4;

  auto operator*(real lhs, const matrix4& rhs) noexcept -> matrix4;
  auto operator*(const matrix4& lhs, real rhs) noexcept -> matrix4;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const matrix4& lhs, const matrix4& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const matrix4& lhs, const matrix4& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two matrix4 relative to \ref tolerance
  ///
  /// \param lhs the left matrix4
  /// \param rhs the right matrix4
  /// \return `true` if the two matrix4 contain almost equal values
  constexpr auto almost_equal(
    const matrix4& lhs,
    const matrix4& rhs,
    real tolerance
  ) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref matrix4
  ///
  /// The result is aliased as `::value`
  template<typename T>
  struct is_matrix4 : std::false_type{};

  template<>
  struct is_matrix4<matrix4> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_matrix4
  template<typename T>
  constexpr bool is_matrix4_v = is_matrix4<T>::value;

} // namespace alloy::core

//==============================================================================
// class : matrix4
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::matrix4::matrix4() noexcept
  : m_matrix{
    {real{0}, real{0}, real{0}, real{0}},
    {real{0}, real{0}, real{0}, real{0}},
    {real{0}, real{0}, real{0}, real{0}},
    {real{0}, real{0}, real{0}, real{0}}
  }
{

}

inline constexpr
alloy::core::matrix4::matrix4(
  const vector4& v0,
  const vector4& v1,
  const vector4& v2,
  const vector4& v3
) noexcept
  : m_matrix{
      {v0.x(), v1.x(), v2.x(), v3.x()},
      {v0.y(), v1.y(), v2.y(), v3.y()},
      {v0.z(), v1.z(), v2.z(), v3.z()},
      {v0.w(), v1.w(), v2.w(), v3.w()}
    }
{

}

inline constexpr
alloy::core::matrix4::matrix4(const real(&array)[16]) noexcept
  : m_matrix {
      {array[0],  array[1],  array[2],  array[3]},
      {array[4],  array[5],  array[6],  array[7]},
      {array[8],  array[9],  array[10], array[11]},
      {array[12], array[13], array[14], array[15]}
    }
{

}

inline constexpr
alloy::core::matrix4::matrix4(const real(&array)[4][4]) noexcept
  : m_matrix {
      {array[0][0], array[0][1], array[0][2], array[0][3]},
      {array[1][0], array[1][1], array[1][2], array[1][3]},
      {array[2][0], array[2][1], array[2][2], array[2][3]},
      {array[3][0], array[3][1], array[3][2], array[3][3]}
    }
{

}

inline constexpr
  alloy::core::matrix4::matrix4(
    real m00,
    real m01,
    real m02,
    real m03,
    real m10,
    real m11,
    real m12,
    real m13,
    real m20,
    real m21,
    real m22,
    real m23,
    real m30,
    real m31,
    real m32,
    real m33
  ) noexcept
  : m_matrix {
      {m00, m10, m20, m30},
      {m01, m11, m21, m31},
      {m02, m12, m22, m32},
      {m03, m13, m23, m33}
    }
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix4::at(index_type r, index_type c) -> reference
{
  if (c >=columns || c < 0 || r >=rows || r < 0) {
    throw std::out_of_range{"matrix4::at: index out of range"};
  }
  return get(r,c);
}


inline constexpr
auto alloy::core::matrix4::at(index_type r, index_type c)
  const -> const_reference
{
  if(c >=columns || c < 0 || r >=rows || r < 0) {
    throw std::out_of_range{"matrix4::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix4::operator[](index_type c) noexcept -> pointer
{
  return m_matrix[c];
}

inline constexpr
auto alloy::core::matrix4::operator[](index_type c)
  const noexcept -> const_pointer
{
  return m_matrix[c];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix4::get(index_type r, index_type c) noexcept -> reference
{
  return m_matrix[c][r];
}

inline constexpr
auto alloy::core::matrix4::get(index_type r, index_type c)
  const noexcept -> const_reference
{
  return m_matrix[c][r];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix4::row(index_type r) const noexcept -> vector4
{
  return vector4{ get(r,0), get(r,1), get(r,2), get(r,3) };
}

inline constexpr
auto alloy::core::matrix4::column(index_type c) const noexcept -> vector4
{
  return vector4{ get(0,c), get(1,c), get(2,c), get(3,c) };
}

//----------------------------------------------------------------------------
// Observers
//----------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix4::size() const noexcept -> size_type
{
  return rows * columns;
}

inline constexpr
auto alloy::core::matrix4::data() noexcept -> pointer
{
  return &get(0,0);
}

inline constexpr
auto alloy::core::matrix4::data() const noexcept -> const_pointer
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::matrix4::operator+=(const matrix4& rhs) noexcept -> matrix4&
{
  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

inline
auto alloy::core::matrix4::operator-=(const matrix4& rhs) noexcept -> matrix4&
{
  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

inline
auto alloy::core::matrix4::operator*=(const matrix4& rhs) noexcept -> matrix4&
{
  auto result = matrix4{};

  // Column c of the product is the sum of the columns of this matrix,
  // each scaled by the matching entry of column c of rhs.
  for (auto c = 0; c < columns; ++c) {
    for (auto i = 0; i < columns; ++i) {
      const auto scale = rhs.get(i,c);

      for (auto r = 0; r < rows; ++r) {
        result.get(r,c) += get(r,i) * scale;
      }
    }
  }

  // Copy result in
  (*this) = result;
  return (*this);
}

inline
auto alloy::core::matrix4::operator*=(real scalar) noexcept -> matrix4&
{
  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

inline
auto alloy::core::matrix4::operator/=(real scalar) noexcept -> matrix4&
{
  const auto inv = real{1} / scalar;

  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      get(r,c) *= inv;
    }
  }
  return (*this);
}

//==============================================================================
// non-member functions : class : matrix4
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::operator+(const matrix4& lhs, const matrix4& rhs)
  noexcept -> matrix4
{
  return matrix4{lhs} += rhs;
}

inline
auto alloy::core::operator-(const matrix4& lhs, const matrix4& rhs)
  noexcept -> matrix4
{
  return matrix4{lhs} -= rhs;
}

inline
auto alloy::core::operator*(const matrix4& lhs, const matrix4& rhs)
  noexcept -> matrix4
{
  return matrix4{lhs} *= rhs;
}

inline
auto alloy::core::operator*(const matrix4& lhs, const vector4& rhs)
  noexcept -> vector4
{
  return lhs.combine(rhs);
}

inline
auto alloy::core::operator*(const vector4& lhs, const matrix4& rhs)
  noexcept -> vector4
{
  auto result = vector4{};

  for (auto c = 0; c < matrix4::columns; ++c) {
    auto sum = real{0};

    for (auto r = 0; r < matrix4::rows; ++r) {
      sum += lhs[r] * rhs.get(r,c);
    }
    result[c] = sum;
  }
  return result;
}

inline
auto alloy::core::operator*(real lhs, const matrix4& rhs) noexcept -> matrix4
{
  return matrix4{rhs} *= lhs;
}

inline
auto alloy::core::operator*(const matrix4& lhs, real rhs) noexcept -> matrix4
{
  return matrix4{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const matrix4& lhs, const matrix4& rhs)
  noexcept -> bool
{
  for (auto c = 0; c < matrix4::columns; ++c) {
    for (auto r = 0; r < matrix4::rows; ++r) {
      if (lhs.get(r,c) != rhs.get(r,c)) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const matrix4& lhs, const matrix4& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//----------------------------------------------------------------------------

inline constexpr auto
  alloy::core::almost_equal(const matrix4& lhs,
                            const matrix4& rhs,
                            real tolerance)
  noexcept -> bool
{
  for (auto c = 0; c < matrix4::columns; ++c) {
    for (auto r = 0; r < matrix4::rows; ++r) {
      if (!almost_equal(lhs.get(r,c), rhs.get(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX4_HPP */

