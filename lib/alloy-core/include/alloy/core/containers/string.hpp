/*****************************************************************************
 * \file string.hpp
 *
 * \todo
 *****************************************************************************/

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
#ifndef ALLOY_CORE_CONTAINERS_STRING_HPP
#define ALLOY_CORE_CONTAINERS_STRING_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <string>
#include <memory> // std::allocator

namespace alloy::core {

  template <typename CharT,
            typename Traits = std::char_traits<CharT>,
            typename Allocator = stl_allocator_adapter<CharT>>
  using basic_string = explicitly_copyable<std::basic_string<CharT,Traits,Allocator>>;

  using string = basic_string<char>;
  using wstring = basic_string<wchar_t>;
  using u16string = basic_string<char16_t>;
  using u32string = basic_string<char32_t>;

  // The 'u8' literal prefix in C++17 produces a char literal, but in C++20
  // will produce a char8_t literal. This is an easy way to toggle between
  // char and char8_t without requiring #ifdef and feature-test macros
  using u8string = basic_string<decltype(u8' ')>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_STRING_HPP */
