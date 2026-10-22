// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file string.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_CORE_STRING_STRING_HPP
#define ALLOY_CORE_STRING_STRING_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <string>

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

#endif /* ALLOY_CORE_STRING_STRING_HPP */
