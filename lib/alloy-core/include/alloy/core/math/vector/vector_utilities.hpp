/******************************************************************************
 * \file vector_utilities.hpp
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

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp" // core::real

#include <type_traits> // std::true_type, std::false_type

namespace alloy::core {

  //============================================================================
  // trait : is_vector
  //============================================================================

  /// \brief Trait to detect whether \p T is a vector type.
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_vector : std::false_type{};

  /// \brief Convenience template variable to extract out
  ///        \c is_vector<T>::value
  template<typename T>
  inline constexpr bool is_vector_v = is_vector<T>::value;

  //============================================================================
  // trait : vector_traits<T>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Traits for accessing vector members
  ///
  /// For vectors that are shorter than 4 components, any accesses outside of
  /// the range will be treated as '0'
  ///
  /// This trait is intended to be used to accept arbitrary vector types in
  /// order to access up to 4 components in a generic manner.
  ///
  /// \note To extend which types are usable by \c vector_traits, explicit
  ///       specializations of \c is_vector is necessary.
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  struct vector_traits
  {
    static_assert(is_vector<T>::value);
    vector_traits() = delete;
    ~vector_traits() = delete;

    static constexpr real x( const T& vec ) noexcept;
    static constexpr real y( const T& vec ) noexcept;
    static constexpr real z( const T& vec ) noexcept;
    static constexpr real w( const T& vec ) noexcept;
  };

} // namespace alloy::core


//==============================================================================
// trait : vector_traits<T>
//==============================================================================

namespace alloy::core::detail {

  template<typename T, typename=std::void_t<>>
  struct vector_has_x : std::false_type{};
  template<typename T>
  struct vector_has_x<T,std::void_t<decltype(std::declval<const T&>().x())>>
    : alloy::core::is_vector<T>{};

  template<typename T, typename=std::void_t<>>
  struct vector_has_y : std::false_type{};
  template<typename T>
  struct vector_has_y<T,std::void_t<decltype(std::declval<const T&>().y())>>
    : alloy::core::is_vector<T>{};

  template<typename T, typename=std::void_t<>>
  struct vector_has_z : std::false_type{};
  template<typename T>
  struct vector_has_z<T,std::void_t<decltype(std::declval<const T&>().z())>>
    : alloy::core::is_vector<T>{};

  template<typename T, typename=std::void_t<>>
  struct vector_has_w : std::false_type{};
  template<typename T>
  struct vector_has_w<T,std::void_t<decltype(std::declval<const T&>().w())>>
    : alloy::core::is_vector<T>{};

} // namespace alloy::core::detail

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::x( const T& vec )
  noexcept
{
  if constexpr (detail::vector_has_x<T>::value) {
    return vec.x();
  } else {
    return real{0};
  }
}

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::y( const T& vec )
  noexcept
{
  if constexpr (detail::vector_has_y<T>::value) {
    return vec.y();
  } else {
    return real{0};
  }
}

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::z( const T& vec )
  noexcept
{
  if constexpr (detail::vector_has_z<T>::value) {
    return vec.z();
  } else {
    return real{0};
  }
}

template<typename T>
inline constexpr alloy::core::real
  alloy::core::vector_traits<T>::w( const T& vec )
  noexcept
{
  if constexpr (detail::vector_has_w<T>::value) {
    return vec.w();
  } else {
    return real{0};
  }
}

#endif /* ALLOY_CORE_MATH_VECTOR_VECTOR_UTILITIES_HPP */