// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file always_false.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_CORE_TRAITS_ALWAYS_FALSE_HPP
#define ALLOY_CORE_TRAITS_ALWAYS_FALSE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <type_traits>

namespace alloy::core {

  template <typename T>
  struct always_false : std::false_type {};

  template <typename T>
  inline constexpr auto always_false_v = always_false<T>::value;

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_ALWAYS_FALSE_HPP */
