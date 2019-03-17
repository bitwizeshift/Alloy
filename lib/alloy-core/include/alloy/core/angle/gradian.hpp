/******************************************************************************
 * \file gradian.hpp
 *
 * \brief This header contains the definition of a gradian angle class
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
#ifndef ALLOY_CORE_ANGLE_GRADIAN_HPP
#define ALLOY_CORE_ANGLE_GRADIAN_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/precision.hpp"
#include "alloy/core/angle/basic_angle.hpp"

#include <type_traits> // std::enable_if
#include <utility>     // std::swap

namespace alloy::core {

  //============================================================================
  // struct : gradian_unit
  //============================================================================

  struct gradian_unit
  {
    static constexpr core::real revolution() noexcept;
  };

  //============================================================================
  // class : gradian
  //============================================================================

  using gradian = basic_angle<gradian_unit>;

  //============================================================================
  // trait : is_gradian
  //============================================================================

  /// \brief Trait to detect whether \p T is a \ref gradian
  ///
  /// The result is aliased as \c ::value
  template<typename T> struct is_gradian : std::false_type{};

  template<> struct is_gradian<gradian> : std::true_type{};

  /// \brief Helper variable template to retrieve the result of \ref is_gradian
  template<typename T>
  constexpr bool is_gradian_v = is_gradian<T>::value;

  //============================================================================
  // struct : gradian_constants
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of gradian constants
  //////////////////////////////////////////////////////////////////////////////
  using gradian_constants = basic_angle_constants<gradian_unit>;

  //============================================================================
  // Literals
  //============================================================================

  inline namespace literals {
    constexpr gradian operator""_grad( long double x ) noexcept;
  } // inline namespace literals
} // namespace alloy::core

#include "detail/gradian.inl"

#endif /* ALLOY_CORE_ANGLE_GRADIAN_HPP */