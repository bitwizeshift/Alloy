// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file delegate.hpp
 *
 * \brief This header defines a utility for managing function dispatch
 *****************************************************************************/

#ifndef ALLOY_CORE_UTILITIES_DELEGATE_HPP
#define ALLOY_CORE_UTILITIES_DELEGATE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/assert.hpp" // ALLOY_ASSERT
#include "alloy/core/config.hpp" // ALLOY_CORE_EXCEPTIONS_ENABLE
#include "alloy/core/traits/function_traits.hpp" // function_traits
#include "alloy/core/traits/is_function_pointer.hpp"

#include <concepts>
#include <type_traits> // std::true_type
#include <functional>  // std::invoke
#include <cstddef>     // std::nullptr_t

#if ALLOY_CORE_EXCEPTIONS_ENABLED
# include <exception> // std::exception
#endif

namespace alloy::core {

  //===========================================================================
  // class : bad_delegate_call
  //===========================================================================

#if ALLOY_CORE_EXCEPTIONS_ENABLED

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An exception thrown when a delegate is invoked without a bound
  ///        function
  /////////////////////////////////////////////////////////////////////////////
  class bad_delegate_call final : public std::exception
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    bad_delegate_call() noexcept = default;
    bad_delegate_call(const bad_delegate_call&) noexcept = default;
    bad_delegate_call(bad_delegate_call&&) noexcept = default;

    auto operator=(const bad_delegate_call&) -> bad_delegate_call& = default;
    auto operator=(bad_delegate_call&&) -> bad_delegate_call& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    auto what() const noexcept -> const char* override;

  };

