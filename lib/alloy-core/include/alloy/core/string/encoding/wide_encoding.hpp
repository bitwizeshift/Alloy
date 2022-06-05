/*****************************************************************************
 * \file wide_encoding.hpp
 *
 * \brief This header defines a generic encoder for wide (wchar_t) encoding
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
// * Extracted wide conversion logic to a trait type
// * Changed naming, structure, etc
// * Changed "decode" to not use out-parameters, and instead return a pair
// * Made use of 'char32' type instead of u32
// * Made constexpr where possible
// * Performed more iterator bounds-checking for safety
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
#ifndef ALLOY_CORE_STRING_ENCODING_WIDE_ENCODING_HPP
#define ALLOY_CORE_STRING_ENCODING_WIDE_ENCODING_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"      // wchar, char32
#include "alloy/core/intrinsics.hpp" // ALLOY_UNLIKELY

#include <utility> // std::pair

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Encoding logic for wchar_t objects
  ///
  /////////////////////////////////////////////////////////////////////////////
  class wide_encoding
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using char_type = wchar;

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

    /// \brief Decodes a stream of wide code units into a single UTF-32
    ///        codepoint
    ///
    /// This function trivially casts the wchar_t to be char32_t, since wchar_t
    /// is either UTF-16LE encoding, or UCS2 -- both of which are valid subsets
    /// of UTF-32.
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end Iterator pointing to the end of the input sequence
    /// \param replacement Replacement character to use in case the char
    ///                    sequence is invalid
    /// \return A pair containing the translated codepoint, and the updated
    ///         iterator location
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto decode(
      ForwardIt begin,
      ForwardIt end,
      char32 replacement = decode_sentinel
    ) noexcept -> std::pair<char32,ForwardIt>;

    /// \brief Encode a single UTF-32 codepoint into a stream of wide characters
    ///
    /// A character is only valid if its numeric representation is valid for
    /// wide encoding
    ///
    /// \param input Codepoint to encode as ansi
    /// \param output Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to UTF-32
    ///                    (use 0 to skip them)
    /// \return Iterator to the end of the output sequence which has been
    ///         written
    template <typename OutputIt>
    static constexpr auto encode(
      char32 input,
      OutputIt output,
      char_type replacement = encode_sentinel
    ) noexcept -> OutputIt;

    /// \brief Advance to the next char character
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end   Iterator pointing to the end of the input sequence
    ///
    /// \return Iterator pointing to one past the last read element of the input
    ///         sequence
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto next(ForwardIt begin, ForwardIt end)
      noexcept -> ForwardIt;
  };

} // namespace alloy::core

//=============================================================================
// Inline definitions
//=============================================================================

//-----------------------------------------------------------------------------
// Encoding
//-----------------------------------------------------------------------------

template <typename ForwardIt>
inline constexpr
auto alloy::core::wide_encoding::decode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement
) noexcept -> std::pair<char32,ForwardIt>
{
   if (ALLOY_UNLIKELY(begin == end)) {
    return {replacement, end};
  }

  // The encoding of wide characters is not well defined and is left to the
  // system; however we can safely assume that it is UCS-2 on Windows and
  // UCS-4 on Unix systems.
  // In both cases, a simple copy is enough (UCS-2 is a subset of UCS-4,
  // and UCS-4 *is* UTF-32).
  const auto value = static_cast<char32>(*begin);
  ++begin;

  return {value, begin};
}


template <typename OutputIt>
inline constexpr
auto alloy::core::wide_encoding::encode(
  char32 input,
  OutputIt output,
  char_type replacement
) noexcept -> OutputIt
{
  // The encoding of wide characters is not well defined and is left to the
  // system; however we can safely assume that it is UCS-2 on Windows and
  // UCS-4 on Unix systems.
  //
  // For UCS-2 we need to check if the source characters fits in (UCS-2 is a
  // subset of UCS-4).
  //
  // For UCS-4 we can do a direct copy (UCS-4 *is* UTF-32).

  if constexpr (sizeof(wchar) == sizeof(char32)) {
    (*output++) = static_cast<wchar>(input);
  } else if constexpr (sizeof(wchar) == sizeof(char16)) {
    if ((input <= 0xFFFF) && ((input < 0xD800) || (input > 0xDFFF))) {
      (*output++) = static_cast<wchar>(input);
    } else if (replacement != encode_sentinel) {
      (*output++) = replacement;
    }
  }
  return output;
}


template <typename ForwardIt>
inline constexpr
auto alloy::core::wide_encoding::next(ForwardIt begin, ForwardIt end)
  noexcept -> ForwardIt
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return end;
  }
  // Wide characters are in either UCS-2 for Windows, which is non-multibyte,
  // or UCS-4, which IS UTF-32.
  // Technically, Windows has recently changed this to declare "UTF-16LE" as
  // the encoding

  return ++begin;
}


#endif /* ALLOY_CORE_STRING_ENCODING_WIDE_ENCODING_HPP */
