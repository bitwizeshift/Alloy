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
#ifndef ALLOY_MATH_VECTOR_VECTOR_UTILITIES_HPP
#define ALLOY_MATH_VECTOR_VECTOR_UTILITIES_HPP

#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"

#include <type_traits> // std::true_type, std::false_type

namespace alloy::math {

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
  // trait : is_vector
  //============================================================================

  /// \brief Trait to detect whether \p T is a vector type.
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_vector : std::false_type{};

  template<typename T> struct is_vector<vector2<T>> : std::true_type{};
  template<typename T> struct is_vector<vector3<T>> : std::true_type{};
  template<typename T> struct is_vector<vector4<T>> : std::true_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector<T>::value
  template<typename T>
  inline constexpr bool is_vector_v = is_vector<T>::value;

} // namespace alloy::math

#include "detail/vector_utilities.inl"

#endif /* ALLOY_MATH_VECTOR_VECTOR_UTILITIES_HPP */