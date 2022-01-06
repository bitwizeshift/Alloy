/*****************************************************************************
 * \file not_null.hpp
 *
 * \brief This header defines a utility for asserting semantic correctness
 *        of non-nullable pointer types
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019-2020, 2022 Matthew Rodusek All rights reserved.

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

  namespace detail {
    ////////////////////////////////////////////////////////////////////////////
    /// \brief A private type that exists to construct no_null's using the
    ///        private constructor (through friendship)
    ////////////////////////////////////////////////////////////////////////////
    struct not_null_factory
    {
      template <typename T>
      static constexpr auto make(T&& p) -> not_null<std::decay_t<T>>;
    };
  } // namespace detail

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
  template <typename T>
  class not_null
  {
    static_assert(
      !std::is_reference<T>::value,
      "not_null<T&> is ill-formed. "
      "not_null may only work with non-CV qualified value types."
    );
    static_assert(
      std::is_assignable<T&, std::nullptr_t>::value &&
      std::is_constructible<T, std::nullptr_t>::value,
      "T must satisfy NullablePointer, otherwise not_null<T> is ill-formed."
    );
    static_assert(
      !std::is_same<T,std::nullptr_t>::value,
      "not_null<std::nullptr_t> is ill-formed. I mean, really, come on."
    );
    static_assert(
      !is_not_null<T>::value,
      "not_null<not_null<T>> is ill-formed."
    );
    static_assert(
      !std::is_const<T>::value && !std::is_volatile<T>::value,
      "not_null<[const] [volatile] T> is ill-formed. "
      "not_null may only work with non-CV qualified value types."
    );

    template <typename U>
    using enable_if_convertible = std::enable_if_t<
      std::conjunction<
        std::negation<is_not_null<std::decay_t<U>>>,
        std::negation<std::is_null_pointer<std::decay_t<U>>>,
        std::negation<std::is_same<std::decay_t<U>,T>>,
        std::is_convertible<U,T>
      >::value
    >;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using element_type = typename std::pointer_traits<T>::element_type;
    using pointer      = element_type*;

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
    constexpr /* IMPLICIT */ not_null(T&& p)
      noexcept(std::is_nothrow_move_constructible<T>::value);

    /// \brief Constructs a not_null from a const l-value of the underlying pointer
    ///
    /// \pre \p p != nullptr
    /// \param p the pointer to copy
    constexpr /* IMPLICIT */ not_null(const T& p)
      noexcept(std::is_nothrow_copy_constructible<T>::value);

    /// \brief Constructs a not_null from any type convertible to the pointer
    ///        type
    ///
    /// \pre \p p != nullptr
    /// \param p the type to instantiate the not_null from
    template <typename U,
             typename = enable_if_convertible<U>>
    constexpr /* IMPLICIT */ not_null(U&& p)
      noexcept(std::is_nothrow_constructible<T, U>::value);

    /// \brief Move-converts a not_null from another one of covariant type
    ///
    /// \post \p other is left in a valid, but unspecified state.
    /// \param other the other not_null to move
    template <typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr not_null(not_null<U>&& other)
      noexcept(std::is_nothrow_constructible<T, U&&>::value);

    /// \brief Copy-converts a not_null from another one of covariant type
    ///
    /// \param other the other not_null to copy
    template <typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    constexpr not_null(const not_null<U>& other)
      noexcept(std::is_nothrow_constructible<T,const U&>::value);

    /// \brief Constructs the underlying pointer type in place with \p args
    ///
    /// \pre The construction of T(args...) != nullptr
    /// \param args the arguments to forward to the underlying pointer's
    ///             constructor
    template <typename...Args,
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
    auto operator=(T&& p) noexcept(std::is_nothrow_move_assignable<T>::value)
      -> not_null&;

    /// \brief Assigns \p p to the underlying pointer to copy
    ///
    /// \pre \p p != nullptr
    /// \param p the pointer to move
    auto operator=(const T& p) noexcept(std::is_nothrow_copy_assignable<T>::value)
        -> not_null&;

    /// \brief Assigns \p other to the underlying pointer
    ///
    /// \pre \p other != nullptr
    /// \param other the other pointer to assign
    /// \return reference to \c (*this)
    template <typename U, typename = enable_if_convertible<U>>
    auto operator=(U&& other) noexcept(std::is_nothrow_assignable<T, U>::value)
      -> not_null&;

    /// \brief Move-assigns the contents from \p other to the underlying pointer
    ///
    /// \post \p other is left in a valid, but unspecified state.
    /// \param other the other pointer to move
    /// \return reference to \c (*this)
    template <typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    auto operator=(not_null<U>&& other) noexcept(
      std::is_nothrow_assignable<T, U&&>::value) -> not_null&;

    /// \brief Copy-assigns the contents from \p other to the underlying pointer
    ///
    /// \param other the other pointer to copy
    /// \return reference to \c (*this)
    template <typename U,
             typename = std::enable_if_t<std::is_convertible_v<U, T>>>
    auto operator=(const not_null<U>& other) noexcept(
      std::is_nothrow_assignable<T, const U&>::value) -> not_null&;

    /// \brief Move-assigns the contents from \p other to the underlying pointer
    ///
    /// \post \p other is left in a valid, but unspecified state.
    /// \param other the other pointer to move
    /// \return reference to \c (*this)
    auto operator=(not_null&& other) -> not_null& = default;

    /// \brief Copy-assigns the contents from \p other to the underlying pointer
    ///
    /// \param other the other pointer to copy
    /// \return reference to \c (*this)
    auto operator=(const not_null& other) -> not_null& = default;

    auto operator=(std::nullptr_t) -> not_null& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying pointer
    ///
    /// \return the underlying pointer
    constexpr auto get() const noexcept -> pointer;

    /// \brief Contextually convertible to bool
    ///
    /// This is always true
    constexpr explicit operator bool() const noexcept;

    //--------------------------------------------------------------------------

    /// \brief Extracts the underlying nullable pointer from not_null
    ///
    /// \return the underlying nullable pointer
    constexpr auto as_nullable() const& noexcept -> const T&;

    /// \brief Extracts the underlying nullable pointer from not_null
    ///
    /// \note This overload allows for stealing the underlying nullable pointer.
    ///       In doing so, the invariant of this class may intentionally be
    ///       violated (subject to the underlying pointer).
    ///       Care needs to be taken to avoid potentially reusing this not_null
    ///       after moving.
    ///
    /// \return the underlying nullable pointer
    constexpr auto as_nullable() && noexcept -> T&&;

    //--------------------------------------------------------------------------

    /// \brief Dereferences the underlying pointer
    ///
    /// \return the underlying pointer
    constexpr auto operator->()
      const noexcept -> pointer;

    /// \brief Dereferences the underlying pointer
    ///
    /// \return reference to the underlying pointer
    constexpr auto operator*()
      const noexcept -> std::add_lvalue_reference_t<element_type>;

    //--------------------------------------------------------------------------
    // Deleted Operators
    //--------------------------------------------------------------------------

    auto operator++() -> not_null& = delete;
    auto operator--() -> not_null& = delete;
    auto operator++(int) -> not_null = delete;
    auto operator--(int) -> not_null = delete;
    auto operator+=(std::ptrdiff_t) -> not_null& = delete;
    auto operator-=(std::ptrdiff_t) -> not_null& = delete;
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
    static constexpr auto to_address(U* p) noexcept -> U*;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    struct ctor_tag{};

    template <typename P>
    constexpr not_null(ctor_tag, P&& ptr)
      noexcept(std::is_nothrow_constructible<std::decay_t<P>, P>::value);

    friend detail::not_null_factory;
  };

  //===========================================================================
  // non-member functions : class : not_null
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Creates a `not_null` object by checking that `ptr` is not null
  ///        first
  ///
  /// `check_not_null` contains the overhead of checking for null first, but
  /// is opt-in. If a type is known to never be null, consider `assume_not_null`
  /// below.
  ///
  /// ### Examples
  ///
  /// Basic use:
  ///
  /// ```cpp
  /// // Adapting legacy API
  /// auto consume_impl(not_null<std::unique_ptr<Widget>>) -> void;
  ///
  /// auto consume(std::unique_ptr<Widget> p) -> void
  /// {
  ///     // Expect this invariant in our code; crash if not.
  ///     consume_impl(check_not_null(std::move(p));
  /// }
  ///
  /// ```
  ///
  /// \throw not_null_contract_violation if `ptr == nullptr`
  /// \param ptr the pointer to check for nullability first
  /// \return a `not_null` object containing `ptr`
  template <typename T>
  constexpr auto check_not_null(T&& ptr)
    -> not_null<std::decay_t<T>>;

  /// \brief Creates a `not_null` object by *assuming* that `ptr` is not null
  ///
  /// Since this function does no proper checking, it is up to the user to
  /// guarantee that `ptr` does not violate the invariant. If the invariant is
  /// *not* upheld, the user may experience **undefined behavior** due to
  /// potential null pointer dereferences, and due to other code assuming nulls
  /// can never happen.
  ///
  /// This function should only be used in cases where it can be guaranteed that
  /// `ptr` can never be null, such as for an object's invariant, or when
  /// using `not_null` with already known non-null objects.
  ///
  /// ### Examples
  ///
  /// Basic use:
  ///
  /// ```cpp
  /// auto x = 5;
  /// auto nn = assume_not_null(&x); // we know 'x' cannot be null
  ///
  /// assert(nn == &x);
  /// ```
  ///
  /// or
  ///
  /// ```cpp
  /// // 'make_shared' never returns null
  /// auto p = assume_not_null(
  ///   std::make_shared<int>(42)
  /// );
  ///
  /// consume_not_null(std::move(p));
  /// ```
  ///
  /// \param ptr the pointer that cannot be null
  /// \return a not_null containing the pointer
  template <typename T>
  constexpr auto assume_not_null(T&& ptr)
    noexcept(std::is_nothrow_constructible<typename std::decay<T>::type,T>::value)
    -> not_null<std::decay_t<T>>;

  template <typename T>
  [[deprecated]]
  constexpr auto make_not_null(T&& t) noexcept(
    std::is_nothrow_constructible<std::decay_t<T>, T>::value)
    -> not_null<std::decay_t<T>>;

  //----------------------------------------------------------------------------
  // Comparisons
  //----------------------------------------------------------------------------

  template <typename T>
  constexpr auto operator==(const not_null<T>& lhs, std::nullptr_t) noexcept -> bool;
  template <typename T>
  constexpr auto operator==(std::nullptr_t, const not_null<T>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() == std::declval<const U&>())>
  constexpr auto operator==(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() == std::declval<const U&>())>
  constexpr auto operator==(const not_null<T>& lhs, const U& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() == std::declval<const U&>())>
  constexpr auto operator==(const T& lhs, const not_null<U>& rhs) noexcept -> bool;

  template <typename T>
  constexpr auto operator!=(const not_null<T>& lhs, std::nullptr_t) noexcept -> bool;
  template <typename T>
  constexpr auto operator!=(std::nullptr_t, const not_null<T>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() != std::declval<const U&>())>
  constexpr auto operator!=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() != std::declval<const U&>())>
  constexpr auto operator!=(const not_null<T>& lhs, const U& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() != std::declval<const U&>())>
  constexpr auto operator!=(const T& lhs, const not_null<U>& rhs) noexcept -> bool;

  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() < std::declval<const U&>())>
  constexpr auto operator<(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() < std::declval<const U&>())>
  constexpr auto operator<(const not_null<T>& lhs, const U& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() < std::declval<const U&>())>
  constexpr auto operator<(const T& lhs, const not_null<U>& rhs) noexcept -> bool;

  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() > std::declval<const U&>())>
  constexpr auto operator>(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() > std::declval<const U&>())>
  constexpr auto operator>(const not_null<T>& lhs, const U& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() > std::declval<const U&>())>
  constexpr auto operator>(const T& lhs, const not_null<U>& rhs) noexcept -> bool;

  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() <= std::declval<const U&>())>
  constexpr auto operator<=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() <= std::declval<const U&>())>
  constexpr auto operator<=(const not_null<T>& lhs, const U& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() <= std::declval<const U&>())>
  constexpr auto operator<=(const T& lhs, const not_null<U>& rhs) noexcept -> bool;

  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() >= std::declval<const U&>())>
  constexpr auto operator>=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() >= std::declval<const U&>())>
  constexpr auto operator>=(const not_null<T>& lhs, const U& rhs) noexcept -> bool;
  template <typename T, typename U,
            typename = decltype(std::declval<const T&>() >= std::declval<const U&>())>
  constexpr auto operator>=(const T& lhs, const not_null<U>& rhs) noexcept -> bool;

} // namespace alloy::core

