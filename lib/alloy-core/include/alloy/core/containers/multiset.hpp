// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file multiset.hpp
 *
 * \brief This header provides a multiset container wrapper around the
 *        `std::multiset`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_MULTISET_HPP
#define ALLOY_CORE_CONTAINERS_MULTISET_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <set>        // std::multiset
#include <functional> // std::less

namespace alloy::core {

  template <typename Key,
            typename Compare = std::less<Key>,
            typename Allocator = stl_allocator_adapter<Key>>
  using multiset = explicitly_copyable<std::multiset<Key,Compare,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_MULTISET_HPP */