#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

  //===========================================================================
  // class : delegate
  //===========================================================================

  template <typename Fn>
  class delegate;

  namespace detail {

    /// \brief True when `F` is invocable with `Ts...` and the result is
    ///        convertible to `R` (or `R` is `void`)
    template <typename R, typename F, typename...Ts>
    concept invocable_returns =
      std::invocable<F, Ts...> &&
      (std::is_void_v<R> ||
       std::convertible_to<std::invoke_result_t<F, Ts...>, R>);

  } // namespace detail

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A class for makeing light-weight non-owning functions
  ///
  /// \tparam R the result type
  /// \tparam Args the argument types
  /////////////////////////////////////////////////////////////////////////////
  template <typename R, typename...Args>
  class delegate<R(Args...)>
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using result_type = R;

    //-------------------------------------------------------------------------
    // Static Factories
    //-------------------------------------------------------------------------

    /// \brief Binds a non-member function pointer to this delegate
    ///
    /// \tparam Fn the function pointer to make
    /// \return the bound delegate
    template <auto Fn>
      requires (alloy::core::detail::invocable_returns<R, decltype(Fn),Args...>)
    static constexpr auto make() noexcept -> delegate;

    /// \brief Binds a member function pointer to this delegate
    ///
    /// \tparam MemberFn the member function pointer to make
    /// \return the bound delegate
    template <auto MemberFn, typename C>
      requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),C*,Args...>)
    static constexpr auto make(C* c) noexcept -> delegate;

    /// \brief Binds a const member function pointer to this delegate
    ///
    /// \tparam MemberFn the const member function pointer to make
    /// \return the bound delegate
    template <auto MemberFn, typename C>
      requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),const C*,Args...>)
    static constexpr auto make(const C* c) noexcept -> delegate;

    /// \{
    /// \brief Binds a callable to this delegate
    ///
    /// \param callable the callable to make
    /// \return the bound delegate
    template <typename Callable>
      requires (alloy::core::detail::invocable_returns<R, Callable&,Args...>)
    static constexpr auto make(Callable* callable) noexcept -> delegate;
    template <typename Callable>
      requires (alloy::core::detail::invocable_returns<R, const Callable&,Args...>)
    static constexpr auto make(const Callable* callable) noexcept -> delegate;
    /// \}

    // Disallow makeing nulls
    template <auto MemberFn>
    static auto make(std::nullptr_t) -> delegate = delete;
    static auto make(std::nullptr_t) -> delegate = delete;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a delegate that does not have a bound function
    constexpr delegate() noexcept;
    constexpr delegate(const delegate&) noexcept = default;
    constexpr delegate(delegate&&) noexcept = default;

    //-------------------------------------------------------------------------

    auto operator=(const delegate&) noexcept -> delegate& = default;
    auto operator=(delegate&&) noexcept -> delegate& = default;

    //-------------------------------------------------------------------------

    /// \brief Binds a non-member function pointer to this delegate
    ///
    /// \tparam Fn the function pointer to bind
    template <auto Fn>
      requires (alloy::core::detail::invocable_returns<R, decltype(Fn),Args...>)
    auto bind() noexcept -> void;

    /// \brief Binds a member function pointer to this delegate
    ///
    /// \tparam MemberFn the member function pointer to bind
    template <auto MemberFn, typename C>
      requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),C*,Args...>)
    auto bind(C* c) noexcept -> void;

    /// \brief Binds a const member function pointer to this delegate
    ///
    /// \tparam MemberFn the const member function pointer to bind
    template <auto MemberFn, typename C>
      requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),const C*,Args...>)
    auto bind(const C* c) noexcept -> void;

    /// \{
    /// \brief Binds a callable to this delegate
    ///
    /// \param callable the callable to bind
    template <typename Callable>
      requires (alloy::core::detail::invocable_returns<R, Callable&,Args...>)
    auto bind(Callable* callable) noexcept -> void;
    template <typename Callable>
      requires (alloy::core::detail::invocable_returns<R, const Callable&,Args...>)
    auto bind(const Callable* callable) noexcept -> void;
    /// \}

    // Disallow binding nulls
    template <auto MemberFn>
    auto bind(std::nullptr_t) -> void = delete;
    auto bind(std::nullptr_t) -> void = delete;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Unbinds any bound function to this delegate
    auto reset() noexcept -> void;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    // Some versions of clang using '-Wdocumentation' seem to not understand
    // that the following template, with R = void, still has correct
    // documentation.
    ALLOY_COMPILER_CLANG_DIAGNOSTIC_PUSH()
    ALLOY_COMPILER_CLANG_DIAGNOSTIC_IGNORE(-Wdocumentation)

    /// \brief Invokes the underlying delegate with the specified \p args
    ///
    /// \param args the arguments
    /// \return the result of the function
    template <typename...UArgs>
      requires (std::invocable<R(*)(Args...), UArgs...>)
    R operator()(UArgs&&...args) const;

    ALLOY_COMPILER_CLANG_DIAGNOSTIC_POP()

    /// \brief Returns `true` if this delegate contains a function
    constexpr explicit operator bool() const noexcept;

    //-------------------------------------------------------------------------
    // Private Member Types
    //-------------------------------------------------------------------------
  private:

    using function_type = R(*)(const void*, Args...);

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    constexpr delegate(const void* instance, function_type function) noexcept;

    //-------------------------------------------------------------------------
    // Private Member Types
    //-------------------------------------------------------------------------
  private:

    const void*   m_instance;
    function_type m_function;

    //-------------------------------------------------------------------------
    // Private Function Stubs
    //-------------------------------------------------------------------------
  private:

    /// \brief The default function bound to the delegate
    ///
    /// If exceptions are enabled, this will always throw an exception. If they
    /// are disabled, this will call the assertion handler.
    [[noreturn]]
    static auto default_stub(const void* p, Args... args) -> R;

    template <auto Fn>
    static auto function_stub(const void* p, Args... args) -> R;

    template <auto MemberFn, typename C>
    static auto member_function_stub(const void* p, Args...args) -> R;

    template <auto MemberFn, typename C>
    static auto const_member_function_stub(const void* p, Args...args) -> R;

    template <typename Callable>
    static auto callable_stub(const void* p, Args...args) -> R;

    template <typename Callable>
    static auto const_callable_stub(const void* p, Args...args) -> R;

    //-------------------------------------------------------------------------

    template <typename R2, typename...Args2>
    friend constexpr auto operator==(
      const delegate<R2(Args2...)>&,
      const delegate<R2(Args2...)>&
    ) noexcept -> bool;
  };

  //===========================================================================
  // non-member functions : class : delegateR(Args...)>
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  template <typename R, typename...Args>
  constexpr auto operator==(
    const delegate<R(Args...)>& lhs,
    const delegate<R(Args...)>& rhs
  ) noexcept -> bool;
  template <typename R, typename...Args>
  constexpr auto operator!=(
    const delegate<R(Args...)>& lhs,
    const delegate<R(Args...)>& rhs
  ) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utility
  //---------------------------------------------------------------------------

  namespace detail {

    template <auto Fn>
    struct enable_make_delegate_function : is_function_pointer<decltype(Fn)>{};

    template <auto MemberFn, typename C, typename = void>
    struct enable_make_delegate_member : std::false_type{};

    template <auto MemberFn, typename C>
    struct enable_make_delegate_member<MemberFn,C,std::void_t<
      decltype(delegate<typename function_traits<MemberFn>::signature_type>::template make<MemberFn>(std::declval<C*>()))
    >> : std::true_type{};
  }

  /// \brief Makes a delegate from a function pointer
  ///
  /// \return a delegate from a function pointer
  template <auto Fn>
    requires (alloy::core::detail::enable_make_delegate_function<Fn>::value)
  constexpr auto make_delegate()
    noexcept
    -> delegate<typename alloy::core::function_traits<Fn>::signature_type>;

  /// \{
  /// \brief Makes a delegate from a member function pointer and a bound class
  ///
  /// \return a delegate from a function pointer
  template <auto MemberFn, typename C>
    requires (alloy::core::detail::enable_make_delegate_member<MemberFn,C>::value)
  constexpr auto make_delegate(C* c)
    noexcept -> delegate<typename function_traits<MemberFn>::signature_type>;
  template <auto MemberFn, typename C>
    requires (alloy::core::detail::enable_make_delegate_member<MemberFn,const C>::value)
  constexpr auto make_delegate(const C* c)
    noexcept -> delegate<typename function_traits<MemberFn>::signature_type>;
  /// \}
  template <auto MemberFn>
  auto make_delegate(std::nullptr_t) -> void = delete;

} // namespace alloy::core


