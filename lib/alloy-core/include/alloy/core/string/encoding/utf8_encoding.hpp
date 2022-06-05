/*****************************************************************************
 * \file utf8_encoding.hpp
 *
 * \brief This header defines an encoder for generic UTF-8 encoding
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
// * Extracted utf8 conversion logic to a trait type
// * Changed naming, structure, etc
// * Changed "decode" to not use out-parameters, and instead return a pair
// * Made use of 'char8' type instead of uint8
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
#ifndef ALLOY_CORE_STRING_ENCODING_UTF8_ENCODING_HPP
#define ALLOY_CORE_STRING_ENCODING_UTF8_ENCODING_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"      // char32, char8
#include "alloy/core/intrinsics.hpp" // ALLOY_UNLIKELY
#include "alloy/core/utilities/quantity.hpp" // uquantity

#include <utility>   // std::pair
#include <array>     // std::array
#include <algorithm> // std::copy
#include <iterator>  // std::distance

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Encoding logic for UTF-8 codepoints
  /////////////////////////////////////////////////////////////////////////////
  class utf8_encoding
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using char_type = char8;

    //-------------------------------------------------------------------------
    // Public Static Constants
    //-------------------------------------------------------------------------
  public:

    static constexpr auto decode_sentinel = char32{};
    static constexpr auto encode_sentinel = char_type{};
    static constexpr auto is_multi_unit = true;

    //-------------------------------------------------------------------------
    // Encoding
    //-------------------------------------------------------------------------
  public:

    /// \brief Decodes a stream of UTF-8 code units into a single UTF-32
    ///        codepoint
    ///
    /// \note Decoding a character means finding its unique 32-bits code
    ///       (called the codepoint) in the Unicode standard.
    ///
    /// \param begin Iterator pointing to the beginning of the input sequence
    /// \param end Iterator pointing to the end of the input sequence
    /// \param replacement Replacement character to use in case the UTF-8
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

    /// \brief Encode a single UTF-32 codepoint into a stream of UTF-8 code
    ///        units
    ///
    /// \note Encoding a character means converting a unique 32-bit code
    ///       (called the codepoint) in the target encoding, UTF-8.
    ///
    /// \param input Codepoint to encode as UTF-8
    /// \param output Iterator pointing to the beginning of the output sequence
    /// \param replacement Replacement for characters not convertible to UTF-8
    ///                    (use 0 to skip them)
    /// \return Iterator to the end of the output sequence which has been
    ///         written
    template <typename OutputIt>
    static constexpr auto encode(
      char32 input,
      OutputIt output,
      char_type replacement = encode_sentinel
    ) noexcept -> OutputIt;

    /// \brief Computes the number of codepoints in this ansi range
    ///
    /// Since ansi is single-unit encoding, this is simply the distance from
    /// begin to end
    ///
    /// \param begin the start of the range
    /// \param end the end of the range
    /// \return `end - begin`
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto length(ForwardIt begin, ForwardIt end)
      noexcept -> uquantity<char32>;

    //-------------------------------------------------------------------------
    // Private Static Constants
    //-------------------------------------------------------------------------
  private:

    // Decoding constants

    // The number of trailing bytes expected for decoding
    static inline constexpr auto s_trailing = std::array<u8,256>{{
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
    }};
    // An offset to add to the encoded value for correctness
    static inline constexpr auto s_offsets = std::array<u32,6>{{
      0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080
    }};

    // Encoding constants

    // The first byte mask, used for encoding
    static inline constexpr auto s_first_bytes = std::array<u8,7>{{
      0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
    }};
  };

} // namespace alloy::core

template <typename ForwardIt>
inline constexpr
auto alloy::core::utf8_encoding::decode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement
) noexcept -> std::pair<char32,ForwardIt>
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return {replacement, end};
  }

  // decode the character
  const auto trailing_bytes  = s_trailing[static_cast<u8>(*begin)];
  const auto required_bytes  = trailing_bytes + 1u;
  const auto available_bytes = std::distance(begin, end);

  if (available_bytes < required_bytes) {
    return {replacement, end};
  }

  // NOTE:
  // This function trades off accuracy for general conversion speed. Rather than
  // testing each bytes for having an appropriate `0b10` prefix, as is required
  // for UTF-8, this assumes the bytes are correct. This means that malformed
  // input cannot be correctly detected from these conversion functions, and
  // may have a slight disparity with the number of detected codepoints from
  // the `length` implementation which makes the same "correctness" assumption
  // and skips continuation prefixes.
  //
  // This may be a todo to fix in the future.

  auto output = char32{};
  switch (trailing_bytes) {
    // NOLINTNEXTLINE(bugprone-branch-clone): this is intentional
    case 5u: output += static_cast<u8>(*begin++); output <<= 6u; [[fallthrough]];
    case 4u: output += static_cast<u8>(*begin++); output <<= 6u; [[fallthrough]];
    case 3u: output += static_cast<u8>(*begin++); output <<= 6u; [[fallthrough]];
    case 2u: output += static_cast<u8>(*begin++); output <<= 6u; [[fallthrough]];
    case 1u: output += static_cast<u8>(*begin++); output <<= 6u; [[fallthrough]];
    case 0u: output += static_cast<u8>(*begin++);
  }
  // The additions above result in the prefix bits being added; these need to
  // be removed from the final value.
  output -= s_offsets[trailing_bytes];

  return {output, begin};
}

template <typename OutputIt>
inline constexpr
auto alloy::core::utf8_encoding::encode(
  char32 input,
  OutputIt output,
  char_type replacement
) noexcept -> OutputIt
{
  // encode the character
  if ((input > 0x0010FFFF) || ((input >= 0xD800) && (input <= 0xDBFF))) {
    // Invalid character
    if (replacement != encode_sentinel) {
      (*output++) = replacement;
    }
    return output;
  }

  // Get the number of bytes to write
  const auto bytes_to_write = [&input]{
    if (input < 0x80) {
      return 1u;
    }
    if (input < 0x800) {
      return 2u;
    }
    if (input < 0x10000) {
      return 3u;
    }
    if (input <= 0x0010FFFF) {
      return 4u;
    }
    return 1u;
  }();

  // Extract the bytes to write
  auto bytes = std::array<u8,4u>{};
  switch (bytes_to_write) {
    case 4u: bytes[3] = static_cast<u8>((input | 0x80) & 0xBF); input >>= 6u; [[fallthrough]];
    case 3u: bytes[2] = static_cast<u8>((input | 0x80) & 0xBF); input >>= 6u; [[fallthrough]];
    case 2u: bytes[1] = static_cast<u8>((input | 0x80) & 0xBF); input >>= 6u; [[fallthrough]];
    case 1u: bytes[0] = static_cast<u8> (input | s_first_bytes[bytes_to_write]);
  }

  // Add them to the output
  output = std::copy(bytes.data(), bytes.data() + bytes_to_write, output);

  return output;
}


template <typename ForwardIt>
inline constexpr
auto alloy::core::utf8_encoding::length(ForwardIt begin, ForwardIt end)
  noexcept -> uquantity<char32>
{
  auto count = uquantity<char32>{0u};
  for (auto it = begin; it != end; ++it) {
    // Only count code units that either don't have the first bit set (which is
    // a single-character codepoint), or has the first two bits set, which is
    // common for all multi-byte sequences (either 110, 1110, or 11110).
    // These all indicate the first character in the multi-unit sequence.
    if ((*it & 0b10000000) == 0u || (*it & 0b11000000) == 0b11000000) {
      ++count;
    }
  }
  return count;
}

#endif /* ALLOY_CORE_STRING_ENCODING_UTF8_ENCODING_HPP */
