/******************************************************************************
 * \file vector2.hpp
 *
 * \brief This header contains the definition of a 2D vector class
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
#ifndef ALLOY_MATH_VECTOR_VECTOR2_HPP
#define ALLOY_MATH_VECTOR_VECTOR2_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"    // core::real
#include "alloy/math/angle/radian.hpp" // math::radian
#include "alloy/math/trigonometry.hpp" // math::cos, math::sin, etc
#include "alloy/math/math.hpp"         // math::sqrt

#include <type_traits> // std::true_type, std::false_type, std::common_type
#include <cstddef>     // std::size_t, std::ptrdiff_t
#include <stdexcept>   // std::out_of_range
#include <cassert>     // assert

namespace alloy::math {

  //============================================================================
  // class : vector2<T>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This object represents a 2-component vector in linear algebra.
  ///
  /// Most operations are marked \c constexpr to allow for compile-time
  /// forward-calculations of vector sums and products.
  ///
  /// Operations on this type are able to promote results to reduce loss of
  /// data, depending on what the operands are.
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class vector2
  {
    static_assert( !std::is_reference<T>::value );
    static_assert( !std::is_void<T>::value );
    static_assert( !std::is_pointer<T>::value );

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
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs a vector2 with an undefined value
    constexpr vector2() noexcept = default;

    /// \brief Constructs a vector2 with a given \p magnitude
    ///        and \p direction
    ///
    /// \param magnitude the distance of the vector
    /// \param direction the angle of the vector
    vector2( value_type magnitude, radian direction ) noexcept;

    /// \brief Constructs a vector2 with components \p x and \p y
    ///
    /// \param x the x-component of the vector2
    /// \param y the y-component of the vector2
    constexpr vector2( value_type x, value_type y ) noexcept;

    /// \brief Copy-constructs a vector2 with the value of another
    ///        vector2
    ///
    /// \param other the other vector2 to copy
    constexpr vector2( const vector2& other ) noexcept = default;

    /// \brief Move-constructs a vector2 with the value of another
    ///        vector2
    ///
    /// \param other the other vector2 to move
    constexpr vector2( vector2&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Copy-assigns \p other to \c this
    ///
    /// \param other the other vector2 to copy
    /// \return reference to \c (*this)
    vector2& operator=( const vector2& other ) noexcept = default;

    /// \brief Move-assigns \p other to \c this
    ///
    /// \param other the other vector2 to move
    /// \return reference to \c (*this)
    vector2& operator=( vector2&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of components in the vector2
    ///
    /// \return the number of components in the vector2
    constexpr size_type size() const noexcept;

    /// \{
    /// \brief Gets the x component of this vector
    ///
    /// \return reference to the x component
    constexpr reference x() noexcept;
    constexpr const_reference x() const noexcept;
    /// \}

    /// \{
    /// \brief Gets the y component of this vector
    ///
    /// \return reference to the y component
    constexpr reference y() noexcept;
    constexpr const_reference y() const noexcept;
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
    /// \throw std::out_of_range if \p n >= 2 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr reference at( index_type n );
    constexpr const_reference at( index_type n ) const;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 2 or \p n < 0
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
    /// \param rhs the other vector2 to perform the dot-product with
    /// \return the dot product of \c this and \p rhs
    template<typename U>
    constexpr std::common_type_t<T,U>
      dot( const vector2<U>& rhs ) const noexcept;

    /// \brief Calculates the cross-product of \c this and \p rhs
    ///
    /// \param rhs the other vector2 to perform the cross-product with
    /// \return the cross product of \c this and \p rhs
    template<typename U>
    constexpr std::common_type_t<T,U>
      cross( const vector2<U>& rhs ) const noexcept;

    /// \brief Gets the magnitude of this vector2
    ///
    /// \return the magnitude of the vector2
    core::real magnitude() const noexcept;

    /// \brief Gets the midpoint between \c this and \p vec
    ///
    /// \param vec the vector2 to get the midpoint from
    /// \return the midpoint between \c this and \p vec
    template<typename U>
    constexpr vector2<std::common_type_t<T,U>>
      midpoint( const vector2<U>& vec ) const noexcept;

    /// \brief Gets the reflection vector2 of \c this reflected through the
    ///        \p normal
    ///
    /// \param normal the normal vector2
    /// \return the reflection
    template<typename U>
    constexpr vector2<std::common_type_t<T,U>>
      reflection( const vector2<U>& normal ) const noexcept;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    template<typename U>
    constexpr vector2<std::common_type_t<T,U>>
      projection( const vector2<U>& vector ) const noexcept;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    template<typename U>
    constexpr vector2<std::common_type_t<T,U>>
      rejection( const vector2<U>& vector ) const noexcept;

    /// \brief Gets the perpendicular of this vector2
    ///
    /// \return the perpendicular vector2 to \c this
    constexpr vector2 perpendicular() const noexcept;

    /// \brief Gets the normalized vector2 of \c this
    ///
    /// \param the normalized vector2 of \c this
    vector2 normalized() const noexcept;

    /// \brief Gets the inverse of \c this vector2
    ///
    /// \return the inverse of \c this vector2
    constexpr vector2 inverse() const noexcept;

    /// \brief Determines the angle between \c this and \p other
    ///
    /// \param other the other vector to determine the angle between
    /// \return the angle between \c this and \p other
    template<typename U>
    radian angle_between( const vector2<U>& other ) const noexcept;

    /// \brief Determines the angle from \p to \p other
    ///
    /// \param other the other vector to get the angle to
    /// \return the angle from \c this to \p other
    template<typename U>
    radian angle_to( const vector2<U>& other ) const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector2 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr vector2& normalize() noexcept;

    /// \brief Inverts this vector2 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr vector2& invert() noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr const vector2& operator+() const noexcept;
    constexpr vector2 operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    template<typename U>
    constexpr vector2& operator+=( const vector2<U>& rhs ) noexcept;
    template<typename U>
    constexpr vector2& operator-=( const vector2<U>& rhs ) noexcept;
    template<typename U>
    constexpr vector2& operator*=( U scalar ) noexcept;
    template<typename U>
    constexpr vector2& operator/=( U scalar ) noexcept;

    //----------------------------------------------------------------------
    // Private Members
    //----------------------------------------------------------------------
  private:

    value_type m_data[2]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : vector2<T>
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  template<typename T, typename U>
  constexpr vector2<std::common_type_t<T,U>>
    operator+( const vector2<T>& lhs, const vector2<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr vector2<std::common_type_t<T,U>>
    operator-( const vector2<T>& lhs, const vector2<U>& rhs ) noexcept;
  template<typename T, typename U,
           typename = std::enable_if<std::is_arithmetic<U>::value>>
  constexpr vector2<std::common_type_t<T,U>>
    operator*( const vector2<T>& lhs, U scalar ) noexcept;
  template<typename T, typename U,
           typename = std::enable_if<std::is_arithmetic<T>::value>>
  constexpr vector2<std::common_type_t<T,U>>
    operator*( T scalar, const vector2<U>& rhs ) noexcept;
  template<typename T, typename U,
           typename = std::enable_if<std::is_arithmetic<U>::value>>
  constexpr vector2<std::common_type_t<T,U>>
    operator/( const vector2<T>& lhs, U scalar ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  template<typename T, typename U>
  constexpr bool operator==( const vector2<T>& lhs,
                             const vector2<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr bool operator!=( const vector2<T>& lhs,
                             const vector2<U>& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two vector2 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return \c true if the two vector2 contain almost equal values
  template<typename T, typename U>
  constexpr bool almost_equal( const vector2<T>& lhs,
                               const vector2<U>& rhs ) noexcept;

  /// \brief Determines equality between two vector2 relative to \p tolerance
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return \c true if the two vector2 contain almost equal values
  template<typename T, typename U, typename Arithmetic,
           typename = std::enable_if_t<std::is_arithmetic<Arithmetic>::value>>
  constexpr bool almost_equal( const vector2<T>& lhs,
                               const vector2<U>& rhs,
                               Arithmetic tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  template<typename T, typename U>
  constexpr std::common_type_t<T,U>
    dot( const vector2<T>& lhs, const vector2<U>& rhs ) noexcept;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  template<typename T, typename U>
  constexpr vector2<std::common_type_t<T,U>>
    cross( const vector2<T>& lhs, const vector2<U>& rhs ) noexcept;

  /// \brief Calculates the magnitude of the vector2 \p vec
  ///
  /// \param vec the vector2 to calculate the magnitude from
  /// \return the magnitude
  template<typename T>
  core::real magnitude( const vector2<T>& vec ) noexcept;

  //============================================================================
  // trait : is_vector2
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref vector2
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_vector2 : std::false_type{};

  template<typename T> struct is_vector2<vector2<T>> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector2<T>::value
  template<typename T>
  inline constexpr bool is_vector2_v = is_vector2<T>::value;

  //============================================================================
  // struct : vector2_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of vector2 constants
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct vector2_constants
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------

    using vector_type = vector2<T>;

    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = vector_type {
      T{0}, T{0}
    };
    static inline constexpr auto unit_x = vector_type {
      T{1}, T{0}
    };
    static inline constexpr auto unit_y = vector_type {
      T{0}, T{1}
    };
    static inline constexpr auto neg_unit_x = -unit_x;
    static inline constexpr auto neg_unit_y = -unit_y;
  };

  //============================================================================
  // aliases
  //============================================================================

  using vector2f  = vector2<float>;
  using vector2d  = vector2<double>;
  using vector2ld = vector2<long double>;
  using vector2r  = vector2<core::real>;

  using vec2f  = vector2f;
  using vec2d  = vector2d;
  using vec2ld = vector2ld;
  using vec2r  = vector2r;

  using vector2f_constants  = vector2_constants<float>;
  using vector2d_constants  = vector2_constants<double>;
  using vector2ld_constants = vector2_constants<long double>;
  using vector2r_constants  = vector2_constants<core::real>;

} // namespace alloy::math

#include "detail/vector2.inl"

#endif /* ALLOY_MATH_VECTOR_VECTOR2_HPP */
