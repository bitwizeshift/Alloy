// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_CORE_TRAITS_COPY_CVREF_HPP
#define ALLOY_CORE_TRAITS_COPY_CVREF_HPP

#ifdef _MSC_VER
# pragma once
#endif

namespace alloy::core {

  template <typename From, typename To>
  struct copy_cvref { using type = To; };

  template <typename From, typename To>
  struct copy_cvref<From&, To> { using type = copy_cvref<From,To>&; };

  template <typename From, typename To>
  struct copy_cvref<From&&, To> { using type = copy_cvref<From,To>&&; };

  template <typename From, typename To>
  struct copy_cvref<const From, To> { using type = const To; };

  template <typename From, typename To>
  struct copy_cvref<volatile From, To> { using type = volatile To; };

  template <typename From, typename To>
  struct copy_cvref<const volatile From, To> { using type = const volatile To; };

  template <typename From, typename To>
  using copy_cvref_t = typename copy_cvref<From,To>::type;

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_COPY_CVREF_HPP */

