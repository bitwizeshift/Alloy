// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file map.hpp
 *
 * \brief This header provides a map container wrapper around the
 *        `std::map`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_MAP_HPP
#define ALLOY_CORE_CONTAINERS_MAP_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <functional> // std::equal_to, std::hash
#include <map>        // std::map
#include <utility>    // std::pair

namespace alloy::core {

  template <typename Key,
            typename T,
            typename Compare = std::less<Key>,
            typename Allocator = stl_allocator_adapter<std::pair<const Key, T>>>
  using map = explicitly_copyable<std::map<Key,T,Compare,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_MAP_HPP */
