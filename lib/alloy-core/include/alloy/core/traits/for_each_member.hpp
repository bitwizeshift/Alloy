////////////////////////////////////////////////////////////////////////////////
/// \file for_each_member.hpp
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

#ifndef ALLOY_CORE_TRAITS_FOR_EACH_MEMBER_HPP
#define ALLOY_CORE_TRAITS_FOR_EACH_MEMBER_HPP

#include "alloy/core/intrinsics.hpp"

#include <cstddef>
#include <type_traits>
#include <utility> // std::forward

namespace alloy::core {

  /// \brief Iterates each member of the specified aggregate object \p agg
  ///        and invokes \p fn on each member
  ///
  /// Each member is guaranteed to be visited in the order in which it appears.
  ///
  /// \param agg the aggregate to visit
  /// \param fn the function to invoke
  template <typename T, typename Fn>
  constexpr auto for_each_member(T&& agg, Fn&& fn) -> void;

} // namespace alloy::core

// GCC throws a false positive here and seems to think that the `void_t`
// expressions are not initializing members of a struct... Which is insane since
// these are unevaluated contexts. This is a garbage diagnostic, and is disabled
ALLOY_COMPILER_DIAGNOSTIC_PUSH()
ALLOY_COMPILER_GCC_DIAGNOSTIC_IGNORE(-Wmissing-field-initializers)

namespace alloy::core::detail {

  // The approach taken here is described in detail in the following blog post:
  // https://bitwizeshift.github.io/posts/2021/03/21/reflecting-over-members-of-an-aggregate/

  // A type that can be implicitly converted to *anything*
  struct anything {
    template <typename T>
    operator T() const; // We don't need to define this function
  };

  template <typename T, typename Is, typename=void>
  struct is_aggregate_constructible_from_n_impl
    : std::false_type{};

  template <typename T, std::size_t...Is>
  struct is_aggregate_constructible_from_n_impl<
    T,
    std::index_sequence<Is...>,
    std::void_t<decltype(T{(void(Is),anything{})...})>
  > : std::true_type{};

  template <typename T, std::size_t N>
  using is_aggregate_constructible_from_n = detail::is_aggregate_constructible_from_n_impl<T,std::make_index_sequence<N>>;

  template <std::size_t Min, std::size_t Range, template <std::size_t N> class target>
  struct maximize
    : std::conditional_t<
        maximize<Min, Range/2, target>{} == (Min+Range/2)-1,
        maximize<Min+Range/2, (Range+1)/2, target>,
        maximize<Min, Range/2, target>
      >{};
  template <std::size_t Min, template <std::size_t N> class target>
  struct maximize<Min, 1, target>
    : std::conditional_t<
        target<Min>{},
        std::integral_constant<std::size_t,Min>,
        std::integral_constant<std::size_t,Min-1>
      >{};
  template <std::size_t Min, template <std::size_t N> class target>
  struct maximize<Min, 0, target>
    : std::integral_constant<std::size_t,Min-1>
  {};

  template <typename T>
  struct construct_searcher {
    template<std::size_t N>
    using result = is_aggregate_constructible_from_n<T, N>;
  };

  template <typename T, std::size_t Cap=32>
  using constructor_arity = detail::maximize< 0, Cap, detail::construct_searcher<T>::template result >;

  template <std::size_t N>
  using ctor_arity = std::integral_constant<std::size_t,N>;

  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<0>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<1>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<2>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<3>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<4>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<5>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<6>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<8>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<7>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<9>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<10>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<11>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<12>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<13>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<14>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<15>) -> void;
  template <typename T, typename Fn>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<16>) -> void;
  template <typename T, typename Fn, std::size_t N>
  constexpr auto for_each_impl(T&& agg, Fn&& fn, ctor_arity<N>) -> void;

} // namespace alloy::core::detail

ALLOY_COMPILER_DIAGNOSTIC_POP()

