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
#ifndef ALLOY_MATH_VECTOR_VECTOR3_HPP
#define ALLOY_MATH_VECTOR_VECTOR3_HPP

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

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This object represents a 3-component vector in linear algebra.
  ///
  /// Most operations are marked \c constexpr to allow for compile-time
  /// forward-calculations of vector sums and products.
  ///
  /// Operations on this type are able to promote results to reduce loss of
  /// data, depending on what the operands are.
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class vector3
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

    /// \brief Default constructs a vector3 with an undefined value
    constexpr vector3() = default;

    /// \brief Constructs a vector3 with components \p x and \p y
    ///
    /// \param x the x-component of the vector3
    /// \param y the y-component of the vector3
    /// \param z the z-component of the vector3
    constexpr vector3( value_type x, value_type y, value_type z ) noexcept;

    /// \brief Copy-constructs a vector3 with the value of another
    ///        vector3
    ///
    /// \param other the other vector3 to copy
    constexpr vector3( const vector3& other ) noexcept = default;

    /// \brief Move-constructs a vector3 with the value of another
    ///        vector3
    ///
    /// \param other the other vector3 to move
    constexpr vector3( vector3&& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Copy-assigns \p other to \c this
    ///
    /// \param other the other vector3 to copy
    /// \return reference to \c (*this)
    vector3& operator=( const vector3& other ) = default;

    /// \brief Move-assigns \p other to \c this
    ///
    /// \param other the other vector3 to move
    /// \return reference to \c (*this)
    vector3& operator=( vector3&& other ) = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the number of components in the vector3
    ///
    /// \return the number of components in the vector3
    constexpr size_type size() const noexcept;
    /// \}

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
    /// \brief Gets the z component of this vector
    ///
    /// \return reference to the z component
    constexpr reference z() noexcept;
    constexpr const_reference z() const noexcept;
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
    /// \throw std::out_of_range if \p n >= 3 or \p n < 0
    ///
    /// \return reference to the \p n entry
    constexpr reference at( index_type n );
    constexpr const_reference at( index_type n ) const;
    /// \}

    /// \{
    /// \brief Gets the entry at the \p n position
    ///
    /// \note Undefined behaviour if \p n >= 3 or \p n < 0
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
    /// \param rhs the other vector3 to perform the dot-product with
    /// \return the dot product of \c this and \p rhs
    template<typename U>
    constexpr std::common_type_t<T,U>
      dot( const vector3<U>& rhs ) const noexcept;

    /// \brief Calculates the cross-product of \c this and \p rhs
    ///
    /// \param rhs the other vector3 to perform the cross-product with
    /// \return the cross product of \c this and \p rhs
    template<typename U>
    constexpr vector3<std::common_type_t<T,U>>
      cross( const vector3<U>& rhs ) const noexcept;

    /// \brief Gets the magnitude of this vector3
    ///
    /// \return the magnitude of the vector3
    core::real magnitude() const noexcept;

    /// \brief Gets the midpoint between \c this and \p vec
    ///
    /// \param vec the vector3 to get the midpoint from
    /// \return the midpoint between \c this and \p vec
    template<typename U>
    constexpr vector3<std::common_type_t<T,U>>
      midpoint( const vector3<U>& vec ) const noexcept;

    /// \brief Gets the reflection vector3 of \c this reflected through the
    ///        \p normal
    ///
    /// \param normal the normal vector3
    /// \return the reflection
    template<typename U>
    constexpr vector3<std::common_type_t<T,U>>
      reflection( const vector3<U>& normal ) const noexcept;

    /// \brief Projects the components of this vector onto \p vector
    ///
    /// \param vector the vector to project onto
    /// \return the projection
    template<typename U>
    constexpr vector3<std::common_type_t<T,U>>
      projection( const vector3<U>& vector ) const noexcept;

    /// \brief Projects the components of this vector off of \p vector
    ///
    /// \param vector the vector to project off of
    /// \return the rejection
    template<typename U>
    constexpr vector3<std::common_type_t<T,U>>
      rejection( const vector3<U>& vector ) const noexcept;

    /// \brief Gets the perpendicular of this vector3
    ///
    /// \return the perpendicular vector3 to \c this
    constexpr vector3 perpendicular() const noexcept;

    /// \brief Gets the normalized vector3 of \c this
    ///
    /// \param the normalized vector3 of \c this
    vector3 normalized() const noexcept;

    /// \brief Gets the inverse of \c this vector3
    ///
    /// \return the inverse of \c this vector3
    constexpr vector3 inverse() const noexcept;

    /// \brief Determines the angle between \c this and \p other
    ///
    /// \param other the other vector to determine the angle between
    /// \return the angle between \c this and \p other
    template<typename U>
    radian angle_between( const vector3<U>& other ) const noexcept;

    /// \brief Determines the angle from \p to \p other
    ///
    /// \param other the other vector to get the angle to
    /// \return the angle from \c this to \p other
    template<typename U>
    radian angle_to( const vector3<U>& other ) const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Normalizes this vector3 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr vector3& normalize() noexcept;

    /// \brief Inverts this vector3 and returns a reference to \c (*this)
    ///
    /// \return the reference to \c (*this)
    constexpr vector3& invert() noexcept;

    //--------------------------------------------------------------------------
    // Unary Operators
    //--------------------------------------------------------------------------
  public:

    constexpr const vector3& operator+() const noexcept;
    constexpr vector3 operator-() const noexcept;

    //--------------------------------------------------------------------------
    // Compound Operators
    //--------------------------------------------------------------------------
  public:

    template<typename U>
    constexpr vector3& operator+=( const vector3<U>& rhs ) noexcept;
    template<typename U>
    constexpr vector3& operator-=( const vector3<U>& rhs ) noexcept;
    template<typename U>
    constexpr vector3& operator*=( U scalar ) noexcept;
    template<typename U>
    constexpr vector3& operator/=( U scalar ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    value_type m_data[3]; ///< The storage data
  };

  //============================================================================
  // non-member functions : class : vector3<T>
  //============================================================================

  //----------------------------------------------------------------------------
  // Arithmetic Operators
  //----------------------------------------------------------------------------

  template<typename T, typename U>
  constexpr vector3<std::common_type_t<T,U>>
    operator+( const vector3<T>& lhs, const vector3<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr vector3<std::common_type_t<T,U>>
    operator-( const vector3<T>& lhs, const vector3<U>& rhs ) noexcept;
  template<typename T, typename U,
           typename = std::enable_if<std::is_arithmetic<U>::value>>
  constexpr vector3<std::common_type_t<T,U>>
    operator*( const vector3<T>& lhs, U scalar ) noexcept;
  template<typename T, typename U,
           typename = std::enable_if<std::is_arithmetic<T>::value>>
  constexpr vector3<std::common_type_t<T,U>>
    operator*( T scalar, const vector3<U>& rhs ) noexcept;
  template<typename T, typename U,
           typename = std::enable_if<std::is_arithmetic<U>::value>>
  constexpr vector3<std::common_type_t<T,U>>
    operator/( const vector3<T>& lhs, U scalar ) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  template<typename T, typename U>
  constexpr bool operator==( const vector3<T>& lhs,
                             const vector3<U>& rhs ) noexcept;
  template<typename T, typename U>
  constexpr bool operator!=( const vector3<T>& lhs,
                             const vector3<U>& rhs ) noexcept;

  //----------------------------------------------------------------------------

  /// \brief Determines equality between two vector3 relative to
  ///        \ref default_tolerance
  ///
  /// \param lhs the left vector3
  /// \param rhs the right vector3
  /// \return \c true if the two vector3 contain almost equal values
  template<typename T, typename U>
  constexpr bool almost_equal( const vector3<T>& lhs,
                               const vector3<U>& rhs ) noexcept;

  /// \brief Determines equality between two vector3 relative to \p tolerance
  ///
  /// \param lhs the left vector3
  /// \param rhs the right vector3
  /// \return \c true if the two vector3 contain almost equal values
  template<typename T, typename U, typename Arithmetic,
           typename = std::enable_if_t<std::is_arithmetic<Arithmetic>::value>>
  constexpr bool almost_equal( const vector3<T>& lhs,
                               const vector3<U>& rhs,
                               Arithmetic tolerance ) noexcept;

  //----------------------------------------------------------------------------
  // Quantifiers
  //----------------------------------------------------------------------------

  /// \brief Performs the dot product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector3
  /// \param rhs the right vector3
  /// \return
  template<typename T, typename U>
  constexpr std::common_type_t<T,U>
    dot( const vector3<T>& lhs, const vector3<U>& rhs ) noexcept;

  /// \brief Performs the cross product between \p lhs and \p rhs
  ///
  /// \param lhs the left vector2
  /// \param rhs the right vector2
  /// \return the result of the dot product
  template<typename T, typename U>
  constexpr vector3<std::common_type_t<T,U>>
    cross( const vector3<T>& lhs, const vector3<U>& rhs ) noexcept;

  /// \brief Calculates the magnitude of the vector3 \p vec
  ///
  /// \param vec the vector3 to calculate the magnitude from
  /// \return the magnitude
  template<typename T>
  core::real magnitude( const vector3<T>& vec ) noexcept;

  //============================================================================
  // trait : is_vector3
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref vector3
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_vector3 : std::false_type{};

  template<typename T> struct is_vector3<vector3<T>> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector4<T>::value
  template<typename T>
  constexpr bool is_vector3_v = is_vector3<T>::value;

  //============================================================================
  // struct : vector3_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of vector3 constants
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct vector3_constants
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------

    using vector_type = vector3<T>;

    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = vector_type {
      T{0}, T{0}, T{0}
    };
    static inline constexpr auto unit_x = vector_type {
      T{1}, T{0}, T{0}
    };
    static inline constexpr auto unit_y = vector_type {
      T{0}, T{1}, T{0}
    };
    static inline constexpr auto unit_z = vector_type {
      T{0}, T{0}, T{1}
    };
    static inline constexpr auto neg_unit_x = -unit_x;
    static inline constexpr auto neg_unit_y = -unit_y;
    static inline constexpr auto neg_unit_z = -unit_z;
  };

  //============================================================================
  // aliases
  //============================================================================

  using vector3f  = vector3<float>;
  using vector3d  = vector3<double>;
  using vector3ld = vector3<long double>;
  using vector3r  = vector3<core::real>;

  using vec3f  = vector3f;
  using vec3d  = vector3d;
  using vec3ld = vector3ld;
  using vec3r  = vector3r;

  using vector3f_constants  = vector3_constants<float>;
  using vector3d_constants  = vector3_constants<double>;
  using vector3ld_constants = vector3_constants<long double>;
  using vector3r_constants  = vector3_constants<core::real>;

} // namespace alloy::math

#include "detail/vector3.inl"

#endif /* ALLOY_MATH_VECTOR_VECTOR3_HPP */
