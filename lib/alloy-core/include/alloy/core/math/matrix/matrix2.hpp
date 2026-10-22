// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/****************************************************************************
 * \file matrix2.hpp
 *
 * \brief This header provides the defintion for a 2x2 matrix class.
 ****************************************************************************/

#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX2_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX2_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/math/math.hpp"
#include "alloy/core/math/vector/vector2.hpp" // vector2

#include "alloy/core/api.hpp"                 // ALLOY_CORE_API
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp"           // core::real

#include <cassert>     // assert
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range
#include <type_traits> // std::true_type, std::false_type, std::common_type
#include <utility>     // std::swap

namespace alloy::core {

  //============================================================================
  // class : matrix2
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Defines a 2x2 Matrix
  ///
  /// The matrix is accessed in column-major format (e.g. mat[col][row]) as
  /// opposed to the native [row][col] way that C++ handles 2d arrays
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API matrix2
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

    static inline constexpr auto rows    = index_type{2};
    static inline constexpr auto columns = index_type{2};
    static inline constexpr auto comparison_tolerance = default_tolerance;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a matrix2.
    ///
    /// Default constructs a matrix2 with an empty matrix (0 values)
    constexpr matrix2() noexcept;

    /// \brief Constructs a matrix2 from 2 row vectors
    ///
    /// This constructor is to allow a fluid interface for construction:
    /// \code
    /// auto mat = matrix2{ vector2{ 1, 0 },
    ///                     vector2{ 0, 1 } };
    /// \endcode
    ///
    /// \param v0 the first row vector
    /// \param v1 the second row vector
    constexpr matrix2(const vector2& v0, const vector2& v1) noexcept;

    /// \brief Constructs a matrix2 from an array of 4 entries
    ///
    /// The entries are read column by column, in the same order that
    /// `data()` exposes them.
    ///
    /// \param array the 1-dimensional array
    explicit constexpr matrix2(const real(&array)[4]) noexcept;

    /// \brief Constructs a matrix2 from a 2-dimensional array
    ///
    /// The array is indexed `array[c][r]`, by column and then by row.
    ///
    /// \param array the 2-dimensional array
    explicit constexpr matrix2(const real(&array)[2][2]) noexcept;

    /// \brief Constructs a matrix2 from value entries
    ///
    /// \param m00 the entry at coordinate (0,0)
    /// \param m01 the entry at coordinate (0,1)
    /// \param m10 the entry at coordinate (1,0)
    /// \param m11 the entry at coordinate (1,1)
    constexpr matrix2(real m00, real m01, real m10, real m11) noexcept;

    /// \brief Copy-constructs a matrix2 from another matrix2
    ///
    /// \param other the other matrix2 to copy
    constexpr matrix2(const matrix2& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a matrix2 from another matrix2
    ///
    /// \param other the other entry to copy
    /// \return reference to `(*this)`
    auto operator=(const matrix2& other) -> matrix2& = default;

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
    /// \brief Retrieves the matrix entry at column \p c and row \p r
    ///
    /// \param r the row
    /// \param c the column
    /// \return reference to the entry
    constexpr auto get(index_type r, index_type c) noexcept -> reference;
    constexpr auto get(index_type r, index_type c)
      const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the vector at row \p r
    ///
    /// \param r the row index to retrieve
    constexpr auto row(index_type r) const noexcept -> vector2;

    /// \brief Retrieves the vector at row \p c
    ///
    /// \param c the column index to retrieve
    constexpr auto column(index_type c) const noexcept -> vector2;

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

    /// Calculates the determinant for this matrix2
    ///
    /// \returns the determinant of this matrix
    auto determinant() const noexcept -> real;

    /// Calculates the trace for this matrix2
    ///
    /// \returns the trace of this matrix
    auto trace() const noexcept -> real;

    /// \brief Computes the inverse of this matrix2
    ///
    /// If no inverse is possible, this returns the identity
    ///
    /// \return the inverse of this matrix2
    auto inverse() const noexcept -> matrix2;

    /// \brief Computes the transpose of this matrix2
    ///
    /// \return the transpose of this matrix2
    auto transposed() const noexcept -> matrix2;

    /// \brief Transforms the column vector \p vec by `(*this)`
    ///
    /// \param vec the vector to transform
    /// \return the product `(*this) * vec`
    auto combine(const vector2& vec) const noexcept -> vector2;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts this current matrix2, and returns a reference to
    ///        `(*this)`
    ///
    /// If there is no inverse possible, this function returns a
    /// reference to \ref matrix2::identity, and the current matrix2
    /// is left unmodified
    ///
    /// \return reference to `(*this)`
    auto invert() noexcept -> matrix2&;

    /// \brief Transposes this current matrix2, returning a reference to
    ///        `(*this)`
    ///
    /// \return reference to `(*this)`
    auto transpose() noexcept -> matrix2&;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

   auto operator+=(const matrix2& rhs) noexcept -> matrix2&;
   auto operator-=(const matrix2& rhs) noexcept -> matrix2&;
   auto operator*=(const matrix2& rhs) noexcept -> matrix2&;
   auto operator*=(real scalar) noexcept -> matrix2&;
   auto operator/=(real scalar) noexcept -> matrix2&;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    real m_matrix[columns][rows]; ///< The entries, stored column by column
  };

