// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file queue.hpp
 *
 * \brief This header provides a queue container wrapper around the
 *        `std::queue` container, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_QUEUE_HPP
#define ALLOY_CORE_CONTAINERS_QUEUE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/containers/deque.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <queue>  // std::queue

namespace alloy::core {

  template <typename T, typename Container = deque<T>>
  using queue = explicitly_copyable<std::queue<T,Container>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_QUEUE_HPP */
