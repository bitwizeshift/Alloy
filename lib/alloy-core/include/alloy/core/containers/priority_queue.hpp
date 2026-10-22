// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file priority_queue.hpp
 *
 * \brief This header provides a priority_queue container wrapper around the
 *        `std::queue` container, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_PRIORITY_QUEUE_HPP
#define ALLOY_CORE_CONTAINERS_PRIORITY_QUEUE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/containers/deque.hpp"

#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <functional> // std::less
#include <queue>      // std::priority_queue

namespace alloy::core {

  template <typename T,
            typename Container = deque<T>,
            typename Compare = std::less<typename Container::value_type>>
  using priority_queue = explicitly_copyable<std::priority_queue<T,Container>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_PRIORITY_QUEUE_HPP */