  //============================================================================
  // non-member functions : class : matrix2
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  auto operator+(const matrix2& lhs, const matrix2& rhs) noexcept -> matrix2;
  auto operator-(const matrix2& lhs, const matrix2& rhs) noexcept -> matrix2;

  /// \brief Multiplies \p lhs by \p rhs
  ///
  /// The product transforms a column vector by \p rhs first, and then by
  /// \p lhs.
  ///
  /// \param lhs the left matrix2
  /// \param rhs the right matrix2
  /// \return the product `lhs * rhs`
  auto operator*(const matrix2& lhs, const matrix2& rhs) noexcept -> matrix2;

  /// \brief Transforms the column vector \p rhs by \p lhs
  ///
  /// \param lhs the matrix2
  /// \param rhs the column vector
  /// \return the column vector `lhs * rhs`
  auto operator*(const matrix2& lhs, const vector2& rhs) noexcept -> vector2;

  /// \brief Multiplies the row vector \p lhs by \p rhs
  ///
  /// \param lhs the row vector
  /// \param rhs the matrix2
  /// \return the row vector `lhs * rhs`, which is `transpose(rhs) * lhs`
  auto operator*(const vector2& lhs, const matrix2& rhs) noexcept -> vector2;

  auto operator*(real lhs, const matrix2& rhs) noexcept -> matrix2;
  auto operator*(const matrix2& lhs, real rhs) noexcept -> matrix2;

  //------------------------------------------------------------------------
  // Comparisons
  //------------------------------------------------------------------------

  constexpr auto operator==(const matrix2& lhs, const matrix2& rhs)
    noexcept -> bool;
  constexpr auto operator!=(const matrix2& lhs, const matrix2& rhs)
    noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two matrix2 relative to \ref tolerance
  ///
  /// \param lhs the left matrix2
  /// \param rhs the right matrix2
  /// \return `true` if the two matrix2 contain almost equal values
  constexpr auto almost_equal(
    const matrix2& lhs,
    const matrix2& rhs,
    real tolerance
  ) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref matrix2
  ///
  /// The result is aliased as `::value`
  template<typename T>
  struct is_matrix2 : std::false_type{};

  template<>
  struct is_matrix2<matrix2> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_matrix2
  template<typename T>
  constexpr bool is_matrix2_v = is_matrix2<T>::value;

} // namespace alloy::core

//==============================================================================
// class : matrix2
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::core::matrix2::matrix2() noexcept
  : m_matrix {
      {real{0}, real{0}},
      {real{0}, real{0}}
    }
{

}

inline constexpr
alloy::core::matrix2::matrix2(const vector2& v0, const vector2& v1) noexcept
  : m_matrix {
      {v0.x(), v1.x()},
      {v0.y(), v1.y()}
    }
{

}

inline constexpr
alloy::core::matrix2::matrix2(const real(&array)[4]) noexcept
  : m_matrix {
      {array[0], array[1]},
      {array[2], array[3]}
    }
{

}

inline constexpr
alloy::core::matrix2::matrix2(const real(&array)[2][2]) noexcept
  : m_matrix {
      {array[0][0], array[0][1]},
      {array[1][0], array[1][1]}
    }
{

}

inline constexpr
alloy::core::matrix2::matrix2(real m00, real m01, real m10, real m11) noexcept
  : m_matrix {
      {m00, m10},
      {m01, m11}
    }
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix2::at(index_type r, index_type c) -> reference
{
  if (c >=columns || c < 0 || r >=rows || r < 0) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}


