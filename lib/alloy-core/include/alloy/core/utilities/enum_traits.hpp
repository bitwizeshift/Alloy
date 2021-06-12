////////////////////////////////////////////////////////////////////////////////
/// \file enum_traits.hpp
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

#ifndef ALLOY_CORE_UTILITIES_ENUM_TRAITS_HPP
#define ALLOY_CORE_UTILITIES_ENUM_TRAITS_HPP

#include <type_traits> // std::true_type, std::underlying_type, etc
#include <limits>      // std::numeric_limits
#include <iterator>    // std::*_iterator_category
#include <cstddef>     // std::size_t

namespace alloy::core {

  //============================================================================
  // trait : enum_is_contiguous
  //============================================================================

  /// \brief A trait which indicates whether an enum contains only contiguous
  ///        value definitions
  ///
  /// This trait should be specialized for each enum whose definitions are
  /// purey contiguous.
  ///
  /// \tparam Enum the enum type
  template <typename Enum>
  struct enum_is_contiguous : std::true_type {};

  template <typename Enum>
  inline constexpr auto enum_is_contiguous_v = enum_is_contiguous<Enum>::value;

  //============================================================================
  // trait : distinct_enumerators
  //============================================================================

  /// \brief A trait which determines how many distinct enumerators there are in
  ///        a given enum
  ///
  /// This trait should be specialized for each enum to indicate the last
  /// enumerator
  ///
  /// \tparam Enum the enum type
  template <typename Enum>
  struct distinct_enumerators : std::integral_constant<
    std::size_t,
    std::numeric_limits<std::underlying_type_t<Enum>>::max()
  >{};

  template <typename Enum>
  inline constexpr auto distinct_enumerators_v = distinct_enumerators<Enum>::value;

  //============================================================================
  // struct : enum_range_iterator
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An iterator for iterating enum ranges
  ///
  /// \tparam Enum the enum type being iterated
  //////////////////////////////////////////////////////////////////////////////
  template <typename Enum>
  class enum_range_iterator
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using value_type        = Enum;
    using pointer           = const Enum*;
    using reference         = const Enum&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using iterator_concept  = std::forward_iterator_tag;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs this enum range iterator
    constexpr enum_range_iterator() noexcept;

    /// \brief Constructs this enum range iterator from the given end point enum
    ///        \p e
    ///
    /// \param e the enum value
    constexpr explicit enum_range_iterator(Enum e) noexcept;

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other iterator
    enum_range_iterator(const enum_range_iterator& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other iterator
    /// \return reference to `(*this)`
    auto operator=(const enum_range_iterator&) -> enum_range_iterator& = default;

    //--------------------------------------------------------------------------
    // Iteration
    //--------------------------------------------------------------------------
  public:

    constexpr auto operator++() noexcept -> enum_range_iterator&;
    constexpr auto operator++(int) noexcept -> enum_range_iterator;

    //--------------------------------------------------------------------------
    // Equality
    //--------------------------------------------------------------------------
  public:

    constexpr auto operator==(const enum_range_iterator& other) const noexcept -> bool;
    constexpr auto operator!=(const enum_range_iterator& other) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using underlying_type = std::underlying_type_t<Enum>;

    Enum m_element;
  };

  //============================================================================
  // struct : enum_range
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A constructible range that contains the sequence of enumerators
  ///        for the given enum.
  ///
  /// This simple utility allows for a convenient way to iterate all enumerators
  /// in an `enum`.
  ///
  /// \tparam Enum the enum to iterate
  //////////////////////////////////////////////////////////////////////////////
  template <typename Enum>
  struct enum_range
  {
    static_assert(std::is_enum_v<Enum>);
    static_assert(
      enum_is_contiguous_v<Enum>,
      "enum_range only works with contiguous enums. "
      "If this is for a custom enum, make sure to specialize "
      "`enum_is_contiguous` to define `::value = true`."
    );

    constexpr auto begin() const noexcept -> enum_range_iterator<Enum>;
    constexpr auto end() const noexcept -> enum_range_iterator<Enum>;
  };

} // namespace alloy::core

//==============================================================================
// class : enum_range_iterator
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename Enum>
inline constexpr
alloy::core::enum_range_iterator<Enum>::enum_range_iterator()
  noexcept
  : m_element{}
{

}

template <typename Enum>
inline constexpr
alloy::core::enum_range_iterator<Enum>::enum_range_iterator(Enum e)
  noexcept
  : m_element{e}
{
}

//------------------------------------------------------------------------------
// Iteration
//------------------------------------------------------------------------------

template <typename Enum>
inline constexpr
auto alloy::core::enum_range_iterator<Enum>::operator++()
  noexcept -> enum_range_iterator&
{
  m_element = static_cast<Enum>(static_cast<underlying_type>(m_element) + 1);
  return (*this);
}

template <typename Enum>
inline constexpr
auto alloy::core::enum_range_iterator<Enum>::operator++(int)
  noexcept -> enum_range_iterator
{
  const auto copy = (*this);
  ++(*this);
  return copy;
}

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

template <typename Enum>
inline constexpr
auto alloy::core::enum_range_iterator<Enum>::operator==(const enum_range_iterator& other)
  const noexcept -> bool
{
  return m_element == other.m_element;
}

template <typename Enum>
inline constexpr
auto alloy::core::enum_range_iterator<Enum>::operator!=(const enum_range_iterator& other)
  const noexcept -> bool
{
  return m_element != other.m_element;
}

//==============================================================================
// struct : enum_range
//==============================================================================

template <typename Enum>
inline constexpr
auto alloy::core::enum_range<Enum>::begin()
  const noexcept -> enum_range_iterator<Enum>
{
  return enum_range_iterator<Enum>{};
}

template <typename Enum>
inline constexpr
auto alloy::core::enum_range<Enum>::end()
  const noexcept -> enum_range_iterator<Enum>
{
  return enum_range_iterator<Enum>{distinct_enumerators_v<Enum>};
}

#endif /* ALLOY_CORE_UTILITIES_ENUM_TRAITS_HPP */