#if ALLOY_CORE_EXCEPTIONS_ENABLED

//==============================================================================
// inline definitions : class : bad_delegate_call
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline
auto alloy::core::bad_delegate_call::what() const noexcept -> const char*
{
  return "bad_delegate_call";
}

#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

//=============================================================================
// inline definitions : class : delegate
//=============================================================================

namespace alloy::core::detail {

  template <typename T>
  using delegate_forward_result_t = std::conditional_t<
    std::is_reference<T>::value, T, std::add_rvalue_reference_t<T>
  >;

  // A small utility for forwarding class template arguments in a variadic
  // member function.
  //
  // This allow by-value move-only types to be propagates to the underlying
  // function (e.g. for if std::unique_ptr is a parameter).
  //
  // * l-value references are passed-through as l-value references
  // * r-value references are passed-through as r-value references
  // * By-value arguments are std::moved to r-value references
  template <typename T, typename U>
  constexpr auto delegate_forward(U&& u) -> delegate_forward_result_t<T>
  {
    return static_cast<delegate_forward_result_t<T>>(u);
  }

} // namespace alloy::core::detail

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn>
  requires (alloy::core::detail::invocable_returns<R, decltype(Fn),Args...>)
inline constexpr
auto alloy::core::delegate<R(Args...)>::make() noexcept -> delegate<R(Args...)>
{
  const auto p  = static_cast<const void*>(nullptr);
  const auto fn = &function_stub<Fn>;

  return delegate{p, fn};
}


template <typename R, typename...Args>
template <auto MemberFn, typename C>
  requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),C*,Args...>)
inline constexpr
auto alloy::core::delegate<R(Args...)>::make(C* c)
  noexcept -> delegate<R(Args...)>
{
  ALLOY_ASSERT(c != nullptr);

  const auto p  = static_cast<const void*>(c);
  const auto fn = &member_function_stub<MemberFn, C>;

  return delegate{p, fn};
}


template <typename R, typename...Args>
template <auto MemberFn, typename C>
  requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),const C*,Args...>)
