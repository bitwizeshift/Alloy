/****************************************************************************
 * \file matrix3.hpp
 *
 * \brief This header provides the defintion for a 3x3 matrix class.
 ****************************************************************************/

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
#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX3_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX3_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/api.hpp"                 // ALLOY_CORE_API
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/precision.hpp"           // core::real
#include "alloy/core/math/vector/vector3.hpp" // vector3

#include <type_traits> // std::true_type, std::false_type, std::common_type
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range
#include <cassert>     // assert
#include <utility>     // std::swap

namespace alloy::core {

  //============================================================================
  // class : matrix3
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Defines a 3x3 Matrix
  ///
  /// The matrix is accessed in column-major format (e.g. mat[col][row]) as
  /// opposed to the native [row][col] way that C++ handles 2d arrays
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API matrix3
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

    static inline constexpr auto rows    = index_type{3};
    static inline constexpr auto columns = index_type{3};
    static inline constexpr auto comparison_tolerance = default_tolerance;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a matrix3.
    ///
    /// Default constructs a matrix3 with an empty matrix (0 values)
    constexpr matrix3() noexcept;

    /// \brief Constructs a matrix3 from 2 row vectors
    ///
    /// This constructor is to allow a fluid interface for construction:
    /// \code
    /// auto mat = matrix3 { { 1, 0 },
    ///                      { 0, 1 } };
    /// \endcode
    ///
    /// \param v0 the first row vector
    /// \param v1 the second row vector
    constexpr matrix3(const vector3& v0,
                      const vector3& v1,
                      const vector3& v2) noexcept;

    /// \brief Constructs a matrix3 from an array of 4 entries
    ///
    /// \param array the 1-dimensional array
    explicit constexpr matrix3(const real(&array)[9]) noexcept;

    /// \brief Constructs a matrix3 from a 2-dimensional array
    ///
    /// \param array the 2-dimensional array
    explicit constexpr matrix3(const real(&array)[3][3]) noexcept;

    /// \brief Constructs a matrix3 from value entries
    ///
    /// \param m00 the entry at coordinate (0,0)
    /// \param m01 the entry at coordinate (0,1)
    /// \param m02 the entry at coordinate (0,2)
    /// \param m10 the entry at coordinate (1,0)
    /// \param m11 the entry at coordinate (1,1)
    /// \param m12 the entry at coordinate (1,2)
    /// \param m20 the entry at coordinate (2,0)
    /// \param m21 the entry at coordinate (2,1)
    /// \param m22 the entry at coordinate (2,2)
    constexpr matrix3(real m00, real m01, real m02,
                      real m10, real m11, real m12,
                      real m20, real m21, real m22) noexcept;

    /// \brief Copy-constructs a matrix3 from another matrix3
    ///
    /// \param other the other matrix3 to copy
    constexpr matrix3(const matrix3& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a matrix3 from another matrix3
    ///
    /// \param other the other entry to copy
    /// \return reference to \c (*this)
    auto operator=(const matrix3& other) -> matrix3& = default;

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

    /// \{
    /// \brief Retrieves the matrix entry at column \p c and row \p r
    ///
    /// \param r the row to retrieve
    /// \param c the column to retrieve
    /// \return a proxy to the row
    constexpr auto get(index_type r, index_type c) noexcept -> reference;
    constexpr auto get(index_type r, index_type c) const noexcept -> const_reference;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the vector at row \p r
    ///
    /// \param r the row index to retrieve
    constexpr auto row(index_type r) const noexcept -> vector3;

    /// \brief Retrieves the vector at row \p c
    ///
    /// \param c the column index to retrieve
    constexpr auto column(index_type c) const noexcept -> vector3;

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

    /// Calculates the determinant for this matrix3
    ///
    /// \returns the determinant of this matrix
    auto determinant() const noexcept -> real;

    /// Calculates the trace for this matrix3
    ///
    /// \returns the trace of this matrix
    auto trace() const noexcept -> real;

    /// \brief Computes the inverse of this matrix3
    ///
    /// If no inverse is possible, this returns \ref matrix3::identity
    ///
    /// \return the inverse of this matrix3
    auto inverse() const noexcept -> matrix3;

    /// \brief Computes the transpose of this matrix3
    ///
    /// \return the transpose of this matrix3
    auto transposed() const noexcept -> matrix3;

    /// \brief Combines \c (*this) with \p vec
    ///
    /// \param vec the vector to combine
    /// \return the result of \c vec * matrix
    auto combine(const vector3& vec) const noexcept -> vector3;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts this current matrix3, and returns a reference to
    ///        \c (*this)
    ///
    /// If there is no inverse possible, this function returns a
    /// reference to \ref matrix3::identity, and the current matrix3
    /// is left unmodified
    ///
    /// \return reference to \c (*this)
    auto invert() noexcept -> matrix3&;

    /// \brief Transposes this current matrix3, returning a reference to
    ///        \c (*this)
    ///
    /// \return reference to \c (*this)
    auto transpose() noexcept -> matrix3&;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

   auto operator+=(const matrix3& rhs) noexcept -> matrix3&;
   auto operator-=(const matrix3& rhs) noexcept -> matrix3&;
   auto operator*=(const matrix3& rhs) noexcept -> matrix3&;
   auto operator*=(real scalar) noexcept -> matrix3&;
   auto operator/=(real scalar) noexcept -> matrix3&;

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
  private:

    real m_matrix[rows][columns]; ///< Linear array that represents the matrix

  };

