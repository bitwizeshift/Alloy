// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file string_view.hpp
 *
 * \brief This header defines a string_view data type by using the C++17
 *        string_view equivalent.
 *****************************************************************************/

#ifndef ALLOY_CORE_STRING_STRING_VIEW_HPP
#define ALLOY_CORE_STRING_STRING_VIEW_HPP

#ifdef _MSC_VER
# pragma once
#endif



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

#endif /* ALLOY_CORE_STRING_STRING_VIEW_HPP */
