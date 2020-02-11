/*****************************************************************************
 * \file not_null.hpp
 *
 * \brief This header defines a utility for asserting semantic correctness
 *        of non-nullable pointer types
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
#ifndef ALLOY_CORE_UTILITIES_NOT_NULL_HPP
#define ALLOY_CORE_UTILITIES_NOT_NULL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"

#include <cstddef>     // std::nullptr_t
#include <utility>     // std::forward, std::move
#include <type_traits> // std::decay_t
#include <memory>      // std::pointer_traits

namespace alloy::core {

  template <typename T>
  class not_null;

  //============================================================================
  // trait : is_not_null
  //============================================================================

  template <typename T>
  struct is_not_null : std::false_type{};
  template <typename T>
  struct is_not_null<not_null<T>> : std::true_type{};

  template <typename T>
  inline constexpr bool is_not_null_v = is_not_null<T>::value;

  //============================================================================
  // class : not_null
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A wrapper type around a pointer to disallow null assignments
  ///
  /// This type can only be used with pointers that may be understood by
  /// std::pointer_traits. This requires a type to either define 'element_type'
  /// or specialize pointer_traits for their respective needs.
  ///
  /// This type is intended primarily to be an annotation type-wrapper, so that
  /// APIs can semantically indicate their nullability requirement in a concise
  /// and coherent way.
  ///
  /// Example use:
  /// \code
  /// void post(not_null<std::unique_ptr<Task>> task)
  /// {
  ///   // pass to internal API that uses 'unique_ptr' directly
  ///   post_internal(std::move(task).as_nullable());
  /// }
  /// \endcode
  ///
  /// \tparam T the underlying pointer type
  //////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class not_null
  {
    static_assert(!std::is_reference<T>::value);
    static_assert(!std::is_void<T>::value);
    static_assert(std::is_assignable<T&, std::nullptr_t>::value);
    static_assert(!std::is_same<T,std::nullptr_t>::value);
    static_assert(!is_not_null<T>::value);

    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using element_type = typename std::pointer_traits<T>::element_type;
    using pointer      = element_type*;
    using reference    = element_type&;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    not_null() = delete;
    not_null(std::nullptr_t) = delete;

    /// \brief Constructs a not_null from a r-value of the underlying pointer
    ///
    /// \pre \p p != nullptr
    /// \param p the pointer to move
    constexpr /* IMPLICIT */ not_null(T&& p) noexcept;

    /// \brief Constructs a not_null from a const l-value of the underlying pointer
    ///
    /// \pre \p p != nullptr
    /// \param p the pointer to copy
    constexpr /* IMPLICIT */ not_null(const T& p) noexcept;

    /// \brief Constructs a not_null from any type convertible to the pointer
    ///        type
    ///
    /// \pre \p p != nullptr
    /// \param p the type to instantiate the not_null from
    template<typename U,
             typename = std::enable_if_t<(std::is_convertible_v<U, T> && !is_not_null_v<U>)>>
    constexpr /* IMPLICIT */ not_null(U&& p) noexcept;

    /// \brief Move-converts a not_null from another one of covariant type
    ///
    /// \post \p other is left in a valid, but unspecified state.
    /// \param other the other not_null to move
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr not_null(not_null<U>&& other) noexcept;

    /// \brief Copy-converts a not_null from another one of covariant type
    ///
    /// \param other the other not_null to copy
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr not_null(const not_null<U>& other) noexcept;

    /// \brief Constructs the underlying pointer type in place with \p args
    ///
    /// \pre The construction of T(args...) != nullptr
    /// \param args the arguments to forward to the underlying pointer's
    ///             constructor
    template<typename...Args,
             typename = std::enable_if_t<std::is_constructible<T,Args...>::value>>
    constexpr not_null(std::in_place_t, Args&&...args)
      noexcept(std::is_nothrow_constructible<T,Args...>::value);

    /// \brief Move-constructs this not_null from an existing one
    ///
    /// \post \p other is left in a valid, but unspecified state.
    /// \param other the other not_null to move
    not_null(not_null&& other) = default;

    /// \brief Copy-constructs this not_null from an existing one
    ///
    /// \param other the other not_null to copy
    not_null(const not_null& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns \p p to the underlying pointer to move
    ///
    /// \pre \p p != nullptr
    /// \param p the pointer to move
    not_null& operator=(T&& p) noexcept;

    /// \brief Assigns \p p to the underlying pointer to copy
    ///
    /// \pre \p p != nullptr
    /// \param p the pointer to move
    not_null& operator=(const T& p) noexcept;

    /// \brief Assigns \p other to the underlying pointer
    ///
    /// \pre \p other != nullptr
    /// \param other the other pointer to assign
    /// \return reference to \c (*this)
    template<typename U,
             typename = std::enable_if_t<(std::is_convertible_v<U, T> && !is_not_null_v<U>)>>
    not_null& operator=(U&& other) noexcept;

    /// \brief Move-assigns the contents from \p other to the underlying pointer
    ///
    /// \post \p other is left in a valid, but unspecified state.
    /// \param other the other pointer to move
    /// \return reference to \c (*this)
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    not_null& operator=(not_null<U>&& other) noexcept;

    /// \brief Copy-assigns the contents from \p other to the underlying pointer
    ///
    /// \param other the other pointer to copy
    /// \return reference to \c (*this)
    template<typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    not_null& operator=(const not_null<U>& other) noexcept;

    /// \brief Move-assigns the contents from \p other to the underlying pointer
    ///
    /// \post \p other is left in a valid, but unspecified state.
    /// \param other the other pointer to move
    /// \return reference to \c (*this)
    not_null& operator=(not_null&& other) noexcept = default;

    /// \brief Copy-assigns the contents from \p other to the underlying pointer
    ///
    /// \param other the other pointer to copy
    /// \return reference to \c (*this)
    not_null& operator=(const not_null& other) noexcept = default;

    not_null& operator=(std::nullptr_t) = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying pointer
    ///
    /// \return the underlying pointer
    constexpr pointer get() const noexcept;

    /// \brief Contextually convertible to bool
    ///
    /// This is always true
    constexpr explicit operator bool() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Extracts the underlying nullable pointer from not_null
    ///
    /// \return the underlying nullable pointer
    constexpr const T& as_nullable() const & noexcept;

    /// \brief Extracts the underlying nullable pointer from not_null
    ///
    /// \note This overload allows for stealing the underlying nullable pointer.
    ///       In doing so, the invariant of this class may intentionally be
    ///       violated (subject to the underlying pointer).
    ///       Care needs to be taken to avoid potentially reusing this not_null
    ///       after moving.
    ///
    /// \return the underlying nullable pointer
    constexpr T&& as_nullable() && noexcept;

    //--------------------------------------------------------------------------

    /// \brief Dereferences the underlying pointer
    ///
    /// \return the underlying pointer
    constexpr pointer operator->() const noexcept;

    /// \brief Dereferences the underlying pointer
    ///
    /// \return reference to the underlying pointer
    constexpr reference operator*() const noexcept;

    //--------------------------------------------------------------------------
    // Deleted Operators
    //--------------------------------------------------------------------------

    not_null& operator++() = delete;
    not_null& operator--() = delete;
    not_null operator++(int) = delete;
    not_null operator--(int) = delete;
    not_null& operator+=(std::ptrdiff_t) = delete;
    not_null& operator-=(std::ptrdiff_t) = delete;
    void operator[](std::ptrdiff_t) const = delete;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    T m_pointer;

    // Recursively call 'operator->()' to eventually get a pointer
    template <typename Ptr>
    static constexpr auto to_address(const Ptr& p) noexcept;
    template <typename U>
    static constexpr U* to_address(U* p) noexcept;
  };

  //============================================================================
  // non-member functions : class : not_null
  //============================================================================

  //----------------------------------------------------------------------------
  // Utilities
  //----------------------------------------------------------------------------

  template<typename T>
  constexpr not_null<std::decay_t<T>> make_not_null(T&& t) noexcept;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  template <typename T>
  constexpr bool operator==(const not_null<T>& lhs, std::nullptr_t) noexcept;
  template <typename T>
  constexpr bool operator==(std::nullptr_t, const not_null<T>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator==(const not_null<T>& lhs, const not_null<U>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator==(const not_null<T>& lhs, const U& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator==(const T& lhs, const not_null<U>& rhs) noexcept;

  template <typename T>
  constexpr bool operator!=(const not_null<T>& lhs, std::nullptr_t) noexcept;
  template <typename T>
  constexpr bool operator!=(std::nullptr_t, const not_null<T>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator!=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator!=(const not_null<T>& lhs, const U& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator!=(const T& lhs, const not_null<U>& rhs) noexcept;

  template <typename T, typename U>
  constexpr bool operator<(const not_null<T>& lhs, const not_null<U>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator<(const not_null<T>& lhs, const U& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator<(const T& lhs, const not_null<U>& rhs) noexcept;

  template <typename T, typename U>
  constexpr bool operator>(const not_null<T>& lhs, const not_null<U>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator>(const not_null<T>& lhs, const U& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator>(const T& lhs, const not_null<U>& rhs) noexcept;

  template <typename T, typename U>
  constexpr bool operator<=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator<=(const not_null<T>& lhs, const U& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator<=(const T& lhs, const not_null<U>& rhs) noexcept;

  template <typename T, typename U>
  constexpr bool operator>=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator>=(const not_null<T>& lhs, const U& rhs) noexcept;
  template <typename T, typename U>
  constexpr bool operator>=(const T& lhs, const not_null<U>& rhs) noexcept;

} // namespace alloy::core

//==============================================================================
// definitions : class : not_null
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template<typename T>
inline constexpr alloy::core::not_null<T>::not_null(T&& p)
  noexcept
  : m_pointer{std::move(p)}
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

template<typename T>
inline constexpr alloy::core::not_null<T>::not_null(const T& p)
  noexcept
  : m_pointer{p}
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

template<typename T>
template<typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(U&& u)
  noexcept
  : m_pointer{std::forward<U>(u)}
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

template<typename T>
template<typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(not_null<U>&& other)
  noexcept
  : m_pointer{std::move(other).as_nullable()}
{
  // No assertion; moves of not_null are assumed to uphold invariant
}

template<typename T>
template<typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(const not_null<U>& other)
  noexcept
  : m_pointer{other.as_nullable()}
{
  // No assertion; copies of not_null are assumed to uphold invariants
}

template<typename T>
template<typename...Args, typename>
inline constexpr alloy::core::not_null<T>::not_null(std::in_place_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<T,Args...>::value)
  : m_pointer(std::forward<Args>(args)...)
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

//------------------------------------------------------------------------------

template<typename T>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(T&& p)
  noexcept
{
  m_pointer = std::move(p);

  ALLOY_ASSERT(m_pointer != nullptr);
  return (*this);
}

template<typename T>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(const T& p)
  noexcept
{
  m_pointer = p;

  ALLOY_ASSERT(m_pointer != nullptr);
  return (*this);
}

template<typename T>
template<typename U, typename>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(U&& other)
  noexcept
{
  m_pointer = std::forward<U>(other);

  ALLOY_ASSERT(m_pointer != nullptr);
  return (*this);
}

template<typename T>
template<typename U, typename>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(not_null<U>&& other)
  noexcept
{
  m_pointer = std::move(other).as_nullable();

  // No assertion; moves of not_null are assumed to uphold invariant
  return (*this);
}

template<typename T>
template<typename U, typename>
inline alloy::core::not_null<T>&
  alloy::core::not_null<T>::operator=(const not_null<U>& other)
  noexcept
{
  m_pointer = other.as_nullable();

  // No assertion; copies of not_null are assumed to uphold invariant
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::not_null<T>::pointer
  alloy::core::not_null<T>::get()
  const noexcept
{
  return to_address(m_pointer);
}

template <typename T>
inline constexpr alloy::core::not_null<T>::operator bool()
  const noexcept
{
  return true;
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr const T& alloy::core::not_null<T>::as_nullable()
  const & noexcept
{
  return m_pointer;
}

template<typename T>
inline constexpr T&& alloy::core::not_null<T>::as_nullable()
  && noexcept
{
  return std::move(m_pointer);
}

//------------------------------------------------------------------------------

template<typename T>
inline constexpr typename alloy::core::not_null<T>::pointer
  alloy::core::not_null<T>::operator->()
  const noexcept
{
  return get();
}

template<typename T>
inline constexpr typename alloy::core::not_null<T>::reference
  alloy::core::not_null<T>::operator*()
  const noexcept
{
  return *get();
}

template <typename T>
template <typename Ptr>
constexpr auto alloy::core::not_null<T>::to_address(const Ptr& p) noexcept
{
  return to_address(p.operator->());
}

template <typename T>
template <typename U>
constexpr U* alloy::core::not_null<T>::to_address(U* p)
  noexcept
{
  return p;
}

//==============================================================================
// non-member functions : class : not_null
//==============================================================================

template<typename T>
inline constexpr alloy::core::not_null<std::decay_t<T>>
  alloy::core::make_not_null(T&& t)
  noexcept
{
  return not_null<std::decay_t<T>>{std::forward<T>(t)};
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template <typename T>
inline constexpr bool
  alloy::core::operator==(const not_null<T>& lhs, std::nullptr_t)
  noexcept
{
  compiler::unused(lhs);
  return false;
}

template <typename T>
inline constexpr bool
  alloy::core::operator==(std::nullptr_t, const not_null<T>& rhs)
  noexcept
{
  compiler::unused(rhs);
  return false;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator==(const not_null<T>& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs.as_nullable() == rhs.as_nullable();
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator==(const not_null<T>& lhs, const U& rhs)
  noexcept
{
  return lhs.as_nullable() == rhs;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator==(const T& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs == rhs.as_nullable();
}


template <typename T>
inline constexpr bool
  alloy::core::operator!=(const not_null<T>& lhs, std::nullptr_t)
  noexcept
{
  compiler::unused(lhs);
  return true;
}

template <typename T>
inline constexpr bool
  alloy::core::operator!=(std::nullptr_t, const not_null<T>& rhs)
  noexcept
{
  compiler::unused(rhs);
  return true;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator!=(const not_null<T>& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs.as_nullable() != rhs.as_nullable();
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator!=(const not_null<T>& lhs, const U& rhs)
  noexcept
{
  return rhs.as_nullable() != rhs;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator!=(const T& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs != rhs.as_nullable();
}


template <typename T, typename U>
inline constexpr bool
  alloy::core::operator<(const not_null<T>& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs.as_nullable() < rhs.as_nullable();
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator<(const not_null<T>& lhs, const U& rhs)
  noexcept
{
  return lhs.as_nullable() < rhs;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator<(const T& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs < rhs.as_nullable();
}


template <typename T, typename U>
inline constexpr bool
  alloy::core::operator>(const not_null<T>& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs.as_nullable() > rhs.as_nullable();
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator>(const not_null<T>& lhs, const U& rhs)
  noexcept
{
  return lhs.as_nullable() > rhs;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator>(const T& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs > rhs.as_nullable();
}


template <typename T, typename U>
inline constexpr bool
  alloy::core::operator<=(const not_null<T>& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs.as_nullable() <= rhs.as_nullable();
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator<=(const not_null<T>& lhs, const U& rhs)
  noexcept
{
  return lhs.as_nullable() <= rhs;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator<=(const T& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs <= rhs.as_nullable();
}


template <typename T, typename U>
inline constexpr bool
  alloy::core::operator>=(const not_null<T>& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs.as_nullable() >= rhs.as_nullable();
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator>=(const not_null<T>& lhs, const U& rhs)
  noexcept
{
  return lhs.as_nullable() >= rhs;
}

template <typename T, typename U>
inline constexpr bool
  alloy::core::operator>=(const T& lhs, const not_null<U>& rhs)
  noexcept
{
  return lhs >= rhs.as_nullable();
}

#endif /* ALLOY_CORE_UTILITIES_NOT_NULL_HPP */