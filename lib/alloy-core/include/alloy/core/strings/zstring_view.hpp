////////////////////////////////////////////////////////////////////////////////
/// \file zstring_view.hpp.h
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#ifndef LIB_ALLOY_CORE_INCLUDE_ALLOY_CORE_STRINGS_ZSTRING_VIEW_HPP
#define LIB_ALLOY_CORE_INCLUDE_ALLOY_CORE_STRINGS_ZSTRING_VIEW_HPP

#include "alloy/core/strings/string_view.hpp"

namespace alloy::core {

  //===========================================================================
  // class : basic_zstring_view<CharT,Traits>
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A non-owning reference to null-terminated strings ("zstrings")
  ///
  /// Unlike the normal string_view, this type can only view strings that are
  /// null-terminated, like string literals. This type is intended for interop
  /// with C-APIs that require null terminated strings, to avoid requiring a
  /// potentially redundant heap allocation just to add the null terminator.
  ///
  /// An instance of a zstring type can be passed as a normal string_view
  ///
  /// \tparam CharT The underlying character type
  /// \tparam Traits The traits that correspond to the character
  /////////////////////////////////////////////////////////////////////////////
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  class basic_zstring_view : public basic_string_view<CharT,Traits>
  {
    using base_type = basic_string_view<CharT,Traits>;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using traits_type     = typename base_type::traits_type;
    using size_type       = typename base_type::size_type;
    using difference_type = typename base_type::difference_type;

    using value_type      = typename base_type::value_type;
    using pointer         = typename base_type::pointer;
    using const_pointer   = typename base_type::const_pointer;
    using reference       = typename base_type::reference;
    using const_reference = typename base_type::const_reference;

    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;

    //-------------------------------------------------------------------------
    // Public Constants
    //-------------------------------------------------------------------------
  public:

    using base_type::npos;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------

    /// \brief Constructs a basic_zstring_view from a string_view type
    ///
    /// This factory allows converting a string_view that may not be null
    /// terminated to a null-terminated view.
    ///
    /// \note This does NOT do any checking of true null-termination in
    ///       production. It is a requirement that consumers of this function
    ///       use this only when a string is guaranteed to be null-terminated
    ///
    /// \param str the string to determine is null-terminated
    /// \return the zstring view
    static constexpr auto from_view(basic_string_view<CharT,Traits> str)
      noexcept -> basic_zstring_view;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a basic_zstring_view that refers to an empty
    ///        null-terminated string
    constexpr basic_zstring_view() noexcept = default;

    /// \brief Constructs a basic_zstring_view that refers to the
    ///        null-terminated string \p str
    ///
    /// \pre \p str cannot be null. The range [str, str + strlen(str)] must be
    ///      valid
    ///
    /// \param str the string to view
    constexpr basic_zstring_view(const CharT* str) noexcept;
    constexpr basic_zstring_view(std::nullptr_t) = delete;

    /// \brief Constructs a basic_zstring_view that views the string owned by
    ///        a std::basic_string
    ///
    /// \param str the string to view
    basic_zstring_view(const basic_string<CharT,Traits>& str) noexcept;

    /// \brief Constructs this basic_zstring_view by copying the contents of
    ///        \p other
    ///
    /// \param other the other basic_zstring_view to copy
    constexpr basic_zstring_view(const basic_zstring_view& other) noexcept = default;

    /// \brief Assigns the contents of \p other to this basic_zstring_view
    ///
    /// \param other the other basic_zstring_view to copy
    /// \return reference to \c (*this)
    auto operator=(const basic_zstring_view& other) -> basic_zstring_view& = default;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    using base_type::operator[];
    using base_type::at;
    using base_type::front;
    using base_type::back;
    using base_type::data;

    /// \brief Returns a pointer to the underlying null-terminated string
    ///
    /// \return the pointer
    constexpr auto c_str() const noexcept -> const_pointer;

    //-------------------------------------------------------------------------
    // Capacity
    //-------------------------------------------------------------------------
  public:

    using base_type::size;
    using base_type::length;
    using base_type::max_size;
    using base_type::empty;

    //-------------------------------------------------------------------------
    // Operations
    //-------------------------------------------------------------------------
  public:

    using base_type::remove_prefix;
    auto remove_suffix(std::size_t) -> void = delete;
    auto substr(std::size_t,std::size_t) -> void = delete;

    //-------------------------------------------------------------------------
    // Iterators
    //-------------------------------------------------------------------------
  public:

    using base_type::begin;
    using base_type::cbegin;
    using base_type::rbegin;
    using base_type::crbegin;
    using base_type::end;
    using base_type::cend;
    using base_type::rend;
    using base_type::crend;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    constexpr basic_zstring_view(const CharT* p, size_type size) noexcept;
  };

  //===========================================================================
  // aliases : class : basic_zstring_view<CharT,Traits>
  //===========================================================================

  using zstring_view    = basic_zstring_view<char>;
  using zwstring_view   = basic_zstring_view<wchar_t>;
  using zu16string_view = basic_zstring_view<char16_t>;
  using zu32string_view = basic_zstring_view<char32_t>;

  // The 'u8' literal prefix in C++17 produces a char literal, but in C++20
  // will produce a char8_t literal. This is an easy way to toggle between
  // char and char8_t without requiring #ifdef and feature-test macros
  using zu8string_view = basic_string_view<decltype(u8' ')>;

} // namespace alloy::core


//=============================================================================
// definitions : class : basic_string_view
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr
auto alloy::core::basic_zstring_view<CharT,Traits>::from_view(
  basic_string_view<CharT,Traits> str
) noexcept -> basic_zstring_view
{
  return basic_zstring_view{str.data(), str.size()};
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr
alloy::core::basic_zstring_view<CharT,Traits>::basic_zstring_view(const CharT* str)
  noexcept
  : base_type{str}
{

}

template <typename CharT, typename Traits>
inline
alloy::core::basic_zstring_view<CharT,Traits>::basic_zstring_view(
  const basic_string<CharT,Traits>& str
) noexcept
  : base_type{str.c_str(), str.size()}
{

}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr
auto alloy::core::basic_zstring_view<CharT,Traits>::c_str()
  const noexcept -> const_pointer
{
  return base_type::data();
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr
alloy::core::basic_zstring_view<CharT,Traits>
  ::basic_zstring_view(const CharT* p, size_type size)
  noexcept
  : base_type{p, size}
{

}

#endif /* LIB_ALLOY_CORE_INCLUDE_ALLOY_CORE_STRINGS_ZSTRING_VIEW_HPP */