template <typename T>
inline constexpr
auto alloy::core::detail::not_null_factory::make(T&& p)
  -> not_null<std::decay_t<T>>
{
  using value_type = std::decay_t<T>;

  return not_null<value_type>{
    typename not_null<value_type>::ctor_tag{},
    std::forward<T>(p)
  };
}

//==============================================================================
// definitions : class : not_null
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename T>
inline constexpr alloy::core::not_null<T>::not_null(T&& p)
  noexcept(std::is_nothrow_move_constructible<T>::value)
  : m_pointer{std::move(p)}
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

template <typename T>
inline constexpr alloy::core::not_null<T>::not_null(const T& p)
  noexcept(std::is_nothrow_copy_constructible<T>::value)
  : m_pointer{p}
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

template <typename T>
template <typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(U&& u)
  noexcept(std::is_nothrow_constructible<T,U>::value)
  : m_pointer{std::forward<U>(u)}
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

template <typename T>
template <typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(not_null<U>&& other)
  noexcept(std::is_nothrow_constructible<T,U&&>::value)
  : m_pointer{std::move(other).as_nullable()}
{
  // No assertion; moves of not_null are assumed to uphold invariant
}

template <typename T>
template <typename U, typename>
inline constexpr alloy::core::not_null<T>::not_null(const not_null<U>& other)
  noexcept(std::is_nothrow_constructible<T,const U&>::value)
  : m_pointer{other.as_nullable()}
{
  // No assertion; copies of not_null are assumed to uphold invariants
}

