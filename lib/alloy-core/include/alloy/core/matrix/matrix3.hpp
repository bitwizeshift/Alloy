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
#ifndef ALLOY_CORE_MATRIX_MATRIX3_HPP
#define ALLOY_CORE_MATRIX_MATRIX3_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"      // core::real
#include "alloy/core/vector/vector3.hpp" // vector3

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
  class matrix3
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
    constexpr matrix3( const vector3& v0,
                       const vector3& v1,
                       const vector3& v2 ) noexcept;

    /// \brief Constructs a matrix3 from an array of 4 entries
    ///
    /// \param array the 1-dimensional array
    explicit constexpr matrix3( const real(&array)[9] ) noexcept;

    /// \brief Constructs a matrix3 from a 2-dimensional array
    ///
    /// \param array the 2-dimensional array
    explicit constexpr matrix3( const real(&array)[3][3] ) noexcept;

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
    constexpr matrix3( real m00, real m01, real m02,
                       real m10, real m11, real m12,
                       real m20, real m21, real m22 ) noexcept;

    /// \brief Copy-constructs a matrix3 from another matrix3
    ///
    /// \param other the other matrix3 to copy
    constexpr matrix3( const matrix3& other ) noexcept = default;

    /// \brief Move-constructs a matrix3 from another matrix3
    ///
    /// \param other the other matrix3 to move
    constexpr matrix3( matrix3&& other ) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Copy-assigns a matrix3 from another matrix3
    ///
    /// \param other the other entry to copy
    /// \return reference to \c (*this)
    matrix3& operator=( const matrix3& other ) = default;


    /// \brief Move-assigns a matrix3 from another matrix3
    ///
    /// \param other the other entry to move
    /// \return reference to \c (*this)
    matrix3& operator=( matrix3&& other ) = default;

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

    /// \{
    /// \brief Retrieves the matrix entry at column \p c and row \p r
    ///
    /// \param r the row to retrieve
    /// \param c the column to retrieve
    /// \return a proxy to the row
    constexpr reference get( index_type r, index_type c ) noexcept;
    constexpr const_reference get( index_type r, index_type c ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the vector at row \p r
    ///
    /// \param r the row index to retrieve
    constexpr vector3 row( index_type r ) const noexcept;

    /// \brief Retrieves the vector at row \p c
    ///
    /// \param c the column index to retrieve
    constexpr vector3 column( index_type c ) const noexcept;

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

    /// Calculates the determinant for this matrix3
    ///
    /// \returns the determinant of this matrix
    constexpr real determinant() const noexcept;

    /// Calculates the trace for this matrix3
    ///
    /// \returns the trace of this matrix
    constexpr real trace() const noexcept;

    /// \brief Computes the inverse of this matrix3
    ///
    /// If no inverse is possible, this returns \ref matrix3::identity
    ///
    /// \return the inverse of this matrix3
    constexpr matrix3 inverse() const noexcept;

    /// \brief Computes the transpose of this matrix3
    ///
    /// \return the transpose of this matrix3
    constexpr matrix3 transposed() const noexcept;

    /// \brief Combines \c (*this) with \p vec
    ///
    /// \param vec the vector to combine
    /// \return the result of \c vec * matrix
    constexpr vector3 combine( const vector3& vec ) const noexcept;

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
    constexpr matrix3& invert() noexcept;

    /// \brief Transposes this current matrix3, returning a reference to
    ///        \c (*this)
    ///
    /// \return reference to \c (*this)
    constexpr matrix3& transpose() noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    matrix3& operator+=( const matrix3& rhs ) noexcept;
    matrix3& operator-=( const matrix3& rhs ) noexcept;
    matrix3& operator*=( const matrix3& rhs ) noexcept;
    matrix3& operator*=( real scalar ) noexcept;
    matrix3& operator/=( real scalar ) noexcept;

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

  matrix3 operator+( const matrix3& lhs,
                     const matrix3& rhs ) noexcept;
  matrix3 operator-( const matrix3& lhs,
                     const matrix3& rhs ) noexcept;
  matrix3 operator*( const matrix3& lhs,
                     const matrix3& rhs ) noexcept;
  vector3 operator*( const vector3& lhs,
                     const matrix3& rhs ) noexcept;
  matrix3 operator*( real lhs,
                     const matrix3& rhs ) noexcept;
  matrix3 operator*( const matrix3& lhs,
                     real rhs ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const matrix3& lhs,
                             const matrix3& rhs ) noexcept;
  constexpr bool operator!=( const matrix3& lhs,
                             const matrix3& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two matrix3 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain almost equal values
  constexpr bool almost_equal( const matrix3& lhs,
                               const matrix3& rhs ) noexcept;

  /// \brief Determines equality between two matrix3 relative to \p tolerance
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain almost equal values
  constexpr bool almost_equal( const matrix3& lhs,
                               const matrix3& rhs,
                               real tolerance ) noexcept;

  //============================================================================
  // aliases
  //============================================================================

  using mat3 = matrix3;

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

#include "detail/matrix3.inl"

#endif /* ALLOY_CORE_MATRIX_MATRIX3_HPP */
