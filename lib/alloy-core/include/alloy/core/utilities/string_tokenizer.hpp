///////////////////////////////////////////////////////////////////////////////
/// \file string_tokenizer.hpp
///
/// \todo(Bitwize): Document this
///////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_UTILITIES_STRING_TOKENIZER_HPP
#define ALLOY_CORE_UTILITIES_STRING_TOKENIZER_HPP

#include "alloy/core/api.hpp"
#include "alloy/core/utilities/string_view.hpp"

#include "alloy/core/utilities/expected.hpp"

#include <utility> // std::forward
#include <string>  // std::char_traits
#include <system_error> // std::error_code
#include <type_traits>  // std::true_type

namespace alloy::core {

  //===========================================================================
  // enum : tokenizer_error
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An enumeration used for tokenizing errors
  /////////////////////////////////////////////////////////////////////////////
  enum class tokenizer_error {
    out_of_tokens = 1,
  };

  //===========================================================================
  // class : basic_string_tokenizer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A tokenizer class for strings.
  ///
  /// This tokenizes any input string_view objects into a series of iterable
  /// tokens based on the input delimiter.
  ///
  /// All tokens are served as non-owning strings, leaving it the responsibility
  /// of the consumer to take ownership of the result. This facilitates
  /// lightweight comparisons if the tokenizer is used in a context where the
  /// results are analyzed from within a scope where the tokenizer stays alive.
  ///
  /// \tparam CharT The character type to tokenize
  /// \tparam Traits The traits for the character type
  /////////////////////////////////////////////////////////////////////////////
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  class basic_string_tokenizer
  {
    //-------------------------------------------------------------------------
    // Public Types
    //-------------------------------------------------------------------------
  public:

    using value_type  = CharT;
    using traits_type = Traits;
    using string_type = basic_string_view<CharT,Traits>;
    using size_type = typename basic_string_view<CharT,Traits>::size_type;

    //-------------------------------------------------------------------------
    // Construction / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Tokenizes a given string with the specified delimiter, with a
    ///        given option to return delimiters are part of the token
    ///
    /// \param str the string to tokenize
    /// \param delim the delimiter to use for tokenization
    constexpr explicit basic_string_tokenizer(string_type str,
                                              string_type delim);

    // Deleted default, copy, and move constructors
    basic_string_tokenizer() = delete;
    basic_string_tokenizer(const basic_string_tokenizer&) = delete;
    basic_string_tokenizer(basic_string_tokenizer&&) = delete;

    //-------------------------------------------------------------------------

    // Deleted copy and move assignment
    auto operator=(const basic_string_tokenizer&) -> basic_string_tokenizer = delete;
    auto operator=(basic_string_tokenizer&&) -> basic_string_tokenizer = delete;

    //-------------------------------------------------------------------------
    // Accessors
    //-------------------------------------------------------------------------
  public:

    /// \brief Retrieves the delimiters for this tokenizer
    ///
    /// \return the delimiters
    constexpr auto delimiters() const noexcept -> string_type;

    /// \brief Retrieves the string used in this tokenizer
    ///
    /// \return the string
    constexpr auto buffer() const noexcept -> string_type;

    //-------------------------------------------------------------------------
    // Tokenizing
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns true if the tokenizer has more tokens available
    ///
    /// \return true if the tokenizer has more tokens
    constexpr auto has_more() const noexcept -> bool;

    /// \brief Gets the next token in this string tokenizer's string.
    ///
    /// \return the next token
    auto next() noexcept -> expected<string_type>;

    /// \brief Gets the next token in this string tokenizer's string.
    ///
    /// First, the set of characters considered to be delimiters by this
    /// basic_string_tokenizer object is temporarily changed to be the
    /// characters in the string \p delim.
    ///
    /// Then the next token in the string after the current position is
    /// returned. The current position is advanced beyond the recognized
    /// token. The delimiter used for parsing is unchanged by the end of
    /// this invocation
    ///
    /// \param delim the delimiter to match
    /// \return the next token
    auto next(string_type delim) noexcept -> expected<string_type>;

    /// \brief Resets the current position within this string tokenizer
    ///        to the beginning of the buffer
    ///
    /// This call allows for the basic_string_tokenizer to be re-used as if
    /// reconstructed.
    auto reset() noexcept -> void;

    //-------------------------------------------------------------------------

    /// \brief Iterates through each token in this tokenizer, calling \p fn on
    ///        each token
    ///
    /// \param fn the function to invoke
    template <typename Fn>
    auto for_each_token(Fn&& fn) noexcept -> void;

    /// \brief Iterates through each token in this tokenizer using \p delim as
    ///        the delimiter, calling \p fn on each token
    ///
    /// \param delim the delimiter to use
    /// \param fn the function to invoke
    template <typename Fn>
    auto for_each_delimited_token(string_type delim, Fn&& fn) noexcept -> void;

    //-------------------------------------------------------------------------
    // Private Member Functions
    //-------------------------------------------------------------------------
  private:

    /// \brief Retrieves the next token in the series
    ///
    /// \param delim the delimiters to check
    auto next_token(string_type delim) noexcept -> expected<string_type>;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    string_type m_buffer;        /// The buffer to tokenize
    string_type m_delimiter;     /// The delimiter used
    size_type   m_current;       /// Current character being read
  };