template <typename T>
template <typename...Args, typename>
inline constexpr alloy::core::not_null<T>::not_null(std::in_place_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<T,Args...>::value)
  : m_pointer(std::forward<Args>(args)...)
{
  ALLOY_ASSERT(m_pointer != nullptr);
}

//------------------------------------------------------------------------------

template <typename T>
inline auto alloy::core::not_null<T>::operator=(T&& p)
  noexcept(std::is_nothrow_move_assignable<T>::value) -> not_null<T>&
{
  m_pointer = std::move(p);

  ALLOY_ASSERT(m_pointer != nullptr);
  return (*this);
}

template <typename T>
inline auto alloy::core::not_null<T>::operator=(const T& p)
  noexcept(std::is_nothrow_copy_assignable<T>::value) -> not_null<T>&
{
  m_pointer = p;

  ALLOY_ASSERT(m_pointer != nullptr);
  return (*this);
}

template <typename T>
template <typename U, typename>
inline auto alloy::core::not_null<T>::operator=(U&& other)
  noexcept(std::is_nothrow_assignable<T, U>::value) -> not_null<T>&
{
  m_pointer = std::forward<U>(other);

  ALLOY_ASSERT(m_pointer != nullptr);
  return (*this);
}

template <typename T>
template <typename U, typename>
inline auto alloy::core::not_null<T>::operator=(not_null<U>&& other)
  noexcept(std::is_nothrow_assignable<T, U&&>::value) -> not_null<T>&
{
  m_pointer = std::move(other).as_nullable();

  // No assertion; moves of not_null are assumed to uphold invariant
  return (*this);
}