  //============================================================================
  // non-member functions : class : matrix3
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  auto operator+(const matrix3& lhs, const matrix3& rhs) noexcept -> matrix3;
  auto operator-(const matrix3& lhs, const matrix3& rhs) noexcept -> matrix3;
  auto operator*(const matrix3& lhs, const matrix3& rhs) noexcept -> matrix3;
  auto operator*(const vector3& lhs, const matrix3& rhs) noexcept -> vector3;
  auto operator*(real lhs, const matrix3& rhs) noexcept -> matrix3;
  auto operator*(const matrix3& lhs, real rhs ) noexcept -> matrix3;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr auto operator==(const matrix3& lhs, const matrix3& rhs) noexcept -> bool;
  constexpr auto operator!=(const matrix3& lhs, const matrix3& rhs) noexcept -> bool;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two matrix3 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain almost equal values
  constexpr auto almost_equal(const matrix3& lhs,
                              const matrix3& rhs) noexcept -> bool;

  /// \brief Determines equality between two matrix3 relative to \p tolerance
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain almost equal values
  constexpr auto almost_equal(const matrix3& lhs,
                              const matrix3& rhs,
                              real tolerance) noexcept -> bool;

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref matrix3
  ///
  /// The result is aliased as \c ::value
  template<typename T>
  struct is_matrix3 : std::false_type{};

  template<>
  struct is_matrix3<matrix3> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_matrix3
  template<typename T>
  constexpr bool is_matrix3_v = is_matrix3<T>::value;

} // namespace alloy::core

//==============================================================================
// class : matrix3
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix3::matrix3()
  noexcept
  : m_matrix {
    {real{0}, real{0}, real{0}},
    {real{0}, real{0}, real{0}},
    {real{0}, real{0}, real{0}}
  }
{

}

inline constexpr alloy::core::matrix3::matrix3(const vector3& v0,
                                               const vector3& v1,
                                               const vector3& v2)
  noexcept
  : m_matrix {
      {v0.x(), v0.y(), v0.z()},
      {v1.x(), v1.y(), v1.z()},
      {v2.x(), v2.y(), v2.z()}
    }
{

}

inline constexpr alloy::core::matrix3::matrix3(const real(&array)[9])
  noexcept
  : m_matrix {
      {array[0], array[1], array[2]},
      {array[3], array[4], array[5]},
      {array[6], array[7], array[8]}
    }
{

}

inline constexpr alloy::core::matrix3::matrix3(const real(&array)[3][3])
  noexcept
  : m_matrix {
      {array[0][0], array[0][1], array[0][2]},
      {array[1][0], array[1][1], array[1][2]},
      {array[2][0], array[2][1], array[2][2]}
    }
{

}

inline constexpr alloy::core::matrix3::matrix3(real m00, real m01, real m02,
                                               real m10, real m11, real m12,
                                               real m20, real m21, real m22)
  noexcept
  : m_matrix {
      {m00, m01, m02},
      {m10, m11, m12},
      {m20, m21, m22}
    }
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr auto
  alloy::core::matrix3::at(index_type r, index_type c)
  -> reference
{
  if (c >= columns || c < 0 || r >= rows || r < 0) {
    throw std::out_of_range{"matrix3::at: index out of range"};
  }
  return get(r,c);
}

