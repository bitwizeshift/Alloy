// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_CORE_ALGORITHMS_MAX_HPP
#define ALLOY_CORE_ALGORITHMS_MAX_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/algorithms/compare_reduce.hpp"

#include "alloy/core/utilities/compare.hpp"

namespace alloy::core {

  /// \brief A functor object for taking the max size of a sequence of elements.
  ///
  /// Unlike the C++ standard, `max` is both variadic, and also will return
  /// the common-reference if all arguments are references, otherwise it only
  /// returns by value. This prevents the dangling-reference issue that the C++
  /// standard's `std::max` suffers from -- e.g. `const auto& x = max(y, 42);` forms
  /// a dangling reference to `42` because const-lifetime-propagation cannot
  /// work through the function parameters. With this, `const auto& x = max(y, 42)`
  /// is well-formed because `x` will be a const-lifetime-extenended return type
  /// that was returned from `max` *by value* rather than by-reference.
  constexpr auto max = compare_reducer<compare_greater>{};

} // namespace alloy::core

#endif /* ALLOY_CORE_ALGORITHMS_MAX_HPP */