template <typename T, typename Fn>
inline constexpr
auto alloy::core::for_each_member(T&& agg, Fn&& fn) -> void
{
  detail::for_each_impl(
    std::forward<T>(agg),
    std::forward<Fn>(fn),
    detail::constructor_arity<T>{}
  );
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(
  [[maybe_unused]] T&& agg,
  [[maybe_unused]] Fn&& fn,
  ctor_arity<0>
) -> void
{
  // do nothing (0 members)
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<1>)
  -> void
{
  auto&& [m0] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<2>)
  -> void
{
  auto&& [m0, m1] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<3>)
  -> void
{
  auto&& [m0, m1, m2] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<4>)
  -> void
{
  auto&& [m0, m1, m2, m3] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<5>)
  -> void
{
  auto&& [m0, m1, m2, m3, m4] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<6>)
  -> void
{
  auto&& [m0, m1, m2, m3, m4, m5] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<7>)
  -> void
{
  auto&& [m0, m1, m2, m3, m4, m5, m6] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<8>)
  -> void
{
  auto&& [m0, m1, m2, m3, m4, m5, m6, m7] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<9>)
  -> void
{
  auto&& [m0, m1, m2, m3, m4, m5, m6, m7, m8] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<10>)
  -> void
{
  auto&& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
  fn(std::forward<decltype(m9)>(m9));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<11>)
  -> void
{
  auto&& [m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
  fn(std::forward<decltype(m9)>(m9));
  fn(std::forward<decltype(m10)>(m10));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<12>)
  -> void
{
  auto&& [
    m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11
  ] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
  fn(std::forward<decltype(m9)>(m9));
  fn(std::forward<decltype(m10)>(m10));
  fn(std::forward<decltype(m11)>(m11));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<13>)
  -> void
{
  auto&& [
    m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12
  ] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
  fn(std::forward<decltype(m9)>(m9));
  fn(std::forward<decltype(m10)>(m10));
  fn(std::forward<decltype(m11)>(m11));
  fn(std::forward<decltype(m12)>(m12));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<14>)
  -> void
{
  auto&& [
    m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13
  ] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
  fn(std::forward<decltype(m9)>(m9));
  fn(std::forward<decltype(m10)>(m10));
  fn(std::forward<decltype(m11)>(m11));
  fn(std::forward<decltype(m12)>(m12));
  fn(std::forward<decltype(m13)>(m13));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<15>)
  -> void
{
  auto&& [
    m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14
  ] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
  fn(std::forward<decltype(m9)>(m9));
  fn(std::forward<decltype(m10)>(m10));
  fn(std::forward<decltype(m11)>(m11));
  fn(std::forward<decltype(m12)>(m12));
  fn(std::forward<decltype(m13)>(m13));
  fn(std::forward<decltype(m14)>(m14));
}

template <typename T, typename Fn>
inline constexpr
auto alloy::core::detail::for_each_impl(T&& agg, Fn&& fn, ctor_arity<16>)
  -> void
{
  auto&& [
    m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15
  ] = std::forward<T>(agg);

  fn(std::forward<decltype(m0)>(m0));
  fn(std::forward<decltype(m1)>(m1));
  fn(std::forward<decltype(m2)>(m2));
  fn(std::forward<decltype(m3)>(m3));
  fn(std::forward<decltype(m4)>(m4));
  fn(std::forward<decltype(m5)>(m5));
  fn(std::forward<decltype(m6)>(m6));
  fn(std::forward<decltype(m7)>(m7));
  fn(std::forward<decltype(m8)>(m8));
  fn(std::forward<decltype(m9)>(m9));
  fn(std::forward<decltype(m10)>(m10));
  fn(std::forward<decltype(m11)>(m11));
  fn(std::forward<decltype(m12)>(m12));
  fn(std::forward<decltype(m13)>(m13));
  fn(std::forward<decltype(m14)>(m14));
  fn(std::forward<decltype(m15)>(m15));
}
#endif /* ALLOY_CORE_TRAITS_FOR_EACH_MEMBER_HPP */
