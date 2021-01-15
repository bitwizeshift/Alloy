/*****************************************************************************
 * \file hashed_string_view.hpp
 *
 * \brief This header defines a hashed string_view
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_STRINGS_HASHED_STRING_VIEW_HPP
#define ALLOY_CORE_STRINGS_HASHED_STRING_VIEW_HPP

#include "alloy/core/utilities/fnv1a_hash.hpp"
#include "alloy/core/strings/string_view.hpp"

#include <utility> // std::hash
#include <climits> // CHAR_BIT

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A string view that contains a (possibly) compile-time generated
  ///        hash of the string
  ///
  /// This class produces hashes which may not be compatible with the
  /// std::hash of basic_string_view or of std::basic_string; no assumption
  /// should be made on these results.
  ///
  /// This class is generally useful for doing lightweight comparisons of
  /// strings so that it can easily be used as an ID for managing resources.
  ///
  /// \tparam CharT The character type
  /// \tparam Traits The character traits type
  /////////////////////////////////////////////////////////////////////////////
  template <typename CharT, typename Traits = std::char_traits<CharT>>
  class basic_hashed_string_view : public basic_string_view<CharT, Traits>
  {
    using base_type = basic_string_view<CharT,Traits>;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using view_type       = basic_string_view<CharT,Traits>;
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
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs the string view
    constexpr basic_hashed_string_view() noexcept;

    /// \brief Constructs a hashed string view from a character sequence
    ///
    /// \param view a string_view
    constexpr /* IMPLICIT */ basic_hashed_string_view(view_type view) noexcept;

    /// \brief Constructs a hashed string view from a character sequence of
    ///        size \p size
    ///
    /// \param p a pointer to string
    /// \param size the number of characters in the string
    constexpr basic_hashed_string_view(const CharT* p, size_type size) noexcept;

    /// \brief Copy-constructs a hashed string view
    ///
    /// \param other the other hashed string view to copy
    constexpr basic_hashed_string_view(const basic_hashed_string_view& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copy-assigns the contents of \p other
    ///
    /// \param other the other view to copy
    /// \return reference to \c (*this)
    constexpr basic_hashed_string_view&
      operator=(const basic_hashed_string_view& other) noexcept = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying hash of this string
    ///
    /// \return the hash of this string
    constexpr size_type hash() const noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    size_type m_hash;
  };

  //=============================================================================
  // aliases : class : basic_hashed_string_view
  //=============================================================================

  using hashed_string_view    = basic_hashed_string_view<char>;
  using hashed_wstring_view   = basic_hashed_string_view<wchar_t>;
  using hashed_u16string_view = basic_hashed_string_view<char16_t>;
  using hashed_u32string_view = basic_hashed_string_view<char32_t>;

  //=============================================================================
  // non-member functions  : class : basic_hashed_string_view
  //=============================================================================

  //-----------------------------------------------------------------------------
  // Equality
  //-----------------------------------------------------------------------------

  template <typename CharT, typename Traits>
  constexpr bool operator==(const basic_hashed_string_view<CharT,Traits>& lhs,
                            const basic_hashed_string_view<CharT,Traits>& rhs) noexcept;

  template <typename CharT, typename Traits>
  constexpr bool operator!=(const basic_hashed_string_view<CharT,Traits>& lhs,
                            const basic_hashed_string_view<CharT,Traits>& rhs) noexcept;

} // namespace alloy::core

//=============================================================================
// definitions : class : basic_hashed_string_view
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr alloy::core::basic_hashed_string_view<CharT,Traits>
  ::basic_hashed_string_view()
  noexcept
  : base_type{},
    m_hash{fnv1a_traits<sizeof(size_type)*CHAR_BIT>::offset}
{

}


template <typename CharT, typename Traits>
inline constexpr alloy::core::basic_hashed_string_view<CharT,Traits>
  ::basic_hashed_string_view(view_type view)
  noexcept
  : base_type{view},
    m_hash{fnv1a_hash<sizeof(size_type)*CHAR_BIT>(view.data(), view.size())}
{

}


template <typename CharT, typename Traits>
inline constexpr alloy::core::basic_hashed_string_view<CharT,Traits>
  ::basic_hashed_string_view(const CharT* p, size_type size)
  noexcept
  : base_type{p},
    m_hash{fnv1a_hash<sizeof(size_type)*CHAR_BIT>(p, size)}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr typename alloy::core::basic_hashed_string_view<CharT,Traits>::size_type
  alloy::core::basic_hashed_string_view<CharT,Traits>::hash()
  const noexcept
{
  return m_hash;
}

//=============================================================================
// definitions : non-member functions : class : basic_hashed_string_view
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

template <typename CharT, typename Traits>
inline constexpr bool
  alloy::core::operator==(const basic_hashed_string_view<CharT,Traits>& lhs,
                          const basic_hashed_string_view<CharT,Traits>& rhs)
  noexcept
{
  using underlying_type = basic_string_view<CharT,Traits>;

  return lhs.hash() == rhs.hash() &&
         static_cast<const underlying_type&>(lhs) == static_cast<const underlying_type&>(rhs);
}

template <typename CharT, typename Traits>
inline constexpr bool
  alloy::core::operator!=(const basic_hashed_string_view<CharT,Traits>& lhs,
                          const basic_hashed_string_view<CharT,Traits>& rhs)
  noexcept
{
  return !(lhs == rhs);
}

#endif /* ALLOY_CORE_STRINGS_HASHED_STRING_VIEW_HPP */