template <typename T>
template <typename U, typename>
inline auto alloy::core::not_null<T>::operator=(const not_null<U>& other)
  noexcept(std::is_nothrow_assignable<T, const U&>::value) -> not_null<T>&
{
  m_pointer = other.as_nullable();

  // No assertion; copies of not_null are assumed to uphold invariant
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::not_null<T>::get()
  const noexcept -> pointer
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

template <typename T>
inline constexpr
auto alloy::core::not_null<T>::as_nullable()
  const & noexcept -> const T&
{
  return m_pointer;
}

template <typename T>
inline constexpr
auto alloy::core::not_null<T>::as_nullable()
  && noexcept -> T&&
{
  return std::move(m_pointer);
}

//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::not_null<T>::operator->()
  const noexcept -> pointer
{
  return get();
}

template <typename T>
inline constexpr
auto alloy::core::not_null<T>::operator*()
  const noexcept -> std::add_lvalue_reference_t<element_type>
{
  return *get();
}

template <typename T>
template <typename Ptr>
inline constexpr
auto alloy::core::not_null<T>::to_address(const Ptr& p)
  noexcept
{
  return to_address(p.operator->());
}

template <typename T>
template <typename U>
inline constexpr
auto alloy::core::not_null<T>::to_address(U* p)
  noexcept -> U*
{
  return p;
}


//-----------------------------------------------------------------------------
// Private Constructor
//-----------------------------------------------------------------------------

template <typename T>
template <typename P>
inline constexpr
alloy::core::not_null<T>::not_null(ctor_tag, P&& ptr)
  noexcept(std::is_nothrow_constructible<std::decay_t<P>, P>::value)
  : m_pointer{std::forward<P>(ptr)}
{

}

//==============================================================================
// non-member functions : class : not_null
//==============================================================================

template <typename T>
inline constexpr
auto alloy::core::check_not_null(T&& ptr)
  -> not_null<std::decay_t<T>>
{
  // TODO: throw contract violation exception ?
  ALLOY_ASSERT(ptr != nullptr);
  return assume_not_null(std::forward<T>(ptr));
}

template <typename T>
inline constexpr
auto alloy::core::assume_not_null(T&& ptr)
  noexcept(std::is_nothrow_constructible<std::decay_t<T>,T>::value)
  -> not_null<std::decay_t<T>>
{
  return detail::not_null_factory::make(std::forward<T>(ptr));
}

template <typename T>
inline constexpr
auto alloy::core::make_not_null(T&& t) noexcept(
  std::is_nothrow_constructible<std::decay_t<T>, T>::value)
  -> not_null<std::decay_t<T>>
{
  return not_null<std::decay_t<T>>{std::forward<T>(t)};
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

template <typename T>
inline constexpr
auto alloy::core::operator==(const not_null<T>& lhs, std::nullptr_t) noexcept
  -> bool
{
  compiler::unused(lhs);
  return false;
}

template <typename T>
inline constexpr
auto alloy::core::operator==(std::nullptr_t, const not_null<T>& rhs) noexcept
  -> bool
{
  compiler::unused(rhs);
  return false;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator==(const not_null<T>& lhs,
                          const not_null<U>& rhs) noexcept -> bool
{
  return lhs.as_nullable() == rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator==(const not_null<T>& lhs, const U& rhs) noexcept -> bool
{
  return lhs.as_nullable() == rhs;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator==(const T& lhs, const not_null<U>& rhs) noexcept -> bool
{
  return lhs == rhs.as_nullable();
}

template <typename T>
inline constexpr
auto alloy::core::operator!=(const not_null<T>& lhs, std::nullptr_t) noexcept
  -> bool
{
  compiler::unused(lhs);
  return true;
}

template <typename T>
inline constexpr
auto alloy::core::operator!=(std::nullptr_t, const not_null<T>& rhs) noexcept
  -> bool
{
  compiler::unused(rhs);
  return true;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator!=(const not_null<T>& lhs,
                          const not_null<U>& rhs) noexcept -> bool
{
  return lhs.as_nullable() != rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator!=(const not_null<T>& lhs, const U& rhs) noexcept -> bool
{
  return lhs.as_nullable() != rhs;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator!=(const T& lhs, const not_null<U>& rhs) noexcept -> bool
{
  return lhs != rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator<(const not_null<T>& lhs,
                         const not_null<U>& rhs) noexcept -> bool
{
  return lhs.as_nullable() < rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator<(const not_null<T>& lhs, const U& rhs) noexcept -> bool
{
  return lhs.as_nullable() < rhs;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator<(const T& lhs, const not_null<U>& rhs) noexcept -> bool
{
  return lhs < rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator>(const not_null<T>& lhs,
                         const not_null<U>& rhs) noexcept -> bool
{
  return lhs.as_nullable() > rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator>(const not_null<T>& lhs, const U& rhs) noexcept -> bool
{
  return lhs.as_nullable() > rhs;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator>(const T& lhs, const not_null<U>& rhs) noexcept -> bool
{
  return lhs > rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator<=(const not_null<T>& lhs,
                          const not_null<U>& rhs) noexcept -> bool
{
  return lhs.as_nullable() <= rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator<=(const not_null<T>& lhs, const U& rhs) noexcept -> bool
{
  return lhs.as_nullable() <= rhs;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator<=(const T& lhs, const not_null<U>& rhs) noexcept -> bool
{
  return lhs <= rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator>=(const not_null<T>& lhs,
                          const not_null<U>& rhs) noexcept -> bool
{
  return lhs.as_nullable() >= rhs.as_nullable();
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator>=(const not_null<T>& lhs, const U& rhs) noexcept -> bool
{
  return lhs.as_nullable() >= rhs;
}

template <typename T, typename U, typename>
inline constexpr
auto alloy::core::operator>=(const T& lhs, const not_null<U>& rhs) noexcept -> bool
{
  return lhs >= rhs.as_nullable();
}

#endif /* ALLOY_CORE_UTILITIES_NOT_NULL_HPP */

