// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file ctor_function.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_UTILITIES_CTOR_FUNCTION_HPP
#define ALLOY_CORE_UTILITIES_CTOR_FUNCTION_HPP

#include "alloy/core/intrinsics.hpp"

#include <concepts>
#include <type_traits> // std::constructible_from
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
    template <typename...Args>
      requires std::constructible_from<T, Args...>
    constexpr auto operator()(Args&&...args) const
      noexcept(std::is_nothrow_constructible_v<T,Args...>) -> T;
  };

  template <typename T>
  inline constexpr auto ctor_function = ctor_function_type<T>{};

} // namespace alloy::core

template <typename T>
template <typename... Args>
  requires std::constructible_from<T, Args...>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::ctor_function_type<T>::operator()(Args&&...args)
  const noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T
{
  return T(std::forward<Args>(args)...);
}

#endif /* ALLOY_CORE_UTILITIES_CTOR_FUNCTION_HPP */
