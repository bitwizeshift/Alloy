// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file deque.hpp
 *
 * \brief This header provides a deque container wrapper around the `std::queue`
 *        container, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_DEQUE_HPP
#define ALLOY_CORE_CONTAINERS_DEQUE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <deque>

namespace alloy::core {

  template <typename T, typename Allocator = stl_allocator_adapter<T>>
  using deque = explicitly_copyable<std::deque<T,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_DEQUE_HPP */
