/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_TRAITS_COMMON_REFERENCE_HPP
#define ALLOY_CORE_TRAITS_COMMON_REFERENCE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/traits/copy_cv.hpp"
#include "alloy/core/traits/copy_cvref.hpp"
#include <utility>
#include <type_traits>

namespace alloy::core {

  template <typename T, typename U, template<typename> class TQual, template<typename> class UQual>
  struct basic_common_reference {};

  template <typename...Types>
  struct common_reference;

  namespace detail {

    template <typename...>
    struct common_type_sequence{};

    template <typename T>
    struct common_ref_qual {
      template <typename U>
      using rebind = copy_cvref_t<T, U>;
    };

    template <class T, class U>
    using common_ref_result_t = decltype(
      false ? std::declval<T(&)()>()() : std::declval<U(&)()>()()
    );

    //==========================================================================
    // Simple common reference
    //==========================================================================


    template <typename T, typename U>
    using common_ref_cv_result_t = common_ref_result_t<
      copy_cv_t<T,U>&,
      copy_cv_t<U,T>&
    >;

    // Implements the 'simple common reference'
    // (1) If T1 is cv1 X & and T2 is cv2 Y & (i.e., both are lvalue reference types):
    // their simple common reference type is
    // `decltype(false? std::declval<cv12 X &>() : std::declval<cv12 Y &>())`,
    // where cv12 is the union of cv1 and cv2, if that type exists and is a
    // reference type;
    template <typename A, typename B, typename = void>
    struct simple_common_reference_subpoint_1 { };

    template <typename A, typename B>
    struct simple_common_reference_subpoint_1<A,B,std::void_t<common_ref_cv_result_t<A,B>>> {
      using type = common_ref_cv_result_t<A,B>;
    };

    // (2) If T1 and T2 are both rvalue reference types: if the simple common
    // reference type of T1 & and T2 & (determined according to the previous
    // bullet) exists, then let C denote that type's corresponding rvalue
    // reference type. If
    // std::is_convertible_v<T1, C> and std::is_convertible_v<T2, C> are both
    // true, then the simple common reference type of T1 and T2 is C.
    template <typename A, typename B, typename = void>
    struct simple_common_reference_subpoint_2 { };

    template <typename A, typename B>
    struct simple_common_reference_subpoint_2<A, B, std::void_t<common_ref_cv_result_t<A,B>>>
      : std::enable_if_t<(
          std::is_convertible_v<A, common_ref_cv_result_t<A,B>> &&
          std::is_convertible_v<B, common_ref_cv_result_t<A,B>>
        ), std::remove_reference<common_ref_cv_result_t<A,B>>&&>{ };

    // (3) Otherwise, one of the two types must be an lvalue reference type A &
    //     and the other must be an rvalue reference type B &&
    //     (A and B might be cv-qualified). Let D denote the simple common
    //     reference type of A & and B const &, if any.
    //     If D exists and std::is_convertible_v<B&&, D> is true, then the
    //     simple common reference type is D.
    template <typename A, typename B, typename = void>
    struct simple_common_reference_subpoint_3 { };

    template <typename A, typename B>
    struct simple_common_reference_subpoint_3<A, B, std::void_t<common_ref_cv_result_t<A,const B>>>
      : std::enable_if_t<(
          std::is_convertible_v<B&&, common_ref_cv_result_t<A,const B>>
        ), common_ref_cv_result_t<A,const B>>{ };

    //--------------------------------------------------------------------------

    // If A or B is not a reference, do not define a `type` member
    template <typename A, typename B>
    struct simple_common_reference {};

    template <typename A, typename B>
    struct simple_common_reference<A&, B&>
      : simple_common_reference_subpoint_1<A,B> {};

    template <typename A, typename B>
    struct simple_common_reference<A&&, B&&>
      : simple_common_reference_subpoint_2<A,B> {};

    template <typename A, typename B>
    struct simple_common_reference<A&, B&&>
      : simple_common_reference_subpoint_3<A,B> {};

    template <typename A, typename B>
    struct simple_common_reference<A&&, B&>
      : simple_common_reference_subpoint_3<B,A> {};

    //--------------------------------------------------------------------------

    // (3.5) Otherwise, there is no member type. (SFINAE)
    template <typename T, typename U>
    struct common_reference_subpoint_5 {};

    // (3.4) Otherwise, if common_type_t<T1, T2> is a valid type, then the
    //       member type `type` names that type;
    template <typename T, typename U, typename = void>
    struct common_reference_subpoint_4
      : common_reference_subpoint_5<T,U> {};

