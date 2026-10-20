// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_CORE_TRAITS_COPY_CV_HPP
#define ALLOY_CORE_TRAITS_COPY_CV_HPP

#ifdef _MSC_VER
# pragma once
#endif

namespace alloy::core {

  template <typename From, typename To>
  struct copy_cv { using type = To; };

  template <typename From, typename To>
  struct copy_cv<const From, To> { using type = const To; };

  template <typename From, typename To>
  struct copy_cv<volatile From, To> { using type = volatile To; };

  template <typename From, typename To>
  struct copy_cv<const volatile From, To> { using type = const volatile To; };

  template <typename From, typename To>
  using copy_cv_t = typename copy_cv<From,To>::type;

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_COPY_CV_HPP */
