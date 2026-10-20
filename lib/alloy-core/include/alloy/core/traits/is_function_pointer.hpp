// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file is_function_pointer.hpp
 *
 * \brief This header defines a trait for detecting function pointers
 *****************************************************************************/

#ifndef ALLOY_CORE_TRAITS_IS_FUNCTION_POINTER_HPP
#define ALLOY_CORE_TRAITS_IS_FUNCTION_POINTER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <type_traits>

namespace alloy::core {

template<typename T>
struct is_function_pointer : std::conjunction<
  std::is_pointer<T>,
  std::is_function<std::remove_pointer_t<T>>
>{};

template <typename T>
inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;

} // namespace alloy::core

#endif /* ALLOY_CORE_TRAITS_IS_FUNCTION_POINTER_HPP */