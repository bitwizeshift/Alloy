/*****************************************************************************
 * \file encoding_traits.hpp
 *
 * \brief This header defines general traits for encoding logic
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
#ifndef ALLOY_CORE_STIRNG_ENCODING_ENCODING_TRAITS_HPP
#define ALLOY_CORE_STIRNG_ENCODING_ENCODING_TRAITS_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp" // char32
#include "alloy/core/assert.hpp"
#include "alloy/core/traits/detected.hpp"    // is_detected
#include "alloy/core/utilities/quantity.hpp" // uquantity

#include <algorithm>   // std::find
#include <utility>     // std::declval, std::pair
#include <tuple>       // std::tie
#include <iterator>    // std::distance
#include <type_traits> // std::is_class_v

namespace alloy::core {

  namespace detail {

    template <typename Encoding, typename...Args>
    using detect_decode = decltype(Encoding::decode(std::declval<Args>()...));

    template <typename Encoding, typename...Args>
    using detect_encode = decltype(Encoding::encode(std::declval<Args>()...));

    template <typename Encoding, typename ForwardIt, typename...Args>
    static inline constexpr auto decodeable = is_detected_v<
      detect_decode, Encoding, ForwardIt, ForwardIt, char32, Args...
    >;
    template <typename Encoding, typename OutputIt, typename...Args>
    static inline constexpr auto encodeable = is_detected_v<
      detect_encode, Encoding, char32, OutputIt, typename Encoding::char_type, Args...
    >;
  } // namespace detail

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Traits for character encodings
  ///
  /// This helps to provide different basic definitions for static functions
  /// that are optionally definable, so that capabilities like finding
  /// characters in a sequence is possible.
  ///
  /// \tparam Encoding the character encoding
  /////////////////////////////////////////////////////////////////////////////
  template <typename Encoding>
  class encoding_traits
  {
    static_assert(std::is_class_v<Encoding>);

    template <typename T>
    using detect_is_multi_unit = decltype(T::is_multi_unit);

    template <typename T, bool HasMultiUnit = is_detected_v<detect_is_multi_unit, T>>
    struct is_multi_unit_impl : std::false_type{};

    template <typename T>
    struct is_multi_unit_impl<T,true> : std::bool_constant<T::is_multi_unit>{};

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using char_type = typename Encoding::char_type;

    //-------------------------------------------------------------------------
    // Public Static Constants
    //-------------------------------------------------------------------------
  public:

    static inline constexpr auto decode_sentinel = Encoding::decode_sentinel;
    static inline constexpr auto encode_sentinel = Encoding::encode_sentinel;
    static inline constexpr auto is_multi_unit   = is_multi_unit_impl<Encoding>::value;

    //-------------------------------------------------------------------------
    // Encoding
    //-------------------------------------------------------------------------
  public:

    /// \brief Decodes a sequence of code-units into a codepoint
    ///
    /// This will always forward to `Encoding::decode`
    ///
    /// \param begin the beginning of the sequence to decode
    /// \param end the end of the sequence to decode
    /// \param replacement the character to replace on failure to decode
    /// \param args additional arguments to forward to the underlying decoder
    /// \return A pair containing the read UTF-32 codepoint, along with the
    ///         iterator
    template <typename ForwardIt, typename...Args,
              typename = std::enable_if_t<detail::decodeable<Encoding,ForwardIt,Args...>>>
    [[nodiscard]]
    static constexpr auto decode(
      ForwardIt begin,
      ForwardIt end,
      char32 replacement = decode_sentinel,
      Args&&...args
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
    template <typename OutputIt, typename...Args,
              typename = std::enable_if_t<detail::encodeable<Encoding, OutputIt, Args...>>>
    static constexpr auto encode(
      char32 input,
      OutputIt output,
      char_type replacement = encode_sentinel,
      Args&&...args
    ) noexcept -> OutputIt;

    //-------------------------------------------------------------------------
    // Advancing
    //-------------------------------------------------------------------------
  public:

    /// \brief Advances `begin` to the start of the next (possibly
    ///        multi-code-unit) codepoint
    ///
    /// \param begin the beginning of the range
    /// \param end the end of the range
    /// \return the incremented iterator
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto next(ForwardIt begin, ForwardIt end)
      noexcept -> ForwardIt;

    /// \brief Advances `begin` to the start of the nth codepoint
    ///
    /// \param begin the beginning of the range
    /// \param end the end of the range
    /// \param n the nth index to move to
    /// \return the incremented iterator
    template <typename ForwardIt>
    static constexpr auto next(ForwardIt begin, ForwardIt end, uquantity<char32> n)
      noexcept -> ForwardIt;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    /// \brief Count the number of characters in this sequence
    ///
    /// This is different from `std::difference(begin, end)` for multi-unit
    /// character encodings like utf-8 or utf-16
    ///
    /// \note This will select `Encoding::length(...)` if it exists, otherwise
    ///       will be `length`
    ///
    /// \param first an iterator to the start of the sequence
    /// \param last  an iterator to the end of the sequence
    /// \return an iterator pointing to the end of the sequence
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto length(ForwardIt first, ForwardIt last)
      noexcept -> uquantity<char32>;

    /// \brief Finds the first occurrence of the character \p ch in the range
    ///        `[first, last)`
    ///
    /// This will return an iterator to the beginning of where the codepoint is
    /// discovered, if found, or to the end of the sequence if not.
    ///
    /// \param first an iterator to the start of the range
    /// \param last an iterator to the end of the range
    /// \param ch the character to find
    /// \return an iterator to where the codepoint starts, if found, or the end
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto find(ForwardIt first, ForwardIt last, char32 ch)
      noexcept -> ForwardIt;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    template <typename UEncoding, typename ForwardIt, typename...Args>
    using detect_next = decltype(
      UEncoding::next(std::declval<ForwardIt>(), std::declval<ForwardIt>(), std::declval<Args>()...)
    );

    template <typename UEncoding, typename ForwardIt>
    using detect_length = decltype(
      UEncoding::length(std::declval<ForwardIt>(), std::declval<ForwardIt>())
    );

    template <typename UEncoding, typename ForwardIt>
    using detect_find = decltype(
      UEncoding::find(
        std::declval<ForwardIt>(),
        std::declval<ForwardIt>(),
        std::declval<char32>()
      )
    );
  };

} // namespace alloy::core

//=============================================================================
// Inline definitions
//=============================================================================

//-----------------------------------------------------------------------------
// Encoding
//-----------------------------------------------------------------------------

template <typename Encoding>
template <typename ForwardIt, typename...Args, typename>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::decode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement,
  Args&&...args
) noexcept -> std::pair<char32,ForwardIt>
{
  return Encoding::decode(begin, end, replacement, std::forward<Args>(args)...);
}


template <typename Encoding>
template <typename OutputIt, typename...Args, typename>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::encode(
  char32 input,
  OutputIt output,
  char_type replacement,
  Args&&...args
) noexcept -> OutputIt
{
  return Encoding::encode(input, output, replacement, std::forward<Args>(args)...);
}


template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::next(ForwardIt begin, ForwardIt end)
  noexcept -> ForwardIt
{
  if constexpr (is_detected_v<detect_next,Encoding,ForwardIt>) {
    return Encoding::next(begin, end);
  } else {
    return decode(begin, end).second;
  }
}

template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::next(
  ForwardIt begin,
  ForwardIt end,
  uquantity<char32> n
) noexcept -> ForwardIt
{
  if constexpr (is_detected_v<detect_next,Encoding,ForwardIt, uquantity<char32>>) {
    return Encoding::next(begin, end, n);
  } else if constexpr (is_multi_unit) {
    ALLOY_ASSERT(n <= length(begin, end));

    // Multi-unit requires decoding n characters before it's possible to get a
    // value
    for (; n != uquantity<char32>{0u}; --n) {
      begin = next(begin, end);
    }
    return begin;
  } else {
    ALLOY_ASSERT(n <= std::distance(begin, end));

    return std::next(begin, n);
  }
}

//-----------------------------------------------------------------------------
// Operations
//-----------------------------------------------------------------------------

template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::length(
  ForwardIt first,
  ForwardIt last
) noexcept -> uquantity<char32>
{
  if constexpr (is_detected_v<detect_length,Encoding,ForwardIt>) {
    return Encoding::length(first, last);
  } else if constexpr (is_multi_unit) {
    auto count = uquantity<char32>{0u};

    while (first != last) {
      first = next(first, last);
      ++count;
    }
    return count;
  } else {
    // Non-multi-unit codepoints can be computed by counting distance
    return uquantity<char32>{std::distance(first, last)};
  }
}


template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::find(
  ForwardIt first,
  ForwardIt last,
  char32 ch
) noexcept -> ForwardIt
{
  if constexpr (is_detected_v<detect_find,Encoding,ForwardIt>) {
    return Encoding::find(first, last, ch);
  } else {
    auto codepoint = char32{};
    while (first != last) {
      const auto it = first;
      std::tie(codepoint, first) = decode(first, last, ch);
      if (codepoint == ch) {
        return it;
      }
    }
    return last;
  }
}

#endif /* ALLOY_CORE_STIRNG_ENCODING_ENCODING_TRAITS_HPP */
