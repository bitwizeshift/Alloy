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
  // class : matrix3<T>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Defines a 3x3 Matrix
  ///
  /// The matrix is accessed in column-major format (e.g. mat[col][row]) as
  /// opposed to the native [row][col] way that C++ handles 2d arrays
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class matrix3
  {
    //--------------------------------------------------------------------------
    // Public Types
    //--------------------------------------------------------------------------
  public:

    using value_type      = T;
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

    static inline constexpr auto rows    = size_type{3};
    static inline constexpr auto columns = size_type{3};

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a matrix3.
    ///
    /// A default constructs matrix3 has an undefined default value
    constexpr matrix3() noexcept = default;

    /// \brief Constructs a matrix3 from 2 row vectors
    ///
    /// This constructor is to allow a fluid interface for construction:
    /// \code
    /// auto mat = matrix3<float> { { 1, 0 },
    ///                             { 0, 1 } };
    /// \endcode
    ///
    /// \param v0 the first row vector
    /// \param v1 the second row vector
    constexpr matrix3( const vector3<T>& v0,
                       const vector3<T>& v1,
                       const vector3<T>& v2 ) noexcept;

    /// \brief Constructs a matrix3 from an array of 4 entries
    ///
    /// \param array the 1-dimensional array
    explicit constexpr matrix3( const value_type(&array)[9] ) noexcept;

    /// \brief Constructs a matrix3 from a 2-dimensional array
    ///
    /// \param array the 2-dimensional array
    explicit constexpr matrix3( const value_type(&array)[3][3] ) noexcept;

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
    constexpr matrix3( value_type m00, value_type m01, value_type m02,
                       value_type m10, value_type m11, value_type m12,
                       value_type m20, value_type m21, value_type m22 ) noexcept;

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
    /// \brief Retrieves the matrix entry at column \p c, and returns a
    ///        proxy to that row
    ///
    /// \param c the column
    /// \return a proxy to the row
    constexpr reference
      operator()( index_type r, index_type c ) noexcept;
    constexpr const_reference
      operator()( index_type r, index_type c ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------

    /// \brief Retrieves the vector at row \p r
    ///
    /// \param r the row index to retrieve
    constexpr vector3<T> row( index_type r ) const noexcept;

    /// \brief Retrieves the vector at row \p c
    ///
    /// \param c the column index to retrieve
    constexpr vector3<T> column( index_type c ) const noexcept;

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
    constexpr value_type determinant() const noexcept;

    /// Calculates the trace for this matrix3
    ///
    /// \returns the trace of this matrix
    constexpr value_type trace() const noexcept;

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
    template<typename U>
    constexpr vector3<std::common_type_t<T,U>>
      combine( const vector3<U>& vec ) const noexcept;

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

    template<typename U>
    matrix3& operator+=( const matrix3<U>& rhs ) noexcept;
    template<typename U>
    matrix3& operator-=( const matrix3<U>& rhs ) noexcept;
    template<typename U>
    matrix3& operator*=( const matrix3<U>& rhs ) noexcept;
    template<typename U>
    matrix3& operator*=( U scalar ) noexcept;
    template<typename U>
    matrix3& operator/=( U scalar ) noexcept;

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
  private:

    value_type m_matrix[rows][columns]; ///< Linear array that represents the matrix

    template<typename> friend class matrix3;

    //----------------------------------------------------------------------
    // Private Member Functions
    //----------------------------------------------------------------------
  private:

    /// \{
    /// \brief Non-throwing matrix element access
    ///
    /// \param r the row
    /// \param c the column
    /// \return reference to the entry
    constexpr reference get( index_type r, index_type c ) noexcept;
    constexpr const_reference get( index_type r, index_type c ) const noexcept;
    /// \}

  };

  //============================================================================
  // non-member functions : class : matrix3<T>
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  template<typename T, typename U>
  constexpr matrix3<std::common_type_t<T,U>>
    operator+( const matrix3<T>& lhs, const matrix3<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr matrix3<std::common_type_t<T,U>>
    operator-( const matrix3<T>& lhs, const matrix3<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr matrix3<std::common_type_t<T,U>>
    operator*( const matrix3<T>& lhs, const matrix3<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr vector3<std::common_type_t<T,U>>
    operator*( const vector3<T>& lhs, const matrix3<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr matrix3<std::common_type_t<T,U>>
    operator*( T lhs, const matrix3<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr matrix3<std::common_type_t<T,U>>
    operator*( const matrix3<T>& lhs, U rhs ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  /// \brief Determines exact equality between two matrix3
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain identical values
  template<typename T, typename U>
  constexpr bool operator==( const matrix3<T>& lhs,
                             const matrix3<U>& rhs ) noexcept;

  /// \brief Determines exact inequality between two matrix3
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain at least 1 different value
  template<typename T, typename U>
  constexpr bool operator!=( const matrix3<T>& lhs,
                             const matrix3<U>& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two matrix3 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain almost equal values
  template<typename T, typename U>
  constexpr bool almost_equal( const matrix3<T>& lhs,
                               const matrix3<U>& rhs ) noexcept;

  /// \brief Determines equality between two matrix3 relative to \p tolerance
  ///
  /// \param lhs the left matrix3
  /// \param rhs the right matrix3
  /// \return \c true if the two matrix3 contain almost equal values
  template<typename T, typename U, typename Arithmetic,
           typename = std::enable_if_t<std::is_arithmetic<Arithmetic>::value>>
  constexpr bool almost_equal( const matrix3<T>& lhs,
                               const matrix3<U>& rhs,
                               Arithmetic tolerance ) noexcept;

  //============================================================================
  // struct : matrix3_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of matrix3 constants
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct matrix3_constants
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------

    using matrix_type = matrix3<T>;

    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = matrix_type {
      T{0}, T{0}, T{0},
      T{0}, T{0}, T{0},
      T{0}, T{0}, T{0}
    };
    static inline constexpr auto identity = matrix_type {
      T{1}, T{0}, T{0},
      T{0}, T{1}, T{0},
      T{0}, T{0}, T{1}
    };
  };

  //============================================================================
  // aliases
  //============================================================================

  using matrix3f  = matrix3<float>;
  using matrix3d  = matrix3<double>;
  using matrix3ld = matrix3<long double>;
  using matrix3r  = matrix3<real>;

  using mat3f  = matrix3f;
  using mat3d  = matrix3d;
  using mat3ld = matrix3ld;
  using mat3r  = matrix3r;

  //----------------------------------------------------------------------------
  // Type Traits
  //----------------------------------------------------------------------------

  /// \brief Trait to detect whether \p T is a \ref matrix3
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_matrix3 : std::false_type{};
  template<typename T> struct is_matrix3<matrix3<T>> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_matrix3
  template<typename T>
  constexpr bool is_matrix3_v = is_matrix3<T>::value;

} // namespace alloy::core

#include "detail/matrix3.inl"

#endif /* ALLOY_CORE_MATRIX_MATRIX3_HPP */
