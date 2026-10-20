// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file hash_set.hpp
 *
 * \brief This header provides a hash_set container wrapper around the
 *        `std::unordered_set`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_HASH_SET_HPP
#define ALLOY_CORE_CONTAINERS_HASH_SET_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <unordered_set> // std::set
#include <functional>    // std::equal_to, std::hash

namespace alloy::core {

  template <typename Key,
            typename Hash = std::hash<Key>,
            typename KeyEqual = std::equal_to<Key>,
            typename Allocator = stl_allocator_adapter<Key>>
  using hash_set = explicitly_copyable<std::unordered_set<Key,Hash,KeyEqual,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_HASH_SET_HPP */
