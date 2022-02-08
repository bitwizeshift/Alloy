/*****************************************************************************
 * \file encoding_converter.hpp
 *
 * \brief This header provides the definition of a generic encoding converter
 *****************************************************************************/

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
#ifndef ALLOY_CORE_STRING_ENCODING_ENCODING_CONVERTER_HPP
#define ALLOY_CORE_STRING_ENCODING_ENCODING_CONVERTER_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp"      // char32
#include "alloy/core/intrinsics.hpp" // compiler::unused
#include "alloy/core/traits/detected.hpp" // is_detected_v
#include "alloy/core/string/encoding/encoding_traits.hpp"

#include <utility> // std::declval, std::pair
#include <tuple>   // std::tie

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A utility for converting between different encodings
  ///
  /// This acts as a general-purpose functor object. If FromEncoding and
  /// ToEncoding are the same, this is simply a `std::copy`; otherwise this
  /// will normalize encoding to `char32_t` first, and then re-encode into the
  /// new format.
  /////////////////////////////////////////////////////////////////////////////
  template <typename FromEncoding, typename ToEncoding>
  class encoding_converter
  {
    template <typename...Args>
    using detect_decode = decltype(FromEncoding::decode(std::declval<Args>()...));

    template <typename...Args>
    using detect_encode = decltype(ToEncoding::encode(std::declval<Args>()...));

    template <typename ForwardIt, typename...Args>
    static inline constexpr auto decodeable = is_detected_v<
      detect_decode, ForwardIt, ForwardIt, char32, Args...
    >;
    template <typename OutputIt, typename...Args>
    static inline constexpr auto encodeable = is_detected_v<
      detect_encode, char32, OutputIt, typename ToEncoding::char_type, Args...
    >;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using from_encoding = FromEncoding;
    using to_encoding = ToEncoding;
    using char_type = typename ToEncoding::char_type;

    //-------------------------------------------------------------------------
    // Public Static Constants
    //-------------------------------------------------------------------------
  public:

    /// \brief Used for bad replacements when decoding
    static inline constexpr auto decode_sentinel = char32{};

    /// \brief Used for bad replacements when encoding
    static inline constexpr auto encode_sentinel = char_type{};

    //-------------------------------------------------------------------------
    // Invocation
    //-------------------------------------------------------------------------
  public:

    /// \brief Re-encodes the range from `begin` to `end` from `FromEncoding`
    ///        into the `output` in `ToEncoding` format
    ///
    /// Any characters that cannot be represented or encoded will be
    /// replaced with `encode_sentinel`
    ///
    /// This function only participates in overload resolution if at least one
    /// of `FromEncoding` or `ToEncoding` can be properly invoked with all
    /// supplied arguments.
    ///
    /// \param begin the start of the range to re-encode
    /// \param end the end of the range to re-encode
    /// \param output the iterator to write to
    /// \param replacement the character to replace on failure
    /// \param args additional arguments to forward to the encoding(s)
    template <typename ForwardIt, typename OutputIt, typename...Args,
              typename = std::enable_if_t<(
                decodeable<ForwardIt,Args...> || encodeable<OutputIt,Args...>
              )>>
    constexpr auto operator()(
      ForwardIt begin,
      ForwardIt end,
      OutputIt output,
      char_type replacement = encode_sentinel,
      Args&&...args
    ) const noexcept -> OutputIt;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    template <typename ForwardIt, typename...Args>
    static constexpr auto do_decode(
      ForwardIt begin,
      ForwardIt end,
      Args&&...args
    ) noexcept -> std::pair<char32,ForwardIt>;

    template <typename OutputIt, typename...Args>
    static constexpr auto do_encode(
      char32 codepoint,
      OutputIt output,
      char_type replacement = char_type{},
      Args&&...args
    ) noexcept -> OutputIt;
  };

} // namespace alloy::core

//=============================================================================
// inline implementation
//=============================================================================

//-----------------------------------------------------------------------------
// Invocation
//-----------------------------------------------------------------------------

template <typename FromEncoding, typename ToEncoding>
template <typename ForwardIt, typename OutputIt, typename...Args, typename>
inline constexpr
auto alloy::core::encoding_converter<FromEncoding,ToEncoding>::operator()(
  ForwardIt begin,
  ForwardIt end,
  OutputIt output,
  char_type replacement,
  Args&&...args
) const noexcept -> OutputIt
{
  if constexpr (std::is_same_v<FromEncoding, ToEncoding>) {
    compiler::unused(replacement, args...);
    return std::copy(begin, end, output);
  } else {
    auto codepoint = char32{};

    // Convert 'from' encoding to utf-32, then convert to new encoding
    while (begin != end) {
      std::tie(codepoint, begin) = do_decode(begin, end, std::forward<Args>(args)...);
      output = do_encode(codepoint, output, replacement, std::forward<Args>(args)...);
    }
    return output;
  }
}

//-----------------------------------------------------------------------------
// Private Members
//-----------------------------------------------------------------------------

template <typename FromEncoding, typename ToEncoding>
template <typename ForwardIt, typename...Args>
inline constexpr
auto alloy::core::encoding_converter<FromEncoding,ToEncoding>::do_decode(
  ForwardIt begin,
  ForwardIt end,
  Args&&...args
) noexcept -> std::pair<char32,ForwardIt>
{
  using traits_type = encoding_traits<FromEncoding>;

  if constexpr (decodeable<ForwardIt,Args...>) {
    return traits_type::decode(begin, end, decode_sentinel, std::forward<Args>(args)...);
  } else {
    compiler::unused(args...);
    return traits_type::decode(begin, end);
  }
}


template <typename FromEncoding, typename ToEncoding>
template <typename OutputIt, typename...Args>
inline constexpr
auto alloy::core::encoding_converter<FromEncoding,ToEncoding>::do_encode(
  char32 codepoint,
  OutputIt output,
  char_type replacement,
  Args&&...args
) noexcept -> OutputIt
{
  using traits_type = encoding_traits<ToEncoding>;

  if constexpr (encodeable<OutputIt,Args...>) {
    return traits_type::encode(codepoint, output, replacement, std::forward<Args>(args)...);
  } else {
    compiler::unused(args...);
    return traits_type::encode(codepoint, output, replacement);
  }
}

#endif /* ALLOY_CORE_STRING_ENCODING_ENCODING_CONVERTER_HPP */
