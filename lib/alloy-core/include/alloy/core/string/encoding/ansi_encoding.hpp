/*****************************************************************************
 * \file ansi_encoding.hpp
 *
 * \brief This header defines a generic encoder for ANSI code points
 *****************************************************************************/

// The encoding logic originates from SFML, whose license is below:

//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2021 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// ForwardIt no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//  you must not claim that you wrote the original software.
//  If you use this software in a product, an acknowledgment
//  in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//  and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//

//-----------------------------------------------------------------------------

// Alterations to this file have been made which are mostly cosmetic, but are
// listed here as required by the above license:
//
// * Extracted ansi conversion logic to a trait type
// * Changed naming, structure, etc
// * Changed "decode" to not use out-parameters, and instead return a pair
//
// The above changes are all covered under Alloy's general MIT license, listed
// below:

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
#ifndef ALLOY_CORE_STRING_ENCODING_ANSI_ENCODING_HPP
#define ALLOY_CORE_STRING_ENCODING_ANSI_ENCODING_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"      // char32
#include "alloy/core/intrinsics.hpp" // ALLOY_UNLIKELY

#include <locale>  // std::locale
#include <utility> // std::pair

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Encoding logic for ANSI
  ///
  /// This is a minimalistic wrapper around ANSI encoding with locale-based
  /// codepages
  /////////////////////////////////////////////////////////////////////////////
  class ansi_encoding
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using char_type = char;

    //-------------------------------------------------------------------------
    // Public Static Constants
    //-------------------------------------------------------------------------
  public:

    static constexpr auto decode_sentinel = char32{};
    static constexpr auto encode_sentinel = char_type{};
    static constexpr auto is_multi_unit = false;

    //-------------------------------------------------------------------------
    // Encoding
    //-------------------------------------------------------------------------
  public:

    /// \brief Decodes a stream of ansi characters into a single UTF-32
    ///        codepoint
    ///
    /// This function trivially just widens the `char` to a `char32`, since
    /// valid ANSI characters are also UTF-8 (e.g. anything below 128)
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end Iterator pointing to the end of the input sequence
    /// \param replacement Replacement character to use in case the char
    ///                    sequence is invalid
    /// \return A pair containing the read UTF-32 codepoint, along with the
    ///         iterator
    template <typename ForwardIt>
    [[nodiscard]]
    static auto decode(
      ForwardIt begin,
      ForwardIt end,
      char32 replacement = decode_sentinel,
      const std::locale& locale = std::locale{}
    ) noexcept -> std::pair<char32,ForwardIt>;

    /// \brief Encode a single UTF-32 codepoint into a stream of ansi characters
    ///
    /// A character is only valid if its numeric representation is valid for
    /// ANSI
    ///
    /// \param input Codepoint to encode as ansi
    /// \param output Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to UTF-8
    ///                    (use 0 to skip them)
    /// \return Iterator to the end of the output sequence which has been
    ///         written
    template <typename OutputIt>
    static auto encode(
      char32 input,
      OutputIt output,
      char_type replacement = encode_sentinel,
      const std::locale& locale = std::locale{}
    ) noexcept -> OutputIt;

    /// \brief Advance to the next char character
    ///
    /// This function is trivial for ANSI; it simply increments the iterator
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    /// \return Iterator pointing to one past the last read element of the input
    ///         sequence
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto next(ForwardIt begin, ForwardIt end)
      noexcept -> ForwardIt;
  };

} // namespace alloy::core

//=============================================================================
// Inline implementation
//=============================================================================

//-----------------------------------------------------------------------------
// Encoding
//-----------------------------------------------------------------------------

template <typename ForwardIt>
inline
auto alloy::core::ansi_encoding::decode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement,
  const std::locale& locale
) noexcept -> std::pair<char32,ForwardIt>
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return {replacement, begin};
  }
  auto it = begin;

  // Get the facet of the locale which deals with character conversion
  const auto& facet = std::use_facet<std::ctype<wchar>>(locale);

  const auto result = static_cast<char32>(facet.widen(*it));
  ++it;

  return {result, it};
}


template <typename OutputIt>
auto alloy::core::ansi_encoding::encode(
  char32 input,
  OutputIt output,
  char_type replacement,
  const std::locale& locale
) noexcept -> OutputIt
{
  if constexpr (sizeof(wchar) == sizeof(char16)) {
    // Encoding to ansi requires first truncating to a `wchar_t`. If it's
    // 16-bit, as it is on windows, make sure that the truncation fits into
    // range -- otherwise it may narrow incorrectly.
    if (ALLOY_UNLIKELY(input > char32{0xFFFF})) {
      if (replacement != encode_sentinel) {
        (*output++) = replacement;
      }
      return output;
    }
  }
  // Get the facet of the locale which deals with character conversion
  const auto& facet = std::use_facet<std::ctype<wchar>>(locale);

  // Use the facet to convert each character of the input string
  (*output++) = facet.narrow(static_cast<wchar>(input), replacement);

  return output;
}


template <typename ForwardIt>
inline constexpr
auto alloy::core::ansi_encoding::next(ForwardIt begin, ForwardIt end)
  noexcept -> ForwardIt
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return end;
  }

  return ++begin;
}

#endif /* ALLOY_CORE_STRING_ENCODING_ANSI_ENCODING_HPP */
