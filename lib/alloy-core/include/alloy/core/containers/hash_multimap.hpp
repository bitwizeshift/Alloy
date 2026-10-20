// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file hash_multimap.hpp
 *
 * \brief This header provides a hash_multimap container wrapper around the
 *        `std::unordered_multimap`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_HASH_MULTIMAP_HPP
#define ALLOY_CORE_CONTAINERS_HASH_MULTIMAP_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <unordered_map> // std::unordered_multimap
#include <functional>    // std::equal_to, std::hash
#include <utility>       // std::pair

namespace alloy::core {

  template <typename Key,
            typename T,
            typename Hash = std::hash<Key>,
            typename KeyEqual = std::equal_to<Key>,
            typename Allocator = stl_allocator_adapter<std::pair<const Key, T>>>
  using hash_multimap = explicitly_copyable<std::unordered_multimap<Key,Hash,KeyEqual,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_HASH_MULTIMAP_HPP */
