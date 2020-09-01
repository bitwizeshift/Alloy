/*****************************************************************************
 * \file expected.hpp
 *
 * \brief This header defines an error-handling utility to avoid the need for
 *        exceptions
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
#ifndef ALLOY_CORE_UTILITIES_EXPECTED_HPP
#define ALLOY_CORE_UTILITIES_EXPECTED_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp" // ALLOY_ASSERT
#include "alloy/core/config.hpp" // ALLOY_CORE_EXCEPTIONS_ENABLED

#include <type_traits> // std::true_type
#include <variant>     // std::variant
#include <utility>     // std::in_place
#include <functional>  // std::reference_wrapper
#include <optional>    // std::optional
#include <initializer_list> // std::initializer_list
#include <system_error>     // std::error_code

#if ALLOY_CORE_EXCEPTIONS_ENABLED
# include <exception> // std::exception
#endif

namespace alloy::core {

  //============================================================================
  // forward declarations
  //============================================================================

  template <typename E>
  class unexpected;
  template <typename T, typename E>
  class expected;

  //============================================================================
  // traits
  //============================================================================

  template <typename T>
  struct is_unexpected : std::false_type{};
  template <typename E>
  struct is_unexpected<unexpected<E>> : std::true_type{};

  template <typename T>
  struct is_expected : std::false_type{};
  template <typename T, typename E>
  struct is_expected<expected<T,E>> : std::true_type{};

  //============================================================================
  // class : unexpected<E>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A semantic type used for distinguishing unexpected values in an
  ///        API that returns expected types
  ///
  /// E must be non-throwing, and both copyable and moveable.
  ///
  /// \tparam E the error type
  //////////////////////////////////////////////////////////////////////////////
  template <typename E>
  class unexpected
  {
    static_assert(!is_expected<std::decay_t<E>>::value);
    static_assert(!is_unexpected<std::decay_t<E>>::value);
    static_assert(!std::is_reference<E>::value);
    static_assert(std::is_nothrow_copy_constructible<E>::value);
    static_assert(std::is_nothrow_move_constructible<E>::value);
    static_assert(std::is_nothrow_copy_assignable<E>::value);
    static_assert(std::is_nothrow_move_assignable<E>::value);

    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using error_type = E;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    unexpected() = delete;

    /// \brief Constructs an unexpected from the given error
    ///
    /// \param error the error to create an unexpected from
    constexpr explicit unexpected(E error) noexcept;

    /// \brief Constructs this unexpected by moving the contents of an existing
    ///        one
    ///
    /// \param other the other unexpected to move
    constexpr unexpected(unexpected&& other) noexcept = default;

    /// \brief Constructs this unexpected by copying the contents of an existing
    ///        one
    ///
    /// \param other the other unexpected to copy
    constexpr unexpected(const unexpected& other) noexcept = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this by move-assignment
    ///
    /// \param other the other unexpected to move
    /// \return reference to \c (*this)
    unexpected& operator=(unexpected&& other) noexcept = default;

    /// \brief Assigns the contents of \p other to this by copy-assignment
    ///
    /// \param other the other unexpected to copy
    /// \return reference to \c (*this)
    unexpected& operator=(const unexpected& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the underlying error
    ///
    /// \return the underlying error
    constexpr E& error() noexcept;
    constexpr const E& error() const noexcept;
    /// \}

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    E m_error;
  };

  template <typename E>
  unexpected(E) -> unexpected<E>;

  //============================================================================
  // class : bad_expected_access
  //============================================================================

#if ALLOY_CORE_EXCEPTIONS_ENABLED

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An exception thrown when an expected is accessed in an invalid
  ///        state
  //////////////////////////////////////////////////////////////////////////////
  class bad_expected_access : public std::exception
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs this bad_expected_access
    bad_expected_access() = default;

    /// \brief Constructs this bad_expected_access by moving the contents of
    ///        \p other
    ///
    /// \param other the other bad_expected_access to move
    bad_expected_access(bad_expected_access&& other) = default;

    /// \brief Constructs this bad_expected_access by copying the contents of
    ///        \p other
    ///
    /// \param other the other bad_expected_access to copy
    bad_expected_access(const bad_expected_access& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other by move-assignment
    ///
    /// \param other the other exception to move
    /// \return reference to \c (*this)
    bad_expected_access& operator=(bad_expected_access&& other) = default;

    /// \brief Assigns the contents of \p other by copy-assignment
    ///
    /// \param other the other exception to copy
    /// \return reference to \c (*this)
    bad_expected_access& operator=(const bad_expected_access& other) = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the error message
    ///
    /// \return the underlying exception message
    const char* what() const noexcept override;
  };

#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

  //============================================================================
  // class : expected<T,E>
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A semantic type for handling error conditions on an API.
  ///
  /// This type is intended to be orthogonal to exception handling by providing
  /// an alternative way of managing errors in a non-throwing manner. This type
  /// allows for APIs to explicitly inform the consumer of the fallability of
  /// a function, as well as the possible reason for the failure -- encoded in
  /// the E argument.
  ///
  /// This type is implicitly constructible by any type that can construct the
  /// underlying T type, or by an unexpected type that can implicitly construct
  /// the underlying E type.
  ///
  /// This allows for fluid APIs where the return type is expected<T,E>, but
  /// the code is simply authored as:
  ///
  /// \code
  /// if (succes) return foo;
  /// else return unexpected(foo_error::some_error_code);
  /// \endcode
  ///
  /// T may be any (possibly cv-qualified) value type, an lvalue-reference, or
  /// a (non-cv-qualified) void.
  ///
  /// E must be non-throwing, and both copyable and moveable.
  ///
  /// Neither T nor E can bea (possibly cv-qualified) expected or unexpected
  /// type.
  ///
  /// \tparam T The expected type
  /// \tparam E The unexpected error type
  //////////////////////////////////////////////////////////////////////////////
  template <typename T, typename E = std::error_code>
  class expected
  {
    static_assert(!std::is_array<T>::value);
    static_assert(!std::is_void<std::decay_t<T>>::value);
    static_assert(!std::is_rvalue_reference<T>::value);
    static_assert(!is_expected<std::decay_t<T>>::value);
    static_assert(!is_unexpected<std::decay_t<T>>::value);

    static_assert(!is_expected<E>::value);
    static_assert(!is_unexpected<E>::value);
    static_assert(!std::is_reference<E>::value);
    static_assert(std::is_nothrow_copy_constructible<E>::value);
    static_assert(std::is_nothrow_move_constructible<E>::value);
    static_assert(std::is_nothrow_copy_assignable<E>::value);
    static_assert(std::is_nothrow_move_assignable<E>::value);

    //--------------------------------------------------------------------------
    // Public Member Type
    //--------------------------------------------------------------------------
  public:

    using expected_type = T;
    using error_type = E;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs the underlying value by default-construction by
    ///        calling T's constructor
    constexpr expected() = default;

    /// \brief Constructs the underlying value of this expected by calling
    ///        T's constructor with the given args
    ///
    /// \param args the arguments to forward to T's constructor
    template <typename...Args>
    constexpr expected(std::in_place_t, Args&&...args)
      noexcept(std::is_nothrow_constructible<T,Args...>::value);

    /// \brief Constructs the underlying value of this expected by calling
    ///        T's constructor with the given args
    ///
    /// \param args the arguments to forward to T's constructor
    /// \param ilist the initializer list
    template <typename...Args, typename U>
    constexpr expected(std::in_place_t,
                       std::initializer_list<U> ilist,
                       Args&&...args)
      noexcept(std::is_nothrow_constructible<T,std::initializer_list<U>,Args...>::value);

    /// \brief Constructs the underlying value of this expected by forwarding
    ///        the value of \p value to T's constructor
    ///
    /// \param value the value to use for construction
    template <typename U,
              typename = std::enable_if_t<std::is_constructible<T,U>::value>>
    constexpr /* implicit */ expected(U&& value)
      noexcept(std::is_nothrow_constructible<T,U>::value);

    /// \brief Constructs the underlying error of this expected
    ///
    /// \param e the unexpected error
    template <typename E2,
              typename = std::enable_if_t<std::is_nothrow_constructible<E,E2>::value>>
    constexpr /* implicit */ expected(unexpected<E2> e) noexcept;

    /// \brief Constructs this expected by moving the contents of \p other
    ///
    /// \param other the other expected to move
    constexpr expected(expected&& other) = default;

    /// \brief Constructs this expected by copying the contents of \p other
    ///
    /// \param other the other expected to copy
    constexpr expected(const expected& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this through move-assignment
    ///
    /// \param other the other expected to move
    /// \return reference to \c (*this)
    expected& operator=(expected&& other) = default;

    /// \brief Assigns the contents of \p other to this through copy-assignment
    ///
    /// \param other the other expected to copy
    /// \return reference to \c (*this)
    expected& operator=(const expected& other) = default;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    constexpr T* operator->() noexcept;
    constexpr const T* operator->() const noexcept;
    constexpr T& operator*() noexcept;
    constexpr const T& operator*() const noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the underlying value from this expected
    ///
    /// \return reference to the underlying value
    constexpr T& value();
    constexpr const T& value() const;
    /// \}

    /// \brief Gets the underlying error from this expected
    ///
    /// \return reference to the underlying error
    constexpr E error() const noexcept;

    /// \brief Convertable to \c true if this has a value
    constexpr explicit operator bool() const noexcept;

    /// \brief Returns whether this expected contains a value
    ///
    /// \return \c true if this contains a value
    constexpr bool has_value() const noexcept;

    /// \brief Returns whether this expected contains an error
    ///
    /// \return \c true if this contains an error
    constexpr bool has_error() const noexcept;

    //--------------------------------------------------------------------------
    // Monadic
    //--------------------------------------------------------------------------

    /// \{
    /// \brief Gets the underlying type from this expected if it contains a
    ///        value, or \p default_value otherwise
    ///
    /// \param default_value the default value to retrieve if this does not
    ///                      contain a value
    /// \return the retrieved value
    template <typename U>
    constexpr auto value_or(U&& default_value) const & -> T;
    template <typename U>
    constexpr auto value_or(U&& default_value) && -> T;
    /// \}

    /// \{
    /// \brief Invokes the function \p fn with the value of this expected as
    ///        the argument
    ///
    /// If this expected contains an error, an expected of the error is returned
    ///
    /// The function being called must return an expected type
    ///
    /// \param fn the function to invoke with this
    /// \return The result of the function being called
    template <typename Fn>
    auto flat_map(Fn&& fn) & -> std::invoke_result_t<Fn,T&>;
    template <typename Fn>
    auto flat_map(Fn&& fn) const & -> std::invoke_result_t<Fn,const T&>;
    template <typename Fn>
    auto flat_map(Fn&& fn) && -> std::invoke_result_t<Fn,T&&>;
    /// \}

    /// \{
    /// \brief Invokes the function \p fn with the value of this expected as
    ///        the argument
    ///
    /// If this expected is an error, the result of this function is that
    /// error. Otherwise this function wraps the result and returns it as an
    /// expected.
    ///
    /// \param fn the function to invoke with this
    /// \return The expected result of the function invoked
    template<typename Fn>
    auto map(Fn&& fn) & -> expected<std::invoke_result_t<Fn,T&>,E>;
    template<typename Fn>
    auto map(Fn&& fn) const & -> expected<std::invoke_result_t<Fn,const T&>,E>;
    template<typename Fn>
    auto map(Fn&& fn) && -> expected<std::invoke_result_t<Fn,T&&>,E>;
    /// \}

    /// \brief Returns \p u if this expected contains a value, the error
    ///        otherwise.
    ///
    /// \param u the value to return as an expected
    /// \return an expected of \p u if this contains a value
    template <typename U>
    auto and_then(U&& u) const -> expected<std::decay_t<U>,E>;


    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Swaps the contents of this with \p other
    ///
    /// \param other the other expected to swap contents with
    void swap(expected<T,E>& other) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using underlying_type = std::conditional_t<
      std::is_lvalue_reference<T>::value,
      std::reference_wrapper<T>,
      T
    >;

    template <typename U>
    static constexpr U& reference_to(U& ref);
    template <typename U>
    static constexpr U& reference_to(std::reference_wrapper<U> ref);

    std::variant<underlying_type,E> m_state;
  };

  //============================================================================
  // class : expected<void,E>
  //============================================================================

  template <typename E>
  class expected<void,E>
  {
    static_assert(!is_expected<E>::value);
    static_assert(!is_unexpected<E>::value);
    static_assert(!std::is_reference<E>::value);
    static_assert(std::is_nothrow_copy_constructible<E>::value);
    static_assert(std::is_nothrow_move_constructible<E>::value);
    static_assert(std::is_nothrow_copy_assignable<E>::value);
    static_assert(std::is_nothrow_move_assignable<E>::value);

    //--------------------------------------------------------------------------
    // Public Member Type
    //--------------------------------------------------------------------------
  public:

    using expected_type = void;
    using error_type = E;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this expected successfully
    constexpr expected() noexcept = default;

    /// \brief Constructs the underlying error of this expected
    ///
    /// \param e the unexpected error
    template <typename E2,
              typename = std::enable_if_t<std::is_nothrow_constructible<E,E2>::value>>
    constexpr /* implicit */ expected(unexpected<E2> e) noexcept;

    /// \brief Constructs this expected by moving the contents of \p other
    ///
    /// \param other the other expected to move
    constexpr expected(expected&& other) = default;

    /// \brief Constructs this expected by copying the contents of \p other
    ///
    /// \param other the other expected to copy
    constexpr expected(const expected& other) = default;

    //--------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this through move-assignment
    ///
    /// \param other the other expected to move
    /// \return reference to \c (*this)
    expected& operator=(expected&& other) = default;

    /// \brief Assigns the contents of \p other to this through copy-assignment
    ///
    /// \param other the other expected to copy
    /// \return reference to \c (*this)
    expected& operator=(const expected& other) = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Attempts to get the underlying value
    ///
    /// This function is here for symmetry with the non-void specialization
    void value() const;

    /// \brief Gets the underlying error from this expected
    ///
    /// \return reference to the underlying error
    constexpr E error() const noexcept;

    /// \brief Convertable to \c true if this has a value
    constexpr explicit operator bool() const noexcept;

    /// \brief Returns whether this expected contains a value
    ///
    /// \return \c true if this contains a value
    constexpr bool has_value() const noexcept;

    /// \brief Returns whether this expected contains an error
    ///
    /// \return \c true if this contains an error
    constexpr bool has_error() const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Swaps the contents of this with \p other
    ///
    /// \param other the other expected to swap contents with
    void swap(expected<void,E>& other) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    std::optional<E> m_state;
  };

  //===========================================================================
  // Non Member Functions : class : expected<T,E>
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison
  //---------------------------------------------------------------------------

  template<typename T, typename U, typename E1, typename E2>
  constexpr bool operator==(const expected<T,E1>& lhs,
                            const expected<U,E2>& rhs) noexcept;
  template<typename T, typename U, typename E1, typename E2>
  constexpr bool operator!=(const expected<T,E1>& lhs,
                            const expected<U,E2>& rhs) noexcept;
  template<typename T, typename U, typename E1, typename E2>
  constexpr bool operator<(const expected<T,E1>& lhs,
                           const expected<U,E2>& rhs) noexcept;
  template<typename T, typename U, typename E1, typename E2>
  constexpr bool operator<=(const expected<T,E1>& lhs,
                            const expected<U,E2>& rhs) noexcept;
  template<typename T, typename U, typename E1, typename E2>
  constexpr bool operator>(const expected<T,E1>& lhs,
                           const expected<U,E2>& rhs) noexcept;
  template<typename T, typename U, typename E1, typename E2>
  constexpr bool operator>=(const expected<T,E1>& lhs,
                            const expected<U,E2>& rhs) noexcept;

  //---------------------------------------------------------------------------

  template<typename T, typename U, typename E>
  constexpr bool operator==(const expected<T,E>& lhs, const U& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator==(const T& lhs, const expected<U,E>& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator!=(const expected<T,E>& lhs, const U& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator!=(const T& lhs, const expected<U,E>& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator<(const expected<T,E>& lhs, const U& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator<(const T& lhs, const expected<U,E>& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator<=(const expected<T,E>& lhs, const U& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator<=(const T& lhs, const expected<U,E>& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator>(const expected<T,E>& lhs, const U& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator>(const T& lhs, const expected<U,E>& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator>=(const expected<T,E>& lhs, const U& rhs) noexcept;
  template<typename T, typename U, typename E>
  constexpr bool operator>=(const T& lhs, const expected<U,E>& rhs) noexcept;

} // namespace alloy::core

//==============================================================================
// inline definitions : class : unexpected<E>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename E>
inline constexpr alloy::core::unexpected<E>::unexpected(E error)
  noexcept
  : m_error{std::move(error)}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename E>
inline constexpr E& alloy::core::unexpected<E>::error()
  noexcept
{
  return m_error;
}

template <typename E>
inline constexpr const E& alloy::core::unexpected<E>::error()
  const noexcept
{
  return m_error;
}

#if ALLOY_CORE_EXCEPTIONS_ENABLED

//==============================================================================
// inline definitions : class : bad_expected_access
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline const char* alloy::core::bad_expected_access::what()
  const noexcept
{
  return "bad_expected_access";
}

#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

//==============================================================================
// inline definitions : class : expected<T,E>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename T, typename E>
template <typename...Args>
inline constexpr alloy::core::expected<T,E>
  ::expected(std::in_place_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<T,Args...>::value)
  : m_state{std::in_place_type<underlying_type>, std::forward<Args>(args)...}
{

}

template <typename T, typename E>
template <typename...Args, typename U>
inline constexpr alloy::core::expected<T,E>
  ::expected(std::in_place_t, std::initializer_list<U> ilist, Args&&...args)
  noexcept(std::is_nothrow_constructible<T,std::initializer_list<U>, Args...>::value)
  : m_state{
    std::in_place_type<underlying_type>,
    ilist,
    std::forward<Args>(args)...
  }
{

}

template <typename T, typename E>
template <typename U, typename>
inline constexpr alloy::core::expected<T,E>::expected(U&& value)
  noexcept(std::is_nothrow_constructible<T,U>::value)
  : m_state{std::in_place_type<underlying_type>, std::forward<U>(value)}
{

}

template <typename T, typename E>
template <typename E2, typename>
inline constexpr alloy::core::expected<T,E>::expected(unexpected<E2> e)
  noexcept
  : m_state{std::in_place_type<E>, std::move(e.error())}
{

}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template <typename T, typename E>
inline constexpr T* alloy::core::expected<T,E>::operator->()
  noexcept
{
  return std::get_if<0>(&m_state);
}

template <typename T, typename E>
inline constexpr const T* alloy::core::expected<T,E>::operator->()
  const noexcept
{
  return std::get_if<0>(&m_state);
}

template <typename T, typename E>
inline constexpr T& alloy::core::expected<T,E>::operator*()
  noexcept
{
  return (*std::get_if<0>(&m_state));
}

template <typename T, typename E>
inline constexpr const T& alloy::core::expected<T,E>::operator*()
  const noexcept
{
  return (*std::get_if<0>(&m_state));
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename T, typename E>
inline constexpr T& alloy::core::expected<T,E>::value()
{
  auto* const p = std::get_if<0>(&m_state);
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (p == nullptr) {
    throw bad_expected_access{};
  }
#else
  ALLOY_ASSERT(p != nullptr);
#endif

  return reference_to(*p);
}

template <typename T, typename E>
inline constexpr const T& alloy::core::expected<T,E>::value()
  const
{
  auto* const p = std::get_if<0>(&m_state);
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (p == nullptr) {
    throw bad_expected_access{};
  }
#else
  ALLOY_ASSERT(p != nullptr);
#endif

  return reference_to(*p);
}

template <typename T, typename E>
inline constexpr E alloy::core::expected<T,E>::error()
  const noexcept
{
  auto* const p = std::get_if<1>(&m_state);
  if (p == nullptr) {
    return E{};
  }

  return *p;
}

template <typename T, typename E>
inline constexpr alloy::core::expected<T,E>::operator bool()
  const noexcept
{
  return has_value();
}

template <typename T, typename E>
inline constexpr bool alloy::core::expected<T,E>::has_value()
  const noexcept
{
  return std::get_if<0>(&m_state) != nullptr;
}

template <typename T, typename E>
inline constexpr bool alloy::core::expected<T,E>::has_error()
  const noexcept
{
  return !has_value();
}

//------------------------------------------------------------------------------
// Monadic
//------------------------------------------------------------------------------

template <typename T, typename E>
template <typename U>
inline constexpr
auto alloy::core::expected<T, E>::value_or(U&& default_value)
  const & -> T
{
  return has_value() ? (**this) : std::forward<U>(default_value);
}

template <typename T, typename E>
template <typename U>
inline constexpr
auto alloy::core::expected<T, E>::value_or(U&& default_value)
  && -> T
{
  return has_value() ? std::move(**this) : std::forward<U>(default_value);
}

template <typename T, typename E>
template <typename Fn>
inline
auto alloy::core::expected<T, E>::flat_map(Fn&& fn)
  & -> std::invoke_result_t<Fn, T&>
{
  if (has_value()) {
    return std::forward<Fn>(fn)(**this);
  }
  return unexpected(error());
}

template <typename T, typename E>
template <typename Fn>
inline
auto alloy::core::expected<T, E>::flat_map(Fn&& fn)
  const & -> std::invoke_result_t<Fn, const T&>
{
  if (has_value()) {
    return std::forward<Fn>(fn)(**this);
  }
  return unexpected(error());
}

template <typename T, typename E>
template <typename Fn>
inline
auto alloy::core::expected<T, E>::flat_map(Fn&& fn)
  && -> std::invoke_result_t<Fn, T&&>
{
  if (has_value()) {
    return std::forward<Fn>(fn)(std::move(**this));
  }
  return unexpected(error());
}

template <typename T, typename E>
template <typename Fn>
inline auto alloy::core::expected<T, E>::map(Fn&& fn)
  & -> expected<std::invoke_result_t<Fn, T&>,E>
{
  if (has_value()) {
    return std::forward<Fn>(fn)(**this);
  }
  return unexpected(error());
}

template <typename T, typename E>
template <typename Fn>
inline auto alloy::core::expected<T, E>::map(Fn&& fn)
  const & -> expected<std::invoke_result_t<Fn, const T&>,E>
{
  if (has_value()) {
    return std::forward<Fn>(fn)(**this);
  }
  return unexpected(error());
}

template <typename T, typename E>
template <typename Fn>
inline
auto alloy::core::expected<T, E>::map(Fn&& fn)
  && -> expected<std::invoke_result_t<Fn, T&&>,E>
{
  if (has_value()) {
    return std::forward<Fn>(fn)(std::move(**this));
  }
  return unexpected(error());
}

template <typename T, typename E>
template <typename U>
inline
auto alloy::core::expected<T, E>::and_then(U&& u)
  const -> expected<std::decay_t<U>,E>
{
  if (has_value()) {
    return std::forward<U>(u);
  }
  return unexpected{error()};
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

/// \brief Swaps the contents of this with \p other
///
/// \param other the other expected to swap contents with
template <typename T, typename E>
inline void alloy::core::expected<T,E>::swap(expected<T,E>& other)
  noexcept
{
  using std::swap;

  swap(m_state, other.m_state);
}

//------------------------------------------------------------------------------
// Private Static Functions
//------------------------------------------------------------------------------

template <typename T, typename E>
template <typename U>
inline constexpr U& alloy::core::expected<T,E>::reference_to(U& ref)
{
  return ref;
}

template <typename T, typename E>
template <typename U>
inline constexpr U&
  alloy::core::expected<T,E>::reference_to(std::reference_wrapper<U> ref)
{
  return ref.get();
}

//==============================================================================
// inline definitions : class : expected<void,E>
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

template <typename E>
template <typename E2, typename>
inline constexpr alloy::core::expected<void,E>::expected(unexpected<E2> e)
  noexcept
  : m_state{std::move(e.error())}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename E>
inline void alloy::core::expected<void,E>::value()
  const
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  if (m_state.has_value()) {
    throw bad_expected_access{};
  }
#else
  ALLOY_ASSERT(!m_state.has_value());
#endif
}

template <typename E>
inline constexpr E alloy::core::expected<void,E>::error()
  const noexcept
{
  if (!m_state.has_value()) {
    return E{};
  }

  return (*m_state);
}

template <typename E>
inline constexpr alloy::core::expected<void,E>::operator bool()
  const noexcept
{
  return has_value();
}

template <typename E>
inline constexpr bool alloy::core::expected<void,E>::has_value()
  const noexcept
{
  return !m_state.has_value();
}

template <typename E>
inline constexpr bool alloy::core::expected<void,E>::has_error()
  const noexcept
{
  return !has_value();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename E>
void alloy::core::expected<void,E>::swap(expected<void,E>& other)
  noexcept
{
  using std::swap;

  swap(m_state,other.m_state);
}

//=============================================================================
// Non Member Functions : class : expected<T,E>
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

template<typename T, typename U, typename E1, typename E2>
inline constexpr bool
  alloy::core::operator==(const expected<T,E1>& lhs, const expected<U,E2>& rhs)
  noexcept
{
  if (lhs.has_value() != rhs.has_value()) {
    return false;
  }
  return lhs.has_value() ? ((*lhs) == (*rhs)) : lhs.error() == rhs.error();
}


template<typename T, typename U, typename E1, typename E2>
inline constexpr bool
  alloy::core::operator!=(const expected<T,E1>& lhs, const expected<U,E2>& rhs)
  noexcept
{
  if (lhs.has_value() != rhs.has_value()) {
    return true;
  }
  return lhs.has_value() ? ((*lhs) != (*rhs)) : lhs.error() != rhs.error();
}


template<typename T, typename U, typename E1, typename E2>
inline constexpr bool
  alloy::core::operator<(const expected<T,E1>& lhs, const expected<U,E2>& rhs)
  noexcept
{
  if (!rhs.has_value()) {
    return false;
  }
  if (!lhs.has_value()) {
    return true;
  }
  return (*lhs) < (*rhs);
}


template<typename T, typename U, typename E1, typename E2>
inline constexpr bool
  alloy::core::operator<=(const expected<T,E1>& lhs, const expected<U,E2>& rhs)
  noexcept
{
  if (!lhs.has_value()) {
    return true;
  }
  if (!rhs.has_value()) {
    return false;
  }
  return (*lhs) <= (*rhs);
}


template<typename T, typename U, typename E1, typename E2>
inline constexpr bool
  alloy::core::operator>(const expected<T,E1>& lhs, const expected<U,E2>& rhs)
  noexcept
{
  if (!lhs.has_value()) {
    return false;
  }
  if (!rhs.has_value()) {
    return true;
  }
  return (*lhs) > (*rhs);
}


template<typename T, typename U, typename E1, typename E2>
inline constexpr bool
  alloy::core::operator>=(const expected<T,E1>& lhs, const expected<U,E2>& rhs)
  noexcept
{
  if (!rhs.has_value()) {
    return true;
  }
  if (!lhs.has_value()) {
    return false;
  }
  return (*lhs) >= (*rhs);
}

//---------------------------------------------------------------------------

template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator==(const expected<T,E>& lhs, const U& rhs)
  noexcept
{
  return lhs.has_value() ? (*lhs) == rhs : false;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator==(const T& lhs, const expected<U,E>& rhs)
  noexcept
{
  return rhs.has_value() ? lhs == (*rhs) : false;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator!=(const expected<T,E>& lhs, const U& rhs)
  noexcept
{
  return lhs.has_value() ? (*lhs) != rhs : true;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator!=(const T& lhs, const expected<U,E>& rhs)
  noexcept
{
  return rhs.has_value() ? lhs != (*rhs) : true;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator<(const expected<T,E>& lhs, const U& rhs)
  noexcept
{
  return lhs.has_value() ? (*lhs) < rhs : true;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator<(const T& lhs, const expected<U,E>& rhs)
  noexcept
{
  return rhs.has_value() ? lhs < (*rhs) : false;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator<=(const expected<T,E>& lhs, const U& rhs)
  noexcept
{
  return lhs.has_value() ? (*lhs) <= rhs : true;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator<=(const T& lhs, const expected<U,E>& rhs)
  noexcept
{
  return rhs.has_value() ? lhs <= (*rhs) : false;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator>(const expected<T,E>& lhs, const U& rhs)
  noexcept
{
  return lhs.has_value() ? (*lhs) > rhs : false;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator>(const T& lhs, const expected<U,E>& rhs)
  noexcept
{
  return rhs.has_value() ? lhs > (*rhs) : true;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator>=(const expected<T,E>& lhs, const U& rhs)
  noexcept
{
  return lhs.has_value() ? (*lhs) >= rhs : false;
}


template<typename T, typename U, typename E>
inline constexpr bool
  alloy::core::operator>=(const T& lhs, const expected<U,E>& rhs)
  noexcept
{
  return rhs.has_value() ? lhs >= (*rhs) : true;
}

#endif /* ALLOY_CORE_UTILITIES_EXPECTED_HPP */
