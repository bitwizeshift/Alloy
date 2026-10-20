// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file queue.hpp
 *
 * \brief This header provides a stack container wrapper around the
 *        `std::stack`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_QUEUE_HPP
#define ALLOY_CORE_CONTAINERS_QUEUE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/utilities/explicitly_copyable.hpp"
#include "alloy/core/containers/deque.hpp"

#include <stack> // std::stack

namespace alloy::core {

  template <typename T, typename Container = deque<T>>
  using stack = explicitly_copyable<std::stack<T,Container>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_QUEUE_HPP */
