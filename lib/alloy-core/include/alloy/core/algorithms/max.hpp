/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_ALGORITHM_MAX_HPP
#define ALLOY_CORE_ALGORITHM_MAX_HPP

#if defined(_MSC_VER)
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

#endif /* ALLOY_CORE_ALGORITHM_MAX_HPP */

