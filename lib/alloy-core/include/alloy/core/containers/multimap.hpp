// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file multimap.hpp
 *
 * \brief This header provides a multimap container wrapper around the
 *        `std::multimap`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_MULTIMAP_HPP
#define ALLOY_CORE_CONTAINERS_MULTIMAP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <map>        // std::map
#include <functional> // std::equal_to, std::hash
#include <utility>    // std::pair

namespace alloy::core {

  template <typename Key,
            typename T,
            typename Compare = std::less<Key>,
            typename Allocator = stl_allocator_adapter<std::pair<const Key, T>>>
  using multimap = explicitly_copyable<std::multimap<Key,T,Compare,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_MULTIMAP_HPP */
