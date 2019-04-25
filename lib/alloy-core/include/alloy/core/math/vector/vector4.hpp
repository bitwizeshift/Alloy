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

#include "alloy/core/precision.hpp" // core::real
#include "alloy/core/utilities/piecewise_compare.hpp" // core::piecewise_compare
#include "alloy/core/math/angle/radian.hpp" // core::radian
#include "alloy/core/math/trigonometry.hpp" // core::cos, core::sin, etc
#include "alloy/core/math/math.hpp"         // core::sqrt

#include <type_traits> // std::true_type, std::false_type, std::common_type
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range
#include <cassert>     // assert

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
  class vector4
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
    constexpr vector4( real x, real y, real z, real w ) noexcept;

    /// \brief Copy-constructs a vector4 with the value of another
    ///        vector4
    ///
    /// \param other the other vector4 to copy
    constexpr vector4( const vector4& other ) noexcept = default;

    /// \brief Move-constructs a vector4 with the value of another
    ///        vector4
    ///
    /// \param other the other vector4 to move
    constexpr vector4( vector4&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Copy-assigns \p other to \c this
    ///
    /// \param other the other vector4 to copy
    /// \return reference to \c (*this)
    vector4& operator=( const vector4& other ) = default;

    /// \brief Move-assigns \p other to \c this
    ///
    /// \param other the other vector4 to move
    /// \return reference to \c (*this)
    vector4& operator=( vector4&& other ) = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector4
    ///
    /// \return the number of components in the vector4
    constexpr size_type size() const noexcept;

    /// \{
    /// \brief Gets the x component of this vector
    ///
    /// \return reference to the x component
    constexpr reference x() noexcept;
    constexpr const_reference x() const noexcept;
    /// \}xx

    /// \{
    /// \brief Gets the y component of this vector
    ///
    /// \return reference to the y component
    constexpr reference y() noexcept;
    constexpr const_reference y() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the z component of this vector
    ///
    /// \return reference to the z component
    constexpr reference z() noexcept;
    constexpr const_reference z() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the w component of this vector
    ///
    /// \return reference to the w component
    constexpr reference w() noexcept;
    constexpr const_reference w() const noexcept;
    /// \}

    /// \{
    /// \brief Gets a pointer to the underlying data
    ///
    /// \return a pointer to the data
    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;
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
    constexpr reference at( index_type n );
    constexpr const_reference at( index_type n ) const;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 4 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr reference operator[]( index_type n ) noexcept;
    constexpr const_reference operator[]( index_type n ) const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Quantifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Determines the dot-product of \c this and \p rhs
    ///
    /// \param rhs the other vector4 to perform the dot-product with
    /// \return the dot product of \c this and \p rhs
    constexpr real dot( const vector4& rhs ) const noexcept;

    /// \brief Calculates the cross-product of \c this and \p rhs
    ///
    /// \note This cross-product is not a true 4-dimensional cross-product,
    ///       it is a cross product constrainted to 3-dimensions, clearing
    ///       the w-entry
    ///
    /// \param rhs the other vector4 to perform the cross-product with
    /// \return the cross product of \c this and \p rhs
    constexpr vector4 cross( const vector4& rhs ) const noexcept;

    /// \brief Gets the square magnitude of this vector4
    ///
    /// \return the square magnitude of the vector4
    constexpr real square_magnitude() const noexcept;

    /// \brief Gets the magnitude of this vector4
    ///
    /// \return the magnitude of the vector4
    real magnitude() const noexcept;

    /// \brief Gets the midpoint between \c this and \p vec
    ///
    /// \param vec the vector4 to get the midpoint from
    /// \return the midpoint between \c this and \p vec
    constexpr vector4 midpoint( const vector4& vec ) const noexcept;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    constexpr vector4 projection( const vector4& vector ) const noexcept;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    constexpr vector4 rejection( const vector4& vector ) const noexcept;

    /// \brief Gets the normalized vector4 of \c this
    ///
    /// \param the normalized vector4 of \c this
    vector4 normalized() const noexcept;

    /// \brief Gets the inverse of \c this vector4
    ///
    /// \return the inverse of \c this vector4
    constexpr vector4 inverse() const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector4 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    vector4& normalize() noexcept;

    /// \brief Inverts this vector4 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr vector4& invert() noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr const vector4& operator+() const noexcept;
    constexpr vector4 operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    constexpr vector4& operator+=( const vector4& rhs ) noexcept;
    constexpr vector4& operator-=( const vector4& rhs ) noexcept;
    constexpr vector4& operator*=( real scalar ) noexcept;
    constexpr vector4& operator/=( real scalar ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    value_type m_data[4]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : vector4
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  constexpr vector4 operator+( const vector4& lhs,
                               const vector4& rhs ) noexcept;
  constexpr vector4 operator-( const vector4& lhs,
                               const vector4& rhs ) noexcept;
  constexpr vector4 operator*( const vector4& lhs,
                               real scalar ) noexcept;
  constexpr vector4 operator*( real scalar,
                               const vector4& rhs ) noexcept;
  constexpr vector4 operator/( const vector4& lhs,
                               real scalar ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  constexpr bool operator==( const vector4& lhs,
                             const vector4& rhs ) noexcept;
  constexpr bool operator!=( const vector4& lhs,
                             const vector4& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two vector4 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return \c true if the two vector4 contain almost equal values
  constexpr bool almost_equal( const vector4& lhs,
                               const vector4& rhs ) noexcept;

  /// \brief Determines equality between two vector4 relative to \p tolerance
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return \c true if the two vector4 contain almost equal values
  constexpr bool almost_equal( const vector4& lhs,
                               const vector4& rhs,
                               real tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return
  constexpr real dot( const vector4& lhs, const vector4& rhs ) noexcept;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector4
  /// \param rhs the right vector4
  /// \return the result of the dot product
  constexpr vector4 cross( const vector4& lhs, const vector4& rhs ) noexcept;

  /// \brief Calculates the magnitude of the vector4 \p vec
  ///
  /// \param vec the vector4 to calculate the magnitude from
  /// \return the magnitude
  real magnitude( const vector4& vec ) noexcept;

  //============================================================================
  // struct : piecewise_compare<vector4>
  //============================================================================

  template<>
  struct piecewise_compare<vector4>
  {
    constexpr bool operator()( const vector4& lhs,
                               const vector4& rhs ) noexcept;
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
  // aliases
  //============================================================================

  using vec4 = vector4;

} // namespace alloy::core

#include "detail/vector4.inl"

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR4_HPP */
