// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file types.hpp
 *
 * \brief This header defines the base types used in Alloy
 *****************************************************************************/

#ifndef ALLOY_CORE_TYPES_HPP
#define ALLOY_CORE_TYPES_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <cstdint>     // std::uint8_t, etc

namespace alloy::core {

  //----------------------------------------------------------------------------
  // Fixed Sizes
  //----------------------------------------------------------------------------

  using u8  = std::uint8_t;
  using s8  = std::int8_t;
  using u16 = std::uint16_t;
  using s16 = std::int16_t;
  using u32 = std::uint32_t;
  using s32 = std::int32_t;
  using u64 = std::uint64_t;
  using s64 = std::int64_t;

  using f32 = float;
  using f64 = double;

  // C++20 introduces 'char8_t', which is a unique char type used for utf-8
  // encoding.
  using char8  = decltype(u8' ');
  using char16 = char16_t;
  using char32 = char32_t;
  using wchar  = wchar_t;

} // namespace alloy::core

#endif /* ALLOY_CORE_TYPES_HPP */
