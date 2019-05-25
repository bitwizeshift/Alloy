/****************************************************************************
 * \file matrix2.hpp
 *
 * \brief This header provides the defintion for a 2x2 matrix class.
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
#ifndef ALLOY_CORE_MATH_MATRIX_MATRIX2_HPP
#define ALLOY_CORE_MATH_MATRIX_MATRIX2_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"           // core::real
#include "alloy/core/math/vector/vector2.hpp" // vector2

#include <type_traits> // std::true_type, std::false_type, std::common_type
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range
#include <cassert>     // assert
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
  class matrix2
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
    constexpr matrix2( const vector2& v0,
                       const vector2& v1 ) noexcept;

    /// \brief Constructs a matrix2 from an array of 4 entries
    ///
    /// \param array the 1-dimensional array
    explicit constexpr matrix2( const real(&array)[4] ) noexcept;

    /// \brief Constructs a matrix2 from a 2-dimensional array
    ///
    /// \param array the 2-dimensional array
    explicit constexpr matrix2( const real(&array)[2][2] ) noexcept;

    /// \brief Constructs a matrix2 from value entries
    ///
    /// \param m00 the entry at coordinate (0,0)
    /// \param m01 the entry at coordinate (0,1)
    /// \param m10 the entry at coordinate (1,0)
    /// \param m11 the entry at coordinate (1,1)
    constexpr matrix2( real m00, real m01,
                       real m10, real m11 ) noexcept;

    /// \brief Copy-constructs a matrix2 from another matrix2
    ///
    /// \param other the other matrix2 to copy
    constexpr matrix2( const matrix2& other ) noexcept = default;

    /// \brief Move-constructs a matrix2 from another matrix2
    ///
    /// \param other the other matrix2 to move
    constexpr matrix2( matrix2&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a matrix2 from another matrix2
    ///
    /// \param other the other entry to copy
    /// \return reference to \c (*this)
    matrix2& operator=( const matrix2& other ) = default;

    /// \brief Move-assigns a matrix2 from another matrix2
    ///
    /// \param other the other entry to move
    /// \return reference to \c (*this)
    matrix2& operator=( matrix2&& other ) = default;

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
    constexpr reference at( index_type r, index_type c );
    constexpr const_reference at( index_type r, index_type c ) const;
    /// \}

    //--------------------------------------------------------------------------

    /// \{
    /// \brief Retrieves the matrix entry at column \p c and row \p r
    ///
    /// \param r the row
    /// \param c the column
    /// \return reference to the entry
    constexpr reference get( index_type r, index_type c ) noexcept;
    constexpr const_reference get( index_type r, index_type c ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the vector at row \p r
    ///
    /// \param r the row index to retrieve
    constexpr vector2 row( index_type r ) const noexcept;

    /// \brief Retrieves the vector at row \p c
    ///
    /// \param c the column index to retrieve
    constexpr vector2 column( index_type c ) const noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Retrieves the number of entries in this matrix
    ///
    /// \return the size of the matrix
    constexpr size_type size() const noexcept;

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// Calculates the determinant for this matrix2
    ///
    /// \returns the determinant of this matrix
    constexpr real determinant() const noexcept;

    /// Calculates the trace for this matrix2
    ///
    /// \returns the trace of this matrix
    constexpr real trace() const noexcept;

    /// \brief Computes the inverse of this matrix2
    ///
    /// If no inverse is possible, this returns \ref matrix2::identity
    ///
    /// \return the inverse of this matrix2
    constexpr matrix2 inverse() const noexcept;

    /// \brief Computes the transpose of this matrix2
    ///
    /// \return the transpose of this matrix2
    constexpr matrix2 transposed() const noexcept;

    /// \brief Combines \c (*this) with \p vec
    ///
    /// \param vec the vector to combine
    /// \return the result of \c vec * matrix
    constexpr vector2 combine( const vector2& vec ) const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts this current matrix2, and returns a reference to
    ///        \c (*this)
    ///
    /// If there is no inverse possible, this function returns a
    /// reference to \ref matrix2::identity, and the current matrix2
    /// is left unmodified
    ///
    /// \return reference to \c (*this)
    matrix2& invert() noexcept;

    /// \brief Transposes this current matrix2, returning a reference to
    ///        \c (*this)
    ///
    /// \return reference to \c (*this)
    matrix2& transpose() noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    matrix2& operator+=( const matrix2& rhs ) noexcept;
    matrix2& operator-=( const matrix2& rhs ) noexcept;
    matrix2& operator*=( const matrix2& rhs ) noexcept;
    matrix2& operator*=( real scalar ) noexcept;
    matrix2& operator/=( real scalar ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    real m_matrix[rows][columns]; ///< Linear array that represents the matrix

  };

  //============================================================================
  // non-member functions : class : matrix2
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  matrix2 operator+( const matrix2& lhs,
                     const matrix2& rhs ) noexcept;
  matrix2 operator-( const matrix2& lhs,
                     const matrix2& rhs ) noexcept;
  matrix2 operator*( const matrix2& lhs,
                     const matrix2& rhs ) noexcept;
  vector2 operator*( const vector2& lhs,
                     const matrix2& rhs ) noexcept;
  matrix2 operator*( real lhs,
                     const matrix2& rhs ) noexcept;
  matrix2 operator*( const matrix2& lhs,
                     real rhs ) noexcept;

  //------------------------------------------------------------------------
  // Comparisons
  //------------------------------------------------------------------------

  constexpr bool operator==( const matrix2& lhs,
                             const matrix2& rhs ) noexcept;
  constexpr bool operator!=( const matrix2& lhs,
                             const matrix2& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two matrix2 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left matrix2
  /// \param rhs the right matrix2
  /// \return \c true if the two matrix2 contain almost equal values
  constexpr bool almost_equal( const matrix2& lhs,
                               const matrix2& rhs ) noexcept;

  /// \brief Determines equality between two matrix2 relative to \ref tolerance
  ///
  /// \param lhs the left matrix2
  /// \param rhs the right matrix2
  /// \return \c true if the two matrix2 contain almost equal values
  constexpr bool almost_equal( const matrix2& lhs,
                               const matrix2& rhs,
                               real tolerance ) noexcept;

  //============================================================================
  // aliases
  //============================================================================

  using mat2 = matrix2;

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref matrix2
  ///
  /// The result is aliased as \c ::value
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

inline constexpr alloy::core::matrix2
  ::matrix2()
  noexcept
  : m_matrix {
      {real{0}, real{0}},
      {real{0}, real{0}}
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( const vector2& v0, const vector2& v1 )
  noexcept
  : m_matrix {
      {v0.x(), v0.y()},
      {v1.x(), v1.y()}
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( const real(&array)[4] )
  noexcept
  : m_matrix {
      {array[0], array[1]},
      {array[2], array[3]}
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( const real(&array)[2][2] )
  noexcept
  : m_matrix {
      {array[0][0], array[0][1]},
      {array[1][0], array[1][1]}
    }
{

}

inline constexpr alloy::core::matrix2
  ::matrix2( real m00, real m01,
             real m10, real m11 )
  noexcept
  : m_matrix {
      {m00, m01},
      {m10, m11}
    }
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix2::reference
  alloy::core::matrix2::at( index_type r, index_type c )
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}


inline constexpr alloy::core::matrix2::const_reference
  alloy::core::matrix2::at( index_type r, index_type c )
  const
{
  if( c >=columns || c < 0 || r >=rows || r < 0 ) {
    throw std::out_of_range{"matrix2::at: index out of range"};
  }
  return get(r,c);
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix2::reference
  alloy::core::matrix2::get( index_type r, index_type c )
  noexcept
{
  return m_matrix[r][c];
}

inline constexpr alloy::core::matrix2::const_reference
  alloy::core::matrix2::get( index_type r, index_type c )
  const noexcept
{
  return m_matrix[r][c];
}

//------------------------------------------------------------------------------

inline constexpr alloy::core::vector2
  alloy::core::matrix2::row( index_type r )
  const noexcept
{
  return vector2{ get(r,0), get(r,1) };
}

inline constexpr alloy::core::vector2
  alloy::core::matrix2::column( index_type c )
  const noexcept
{
  return vector2{ get(0,c), get(1,c) };
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline constexpr alloy::core::matrix2::size_type
  alloy::core::matrix2::size()
  const noexcept
{
  return rows*columns;
}

inline constexpr alloy::core::matrix2::pointer
  alloy::core::matrix2::data()
  noexcept
{
  return &get(0,0);
}

inline constexpr alloy::core::matrix2::const_pointer
  alloy::core::matrix2::data()
  const noexcept
{
  return &get(0,0);
}

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

inline constexpr alloy::core::real
  alloy::core::matrix2::determinant()
  const noexcept
{
  return (get(0,0) * get(1,1)) - (get(0,1) * get(1,0));
}

inline constexpr alloy::core::real
  alloy::core::matrix2::trace()
  const noexcept
{
  return (get(0,0) + get(1,1));
}

inline constexpr alloy::core::matrix2
  alloy::core::matrix2::inverse()
  const noexcept
{
  const auto det = determinant();

  if (det == real{0}) {
    return matrix2{};
  }

  const auto inv_det = (real{1} / det);
  return matrix2{
     get(1,1) * inv_det, -get(1,0) * inv_det,
    -get(0,1) * inv_det,  get(0,0) * inv_det
  };
}

inline constexpr alloy::core::matrix2
  alloy::core::matrix2::transposed()
  const noexcept
{
  return matrix2{
    get(0,0), get(1,0),
    get(0,1), get(1,1)
  };
}

constexpr alloy::core::vector2
  alloy::core::matrix2::combine( const vector2& vec )
  const noexcept
{
  auto result = vector2{};

  for (auto r = 0; r < columns; ++r) {
    auto sum = real{0};

    for (auto c = 0; c < rows; ++c) {
      sum += vec[c] * get(r,c);
    }
    result[r] = sum;
  }
  return result;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline alloy::core::matrix2&
  alloy::core::matrix2::invert()
  noexcept
{
  (*this) = inverse();
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::transpose()
  noexcept
{
  using std::swap;

  swap(get(0,1),get(1,0));
  return (*this);
}


//------------------------------------------------------------------------------
// Compound Operators
//------------------------------------------------------------------------------

inline alloy::core::matrix2&
  alloy::core::matrix2::operator+=( const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) += rhs.get(r,c);
    }
  }
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::operator-=( const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) -= rhs.get(r,c);
    }
  }
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::operator*=( const matrix2& rhs )
  noexcept
{
  auto result = matrix2{};

  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {

      auto sum = real{0};

      for (auto i = 0; i < columns; ++i) {
        sum += (get(i,c) * rhs.get(r,i));
      }
      result.get(r,c) = sum;
    }
  }

  // Copy result in
  (*this) = result;
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::operator*=( real scalar )
  noexcept
{
  for (auto r = 0; r < rows; ++r) {
    for (auto c = 0; c < columns; ++c) {
      get(r,c) *= scalar;
    }
  }
  return (*this);
}

inline alloy::core::matrix2&
  alloy::core::matrix2::operator/=( real scalar )
  noexcept
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
// non-member functions : class : matrix2
//==============================================================================

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

inline alloy::core::matrix2
  alloy::core::operator+( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{lhs} += rhs;
}

inline alloy::core::matrix2
  alloy::core::operator-( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{lhs} -= rhs;
}

inline alloy::core::matrix2
  alloy::core::operator*( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{lhs} *= rhs;
}

inline alloy::core::vector2
  alloy::core::operator*( const vector2& lhs, const matrix2& rhs )
  noexcept
{
  return rhs.combine(lhs);
}

inline alloy::core::matrix2
  alloy::core::operator*( real lhs, const matrix2& rhs )
  noexcept
{
  return matrix2{rhs} *= lhs;
}

inline alloy::core::matrix2
  alloy::core::operator*( const matrix2& lhs, real rhs )
  noexcept
{
  return matrix2{lhs} *= rhs;
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::operator==( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < matrix2::rows; ++r) {
    for (auto c = 0; c < matrix2::columns; ++c) {
      if (lhs.get(r,c) != rhs.get(r,c)) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::operator!=( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  return !(lhs==rhs);
}

//------------------------------------------------------------------------------

inline constexpr bool
  alloy::core::almost_equal( const matrix2& lhs, const matrix2& rhs )
  noexcept
{
  for (auto r = 0; r < matrix2::rows; ++r) {
    for (auto c = 0; c < matrix2::columns; ++c) {
      if (!almost_equal(rhs.get(r,c), rhs.get(r,c))) {
        return false;
      }
    }
  }
  return true;
}

inline constexpr bool
  alloy::core::almost_equal( const matrix2& lhs,
                             const matrix2& rhs,
                             real tolerance )
  noexcept
{
  for (auto r = 0; r < matrix2::rows; ++r) {
    for (auto c = 0; c < matrix2::columns; ++c) {
      if (!almost_equal(rhs.get(r,c), rhs.get(r,c), tolerance)) {
        return false;
      }
    }
  }
  return true;
}

#endif /* ALLOY_CORE_MATH_MATRIX_MATRIX2_HPP */
