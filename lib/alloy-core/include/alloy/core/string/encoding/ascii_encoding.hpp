/*****************************************************************************
 * \file ascii_encoding.hpp
 *
 * \brief This header defines the encoding type for ASCII
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022-2023 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_STRING_ENCODING_ASCII_ENCODING_HPP
#define ALLOY_CORE_STRING_ENCODING_ASCII_ENCODING_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"      // char32
#include "alloy/core/intrinsics.hpp" // ALLOY_UNLIKELY

#include <utility> // std::pair

namespace alloy::core {


  /////////////////////////////////////////////////////////////////////////////
  /// \brief Encoding logic for ASCII
  ///
  /// This encoding is related to ANSI in that it shares the first 127 codepoints
  /// (e.g. the low 7 bits of the char) with it. However, unlike ANSI, this does
  /// not perform any code-page management -- meaning that the character set is
  /// singly-encoded and limited to exactly 127 characters and nothing more.
  ///
  /// In most cases, basic char encoded strings are likely to be ASCII and not
  /// ANSI.
  /////////////////////////////////////////////////////////////////////////////
  class ascii_encoding
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

    static constexpr auto decode_sentinel = char32{U'\uFFFD'};
    static constexpr auto encode_sentinel = char_type{};

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
    static constexpr auto decode(
      ForwardIt begin,
      ForwardIt end,
      char32 replacement = decode_sentinel
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
    static constexpr auto encode(
      char32 input,
      OutputIt output,
      char_type replacement = encode_sentinel
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
inline constexpr
auto alloy::core::ascii_encoding::decode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement
) noexcept -> std::pair<char32,ForwardIt>
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return {replacement, begin};
  }
  const auto unit = static_cast<unsigned char>(*begin);
  ++begin;

  // ASCII only has 127-valid codepoints. Anything outside this range is invalid.
  if (ALLOY_UNLIKELY(unit > 127)) {
    return {replacement, begin};
  }
  return {unit, begin};
}

template <typename OutputIt>
inline constexpr
auto alloy::core::ascii_encoding::encode(
  char32 input,
  OutputIt output,
  char_type replacement
) noexcept -> OutputIt
{
  // Don't optimize for bad-case, since badly formed strings are less common
  // than well-formed strings
  if (ALLOY_UNLIKELY(input > 127)) {
    (*output++) = replacement;
  } else {
    // ASCII is fully compatible with UTF-32 codepoints by simply widening the
    // value. Since we know the value is within the valid range, we can simply
    // expand the value here and assign it.
    (*output++) = static_cast<char_type>(input);
  }

  return output;
}

template <typename ForwardIt>
inline constexpr
auto alloy::core::ascii_encoding::next(ForwardIt begin, ForwardIt end)
  noexcept -> ForwardIt
{
  if (ALLOY_UNLIKELY(begin == end)) {
    return end;
  }

  return ++begin;
}

#endif /* ALLOY_CORE_STRING_ENCODING_ASCII_ENCODING_HPP */
