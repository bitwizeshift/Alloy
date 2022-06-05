/*****************************************************************************
 * \file encoding_utilities.hpp
 *
 * \brief This header defines general utilities for encoding.
 *
 * This header is generally the header that should be included for most
 * project work.
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
#ifndef ALLOY_CORE_STIRNG_ENCODING_ENCODING_UTILITIES_HPP
#define ALLOY_CORE_STIRNG_ENCODING_ENCODING_UTILITIES_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/types.hpp" // char32
#include "alloy/core/utilities/mixins.hpp"   // uninstantiable
#include "alloy/core/utilities/quantity.hpp" // uquantity

#include "alloy/core/string/encoding/ansi_encoding.hpp"
#include "alloy/core/string/encoding/latin1_encoding.hpp"
#include "alloy/core/string/encoding/wide_encoding.hpp"
#include "alloy/core/string/encoding/utf8_encoding.hpp"
#include "alloy/core/string/encoding/utf16_encoding.hpp"
#include "alloy/core/string/encoding/utf32_encoding.hpp"
#include "alloy/core/string/encoding/encoding_traits.hpp"
#include "alloy/core/string/encoding/encoding_converter.hpp"

namespace alloy::core {

  //===========================================================================
  // class : encoding_utilities
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of generic utilities for encoded character types
  ///
  /////////////////////////////////////////////////////////////////////////////
  class encoding_utilities : uninstantiable
  {
    template <typename Encoding>
    struct encoded_length_functor {
      template <typename ForwardIt>
      auto operator()(ForwardIt first, ForwardIt last)
        const noexcept -> uquantity<char32>
      {
        return encoding_traits<Encoding>::length(first, last);
      }
    };

    template <typename Encoding>
    struct encoded_find_functor {
      template <typename ForwardIt>
      constexpr auto operator()(ForwardIt first, ForwardIt last, char32 ch)
        const noexcept -> ForwardIt
      {
        return encoding_traits<Encoding>::find(first, last, ch);
      }
    };

    //-------------------------------------------------------------------------
    // Public types
    //-------------------------------------------------------------------------
  public:

    template <typename Encoding,
              typename Iterator = const typename Encoding::char_type*>
    class basic_iterator;

    template <typename Encoding,
              typename Iterator = const typename Encoding::char_type*>
    class basic_range;

    //-------------------------------------------------------------------------
    // Public Utilities
    //-------------------------------------------------------------------------
  public:

    /// \brief A functor object for converting from `FromEncoding` to
    ///        `ToEncoding`
    ///
    /// \note
    /// This is a functor to simplify passing around to generic algorithms, and
    /// to also simplify reusing this function for the various convenience
    /// conversion classes below (e.g. `ansi::to_wide` and the like are all
    /// specialization of this)
    template <typename FromEncoding, typename ToEncoding>
    static inline constexpr auto convert = encoding_converter<
      FromEncoding,
      ToEncoding
    >{};

    /// \brief A functor object for determining the encoded length of a string
    ///
    /// \note
    /// This is a functor to simplify passing around to generic algorithms, and
    /// to also simplify reusing this function for the various convenience
    /// conversion classes below (e.g. `utf8::length` and the like are all
    /// specialization of this)
    template <typename Encoding>
    static inline constexpr auto encoded_length = encoded_length_functor<Encoding>{};

    /// \brief A functor object for finding the start of a character in an
    ///        encoded sequence
    ///
    /// Typical 'find' logic does not work in multi-code-unit encodings like
    /// utf-8 and utf-16, since the value may be spread over multiple values.
    /// This provides a means to find where the given code-point exists in a
    /// multi-unit string, and returns an iterator to the beginning of the
    /// sequence.
    ///
    /// \note
    /// This is a functor to simplify passing around to generic algorithms, and
    /// to also simplify reusing this function for the various convenience
    /// conversion classes below (e.g. `utf8::find` and the like are all
    /// specialization of this)
    template <typename Encoding>
    static inline constexpr auto encoded_find = encoded_find_functor<Encoding>{};

    //-------------------------------------------------------------------------
    // Conversion Utilities
    //-------------------------------------------------------------------------
  public:

    template <typename Encoding>
    struct encoding;

    using ansi   = encoding<ansi_encoding>;
    using latin1 = encoding<latin1_encoding>;
    using wide   = encoding<wide_encoding>;
    using utf8   = encoding<utf8_encoding>;
    using utf16  = encoding<utf16_encoding>;
    using utf32  = encoding<utf32_encoding>;

    //-------------------------------------------------------------------------
    // Traits
    //-------------------------------------------------------------------------
  public:

    using ansi_encoding_traits  = encoding_traits<ansi_encoding>;
    using wide_encoding_traits  = encoding_traits<wide_encoding>;
    using utf8_encoding_traits  = encoding_traits<utf8_encoding>;
    using utf16_encoding_traits = encoding_traits<utf16_encoding>;
    using utf32_encoding_traits = encoding_traits<utf32_encoding>;
  };

  //===========================================================================
  // class : encoding_utilities::basic_iterator
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An iterator type for converting encoded sequences into char32
  ///        sequences
  ///
  /// Encoding iterators are only capable of immutable views of the data, since
  /// writes may require relocating the rest of the sequence.
  ///
  /// \tparam Encoding the encoding type
  /// \tparam Iterator the underlying iterator to wrap
  /////////////////////////////////////////////////////////////////////////////
  template <typename Encoding, typename Iterator>
  class encoding_utilities::basic_iterator
  {
    /// \brief A small pointer-like type used for `operator->`
    class char32_pointer
    {
    public:

      char32_pointer(const char32_pointer& other) = default;
      constexpr char32_pointer(char32 v) : m_char{v}{}
      auto operator=(const char32_pointer& other) -> char32_pointer& = default;
      auto operator->() const noexcept -> const char32* { return &m_char; }

    private:
      char32 m_char;
    };
    using traits_type = encoding_traits<Encoding>;
    using itraits_type = std::iterator_traits<Iterator>;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using value_type = const char32;
    using reference  = const char32;
    using pointer    = char32_pointer;
    using difference_type = typename itraits_type::difference_type;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept  = std::forward_iterator_tag;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    basic_iterator() = default;
    constexpr explicit basic_iterator(Iterator it, Iterator end) noexcept;

    basic_iterator(const basic_iterator& other) = default;
    basic_iterator(basic_iterator&& other) = default;

    auto operator=(const basic_iterator& other) -> basic_iterator& = default;
    auto operator=(basic_iterator&& other) -> basic_iterator& = default;

    //-------------------------------------------------------------------------
    // Iteration & Access
    //-------------------------------------------------------------------------
  public:

    constexpr auto operator++() noexcept -> basic_iterator&;
    constexpr auto operator++(int) noexcept -> basic_iterator;

    constexpr auto operator*() const noexcept -> char32;
    constexpr auto operator->() const noexcept -> char32_pointer;

    //-------------------------------------------------------------------------
    // Equality
    //-------------------------------------------------------------------------
  public:

    constexpr auto operator==(const basic_iterator& other) const noexcept -> bool;
    constexpr auto operator!=(const basic_iterator& other) const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    Iterator m_iterator;
    Iterator m_end;
  };

  //===========================================================================
  // class : encoding_utilities::basic_range
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An iterator type for converting encoded sequences into char32
  ///        sequences
  ///
  /// Encoding iterators are only capable of immutable views of the data, since
  /// writes may require relocating the rest of the sequence.
  ///
  /// \tparam Encoding the encoding type
  /// \tparam Iterator the underlying iterator to wrap
  /////////////////////////////////////////////////////////////////////////////
  template <typename Encoding, typename Iterator>
  class encoding_utilities::basic_range
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using iterator = basic_iterator<Encoding,Iterator>;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    basic_range() = default;
    constexpr basic_range(Iterator begin, Iterator end) noexcept;

    basic_range(const basic_range& other) = default;
    basic_range(basic_range&& other) = default;

    auto operator=(const basic_range& other) -> basic_range& = default;
    auto operator=(basic_range&& other) -> basic_range& = default;

    //-------------------------------------------------------------------------
    // Iteration
    //-------------------------------------------------------------------------
  public:

    constexpr auto begin() const noexcept -> iterator;
    constexpr auto end() const noexcept -> iterator;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    Iterator m_begin;
    Iterator m_end;
  };

  //===========================================================================
  // class : encoding_utilities::encoding
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Utilities for for a specific encoding
  ///
  /// This is primarily intended to be used through the convenience aliases,
  /// such as `ansi`, `wide`, etc -- though it can also be used for custom
  /// encoding conversion logic.
  /////////////////////////////////////////////////////////////////////////////
  template <typename Encoding>
  struct encoding_utilities::encoding
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------

    using encoding_type = Encoding;
    using traits_type = encoding_traits<encoding_type>;
    using char_type = typename traits_type::char_type;

    template <typename Iterator>
    using basic_iterator = encoding_utilities::basic_iterator<Encoding,Iterator>;
    using iterator = basic_iterator<const char_type*>;

    template <typename Iterator>
    using basic_range = encoding_utilities::basic_range<Encoding,Iterator>;
    using range = basic_range<const char_type*>;

    //-------------------------------------------------------------------------
    // Utilities
    //-------------------------------------------------------------------------

    /// \brief Converts this encoding to `ToEncoding`
    template <typename ToEncoding>
    static inline constexpr auto to = convert<encoding_type,ToEncoding>;

    /// \{
    /// \brief Converts this encoding to the specified encoding
    static inline constexpr auto to_ansi   = to<ansi_encoding>;
    static inline constexpr auto to_wide   = to<wide_encoding>;
    static inline constexpr auto to_latin1 = to<latin1_encoding>;
    static inline constexpr auto to_utf8   = to<utf8_encoding>;
    static inline constexpr auto to_utf16  = to<utf16_encoding>;
    static inline constexpr auto to_utf32  = to<utf32_encoding>;
    /// \}

    /// \brief Computes the length of the specified string, in codepoints
    static inline constexpr auto length = encoded_length<encoding_type>;

    /// \brief Finds the specified codepoint in the encoded string
    static inline constexpr auto find = encoded_find<encoding_type>;

    /// \brief Produces an iteratable range from the given forward range
    ///
    /// \param begin the start of the range
    /// \param end the end of the range
    template <typename ForwardIt>
    static constexpr auto range_from(ForwardIt begin, ForwardIt end)
      noexcept -> basic_range<ForwardIt>;
  };

} // namespace alloy::core


//=============================================================================
// class : encoding_utilities::basic_iterator
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename Encoding, typename Iterator>
inline constexpr
alloy::core::encoding_utilities::basic_iterator<Encoding,Iterator>::basic_iterator(
  Iterator it,
  Iterator end
) noexcept
  : m_iterator{it},
    m_end{end}
{

}

//-----------------------------------------------------------------------------
// Iteration & Access
//-----------------------------------------------------------------------------

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_iterator<Encoding,Iterator>::operator++()
  noexcept -> basic_iterator&
{
  m_iterator = traits_type::next(m_iterator, m_end);
  return (*this);
}

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_iterator<Encoding,Iterator>::operator++(int)
  noexcept -> basic_iterator
{
  auto copy = (*this);
  ++(*this);
  return copy;
}

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_iterator<Encoding,Iterator>::operator*()
  const noexcept -> char32
{
  const auto [value, it] = traits_type::decode(m_iterator, m_end);
  compiler::unused(it);
  return value;
}

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_iterator<Encoding,Iterator>::operator->()
  const noexcept -> char32_pointer
{
  return char32_pointer{(**this)};
}

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_iterator<Encoding,Iterator>::operator==(
  const basic_iterator& other
) const noexcept -> bool
{
  return m_iterator == other.m_iterator;
}

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_iterator<Encoding,Iterator>::operator!=(
  const basic_iterator& other
) const noexcept -> bool
{
  return m_iterator != other.m_iterator;
}

//=============================================================================
// class : encoding_utilities::basic_range
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename Encoding, typename Iterator>
inline constexpr
alloy::core::encoding_utilities::basic_range<Encoding,Iterator>::basic_range(
  Iterator begin,
  Iterator end
) noexcept
  : m_begin{begin},
    m_end{end}
{

}

//-----------------------------------------------------------------------------
// Iteration
//-----------------------------------------------------------------------------

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_range<Encoding,Iterator>::begin()
  const noexcept -> iterator
{
  return iterator{m_begin, m_end};
}

template <typename Encoding, typename Iterator>
inline constexpr
auto alloy::core::encoding_utilities::basic_range<Encoding,Iterator>::end()
  const noexcept -> iterator
{
  return iterator{m_end, m_end};
}

//=============================================================================
// class : encoding_utilities::encoding<Encoding>
//=============================================================================

template <typename Encoding>
template <typename ForwardIt>
inline constexpr
auto alloy::core::encoding_utilities::encoding<Encoding>::range_from(
  ForwardIt begin,
  ForwardIt end
) noexcept -> basic_range<ForwardIt>
{
  return basic_range<ForwardIt>{begin, end};
}


#endif /* ALLOY_CORE_STIRNG_ENCODING_ENCODING_UTILITIES_HPP */
