////////////////////////////////////////////////////////////////////////////////
/// \file ctor_function.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_CORE_UTILITIES_CTOR_FUNCTION_HPP
#define ALLOY_CORE_UTILITIES_CTOR_FUNCTION_HPP

#include "alloy/core/intrinsics.hpp"

#include <type_traits> // std::enable_if
#include <utility>     // std::forward

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A utility type for representing a constructor invocation as a
  ///        callable object.
  ///
  /// C++17 introduced guaranteed-copy-elision, which ensures that you can
  /// return a newly-constructed type even if it does not offer a copy or move
  /// constructor. This effectively makes the distinction between what a
  /// constructor does, and what a function does, transparent -- since the two
  /// will be viable in the same conditions, and are required to generate the
  /// same assembly.
  ///
  /// This utility makes use of this fact, with the intended purpose of
  /// being used in composable functionalities -- such as being used as the
  /// input to monadic functions like `foo.map(ctor_function<bar>{})` to convert
  /// a `result<foo,E>` into a `result<bar,E>`.
  ///
  /// \tparam T the type to construct
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  struct ctor_function_type
  {
    template <typename...Args, typename=std::enable_if_t<std::is_constructible_v<T,Args...>>>
    constexpr auto operator()(Args&&...args)
      noexcept(std::is_nothrow_constructible_v<T,Args...>) -> T;
  };

  template <typename T>
  inline constexpr auto ctor_function = ctor_function_type<T>{};

} // namespace alloy::core

template <typename T>
template <typename... Args, typename>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::ctor_function_type<T>::operator()(Args&&...args)
  noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T
{
  return T(std::forward<Args>(args)...);
}

#endif /* ALLOY_CORE_UTILITIES_CTOR_FUNCTION_HPP */