inline constexpr
auto alloy::core::delegate<R(Args...)>::make(const C* c)
  noexcept -> delegate<R(Args...)>
{
  ALLOY_ASSERT(c != nullptr);

  const auto p  = static_cast<const void*>(c);
  const auto fn = &const_member_function_stub<MemberFn, C>;

  return delegate{p, fn};
}


template <typename R, typename...Args>
template <typename Callable>
  requires (alloy::core::detail::invocable_returns<R, Callable&,Args...>)
inline constexpr
auto alloy::core::delegate<R(Args...)>::make(Callable* callable)
  noexcept -> delegate<R(Args...)>
{
  ALLOY_ASSERT(callable != nullptr);

  const auto ptr = static_cast<const void*>(callable);
  const auto fn  = &callable_stub<Callable>;

  return delegate{ptr, static_cast<function_type>(fn)};
}


template <typename R, typename...Args>
template <typename Callable>
  requires (alloy::core::detail::invocable_returns<R, const Callable&,Args...>)
inline constexpr
auto alloy::core::delegate<R(Args...)>::make(const Callable* callable)
  noexcept -> delegate<R(Args...)>
{
  ALLOY_ASSERT(callable != nullptr);

  const auto p  = static_cast<const void*>(callable);
  const auto fn = &const_callable_stub<Callable>;

  return delegate{p, fn};
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline constexpr
alloy::core::delegate<R(Args...)>::delegate() noexcept
  : delegate{nullptr, &default_stub}
{

}

//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn>
  requires (alloy::core::detail::invocable_returns<R, decltype(Fn),Args...>)
inline
auto alloy::core::delegate<R(Args...)>::bind() noexcept -> void
{
  (*this) = make<Fn>();
}


template <typename R, typename...Args>
template <auto MemberFn, typename C>
  requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),C*,Args...>)
inline
auto alloy::core::delegate<R(Args...)>::bind(C* c) noexcept -> void
{
  (*this) = make<MemberFn>(c);
}


template <typename R, typename...Args>
template <auto MemberFn, typename C>
  requires (alloy::core::detail::invocable_returns<R, decltype(MemberFn),const C*,Args...>)
inline
auto alloy::core::delegate<R(Args...)>::bind(const C* c) noexcept -> void
{
  (*this) = make<MemberFn>(c);
}


template <typename R, typename...Args>
template <typename Callable>
  requires (alloy::core::detail::invocable_returns<R, Callable&,Args...>)
inline
auto alloy::core::delegate<R(Args...)>::bind(Callable* callable)
  noexcept -> void
{
  (*this) = make(callable);
}


template <typename R, typename...Args>
template <typename Callable>
  requires (alloy::core::detail::invocable_returns<R, const Callable&,Args...>)
