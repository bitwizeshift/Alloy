/*****************************************************************************
 * \file utf32_encoding.hpp
 *
 * \brief This header defines the encoding for UTF-32 codepoints
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
// * Extracted utf32 conversion logic to a trait type
// * Changed naming, structure, etc
// * Changed "decode" to not use out-parameters, and instead return a pair
// * Made use of 'char32' type instead of uint8
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
#ifndef ALLOY_CORE_STRING_ENCODING_UTF32_ENCODING_HPP
#define ALLOY_CORE_STRING_ENCODING_UTF32_ENCODING_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"      // char32
#include "alloy/core/intrinsics.hpp" // ALLOY_UNLIKELY, compiler::unused

#include <utility> // std::pair

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Encoding logic for UTF-32 codepoints
  /////////////////////////////////////////////////////////////////////////////
  class utf32_encoding
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using char_type = char32;

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

    /// \brief Decodes a stream of UTF-32 code units into a single UTF-32
    ///        codepoint
    ///
    /// \note Decoding a character means finding its unique 32-bits code (called
    ///       the codepoint) in the Unicode standard.
    ///       For UTF-32, the character value is the same as the codepoint.
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end Iterator pointing to the end of the input sequence
    /// \param replacement Replacement character to use in case the UTF
    ///                    sequence is invalid (unused)
    /// \return A pair containing the translated codepoint, and the updated
    ///         iterator location
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto decode(
      ForwardIt begin,
      ForwardIt end,
      char32 replacement = decode_sentinel
    ) noexcept -> std::pair<char32,ForwardIt>;

    /// \brief Encodes a single UTF-32 codepoint as a sequence of UTF-32 code
    ///        units
    ///
    /// \note Encoding a character means converting a unique 32-bits code
    ///       (called the codepoint) in the target encoding, UTF-32.
    ///       For UTF-32, the codepoint is the same as the character value.
    ///
    /// \param input Codepoint to encode as UTF-32
    /// \param output Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to UTF-32
    ///                    (use 0 to skip them)
    /// \return Iterator to the end of the output sequence which has been written
    template <typename OutputIt>
    static constexpr auto encode(
      char32 input,
      OutputIt output,
      char_type replacement = encode_sentinel
    ) noexcept -> OutputIt;

    /// \brief Advance to the next UTF-32 character
    ///
    /// \note This function is trivial for UTF-32, which can store every
    ///       character in a single storage element.
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
auto alloy::core::utf32_encoding::decode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement
) noexcept -> std::pair<char32,ForwardIt>
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return {replacement, end};
  }

  auto it = begin;
  const auto value = *it;
  ++it;
  return {value, it};
}


template <typename OutputIt>
inline constexpr
auto alloy::core::utf32_encoding::encode(
  char32 input,
  OutputIt output,
  char_type replacement
) noexcept -> OutputIt
{
  compiler::unused(replacement);

  (*output++) = input;
  return output;
}


template <typename ForwardIt>
inline constexpr
auto alloy::core::utf32_encoding::next(
  ForwardIt begin,
  ForwardIt end
) noexcept -> ForwardIt
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return end;
  }

  return ++begin;
}

#endif /* ALLOY_CORE_STRING_ENCODING_UTF32_ENCODING_HPP */