  //===========================================================================
  // non-member functions : class : basic_string_tokenizer
  //===========================================================================

  /// \brief Makes an error_code from a tokenizer_error
  ///
  /// \param error the error to convert
  /// \return the std::error_code object representing the error
  ALLOY_CORE_API
  auto make_error_code(tokenizer_error error) noexcept -> std::error_code;

  //===========================================================================
  // aliases : basic_string_tokenizer
  //===========================================================================

  using string_tokenizer    = basic_string_tokenizer<char,std::char_traits<char>>;
  using wstring_tokenizer   = basic_string_tokenizer<wchar_t,std::char_traits<wchar_t>>;
  using u16string_tokenizer = basic_string_tokenizer<char16_t,std::char_traits<char16_t>>;
  using u32string_tokenizer = basic_string_tokenizer<char32_t,std::char_traits<char32_t>>;

} // namespace alloy::core

namespace std {
  template <>
  struct is_error_code_enum<alloy::core::tokenizer_error> : std::true_type{};
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr
alloy::core::basic_string_tokenizer<CharT,Traits>
  ::basic_string_tokenizer(string_type str,
                           string_type delim)
  : m_buffer{str},
    m_delimiter{delim},
    m_current{0}
{

}

//-----------------------------------------------------------------------------
// Accessors
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr
auto alloy::core::basic_string_tokenizer<CharT,Traits>::delimiters()
  const noexcept -> string_type
{
  return m_delimiter;
}

template <typename CharT, typename Traits>
inline constexpr
auto alloy::core::basic_string_tokenizer<CharT,Traits>::buffer()
  const noexcept -> string_type
{
  return m_buffer;
}

//-----------------------------------------------------------------------------
// Tokenizing
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr auto
  alloy::core::basic_string_tokenizer<CharT,Traits>::has_more()
  const noexcept -> bool
{
  return m_current < m_buffer.size();
}

template <typename CharT, typename Traits>
inline
auto alloy::core::basic_string_tokenizer<CharT,Traits>::next()
  noexcept -> expected<string_type>
{
  return next_token(m_delimiter);
}

template <typename CharT, typename Traits>
inline
auto alloy::core::basic_string_tokenizer<CharT,Traits>::next(string_type delim)
  noexcept -> expected<string_type>
{
  return next_token(delim);
}

template <typename CharT, typename Traits>
inline
auto alloy::core::basic_string_tokenizer<CharT,Traits>::reset()
  noexcept -> void
{
  m_current = 0;
}

//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
template <typename Fn>
inline
auto alloy::core::basic_string_tokenizer<CharT, Traits>
  ::for_each_token(Fn&& fn)
  noexcept -> void
{
  for_each_delimited_token(m_delimiter, std::forward<Fn>(fn));
}

template <typename CharT, typename Traits>
template <typename Fn>
inline
auto alloy::core::basic_string_tokenizer<CharT, Traits>
  ::for_each_delimited_token(string_type delim, Fn&& fn)
  noexcept -> void
{
  while (has_more()) {
    const auto token = next(delim);

    if (token) {
      std::forward<Fn>(fn)(*token);
    }
  }
}

//-----------------------------------------------------------------------------
// Private Member Functions
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline
auto alloy::core::basic_string_tokenizer<CharT,Traits>
  ::next_token(string_type delim)
  noexcept -> expected<string_type>
{
  const auto buffer_size = m_buffer.size();

  // Search until a delimiter is found, or until we reach the end of the buffer
  while (m_current < buffer_size) {
    const auto start = m_current;
    const auto end = m_buffer.find_first_of(delim, start);

    // There is no more delimiter for the buffer; return the rest of the buffer
    if (end == string_type::npos) {
      m_current = buffer_size;
      return m_buffer.substr(start);
    }

    // Two delimiters are back to back. Emit an empty string
    if (end == start) {
      ++m_current;
      return "";
    }

    const auto size = end - start;
    m_current = end + 1;
    return m_buffer.substr(start, size);
  }

  return unexpected(tokenizer_error::out_of_tokens);
}

#endif /* ALLOY_CORE_UTILITIES_STRING_TOKENIZER_HPP */
