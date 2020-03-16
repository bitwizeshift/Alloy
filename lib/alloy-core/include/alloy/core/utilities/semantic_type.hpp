/*****************************************************************************
 * \file semantic_type.hpp
 *
 * \brief This header contains the definition of 'semantic_type', used for
 *        creating semantic wrappers that are incomparable with the underlying
 *        types
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
#ifndef ALLOY_CORE_UTILITIES_BOXED_TYPE_HPP
#define ALLOY_CORE_UTILITIES_BOXED_TYPE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <type_traits> // std::true_type, std::false_type
#include <utility>     // std::forward

namespace alloy::core {

  //===========================================================================
  // forward declarations
  //===========================================================================

  template <typename, typename>
  class semantic_type;

  //===========================================================================
  // trait : is_semantic_type
  //===========================================================================

  template <typename T>
  struct is_semantic_type : std::false_type{};

  template <typename T, typename Tag>
  struct is_semantic_type<semantic_type<T,Tag>> : std::true_type{};

  template <typename T>
  constexpr bool is_semantic_type_v = is_semantic_type<T>::value;

  //===========================================================================
  // class : is_semantic_type
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A semantic type used for adding type-safety around underlying types
  ///
  /// This class wraps underlying types and propagates their operators to
  /// allow simple access, while also preventing incomparable assignments from
  /// underlying types.
  ///
  /// The intended purpose of this is to create unique semantic types from
  /// underlying base types, such as wrapping up a 'string_view' or integral
  /// values into something with a stronger meaning. It requires explicit
  /// construction from the underlying type, giving better readability at the
  /// site of construction.
  ///
  /// For example:
  /// \code
  ///
  /// using percent = semantic_type<int,struct percent_tag>;
  /// auto p = percent{100};
  /// // p += 1; -- doesn't work!
  /// p += percent{1}; // works! 0 overhead.
  /// \endcode
  ///
  /// This is recommended to be used on any standard types like strings/ints
  /// to ensure that their context doesn't get misused. It can even be used
  /// granularly to prevent bugs at compile-time, for example:
  ///
  /// \code
  /// using hp_type = semantic_type<int,struct hp_tag>;
  /// using mp_type = semantic_type<int,struct mp_tag>;
  /// struct stat_component
  /// {
  ///   hp_type hp;
  ///   mp_type mp;
  /// };
  ///
  /// auto s = stat_component{100,50};
  /// // s.hp += s.mp; -- illegal! source of bug
  /// s.hp += semantic_cast<hp_type>(s.mp); // forced-conversion -- context is!
  ///                                       // intentional at call-site
  /// \endcode
  ///
  /// \tparam T The underlying type
  /// \tparam Tag A tag for uniqueness
  /////////////////////////////////////////////////////////////////////////////
  template <typename T, typename Tag>
  class semantic_type
  {
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_pointer_v<T>);
    static_assert(!std::is_void_v<T>);
    static_assert(!std::is_abstract_v<T>);
    static_assert(!std::is_const_v<T>);
    static_assert(!std::is_volatile_v<T>);
    static_assert(!is_semantic_type_v<T>);

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using const_reference = std::add_const_t<T>&;
    using const_pointer = std::add_const_t<T>*;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs the underlying semantic object via default construction
    constexpr semantic_type() = default;

    /// \brief Constructs the underlying semantic object by forwarding \p u
    ///
    /// \param u the underlying value to forward to T's constructor
    template <typename U,
              typename = std::enable_if_t<std::is_constructible_v<T,U>>>
    constexpr explicit semantic_type(U&& u)
      noexcept(std::is_nothrow_constructible_v<T,U>);

    /// \brief Constructs the underlying semantic object by moving an existing one
    ///
    /// \param other the other semantic object to move
    constexpr semantic_type(semantic_type&& other) = default;

    /// \brief Constructs the underlying semantic object by copying an existing
    ///        one
    ///
    /// \param other the other semantic object to copy
    constexpr semantic_type(const semantic_type& other) = default;

    //-------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this through move assignment
    ///
    /// \param other the other semantic object to move
    /// \return reference to \c (*this)
    semantic_type& operator=(semantic_type&&) = default;

    /// \brief Assigns the contents of \p other to this through copy assignment
    ///
    /// \param other the other semantic object to copy
    /// \return reference to \c (*this)
    semantic_type& operator=(const semantic_type&) = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Unboxes the semantic object
    ///
    /// \return reference to the semantic object
    constexpr reference unbox() noexcept;
    constexpr const_reference unbox() const noexcept;
    /// \}

    //-------------------------------------------------------------------------
    // Operators
    //-------------------------------------------------------------------------
  public:

    template <typename U=T, typename=decltype(++std::declval<U&>())>
    constexpr semantic_type& operator++()
      noexcept(noexcept(++std::declval<U&>()));

    template <typename U=T, typename=decltype(std::declval<U&>()++)>
    constexpr semantic_type operator++(int)
      noexcept(noexcept(std::declval<U&>()++));

    template <typename U=T, typename=decltype(--std::declval<U&>())>
    constexpr semantic_type& operator--()
      noexcept(noexcept(--std::declval<U&>()));

    template <typename U=T, typename=decltype(std::declval<U&>()--)>
    constexpr semantic_type operator--(int)
      noexcept(noexcept(std::declval<U&>()--));

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    T m_value;
  };

  //===========================================================================
  // non-member functions : class : semantic_type
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison Operators
  //---------------------------------------------------------------------------

  // TODO(bitwizeshift): Add SFINA to all these operators

  // TODO(bitwizeshift): Add correct noexcept propagation for these operators

  template <typename T, typename U, typename Tag>
  constexpr bool operator==(const semantic_type<T,Tag>& lhs,
                            const semantic_type<U,Tag>& rhs)
    noexcept(std::declval<const T&>() == std::declval<const T&>());

  template <typename T, typename U, typename Tag>
  constexpr bool operator!=(const semantic_type<T,Tag>& lhs,
                            const semantic_type<U,Tag>& rhs)
    noexcept(std::declval<const T&>() != std::declval<const T&>());

  template <typename T, typename U, typename Tag>
  constexpr bool operator<(const semantic_type<T,Tag>& lhs,
                           const semantic_type<U,Tag>& rhs)
    noexcept(std::declval<const T&>() < std::declval<const T&>());

  template <typename T, typename U, typename Tag>
  constexpr bool operator>(const semantic_type<T,Tag>& lhs,
                           const semantic_type<U,Tag>& rhs)
    noexcept(std::declval<const T&>() > std::declval<const T&>());

  template <typename T, typename U, typename Tag>
  constexpr bool operator<=(const semantic_type<T,Tag>& lhs,
                            const semantic_type<U,Tag>& rhs)
    noexcept(std::declval<const T&>() <= std::declval<const T&>());

  template <typename T, typename U, typename Tag>
  constexpr bool operator>=(const semantic_type<T,Tag>& lhs,
                            const semantic_type<U,Tag>& rhs)
    noexcept(std::declval<const T&>() >= std::declval<const T&>());

  //---------------------------------------------------------------------------
  // Compound Assignment
  //---------------------------------------------------------------------------

  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator+=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator-=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator*=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator/=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator%=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator|=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator&=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  semantic_type<T,Tag>& operator^=(semantic_type<T,Tag>& lhs,
                                   const semantic_type<U,Tag>& rhs);

  //---------------------------------------------------------------------------
  // Arithmetic Operators
  //---------------------------------------------------------------------------

  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator+(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator-(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator*(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator/(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator%(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator|(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator&(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);
  template <typename T, typename U, typename Tag>
  constexpr semantic_type<T,Tag> operator^(const semantic_type<T,Tag>& lhs,
                                           const semantic_type<U,Tag>& rhs);

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  inline namespace casts {

    /// \brief Casts a box from one semantic type to another
    ///
    /// This is used for explicitly converting underlying semantic types from one
    /// type to another
    ///
    /// \tparam SemanticType the semantic type being cast to
    /// \param other the other type to cast from
    /// \return the new semantic type
    template <typename SemanticType, typename T, typename Tag,
              typename = std::enable_if_t<is_semantic_type_v<SemanticType>>,
              typename = decltype(SemanticType{static_cast<typename SemanticType::value_type>(std::declval<const T&>())})>
    constexpr SemanticType semantic_cast(const semantic_type<T,Tag>& other);

  } // inline namespace casts
} // namespace alloy::core

//=============================================================================
// definitions : class : semantic_type
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename T, typename Tag>
template <typename U, typename>
inline constexpr alloy::core::semantic_type<T,Tag>::semantic_type(U&& u)
  noexcept(std::is_nothrow_constructible_v<T,U>)
  : m_value{std::forward<U>(u)}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T, typename Tag>
inline constexpr typename alloy::core::semantic_type<T,Tag>::reference
  alloy::core::semantic_type<T,Tag>::unbox()
  noexcept
{
  return m_value;
}

template <typename T, typename Tag>
inline constexpr typename alloy::core::semantic_type<T,Tag>::const_reference
  alloy::core::semantic_type<T,Tag>::unbox()
  const noexcept
{
  return m_value;
}

//-----------------------------------------------------------------------------
// Operators
//-----------------------------------------------------------------------------


template <typename T, typename Tag>
template <typename U, typename>
inline constexpr alloy::core::semantic_type<T,Tag>&
  alloy::core::semantic_type<T,Tag>::operator++()
  noexcept(noexcept(++std::declval<U&>()))
{
  ++m_value;
  return (*this);
}

template <typename T, typename Tag>
template <typename U, typename>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::semantic_type<T,Tag>::operator++(int)
  noexcept(noexcept(std::declval<U&>()++))
{
  return semantic_type{m_value++};
}

template <typename T, typename Tag>
template <typename U, typename>
inline constexpr alloy::core::semantic_type<T,Tag>&
  alloy::core::semantic_type<T,Tag>::operator--()
  noexcept(noexcept(--std::declval<U&>()))
{
  --m_value;
  return (*this);
}

template <typename T, typename Tag>
template <typename U, typename>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::semantic_type<T,Tag>::operator--(int)
  noexcept(noexcept(std::declval<U&>()--))
{
  return semantic_type{m_value--};
}

//=============================================================================
// definitions : non-member functions : class : semantic_type
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison Operators
//-----------------------------------------------------------------------------

template <typename T, typename U, typename Tag>
inline constexpr bool
alloy::core::operator==(const semantic_type<T,Tag>& lhs,
                        const semantic_type<U,Tag>& rhs)
  noexcept(std::declval<const T&>() == std::declval<const T&>())
{
  return lhs.unbox() == rhs.unbox();
}

template <typename T, typename U, typename Tag>
inline constexpr bool
  alloy::core::operator!=(const semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
  noexcept(std::declval<const T&>() != std::declval<const T&>())
{
  return lhs.unbox() != rhs.unbox();
}

template <typename T, typename U, typename Tag>
inline constexpr bool
  alloy::core::operator<(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
  noexcept(std::declval<const T&>() < std::declval<const T&>())
{
  return lhs.unbox() < rhs.unbox();
}

template <typename T, typename U, typename Tag>
inline constexpr bool
  alloy::core::operator>(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
  noexcept(std::declval<const T&>() > std::declval<const T&>())
{
  return lhs.unbox() > rhs.unbox();
}

template <typename T, typename U, typename Tag>
inline constexpr bool
  alloy::core::operator<=(const semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
  noexcept(std::declval<const T&>() <= std::declval<const T&>())
{
  return lhs.unbox() <= rhs.unbox();
}

template <typename T, typename U, typename Tag>
inline constexpr bool
  alloy::core::operator>=(const semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
  noexcept(std::declval<const T&>() >= std::declval<const T&>())
{
  return lhs.unbox() >= rhs.unbox();
}

//-----------------------------------------------------------------------------
// Compound Assignment
//-----------------------------------------------------------------------------

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator+=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() += rhs.unbox();
  return lhs;
}

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator-=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() -= rhs.unbox();
  return lhs;
}

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator*=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() *= rhs.unbox();
  return lhs;
}

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator/=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() /= rhs.unbox();
  return lhs;
}

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator%=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() %= rhs.unbox();
  return lhs;
}

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator|=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() |= rhs.unbox();
  return lhs;
}

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator&=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() &= rhs.unbox();
  return lhs;
}

template <typename T, typename U, typename Tag>
inline alloy::core::semantic_type<T,Tag>&
  alloy::core::operator^=(semantic_type<T,Tag>& lhs,
                          const semantic_type<U,Tag>& rhs)
{
  lhs.unbox() ^= rhs.unbox();
  return lhs;
}

//---------------------------------------------------------------------------
// Arithmetic Operators
//---------------------------------------------------------------------------

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator+(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() + rhs.unbox()};
}

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator-(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() - rhs.unbox()};
}

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator*(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() * rhs.unbox()};
}

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator/(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() / rhs.unbox()};
}

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator%(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() % rhs.unbox()};
}

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator|(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() | rhs.unbox()};
}

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator&(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() & rhs.unbox()};
}

template <typename T, typename U, typename Tag>
inline constexpr alloy::core::semantic_type<T,Tag>
  alloy::core::operator^(const semantic_type<T,Tag>& lhs,
                         const semantic_type<U,Tag>& rhs)
{
  return semantic_type<T,Tag>{lhs.unbox() ^ rhs.unbox()};
}

//---------------------------------------------------------------------------
// Utilities
//---------------------------------------------------------------------------

template <typename SemanticType, typename T, typename Tag, typename, typename>
inline constexpr SemanticType
  alloy::core::casts::semantic_cast(const semantic_type<T,Tag>& other)
{
  using value_type = typename SemanticType::value_type;
  if constexpr(std::is_same_v<T,value_type>) {
    return SemanticType{other.get()};
  } else {
    return SemanticType{static_cast<value_type>(other.get())};
  }
}

#endif /* ALLOY_CORE_UTILITIES_BOXED_TYPE_HPP */
