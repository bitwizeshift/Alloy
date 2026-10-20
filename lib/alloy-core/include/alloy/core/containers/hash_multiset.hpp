// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file hash_multiset.hpp
 *
 * \brief This header provides a hash_multiset container wrapper around the
 *        `std::unordered_multiset`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_HASH_MULTISET_HPP
#define ALLOY_CORE_CONTAINERS_HASH_MULTISET_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <unordered_set> // std::unordered_multiset
#include <functional>    // std::equal_to, std::hash

namespace alloy::core {

  template <typename Key,
            typename Hash = std::hash<Key>,
            typename KeyEqual = std::equal_to<Key>,
            typename Allocator = stl_allocator_adapter<Key>>
  using hash_multiset = explicitly_copyable<std::unordered_multiset<Key,Hash,KeyEqual,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_HASH_MULTISET_HPP */
