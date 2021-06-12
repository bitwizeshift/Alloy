////////////////////////////////////////////////////////////////////////////////
/// \file option_set.hpp
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

#ifndef ALLOY_CORE_UTILITIES_OPTION_SET_HPP
#define ALLOY_CORE_UTILITIES_OPTION_SET_HPP

#include "alloy/core/utilities/enum_traits.hpp"

#include <bitset>  // std::bitset
#include <utility> // std::index_sequence

namespace alloy::core {

  //============================================================================
  // class : option_set
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief This class is a wrapper around enums to be used as a set of options
  ///
  /// \tparam Enum the enum type to store in this set
  //////////////////////////////////////////////////////////////////////////////
  template <typename Enum>
  class option_set
  {
    static_assert(std::is_enum_v<Enum>);
    static_assert(
      enum_is_contiguous_v<Enum>,
      "option_set only works for enums that define contiguous entries"
    );

    //--------------------------------------------------------------------------
    // Constructors / assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this container without any enums set
    option_set() = default;

    /// \brief Constructs this container with only the specified \p enums set
    ///
    /// \param enums the enumerators to set
    template <typename...Enums,
              typename = std::enable_if_t<std::conjunction_v<std::is_same<Enums,Enum>...>>>
    constexpr option_set(Enums...enums) noexcept;

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other set to copy
    option_set(const option_set& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Copies the contents of \p other
    ///
    /// \param other the other set to copy
    /// \return reference to `(*this)`
    auto operator=(const option_set& other) -> option_set& = default;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Tests if the enum value \p e is set
    ///
    /// \param e the enum to test
    /// \return `true` if `e` is set
    auto test(Enum e) const -> bool;

    /// \brief Tests if all enums are set
    ///
    /// \return `true` if all enums are set
    auto all() const noexcept -> bool;

    /// \brief Tests if any enums are set
    ///
    /// \return `true` if any enums are set
    auto any() const noexcept -> bool;

    /// \brief Tests if no enums are set
    ///
    /// \return `true` if no enums are set
    auto none() const noexcept -> bool;

    /// \brief Counts the number of set enums
    ///
    /// \return the number of set enums
    auto count() const noexcept -> std::size_t;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of bits in this set
    ///
    /// \return the number of bits
    constexpr auto size() const noexcept -> std::size_t;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    auto operator&=(const option_set& other) noexcept -> option_set&;
    auto operator^=(const option_set& other) noexcept -> option_set&;
    auto operator|=(const option_set& other) noexcept -> option_set&;
    auto operator~() const noexcept -> option_set;

    //--------------------------------------------------------------------------

    /// \brief Sets all enums in this set
    ///
    /// \return reference to `(*this)`
    auto set() noexcept -> option_set&;

    /// \brief Sets the enum \p e with the specified value \p value
    ///
    /// \param e the enum
    /// \param value the value
    /// \return reference to `(*this)`
    auto set(Enum e, bool value = true) -> option_set&;

    /// \brief Unsets all bits
    ///
    /// \return reference to `(*this)`
    auto reset() noexcept -> option_set&;

    /// \brief Unsets the specified enum \p p
    ///
    /// \param e the enum
    /// \return reference to `(*this)`
    auto reset(Enum e) -> option_set&;

    /// \brief Flips all enums
    ///
    /// \return reference to `(*this)`
    auto flip() noexcept -> option_set&;

    /// \brief Flips the specified enum \p e
    ///
    /// \param e the enum
    /// \return referenec to `(*this)`
    auto flip(Enum e) -> option_set&;

    //--------------------------------------------------------------------------
    // Conversions
    //--------------------------------------------------------------------------
  public:

    auto to_ulong() const -> unsigned long;
    auto to_ullong() const -> unsigned long long;

    //--------------------------------------------------------------------------
    // Equality
    //--------------------------------------------------------------------------
  public:

    auto operator==(const option_set& rhs) const noexcept -> bool;
    auto operator!=(const option_set& rhs) const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    inline static constexpr auto bits = distinct_enumerators_v<Enum>;

    std::bitset<bits> m_bitset;
  };

  //============================================================================
  // non-member functions : class : option_set
  //============================================================================

