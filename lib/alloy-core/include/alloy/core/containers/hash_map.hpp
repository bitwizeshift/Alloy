// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file hash_map.hpp
 *
 * \brief This header provides a hash_map container wrapper around the
 *        `std::unordered_map`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_HASH_MAP_HPP
#define ALLOY_CORE_CONTAINERS_HASH_MAP_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <functional>    // std::equal_to, std::hash
#include <unordered_map> // std::map
#include <utility>       // std::pair

namespace alloy::core {

  template <typename Key,
            typename T,
            typename Hash = std::hash<Key>,
            typename KeyEqual = std::equal_to<Key>,
            typename Allocator = stl_allocator_adapter<std::pair<const Key, T>>>
  using hash_map = explicitly_copyable<std::unordered_map<Key,T,Hash,KeyEqual,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_HASH_MAP_HPP */
