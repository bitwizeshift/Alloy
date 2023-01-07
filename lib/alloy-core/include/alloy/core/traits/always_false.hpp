/*****************************************************************************
 * \file always_false.hpp
 *
 * \brief
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2023 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_TRAITS_ALWAYS_FALSE_HPP
#define ALLOY_CORE_TRAITS_ALWAYS_FALSE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <type_traits>

namespace alloy::core {

  template <typename T>
  struct always_false : std::false_type {};

  template <typename T>
  inline constexpr auto always_false_v = always_false<T>::value;

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_ALWAYS_FALSE_HPP */
