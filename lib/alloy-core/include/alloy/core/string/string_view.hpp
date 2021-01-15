/*****************************************************************************
 * \file string_view.hpp
 *
 * \brief This header defines a string_view data type by using the C++17
 *        string_view equivalent.
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
#ifndef ALLOY_CORE_STRINGS_STRING_VIEW_HPP
#define ALLOY_CORE_STRINGS_STRING_VIEW_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/string/string.hpp"

#include "alloy/core/macros.hpp" // ALLOY_UNIQUE_NAME

#include <string_view> // std::basic_string_view
#include <string>      // std::char_traits, std::basic_string

namespace alloy::core {

  //===========================================================================
  // class : basic_string_view<CharT,Traits>
  //===========================================================================

  template <typename CharT, typename Traits = std::char_traits<CharT>>
  using basic_string_view = std::basic_string_view<CharT,Traits>;

  //===========================================================================
  // aliases : class : basic_string_view<CharT,Traits>
  //===========================================================================

  using string_view    = basic_string_view<char>;
  using wstring_view   = basic_string_view<wchar_t>;
  using u16string_view = basic_string_view<char16_t>;
  using u32string_view = basic_string_view<char32_t>;
  using u8string_view  = basic_string_view<decltype(u8' ')>;

} // namespace alloy::core

#endif /* ALLOY_CORE_STRINGS_STRING_VIEW_HPP */
