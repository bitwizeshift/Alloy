/*****************************************************************************
 * \file encoding_traits.hpp
 *
 * \brief This header defines general traits for encoding logic
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022-2023, 2025-2026 Matthew Rodusek All rights reserved.

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
#include "alloy/core/traits/always_false.hpp"

#include <algorithm>   // std::find
#include <utility>     // std::declval, std::pair
#include <tuple>       // std::tie
#include <iterator>    // std::distance
#include <type_traits> // std::is_class_v

namespace alloy::core {

  /// \brief A trait for identifying encodings that may be subsets of other
  ///        encodings.
  ///
  /// This should be specialized for any encoding wanting to identify being a
  /// subset of another one, such as ascii and utf8, or wide and utf16 on Windows.
  ///
  /// This trait enables optimizations for certain operations since they can be
  /// treated as the same underlying encoding type.
  ///
  /// \tparam SubsetEncoding the encoding to test is a subset
  /// \tparam FullEncoding the encoding to test is the superset
  template <typename SubsetEncoding, typename FullEncoding>
  struct is_subset_encoding_of : std::false_type{};

  /// \brief A trait for identifying compatible encodings which may be compared
  ///        directly with one-another without requiring decoding of code-points.
  ///
  /// This trait should NOT be specialized or modified. Specialize
  /// `is_subset_encoding_of` instead, and allow this trait to pick up that the
  /// two are compatible.
  ///
  /// This trait is effectively an aggregate that checks `is_subset_encoding_of`
  /// bidirectionally between `Encoding1` and `Encoding2`.
  ///
  /// \tparam Encoding1 the first encoding to compare
  /// \tparam Encoding2 the second encoding to compare
  template <typename Encoding1, typename Encoding2>
  struct is_compatible_encoding : std::disjunction<
    is_subset_encoding_of<Encoding1, Encoding2>,
    is_subset_encoding_of<Encoding2, Encoding1>
  >{};

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
    using detect_max_units_per_char = decltype(T::max_units_per_char);

    template <typename UEncoding, typename...Args>
    using detect_decode = decltype(UEncoding::decode(std::declval<Args>()...));

    template <typename UEncoding, typename...Args>
    using detect_rdecode = decltype(UEncoding::rdecode(std::declval<Args>()...));

    template <typename UEncoding, typename...Args>
    using detect_encode = decltype(UEncoding::encode(std::declval<Args>()...));

    template <typename T, bool HasMaxUnits = is_detected_v<detect_max_units_per_char, T>>
    struct max_units_per_char_impl : std::integral_constant<std::size_t,1>{};

    template <typename T>
    struct max_units_per_char_impl<T,true> : std::integral_constant<std::size_t,T::max_units_per_char>{};

    template <typename UEncoding>
    using detect_is_char_boundary = decltype(
      UEncoding::is_char_boundary(std::declval<typename UEncoding::char_type>())
    );

    template <typename UEncoding, typename ForwardIt, typename...Args>
    static inline constexpr auto decodeable = is_detected_v<
      detect_decode, UEncoding, ForwardIt, ForwardIt, char32, Args...
    >;
    template <typename UEncoding, typename ForwardIt, typename...Args>
    static inline constexpr auto rdecodeable = is_detected_v<
      detect_rdecode, UEncoding, ForwardIt, ForwardIt, char32, Args...
    >;
    template <typename UEncoding, typename OutputIt, typename...Args>
    static inline constexpr auto encodeable = is_detected_v<
      detect_encode, UEncoding, char32, OutputIt, typename UEncoding::char_type, Args...
    >;

    template <typename UEncoding>
    static inline constexpr auto has_is_char_boundary = is_detected_convertible_v<
      bool, detect_is_char_boundary, UEncoding
    >;

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

    static inline constexpr auto max_units_per_char = max_units_per_char_impl<Encoding>::value;
    static inline constexpr auto is_multi_unit      = max_units_per_char != 1u;

    //-------------------------------------------------------------------------
    // Queries
    //-------------------------------------------------------------------------
  public:

    /// \brief A helper function to detect if a given unit is the start of a
    ///        code-point boundary
    ///
    /// This function only participates in overload resolution if the underlying
    /// encoding either defines `is_char_boundary(char_type)`, or the encoding
    /// is a non-multi-unit encoded system.
    ///
    /// \param unit the code unit to check
    /// \return true if this is the start of a code-unit sequence
    template <typename E=Encoding>
    static constexpr auto is_char_boundary(char_type unit) noexcept -> bool
      requires (has_is_char_boundary<E> || !is_multi_unit);

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
    template <typename ForwardIt, typename...Args>
    [[nodiscard]]
    static constexpr auto decode(
      ForwardIt begin,
      ForwardIt end,
      char32 replacement = decode_sentinel,
      Args&&...args
    ) noexcept -> std::pair<char32,ForwardIt>
      requires decodeable<Encoding, ForwardIt, Args...>;

    /// \brief Decodes a reverse-sequence of code-units into a codepoint
    ///
    /// This will forward to `Encoding::rdecode` if an implementation is present,
    /// or to `decode` for single-unit encodings like UTF-32 or ANSI.
    ///
    /// \param begin the beginning of the reverse-sequence to decode
    /// \param end the end of the reverse-sequence to decode
    /// \param replacement the character to replace on failure to decode
    /// \param args additional arguments to forward to the underlying decoder
    /// \return A pair containing the read UTF-32 codepoint, along with the
    ///         iterator
    template <typename ForwardIt, typename...Args>
    [[nodiscard]]
    static constexpr auto rdecode(
      ForwardIt begin,
      ForwardIt end,
      char32 replacement = decode_sentinel,
      Args&&...args
    ) noexcept -> std::pair<char32,ForwardIt>
      requires (rdecodeable<Encoding, ForwardIt, Args...> ||
                decodeable<Encoding, ForwardIt, Args...>);

    //-------------------------------------------------------------------------

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
    template <typename OutputIt, typename...Args>
    static constexpr auto encode(
      char32 input,
      OutputIt output,
      char_type replacement = encode_sentinel,
      Args&&...args
    ) noexcept -> OutputIt
      requires encodeable<Encoding, OutputIt, Args...>;

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
    [[nodiscard]]
    static constexpr auto next(ForwardIt begin, ForwardIt end, uquantity<char32> n)
      noexcept -> ForwardIt;

    //-------------------------------------------------------------------------

    /// \brief Advances `begin` to the end of the previous (possibly
    ///        multi-code-unit) codepoint
    ///
    /// This function operates on code-units from reverse-ordering.
    ///
    /// \param begin the beginning of the range
    /// \param end the end of the range
    /// \return the incremented iterator
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto rnext(ForwardIt begin, ForwardIt end)
      noexcept -> ForwardIt;

    /// \brief Advances `begin` to the end of the previous (possibly
    ///        multi-code-unit) codepoint
    ///
    /// This function operates on code-units from reverse-ordering.
    ///
    /// \param begin the beginning of the range
    /// \param end the end of the range
    /// \param n the nth index to move to
    /// \return the incremented iterator
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto rnext(ForwardIt begin, ForwardIt end, uquantity<char32> n)
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

    /// \brief Finds the first occurrence of the character \p ch in the reverse-range
    ///        `[first, last)`
    ///
    /// This will return an iterator to the end of the code-point sequence that
    /// forms \p ch so that subsequent calls with `rdecode` will form \p ch.
    ///
    /// \param first an iterator to the start of the reverse-range
    /// \param last an iterator to the end of the reverse-range
    /// \param ch the character to find
    /// \return an iterator to where the codepoint starts, if found, or the end
    template <typename ForwardIt>
    [[nodiscard]]
    static constexpr auto rfind(ForwardIt first, ForwardIt last, char32 ch)
      noexcept -> ForwardIt;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    template <typename UEncoding, typename ForwardIt, typename...Args>
    using detect_next = decltype(
      UEncoding::next(std::declval<ForwardIt>(), std::declval<ForwardIt>(), std::declval<Args>()...)
    );

    template <typename UEncoding, typename ForwardIt, typename...Args>
    using detect_rnext = decltype(
      UEncoding::rnext(std::declval<ForwardIt>(), std::declval<ForwardIt>(), std::declval<Args>()...)
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

    template <typename UEncoding, typename ForwardIt>
    using detect_rfind = decltype(
      UEncoding::rfind(
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

template <typename Encoding>
template <typename E>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::is_char_boundary(char_type unit)
  noexcept -> bool
  requires (has_is_char_boundary<E> || !is_multi_unit)
{
  if constexpr (has_is_char_boundary<Encoding>) {
    return Encoding::is_char_boundary(unit);
  } else if constexpr (!is_multi_unit) {
    compiler::unused(unit);
    return true;
  } else {
    static_assert(
      always_false_v<Encoding>,
      "Supplied encoding is multi-unit encoded, but does not provide an implementation "
      "of `is_char_boundary(...)`. "
      "This should be unreachable due to SFINAE."
    );
  }
}

//-----------------------------------------------------------------------------
// Encoding
//-----------------------------------------------------------------------------

template <typename Encoding>
template <typename ForwardIt, typename...Args>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::decode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement,
  Args&&...args
) noexcept -> std::pair<char32,ForwardIt>
  requires decodeable<Encoding, ForwardIt, Args...>
{
  return Encoding::decode(begin, end, replacement, std::forward<Args>(args)...);
}

template <typename Encoding>
template <typename ForwardIt, typename...Args>
ALLOY_FORCE_INLINE constexpr
auto alloy::core::encoding_traits<Encoding>::rdecode(
  ForwardIt begin,
  ForwardIt end,
  char32 replacement,
  Args&&...args
) noexcept -> std::pair<char32,ForwardIt>
  requires (rdecodeable<Encoding, ForwardIt, Args...> ||
           decodeable<Encoding, ForwardIt, Args...>)
{
  if constexpr (rdecodeable<Encoding,ForwardIt,Args...>) {
    return Encoding::rdecode(begin, end, replacement, std::forward<Args>(args)...);
  } else if constexpr (!is_multi_unit) {
    // single-unit encodings will decode the same forward as they will backwards.
    return Encoding::decode(begin, end, replacement, std::forward<Args>(args)...);
  } else {
    auto units = std::array<char_type,max_units_per_char>{};

    // Copy at most N units to decode using the forward-method. This searches for
    // the first code-unit in a multi-unit sequence, reconstructs it in-order, and
    // then decodes the value.
    auto it  = begin;
    auto unit_it = units.rbegin();
    const auto unit_end = units.rend();
    while ((it != end) && (unit_it != unit_end)) {
      const auto unit = *it;
      *unit_it = unit;
      ++unit_it;
      ++it;
      if (is_char_boundary(unit)) {
        break;
      }
    }
    const auto offset = unit_end - unit_it;

    const auto codepoint = decode(
      std::next(units.begin(), offset),
      units.end(),
      replacement
    ).first;

    return {codepoint, it};
  }
}

template <typename Encoding>
template <typename OutputIt, typename...Args>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::encode(
  char32 input,
  OutputIt output,
  char_type replacement,
  Args&&...args
) noexcept -> OutputIt
  requires encodeable<Encoding, OutputIt, Args...>
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
  if constexpr (is_detected_v<detect_next,Encoding,ForwardIt,uquantity<char32>>) {
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


template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::rnext(ForwardIt begin, ForwardIt end)
  noexcept -> ForwardIt
{
  if constexpr (is_detected_v<detect_rnext,Encoding,ForwardIt>) {
    return Encoding::rnext(begin, end);
  } else {
    return rdecode(begin, end).second;
  }
}

template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::rnext(
  ForwardIt begin,
  ForwardIt end,
  uquantity<char32> n
) noexcept -> ForwardIt
{
  if constexpr (is_detected_v<detect_rnext,Encoding,ForwardIt,uquantity<char32>>) {
    return Encoding::rnext(begin, end, n);
  } else if constexpr (is_multi_unit) {
    ALLOY_ASSERT(n <= length(begin, end));

    // Multi-unit requires decoding n characters before it's possible to get a
    // value
    for (; n != uquantity<char32>{0u}; --n) {
      begin = rnext(begin, end);
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
    return uquantity<char32>{static_cast<std::size_t>(std::distance(first, last))};
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

template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_traits<Encoding>::rfind(
  ForwardIt first,
  ForwardIt last,
  char32 ch
) noexcept -> ForwardIt
{
  if constexpr (is_detected_v<detect_rfind,Encoding,ForwardIt>) {
    return Encoding::rfind(first, last, ch);
  } else if constexpr (!is_multi_unit && is_detected_v<detect_find,Encoding,ForwardIt>) {
    // Single-unit encodings will search the same forward as they do backwards,
    // since there are no multi-units to decode first.
    return Encoding::find(first, last, ch);
  } else {
    auto codepoint = char32{};
    while (first != last) {
      const auto it = first;
      std::tie(codepoint, first) = rdecode(first, last, ch);
      if (codepoint == ch) {
        return it;
      }
    }
    return last;
  }
}
#endif /* ALLOY_CORE_STIRNG_ENCODING_ENCODING_TRAITS_HPP */