inline
auto alloy::core::delegate<R(Args...)>::bind(const Callable* callable)
  noexcept -> void
{
  (*this) = make(callable);
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline
auto alloy::core::delegate<R(Args...)>::reset() noexcept -> void
{
  m_function = &default_stub;
  m_instance = nullptr;
}

//-----------------------------------------------------------------------------
// Observer
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <typename...UArgs>
  requires (std::invocable<R(*)(Args...), UArgs...>)
inline
auto alloy::core::delegate<R(Args...)>::operator()(UArgs&&...args) const -> R
{
  return m_function(m_instance, std::forward<UArgs>(args)...);
}


template <typename R, typename...Args>
inline constexpr
alloy::core::delegate<R(Args...)>::operator bool() const noexcept
{
  return m_function != &default_stub;
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline constexpr
alloy::core::delegate<R(Args...)>::delegate(
  const void* instance,
  function_type function
) noexcept
  : m_instance{instance},
    m_function{function}
{

}

//-----------------------------------------------------------------------------
// Private Static Functions
//-----------------------------------------------------------------------------

ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_MSVC_DIAGNOSTIC_IGNORE(4646) // Ignore '[[noreturn]]' on non-void

template <typename R, typename...Args>
inline R alloy::core::delegate<R(Args...)>::default_stub(const void* p, Args...args)
{
  compiler::unused(p, args...);
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  throw bad_delegate_call{};
#else
  ALLOY_ASSERT(false, "no function bound to delegate");
  std::terminate();
#endif
}

ALLOY_COMPILER_DIAGNOSTIC_POP()

template <typename R, typename...Args>
template <auto Fn>
inline R
  alloy::core::delegate<R(Args...)>::function_stub(const void* p, Args...args)
{
  compiler::unused(p);

  if constexpr (std::is_void<R>::value) {
    std::invoke(Fn, detail::delegate_forward<Args>(args)...);
  } else {
    return std::invoke(Fn, detail::delegate_forward<Args>(args)...);
  }
}

template <typename R, typename...Args>
template <auto MemberFn, typename C>
inline
auto alloy::core::delegate<R(Args...)>::member_function_stub(
  const void* p,
  Args...args
) -> R
{
  auto* pc = static_cast<C*>(const_cast<void*>(compiler::assume_not_null(p)));

  if constexpr (std::is_void<R>::value) {
    std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
  } else {
    return std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
  }
}


template <typename R, typename...Args>
template <auto MemberFn, typename C>
inline
auto alloy::core::delegate<R(Args...)>::const_member_function_stub(
  const void* p,
  Args... args
) -> R
{
  const auto* pc = static_cast<const C*>(compiler::assume_not_null(p));

  if constexpr (std::is_void<R>::value) {
    std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
  } else {
    return std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
  }
}

template <typename R, typename...Args>
template <typename Callable>
inline
auto alloy::core::delegate<R(Args...)>::callable_stub(
  const void* p,
  Args...args
) -> R
{
  auto* pc = static_cast<Callable*>(const_cast<void*>(compiler::assume_not_null(p)));

  if constexpr (std::is_void<R>::value) {
    std::invoke(*pc, detail::delegate_forward<Args>(args)...);
  } else {
    return std::invoke(*pc, detail::delegate_forward<Args>(args)...);
  }
}

template <typename R, typename...Args>
template <typename Callable>
inline
auto alloy::core::delegate<R(Args...)>::const_callable_stub(
  const void* p,
  Args...args
) -> R
{
  auto* pc = static_cast<const Callable*>(compiler::assume_not_null(p));

  if constexpr (std::is_void<R>::value) {
    std::invoke(*pc, detail::delegate_forward<Args>(args)...);
  } else {
    return std::invoke(*pc, detail::delegate_forward<Args>(args)...);
  }

}


//=============================================================================
// inline definitions : non-member functions
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline constexpr
auto alloy::core::operator==(
  const delegate<R(Args...)>& lhs,
  const delegate<R(Args...)>& rhs
) noexcept -> bool
{
  return lhs.m_instance == rhs.m_instance &&
         lhs.m_function == rhs.m_function;
}

template <typename R, typename...Args>
inline constexpr
auto alloy::core::operator!=(
  const delegate<R(Args...)>& lhs,
  const delegate<R(Args...)>& rhs
) noexcept -> bool
{
  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <auto Fn>
  requires (alloy::core::detail::enable_make_delegate_function<Fn>::value)
inline constexpr
auto alloy::core::make_delegate()
  noexcept -> delegate<typename function_traits<Fn>::signature_type>
{
  using delegate_type = delegate<typename function_traits<Fn>::signature_type>;

  return delegate_type::template make<Fn>();
}

template <auto MemberFn, typename C>
  requires (alloy::core::detail::enable_make_delegate_member<MemberFn,C>::value)
inline constexpr
auto alloy::core::make_delegate(C* c)
  noexcept -> delegate<typename function_traits<MemberFn>::signature_type>
{
  using delegate_type = delegate<typename function_traits<MemberFn>::signature_type>;

  return delegate_type::template make<MemberFn>(c);
}

template <auto MemberFn, typename C>
  requires (alloy::core::detail::enable_make_delegate_member<MemberFn,const C>::value)
inline constexpr
auto alloy::core::make_delegate(const C* c)
  noexcept -> delegate<typename function_traits<MemberFn>::signature_type>
{
  using delegate_type = delegate<typename function_traits<MemberFn>::signature_type>;

  return delegate_type::template make<MemberFn>(c);
}

#endif /* ALLOY_CORE_UTILITIES_DELEGATE_HPP */