inline constexpr auto
  alloy::core::matrix3::at(index_type r, index_type c)
  const -> const_reference
{
  if (c >= columns || c < 0 || r >= rows || r < 0) {
    throw std::out_of_range{"matrix3::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

inline constexpr auto
  alloy::core::matrix3::get(index_type r, index_type c)
  noexcept -> reference
{
  return m_matrix[r][c];
}

inline constexpr auto
  alloy::core::matrix3::get(index_type r, index_type c)
  const noexcept -> const_reference
{
  return m_matrix[r][c];
}

//------------------------------------------------------------------------------

inline constexpr auto
  alloy::core::matrix3::row(index_type r)
  const noexcept -> vector3
{
  return vector3{ get(r,0), get(r,1), get(r,2) };
}

inline constexpr auto
  alloy::core::matrix3::column(index_type c)
  const noexcept -> vector3
{
  return vector3{ get(0,c), get(1,c), get(2,c) };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr auto
  alloy::core::matrix3::size()
  const noexcept -> size_type
{
  return rows * columns;
}

inline constexpr auto
  alloy::core::matrix3::data()
  noexcept -> pointer
{
  return &get(0,0);
}

inline constexpr auto
  alloy::core::matrix3::data()
  const noexcept -> const_pointer
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline auto
  alloy::core::matrix3::invert()
  noexcept -> matrix3&
{
  (*this) = inverse();
  return (*this);
}

//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline auto
  alloy::core::matrix3::operator+=(const matrix3& rhs)
  noexcept -> matrix3&
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

inline auto
  alloy::core::matrix3::operator-=(const matrix3& rhs)
  noexcept -> matrix3&
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

inline auto
  alloy::core::matrix3::operator*=(const matrix3& rhs)
  noexcept -> matrix3&
{
  auto result = matrix3{};

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {

      auto sum = real{0};

      for (auto i = 0; i < rows; ++i) {
        sum += (get(i,c) * rhs.get(r,i));
      }
      result.get(r,c) = sum;
    }
  }

  // Copy result in
  (*this) = result;

  return (*this);
}

inline auto
  alloy::core::matrix3::operator*=(real scalar)
  noexcept -> matrix3&
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

inline auto
  alloy::core::matrix3::operator/=(real scalar)
  noexcept -> matrix3&
{
  const auto inv = real{1} / scalar;

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= inv;
    }
  }
  return (*this);
}

//==============================================================================
// non-member functions : class : matrix3
//==============================================================================

//--------------------------------------------------------------------------------
// Arithmetic Operators
//--------------------------------------------------------------------------------

inline auto
  alloy::core::operator+(const matrix3& lhs, const matrix3& rhs)
  noexcept -> matrix3
{
  return matrix3{lhs} += rhs;
}

inline auto
  alloy::core::operator-(const matrix3& lhs, const matrix3& rhs)
  noexcept -> matrix3
{
  return matrix3{lhs} -= rhs;
}

inline auto
  alloy::core::operator*(const matrix3& lhs, const matrix3& rhs)
  noexcept -> matrix3
{
  return matrix3{lhs} *= rhs;
}

inline auto
  alloy::core::operator*(const vector3& lhs, const matrix3& rhs)
  noexcept -> vector3
{
  return rhs.combine(lhs);
}

inline auto
  alloy::core::operator*(real lhs, const matrix3& rhs)
  noexcept -> matrix3
{
  return matrix3{rhs} *= lhs;
}

inline auto
  alloy::core::operator*(const matrix3& lhs, real rhs)
  noexcept -> matrix3
{
  return matrix3{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GNULIKE_DIAGNOSTIC_IGNORE(-Wfloat-equal)

inline constexpr auto
  alloy::core::operator==(const matrix3& lhs, const matrix3& rhs)
  noexcept -> bool
{
  for (auto r = 0; r < matrix3::rows; ++r) {
    for (auto c = 0; c < matrix3::columns; ++c) {
      if (lhs.get(r,c) != rhs.get(r,c)) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr auto
  alloy::core::operator!=(const matrix3& lhs, const matrix3& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

//------------------------------------------------------------------------------

inline constexpr auto
  alloy::core::almost_equal(const matrix3& lhs, const matrix3& rhs)
  noexcept -> bool
{
  return almost_equal(lhs, rhs, matrix3::comparison_tolerance);
}

inline constexpr auto
  alloy::core::almost_equal(const matrix3& lhs,
                            const matrix3& rhs,
                            real tolerance)
  noexcept -> bool
{
  for (auto r = 0; r < matrix3::rows; ++r) {
    for (auto c = 0; c < matrix3::columns; ++c) {
      if (!almost_equal(lhs.get(r,c), rhs.get(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX3_HPP */