    template <typename T, typename U>
    struct common_reference_subpoint_4<T, U, std::void_t<std::common_type_t<T,U>>>
      : std::common_type<T, U> {};

    // (3.3) if decltype(false? val<T1>() : val<T2>()), where val is a function
    //       template `template<class T> T val();`, is a valid type, then the
    //       member type type names that type
    template <typename T, typename U, typename = void>
    struct common_reference_subpoint_3
      : common_reference_subpoint_4<T,U> {};

    template <typename T, typename U>
    struct common_reference_subpoint_3<T,U,std::void_t<common_ref_result_t<T,U>>> {
      using type = common_ref_result_t<T,U>;
    };

    // (3.2) if `basic_common_reference<remove_cvref_t<T1>, remove_cvref_t<T2>, T1Q, T2Q>::type`
    //       exists, where TiQ is a unary alias template such that TiQ<U> is U
    //       with the addition of Ti's cv- and reference qualifiers, then the
    //       member type type names that type;
    template <typename T, typename U, typename = void>
    struct common_reference_subpoint_2
      : common_reference_subpoint_3<T,U> {};

    template <typename T, typename U>
    struct common_reference_subpoint_2<T,U,std::void_t<
      typename basic_common_reference<
        std::remove_cv_t<std::remove_reference_t<T>>,
        std::remove_cv_t<std::remove_reference_t<U>>,
        common_ref_qual<copy_cvref_t<T,U>>::template rebind,
        common_ref_qual<copy_cvref_t<U,T>>::template rebind
      >::type>
    > : basic_common_reference<
          std::remove_cv_t<std::remove_reference_t<T>>,
          std::remove_cv_t<std::remove_reference_t<U>>,
          common_ref_qual<U>::template rebind,
          common_ref_qual<T>::template rebind
        > {};


    // (3.1) If T1 and T2 are both reference types, and the simple common
    //       reference type S of T1 and T2 exists, then the member type `type`
    //       names S;
    template <typename T, typename U, typename = void>
    struct common_reference_subpoint_1
      : common_reference_subpoint_2<T,U> {};

    // Note: simple_common_reference SFINAEs if T or U is not a reference.
    template <typename T, typename U>
    struct common_reference_subpoint_1<T,U,std::void_t<typename simple_common_reference<T,U>::type>>
      : simple_common_reference<T,U> {};

    // Used to dispatch the subpoints for common_reference<T,U>
    template <typename T, typename U>
    struct common_reference2_impl : common_reference_subpoint_1<T,U> {};

    // Used to dispatch variadic common_reference so if one common reference
    // cannot be formed, the whole common_reference fails to form.
    template <typename Types, typename = void>
    struct common_reference_impl { };

    template <typename T0, typename T1, typename T2, typename...TRest>
    struct common_reference_impl<
      common_type_sequence<T0,T1,T2,TRest...>,
      std::void_t<typename common_reference<T0,T1>::type>
    > : common_reference_impl<
          common_type_sequence<typename common_reference<T0,T1>::type,T2,TRest...>
        > {};

    // Delegate back to common_reference2_impl which handles the reference
    // subpoints. Ends the recursion
    template <typename T0, typename T1>
    struct common_reference_impl<
      common_type_sequence<T0,T1>,
      std::void_t<typename common_reference<T0,T1>::type>
    > : common_reference2_impl<T0,T1> {};

  } // namespace detail

  template <typename...Types>
  struct common_reference{};

  // (1) If sizeof...(T) is zero, there is no member type.
  template <>
  struct common_reference<> {};

  // (2) If sizeof...(T) is one (i.e., T... contains only one type T0), the
  //     member type names the same type as T0.
  template <typename T>
  struct common_reference<T> {
    using type = T;
  };

  // (3) If sizeof...(T) is two (i.e., T... contains two types T1 and T2):
  //     (Follow subpoints 3.1 - 3.5 above)
  template <typename T0, typename T1>
  struct common_reference<T0, T1>
    : detail::common_reference2_impl<T0, T1> {};

  // (4) If sizeof...(T) is greater than two ...
  template <typename T0, typename T1, typename T2, typename...TRest>
  struct common_reference<T0,T1,T2,TRest...>
    : detail::common_reference_impl<detail::common_type_sequence<T0,T1,T2,TRest...>> {};

  template <typename...Types>
  using common_reference_t = typename common_reference<Types...>::type;

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_COMMON_REFERENCE_HPP */
