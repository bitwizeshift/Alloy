/****************************************************************************
 * \file matrix4.hpp
 *
 * \brief This header provides the defintion for a 4x4 matrix class.
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
#ifndef ALLOY_CORE_MATRIX_MATRIX4_HPP
#define ALLOY_CORE_MATRIX_MATRIX4_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"      // core::real
#include "alloy/core/vector/vector3.hpp" // vector3
#include "alloy/core/vector/vector4.hpp" // vector4

#include <type_traits> // std::true_type, std::false_type, std::common_type
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range
#include <cassert>     // assert
#include <utility>     // std::swap

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
  class matrix4
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
    constexpr matrix4( const vector4& v0,
                       const vector4& v1,
                       const vector4& v2,
                       const vector4& v3 ) noexcept;

    /// \brief Constructs a matrix4 from an array of 4 entries
    ///
    /// \param array the 1-dimensional array
    constexpr matrix4( const real(&array)[16] ) noexcept;

    /// \brief Constructs a matrix4 from a 2-dimensional array
    ///
    /// \param array the 2-dimensional array
    constexpr matrix4( const real(&array)[4][4] ) noexcept;

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
    constexpr matrix4( real m00, real m01, real m02, real m03,
                       real m10, real m11, real m12, real m13,
                       real m20, real m21, real m22, real m23,
                       real m30, real m31, real m32, real m33 ) noexcept;

    /// \brief Copy-constructs a matrix4 from another matrix4
    ///
    /// \param other the other matrix4 to copy
    constexpr matrix4( const matrix4& other ) noexcept = default;

    /// \brief Move-constructs a matrix4 from another matrix4
    ///
    /// \param other the other matrix4 to move
    constexpr matrix4( matrix4&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a matrix4 from another matrix4
    ///
    /// \param other the other entry to copy
    /// \return reference to \c (*this)
    matrix4& operator=( const matrix4& other ) = default;

    /// \brief Move-assigns a matrix4 from another matrix4
    ///
    /// \param other the other entry to move
    /// \return reference to \c (*this)
    matrix4& operator=( matrix4&& other ) = default;

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
    /// \brief Retrieves the matrix entry at column \p c, and returns a
    ///        proxy to that row
    ///
    /// \param c the column
    /// \return a proxy to the row
    constexpr reference get( index_type r, index_type c ) noexcept;
    constexpr const_reference get( index_type r, index_type c ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the vector at row \p r
    ///
    /// \param r the row index to retrieve
    constexpr vector4 row( index_type r ) const noexcept;

    /// \brief Retrieves the vector at row \p c
    ///
    /// \param c the column index to retrieve
    constexpr vector4 column( index_type c ) const noexcept;

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

    /// Calculates the determinant for this matrix4
    ///
    /// \returns the determinant of this matrix
    constexpr real determinant() const noexcept;

    /// Calculates the trace for this matrix4
    ///
    /// \returns the trace of this matrix
    constexpr real trace() const noexcept;

    /// \brief Computes the inverse of this matrix4
    ///
    /// If no inverse is possible, this returns \ref matrix4::identity
    ///
    /// \return the inverse of this matrix4
    constexpr matrix4 inverse() const noexcept;

    /// \brief Computes the transpose of this matrix4
    ///
    /// \return the transpose of this matrix4
    constexpr matrix4 transposed() const noexcept;

    /// \brief Combines \c (*this) with \p vec
    ///
    /// \param vec the vector to combine
    /// \return the result of \c vec * matrix
    constexpr vector4 combine( const vector4& vec ) const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Inverts this current matrix4, and returns a reference to
    ///        \c (*this)
    ///
    /// If there is no inverse possible, this function returns a
    /// reference to \ref matrix4::identity, and the current matrix4
    /// is left unmodified
    ///
    /// \return reference to \c (*this)
    constexpr matrix4& invert() noexcept;

    /// \brief Transposes this current matrix4, returning a reference to
    ///        \c (*this)
    ///
    /// \return reference to \c (*this)
    constexpr matrix4& transpose() noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    matrix4& operator+=( const matrix4& rhs ) noexcept;
    matrix4& operator-=( const matrix4& rhs ) noexcept;
    matrix4& operator*=( const matrix4& rhs ) noexcept;
    matrix4& operator*=( real scalar ) noexcept;
    matrix4& operator/=( real scalar ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    alignas(16) real m_matrix[rows][columns]; ///< Linear array that represents the matrix

  };

  //============================================================================
  // non-member functions : class : matrix4
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  matrix4 operator+( const matrix4& lhs,
                     const matrix4& rhs ) noexcept;
  matrix4 operator-( const matrix4& lhs,
                     const matrix4& rhs ) noexcept;
  matrix4 operator*( const matrix4& lhs,
                     const matrix4& rhs ) noexcept;
  vector4 operator*( const vector4& lhs,
                     const matrix4& rhs ) noexcept;
  matrix4 operator*( real lhs,
                     const matrix4& rhs ) noexcept;
  matrix4 operator*( const matrix4& lhs,
                     real rhs ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const matrix4& lhs, const matrix4& rhs ) noexcept;
  constexpr bool operator!=( const matrix4& lhs, const matrix4& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two matrix4 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left matrix4
  /// \param rhs the right matrix4
  /// \return \c true if the two matrix4 contain almost equal values
  constexpr bool almost_equal( const matrix4& lhs,
                               const matrix4& rhs ) noexcept;

  /// \brief Determines equality between two matrix4 relative to \ref tolerance
  ///
  /// \param lhs the left matrix4
  /// \param rhs the right matrix4
  /// \return \c true if the two matrix4 contain almost equal values
  constexpr bool almost_equal( const matrix4& lhs,
                               const matrix4& rhs,
                               real tolerance ) noexcept;

  //============================================================================
  // aliases
  //============================================================================

  using mat4 = matrix4;

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref matrix4
  ///
  /// The result is aliased as \c ::value
  template<typename T>
  struct is_matrix4 : std::false_type{};

  template<>
  struct is_matrix4<matrix4> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_matrix4
  template<typename T>
  constexpr bool is_matrix4_v = is_matrix4<T>::value;

} // namespace alloy::core

#include "detail/matrix4.inl"

#endif /* ALLOY_CORE_MATRIX_MATRIX4_HPP */