  //----------------------------------------------------------------------------
  // Bitwise Operators
  //----------------------------------------------------------------------------

  template <typename Enum>
  auto operator&(const option_set<Enum>& lhs, const option_set<Enum>& rhs)
    noexcept -> option_set<Enum>;
  template <typename Enum>
  auto operator|(const option_set<Enum>& lhs, const option_set<Enum>& rhs)
    noexcept -> option_set<Enum>;
  template <typename Enum>
  auto operator^(const option_set<Enum>& lhs, const option_set<Enum>& rhs)
    noexcept -> option_set<Enum>;

} // namespace alloy::core

//==============================================================================
// class : option_set
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename Enum>
template <typename...Enums, typename>
inline constexpr
alloy::core::option_set<Enum>::option_set(Enums...enums)
  noexcept
  : m_bitset{((1u << static_cast<std::size_t>(enums)) | ...)}
{
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::test(Enum e)
  const -> bool
{
  return m_bitset.test(static_cast<std::size_t>(e));
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::all()
  const noexcept -> bool
{
  return m_bitset.all();
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::any()
  const noexcept -> bool
{
  return m_bitset.any();
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::none()
  const noexcept -> bool
{
  return m_bitset.none();
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::count()
  const noexcept -> std::size_t
{
  return m_bitset.count();
}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

template <typename Enum>
inline constexpr
auto alloy::core::option_set<Enum>::size()
  const noexcept -> std::size_t
{
  return bits;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::operator&=(const option_set& other)
  noexcept -> option_set&
{
  m_bitset &= other.m_bitset;
  return (*this);
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::operator^=(const option_set& other)
  noexcept -> option_set&
{
  m_bitset ^= other.m_bitset;
  return (*this);
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::operator|=(const option_set& other)
  noexcept -> option_set&
{
  m_bitset |= other.m_bitset;
  return (*this);
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::operator~()
  const noexcept -> option_set
{
  ~m_bitset;
  return (*this);
}

//------------------------------------------------------------------------------

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::set()
  noexcept -> option_set&
{
  m_bitset.set();
  return (*this);
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::set(Enum e, bool value)
  -> option_set&
{
  m_bitset.set(static_cast<std::size_t>(e), value);
  return (*this);
}

//------------------------------------------------------------------------------

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::reset()
  noexcept -> option_set&
{
  m_bitset.reset();
  return (*this);
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::reset(Enum e)
  -> option_set&
{
  m_bitset.reset(static_cast<std::size_t>(e));
  return (*this);
}

//------------------------------------------------------------------------------

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::flip()
  noexcept -> option_set&
{
  m_bitset.flip();
  return (*this);
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::flip(Enum e)
  -> option_set&
{
  m_bitset.flip(static_cast<std::size_t>(e));
  return (*this);
}

//------------------------------------------------------------------------------
// Equality Operators
//------------------------------------------------------------------------------

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::operator==(const option_set& rhs)
  const noexcept -> bool
{
  return m_bitset == rhs.m_bitset;
}

template <typename Enum>
inline
auto alloy::core::option_set<Enum>::operator!=(const option_set& rhs)
  const noexcept -> bool
{
  return m_bitset != rhs.m_bitset;
}

//==============================================================================
// non-member functions : class : option_set
//==============================================================================

//------------------------------------------------------------------------------
// Bitwise Operators
//------------------------------------------------------------------------------

template <typename Enum>
inline
auto alloy::core::operator&(const option_set<Enum>& lhs, const option_set<Enum>& rhs)
  noexcept -> option_set<Enum>
{
  auto copy = lhs;
  copy &= rhs;
  return copy;
}

template <typename Enum>
inline
auto alloy::core::operator|(const option_set<Enum>& lhs, const option_set<Enum>& rhs)
  noexcept -> option_set<Enum>
{
  auto copy = lhs;
  copy |= rhs;
  return copy;
}

template <typename Enum>
inline
auto alloy::core::operator^(const option_set<Enum>& lhs, const option_set<Enum>& rhs)
  noexcept -> option_set<Enum>
{
  auto copy = lhs;
  copy ^= rhs;
  return copy;
}

#endif /* ALLOY_CORE_UTILITIES_OPTION_SET_HPP */