inline constexpr
auto alloy::core::matrix2::at(index_type r, index_type c)
  const -> const_reference
{
  if (c >=columns || c < 0 || r >=rows || r < 0) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix2::operator[](index_type c) noexcept -> pointer
{
  return m_matrix[c];
}

inline constexpr
auto alloy::core::matrix2::operator[](index_type c)
  const noexcept -> const_pointer
{
  return m_matrix[c];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix2::get(index_type r, index_type c) noexcept -> reference
{
  return m_matrix[c][r];
}

inline constexpr
auto alloy::core::matrix2::get(index_type r, index_type c)
  const noexcept -> const_reference
{
  return m_matrix[c][r];
}

//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix2::row(index_type r) const noexcept -> vector2
{
  return vector2{ get(r,0), get(r,1) };
}

inline constexpr
auto alloy::core::matrix2::column(index_type c) const noexcept -> vector2
{
  return vector2{ get(0,c), get(1,c) };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr
auto alloy::core::matrix2::size() const noexcept -> size_type
{
  return rows*columns;
}

inline constexpr
auto alloy::core::matrix2::data() noexcept -> pointer
{
  return &get(0,0);
}

inline constexpr
auto alloy::core::matrix2::data() const noexcept -> const_pointer
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline
auto alloy::core::matrix2::invert() noexcept -> matrix2&
{
  (*this) = inverse();
  return (*this);
}

inline
auto alloy::core::matrix2::transpose() noexcept -> matrix2&
{
  using std::swap;

  swap(get(0,1),get(1,0));
  return (*this);
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::matrix2::operator+=(const matrix2& rhs) noexcept -> matrix2&
{
  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

inline
auto alloy::core::matrix2::operator-=(const matrix2& rhs) noexcept -> matrix2&
{
  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

inline
auto alloy::core::matrix2::operator*=(const matrix2& rhs) noexcept -> matrix2&
{
  auto result = matrix2{};

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
auto alloy::core::matrix2::operator*=(real scalar) noexcept -> matrix2&
{
  for (auto c = 0; c < columns; ++c) {
    for (auto r = 0; r < rows; ++r) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

inline
auto alloy::core::matrix2::operator/=(real scalar) noexcept -> matrix2&
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
// non-member functions : class : matrix2
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline
auto alloy::core::operator+(const matrix2& lhs, const matrix2& rhs)
  noexcept -> matrix2
{
  return matrix2{lhs} += rhs;
}

inline
auto alloy::core::operator-(const matrix2& lhs, const matrix2& rhs)
  noexcept -> matrix2
{
  return matrix2{lhs} -= rhs;
}

inline
auto alloy::core::operator*(const matrix2& lhs, const matrix2& rhs)
  noexcept -> matrix2
{
  return matrix2{lhs} *= rhs;
}

inline
auto alloy::core::operator*(const matrix2& lhs, const vector2& rhs)
  noexcept -> vector2
{
  return lhs.combine(rhs);
}

inline
auto alloy::core::operator*(const vector2& lhs, const matrix2& rhs)
  noexcept -> vector2
{
  auto result = vector2{};

  for (auto c = 0; c < matrix2::columns; ++c) {
    auto sum = real{0};

    for (auto r = 0; r < matrix2::rows; ++r) {
      sum += lhs[r] * rhs.get(r,c);
    }
    result[c] = sum;
  }
  return result;
}

inline
auto alloy::core::operator*(real lhs, const matrix2& rhs) noexcept -> matrix2
{
  return matrix2{rhs} *= lhs;
}

inline
auto alloy::core::operator*(const matrix2& lhs, real rhs) noexcept -> matrix2
{
  return matrix2{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr
auto alloy::core::operator==(const matrix2& lhs, const matrix2& rhs)
  noexcept -> bool
{
  for (auto c = 0; c < matrix2::columns; ++c) {
    for (auto r = 0; r < matrix2::rows; ++r) {
      if (lhs.get(r,c) != rhs.get(r,c)) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr
auto alloy::core::operator!=(const matrix2& lhs, const matrix2& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr auto
  alloy::core::almost_equal(const matrix2& lhs,
                            const matrix2& rhs,
                            real tolerance)
  noexcept -> bool
{
  for (auto c = 0; c < matrix2::columns; ++c) {
    for (auto r = 0; r < matrix2::rows; ++r) {
      if (!almost_equal(lhs.get(r,c), rhs.get(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX2_HPP */

