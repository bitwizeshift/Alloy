/*****************************************************************************
 * \file delegate.hpp
 *
 * \brief This header defines a utility for managing function dispatch
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
#ifndef ALLOY_CORE_UTILITIES_DELEGATE_HPP
#define ALLOY_CORE_UTILITIES_DELEGATE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp" // ALLOY_ASSERT
#include "alloy/core/config.hpp" // ALLOY_CORE_EXCEPTIONS_ENABLE
#include "alloy/core/traits/function_traits.hpp" // function_traits
#include "alloy/core/traits/is_function_pointer.hpp"

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

    bad_delegate_call& operator=(const bad_delegate_call&) = default;
    bad_delegate_call& operator=(bad_delegate_call&&) = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    const char* what() const noexcept override;

  };

#endif // ALLOY_CORE_EXCEPTIONS_ENABLED

  //===========================================================================
  // class : delegate
  //===========================================================================

  template <typename Fn>
  class delegate;

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A class for makeing light-weight non-owning functions
  ///
  /// \tparam R the result type
  /// \tparam Args the argument types
  /////////////////////////////////////////////////////////////////////////////
  template <typename R, typename...Args>
  class delegate<R(Args...)>
  {
    template <typename F, typename...Ts>
    using enable_if_invocable_t = std::enable_if_t<
      std::conjunction_v<
        std::is_invocable<F, Ts...>,
        std::disjunction<
          std::is_void<R>,
          std::is_convertible<std::invoke_result_t<F, Ts...>,R>
        >
      >
    >;

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
    template <auto Fn,
              typename=enable_if_invocable_t<decltype(Fn),Args...>>
    static constexpr delegate make() noexcept;

    /// \brief Binds a member function pointer to this delegate
    ///
    /// \tparam MemberFn the member function pointer to make
    /// \return the bound delegate
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),C*,Args...>>
    static constexpr delegate make(C* c) noexcept;

    /// \brief Binds a const member function pointer to this delegate
    ///
    /// \tparam MemberFn the const member function pointer to make
    /// \return the bound delegate
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),const C*,Args...>>
    static constexpr delegate make(const C* c) noexcept;

    /// \{
    /// \brief Binds a callable to this delegate
    ///
    /// \param callable the callable to make
    /// \return the bound delegate
    template <typename Callable,
              typename=enable_if_invocable_t<Callable&,Args...>>
    static constexpr delegate make(Callable* callable) noexcept;
    template <typename Callable,
              typename=enable_if_invocable_t<const Callable&,Args...>>
    static constexpr delegate make(const Callable* callable) noexcept;
    /// \}

    // Disallow makeing nulls
    template <auto MemberFn>
    static delegate make(std::nullptr_t) = delete;
    static delegate make(std::nullptr_t) = delete;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a delegate that does not have a bound function
    constexpr delegate() noexcept;
    constexpr delegate(const delegate&) noexcept = default;
    constexpr delegate(delegate&&) noexcept = default;

    //-------------------------------------------------------------------------

    delegate& operator=(const delegate&) noexcept = default;
    delegate& operator=(delegate&&) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Binds a non-member function pointer to this delegate
    ///
    /// \tparam Fn the function pointer to bind
    template <auto Fn,
              typename=enable_if_invocable_t<decltype(Fn),Args...>>
    void bind() noexcept;

    /// \brief Binds a member function pointer to this delegate
    ///
    /// \tparam MemberFn the member function pointer to bind
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),C*,Args...>>
    void bind(C* c) noexcept;

    /// \brief Binds a const member function pointer to this delegate
    ///
    /// \tparam MemberFn the const member function pointer to bind
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),const C*,Args...>>
    void bind(const C* c) noexcept;

    /// \{
    /// \brief Binds a callable to this delegate
    ///
    /// \param callable the callable to bind
    template <typename Callable,
              typename=enable_if_invocable_t<Callable&,Args...>>
    void bind(Callable* callable) noexcept;
    template <typename Callable,
              typename=enable_if_invocable_t<const Callable&,Args...>>
    void bind(const Callable* callable) noexcept;
    /// \}

    // Disallow binding nulls
    template <auto MemberFn>
    void bind(std::nullptr_t) = delete;
    void bind(std::nullptr_t) = delete;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Unbinds any bound function to this delegate
    void reset() noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    // Some versions of clang using '-Wdocumentation' seem to not understand
    // that the following template, with R = void, still has correct
    // documentation.
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation"
#endif

    /// \brief Invokes the underlying delegate with the specified \p args
    ///
    /// \param args the arguments
    /// \return the result of the function
    template <typename...UArgs,
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>>
    R operator()(UArgs&&...args) const;

#if defined(__clang__)
# pragma clang diagnostic pop
#endif

    /// \brief Returns \c true if this delegate contains a function
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
    // Private Static Functions
    //-------------------------------------------------------------------------
  private:

    /// \brief The default function bound to the delegate
    ///
    /// If exceptions are enabled, this will always throw an exception. If they
    /// are disabled, this will call the assertion handler.
    [[noreturn]]
    static R default_function(const void* instance, Args...);

    template <typename R2, typename...Args2>
    friend constexpr bool operator==(const delegate<R2(Args2...)>&,
                                     const delegate<R2(Args2...)>&) noexcept;
  };

  //===========================================================================
  // non-member functions : class : delegateR(Args...)>
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  template <typename R, typename...Args>
  constexpr bool operator==(const delegate<R(Args...)>& lhs,
                            const delegate<R(Args...)>& rhs) noexcept;
  template <typename R, typename...Args>
  constexpr bool operator!=(const delegate<R(Args...)>& lhs,
                            const delegate<R(Args...)>& rhs) noexcept;

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
  template <auto Fn,
            typename = std::enable_if_t<detail::enable_make_delegate_function<Fn>::value>>
  constexpr delegate<typename alloy::core::function_traits<Fn>::signature_type>
    make_delegate() noexcept;

  /// \{
  /// \brief Makes a delegate from a member function pointer and a bound class
  ///
  /// \return a delegate from a function pointer
  template <auto MemberFn, typename C,
            typename = std::enable_if_t<detail::enable_make_delegate_member<MemberFn,C>::value>>
  constexpr delegate<typename alloy::core::function_traits<MemberFn>::signature_type>
    make_delegate(C* c) noexcept;
  template <auto MemberFn, typename C,
            typename = std::enable_if_t<detail::enable_make_delegate_member<MemberFn,const C>::value>>
  constexpr delegate<typename alloy::core::function_traits<MemberFn>::signature_type>
    make_delegate(const C* c) noexcept;
  /// \}
  template <auto MemberFn>
  void make_delegate(std::nullptr_t) = delete;

} // namespace alloy::core


#if ALLOY_CORE_EXCEPTIONS_ENABLED

//==============================================================================
// inline definitions : class : bad_delegate_call
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline const char* alloy::core::bad_delegate_call::what()
  const noexcept
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
  constexpr delegate_forward_result_t<T> delegate_forward(U&& u)
  {
    return static_cast<delegate_forward_result_t<T>>(u);
  }

} // namespace alloy::core::detail

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn, typename>
inline constexpr alloy::core::delegate<R(Args...)>
  alloy::core::delegate<R(Args...)>::make()
  noexcept
{
  const auto ptr = static_cast<const void*>(nullptr);
  const auto fn  = [](const void* p, Args...args) -> R
  {
    (void) p;

    if constexpr (std::is_void<R>::value) {
      std::invoke(Fn, detail::delegate_forward<Args>(args)...);
    } else {
      return std::invoke(Fn, detail::delegate_forward<Args>(args)...);
    }
  };

  return delegate{ptr, static_cast<function_type>(fn)};
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline constexpr alloy::core::delegate<R(Args...)>
  alloy::core::delegate<R(Args...)>::make(C* c)
  noexcept
{
  ALLOY_ASSERT(c != nullptr);

  const auto ptr = static_cast<const void*>(c);
  const auto fn  = [](const void* p, Args...args) -> R
  {
    auto* pc = static_cast<C*>(const_cast<void*>(p));

    if constexpr (std::is_void<R>::value) {
      std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
    } else {
      return std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
    }
  };

  return delegate{ptr, static_cast<function_type>(fn)};
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline constexpr alloy::core::delegate<R(Args...)>
  alloy::core::delegate<R(Args...)>::make(const C* c)
  noexcept
{
  ALLOY_ASSERT(c != nullptr);

  const auto ptr = static_cast<const void*>(c);
  const auto fn  = [](const void* p, Args...args) -> R
  {
    auto* pc = static_cast<const C*>(p);

    if constexpr (std::is_void<R>::value) {
      std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
    } else {
      return std::invoke(MemberFn, pc, detail::delegate_forward<Args>(args)...);
    }
  };

  return delegate{ptr, static_cast<function_type>(fn)};
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline constexpr alloy::core::delegate<R(Args...)>
  alloy::core::delegate<R(Args...)>::make(Callable* callable)
  noexcept
{
  ALLOY_ASSERT(callable != nullptr);

  const auto ptr = static_cast<const void*>(callable);
  const auto fn  = [](const void* p, Args...args) -> R
  {
    auto* pc = static_cast<Callable*>(const_cast<void*>(p));

    if constexpr (std::is_void<R>::value) {
      std::invoke(*pc, detail::delegate_forward<Args>(args)...);
    } else {
      return std::invoke(*pc, detail::delegate_forward<Args>(args)...);
    }
  };

  return delegate{ptr, static_cast<function_type>(fn)};
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline constexpr alloy::core::delegate<R(Args...)>
  alloy::core::delegate<R(Args...)>::make(const Callable* callable)
  noexcept
{
  ALLOY_ASSERT(callable != nullptr);

  const auto ptr = static_cast<const void*>(callable);
  const auto fn  = [](const void* p, Args...args) -> R
  {
    const auto* pc = static_cast<const Callable*>(p);

    if constexpr (std::is_void<R>::value) {
      std::invoke(*pc, detail::delegate_forward<Args>(args)...);
    } else {
      return std::invoke(*pc, detail::delegate_forward<Args>(args)...);
    }
  };

  return delegate{ptr, static_cast<function_type>(fn)};
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline constexpr alloy::core::delegate<R(Args...)>::delegate()
  noexcept
  : delegate{nullptr, &default_function}
{

}

//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn, typename>
inline void alloy::core::delegate<R(Args...)>::bind()
  noexcept
{
  (*this) = make<Fn>();
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline void alloy::core::delegate<R(Args...)>::bind(C* c)
  noexcept
{
  (*this) = make<MemberFn>(c);
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline void alloy::core::delegate<R(Args...)>::bind(const C* c)
  noexcept
{
  (*this) = make<MemberFn>(c);
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline void alloy::core::delegate<R(Args...)>::bind(Callable* callable)
  noexcept
{
  (*this) = make(callable);
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline void alloy::core::delegate<R(Args...)>::bind(const Callable* callable)
  noexcept
{
  (*this) = make(callable);
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline void alloy::core::delegate<R(Args...)>::reset()
  noexcept
{
  m_function = &default_function;
  m_instance = nullptr;
}

//-----------------------------------------------------------------------------
// Observer
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <typename...UArgs, typename>
inline R alloy::core::delegate<R(Args...)>::operator()(UArgs&&...args)
  const
{
  return m_function(m_instance, std::forward<UArgs>(args)...);
}


template <typename R, typename...Args>
inline constexpr alloy::core::delegate<R(Args...)>::operator bool()
  const noexcept
{
  return m_function != &default_function;
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline constexpr alloy::core::delegate<R(Args...)>
  ::delegate(const void* instance, function_type function)
  noexcept
  : m_instance{instance},
    m_function{function}
{

}

//-----------------------------------------------------------------------------
// Private Static Functions
//-----------------------------------------------------------------------------

template<typename R, typename... Args>
R alloy::core::delegate<R(Args...)>::default_function(const void*, Args...)
{
#if ALLOY_CORE_EXCEPTIONS_ENABLED
  throw bad_delegate_call{};
#else
  ALLOY_ASSERT(false, "no function bound to delegate");
  std::terminate();
#endif
}

//=============================================================================
// inline definitions : non-member functions
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline constexpr bool
  alloy::core::operator==(const delegate<R(Args...)>& lhs,
                          const delegate<R(Args...)>& rhs)
  noexcept
{
  return lhs.m_instance == rhs.m_instance &&
         lhs.m_function == rhs.m_function;
}

template <typename R, typename...Args>
inline constexpr bool
  alloy::core::operator!=(const delegate<R(Args...)>& lhs,
                          const delegate<R(Args...)>& rhs)
  noexcept
{
  return !(lhs == rhs);
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <auto Fn, typename>
inline constexpr alloy::core::delegate<typename alloy::core::function_traits<Fn>::signature_type>
  alloy::core::make_delegate()
  noexcept
{
  using delegate_type = delegate<typename function_traits<Fn>::signature_type>;

  return delegate_type::template make<Fn>();
}

template <auto MemberFn, typename C, typename>
inline constexpr alloy::core::delegate<typename alloy::core::function_traits<MemberFn>::signature_type>
  alloy::core::make_delegate(C* c)
  noexcept
{
  using delegate_type = delegate<typename function_traits<MemberFn>::signature_type>;

  return delegate_type::template make<MemberFn>(c);
}

template <auto MemberFn, typename C, typename>
inline constexpr alloy::core::delegate<typename alloy::core::function_traits<MemberFn>::signature_type>
  alloy::core::make_delegate(const C* c)
  noexcept
{
  using delegate_type = delegate<typename function_traits<MemberFn>::signature_type>;

  return delegate_type::template make<MemberFn>(c);
}

#endif /* ALLOY_CORE_UTILITIES_DELEGATE_HPP */
