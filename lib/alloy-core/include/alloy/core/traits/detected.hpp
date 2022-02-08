/*****************************************************************************
 * \file detected.hpp
 *
 * \brief This header defines the detected_t idiom for C++17 "concept"-like
 *        support
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
#ifndef ALLOY_CORE_TRAITS_DETECTED_HPP
#define ALLOY_CORE_TRAITS_DETECTED_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <type_traits>

namespace alloy::core {

  /// \brief A sentinel type used to indicate no such existing object
  struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    auto operator=(nonesuch const&) -> void = delete;
  };

  namespace detail {

    template <typename Default, typename AlwaysVoid,
              template <typename...> typename Op, typename... Args>
    struct detector {
      using value_t = std::false_type;
      using type = Default;
    };

    template <typename Default, template <typename...> typename Op, typename... Args>
    struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
      using value_t = std::true_type;
      using type = Op<Args...>;
    };
  } // namespace detail

  /// \brief Trait to determine if a functionality is detected
  ///
  /// The result is aliased as `::value`
  ///
  /// \tparam Op the template to attempt to evaluate
  /// \tparam Args the arguments to supply to `Op`
  template <template <typename...> typename Op, typename... Args>
  using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

  /// \brief Convenience constant to extract `::value` from `is_detected`
  template <template <typename...> typename Op, typename... Args >
  constexpr inline auto is_detected_v = is_detected<Op, Args...>::value;

  //---------------------------------------------------------------------------

  /// \brief A utility to extract the computed type from a given expression
  ///
  /// The result is aliased as `::type`
  ///
  /// \tparam Op the template to evaluate
  /// \tparam Args the arguments to forward to `Op`
  template <template <typename...> typename Op, typename... Args>
  using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

  /// \brief A utility to extract the computed type from a given expression, or
  ///        a specified default
  ///
  /// The result is aliased as `::type`
  ///
  /// \tparam Default the default type if evaluation fails
  /// \tparam Op the template to evaluate
  /// \tparam Args the arguments to forward to `Op`
  template <typename Default, template <typename...> typename Op, typename... Args>
  using detected_or = detail::detector<Default, void, Op, Args...>;

  /// \brief Convenience alias to extract `::type` from `detected_or`
  template <typename Default, template <typename...> typename Op, typename... Args >
  using detected_or_t = typename detected_or<Default, Op, Args...>::type;

  //---------------------------------------------------------------------------

  /// \brief Trait to determine if a functionality is detected with the exact
  ///        result
  ///
  /// The result is aliased as `::value`
  ///
  /// \note This is effectively a concept with `std::same_as` for the
  ///       requirement expression
  ///
  /// \tparam Expected the exact expected type
  /// \tparam Op the template to attempt to evaluate
  /// \tparam Args the arguments to supply to `Op`
  template <typename Expected, template <typename...> typename Op, typename... Args>
  using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

  /// \brief Convenience constant to extract `::value` from `is_detected_exact`
  template <typename Expected, template <typename...> typename Op, typename... Args>
  constexpr inline bool is_detected_exact_v = (
    is_detected_exact<Expected, Op, Args...>::value
  );

  //---------------------------------------------------------------------------

  /// \brief Trait to determine if a functionality is detected with an
  ///        equivalent (convertible) result
  ///
  /// The result is aliased as `::value`
  ///
  /// \note This is effectively a concept with `std::convertible_to` for the
  ///       requirement expression
  ///
  /// \tparam Expected the type to be convertible to
  /// \tparam Op the template to attempt to evaluate
  /// \tparam Args the arguments to supply to `Op`
  template <typename To, template <typename...> typename Op, typename... Args>
  using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

  /// \brief Convenience constant to extract `::value` from
  ///        `is_detected_convertible`
  template <typename To, template <typename...> typename Op, typename... Args>
  constexpr inline bool is_detected_convertible_v = (
    is_detected_convertible<To, Op, Args...>::value
  );

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_DETECTED_HPP */
