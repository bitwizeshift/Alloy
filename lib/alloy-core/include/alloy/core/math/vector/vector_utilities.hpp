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
#ifndef ALLOY_CORE_MATH_VECTOR_VECTOR_UTILITIES_HPP
#define ALLOY_CORE_MATH_VECTOR_VECTOR_UTILITIES_HPP

#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"

#include <type_traits> // std::true_type, std::false_type

namespace alloy::core {

  //----------------------------------------------------------------------------
  // Casting
  //----------------------------------------------------------------------------

  inline namespace casts {

    /// \brief Casts from one vector type to another
    ///
    /// \param from the vector to cast from
    /// \return the vector to cast to
    template<typename To, typename From>
    constexpr To vector_cast( const From& from ) noexcept;

  } // inline namespace casts

  //============================================================================
  // struct : vector2_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of vector2 constants
  //////////////////////////////////////////////////////////////////////////////
  struct vector2_constants
  {
    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = vector2{
      real{0}, real{0}
    };
    static inline constexpr auto unit_x = vector2{
      real{1}, real{0}
    };
    static inline constexpr auto unit_y = vector2{
      real{0}, real{1}
    };
    static inline constexpr auto neg_unit_x = -unit_x;
    static inline constexpr auto neg_unit_y = -unit_y;
  };

  //============================================================================
  // struct : vector3_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of vector3 constants
  //////////////////////////////////////////////////////////////////////////////

  struct vector3_constants
  {
    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = vector3{
      real{0},  real{0},  real{0}
    };
    static inline constexpr auto unit_x = vector3{
      real{1}, real{0}, real{0}
    };
    static inline constexpr auto unit_y = vector3{
      real{0}, real{1}, real{0}
    };
    static inline constexpr auto unit_z = vector3{
      real{0}, real{0}, real{1}
    };
    static inline constexpr auto neg_unit_x = -unit_x;
    static inline constexpr auto neg_unit_y = -unit_y;
    static inline constexpr auto neg_unit_z = -unit_z;
  };

  //============================================================================
  // struct : vector4_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of vector4 constants
  //////////////////////////////////////////////////////////////////////////////
  struct vector4_constants
  {
    //--------------------------------------------------------------------------
    // Public Constants
    //--------------------------------------------------------------------------

    static inline constexpr auto zero = vector4{
      real{0}, real{0}, real{0}, real{0}
    };
    static inline constexpr auto unit_x = vector4{
      real{1}, real{0}, real{0}, real{0}
    };
    static inline constexpr auto unit_y = vector4{
      real{0}, real{1}, real{0}, real{0}
    };
    static inline constexpr auto unit_z = vector4{
      real{0}, real{0}, real{1}, real{0}
    };
    static inline constexpr auto unit_w = vector4{
      real{0}, real{0}, real{0}, real{1}
    };
    static inline constexpr auto neg_unit_x = -unit_x;
    static inline constexpr auto neg_unit_y = -unit_y;
    static inline constexpr auto neg_unit_z = -unit_z;
    static inline constexpr auto neg_unit_w = -unit_w;
  };

  //============================================================================
  // aliases
  //============================================================================

  using vec2_constants = vector2_constants;
  using vec3_constants = vector3_constants;
  using vec4_constants = vector4_constants;

  //============================================================================
  // trait : is_vector
  //============================================================================

  /// \brief Trait to detect whether \p T is a vector type.
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_vector : std::false_type{};

  template<typename T> struct is_vector<vector2> : std::true_type{};
  template<typename T> struct is_vector<vector3> : std::true_type{};
  template<typename T> struct is_vector<vector4> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector<T>::value
  template<typename T>
  inline constexpr bool is_vector_v = is_vector<T>::value;

  //============================================================================
  // trait : vector_traits<T>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Traits for accessing vector types
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct vector_traits
  {
    vector_traits() = delete;
    ~vector_traits() = delete;

    static constexpr real x( const T& x ) noexcept;
    static constexpr real y( const T& y ) noexcept;
    static constexpr real z( const T& z ) noexcept;
    static constexpr real w( const T& w ) noexcept;
  };

} // namespace alloy::core

#include "detail/vector_utilities.inl"

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR_UTILITIES_HPP */