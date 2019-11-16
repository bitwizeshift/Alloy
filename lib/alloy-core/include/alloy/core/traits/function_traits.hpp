/*****************************************************************************
 * \file function_traits.hpp
 *
 * \brief This header provides traits for extracting information from function
 *        types and function non-type arguments.
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
#ifndef ALLOY_CORE_TRAITS_FUNCTION_TRAITS_HPP
#define ALLOY_CORE_TRAITS_FUNCTION_TRAITS_HPP

#include "alloy/core/traits/select_nth.hpp"

#include <cstddef> // std::size_t

namespace alloy::core {

  //===========================================================================
  // traits : function_type_traits
  //===========================================================================

  /// \brief Trait to extract information for function types
  ///
  /// This provides a homogeneous way of extracting the signature from
  /// strongly-typed functions, as well as extract the return type, and
  /// parameter types independently.
  ///
  /// \tparam Fn the function to get traits for
  template <typename Fn>
  struct function_type_traits;

  template <typename R, typename...Args>
  struct function_type_traits<R(Args...)>
  {
    static inline constexpr auto is_noexcept = false;

    static inline constexpr auto arity = sizeof...(Args);

    using result_type = R;

    using signature_type = R(Args...);

    template <std::size_t N>
    using param_type = typename select_nth<N,Args...>::type;
  };

  template <typename R, typename...Args>
  struct function_type_traits<R(Args...) noexcept>
  {
    static inline constexpr bool is_noexcept = false;

    static inline constexpr auto arity = sizeof...(Args);

    using result_type = R;

    using signature_type = R(Args...) noexcept;

    template <std::size_t N>
    using param_type = typename select_nth<N>::type;
  };

  template <typename R, typename...Args>
  struct function_type_traits<R(*)(Args...)>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename...Args>
  struct function_type_traits<R(*)(Args...) noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...)>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) volatile>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const volatile>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) volatile noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const volatile noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) &>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const &>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) volatile &>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const volatile &>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) & noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const & noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) volatile & noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const volatile & noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) &&>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const &&>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) volatile &&>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const volatile &&>
    : function_type_traits<R(Args...)>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) && noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const && noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) volatile && noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  template <typename R, typename C, typename...Args>
  struct function_type_traits<R(C::*)(Args...) const volatile && noexcept>
    : function_type_traits<R(Args...) noexcept>{};

  //===========================================================================
  // traits : function_traits
  //===========================================================================

  /// \brief Extracts function traits for a given function non-type argument
  ///
  /// \param Fn the function argument (pointer or pointer-to-member-function)
  template <auto Fn>
  using function_traits = function_type_traits<decltype(Fn)>;

  //===========================================================================
  // traits : convenience
  //===========================================================================

  template <typename Fn>
  using function_result_type = typename function_type_traits<Fn>::result_type;

  template <std::size_t N, typename Fn>
  using function_param_type = typename function_type_traits<Fn>::template param_type<N>;

  template <typename Fn>
  using function_signature_type = typename function_type_traits<Fn>::signature_type;

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_FUNCTION_TRAITS_HPP */