// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file set.hpp
 *
 * \brief This header provides a set container wrapper around the
 *        `std::set`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_SET_HPP
#define ALLOY_CORE_CONTAINERS_SET_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <functional> // std::less
#include <set>        // std::set

namespace alloy::core {

  template <typename Key,
            typename Compare = std::less<Key>,
            typename Allocator = stl_allocator_adapter<Key>>
  using set = explicitly_copyable<std::set<Key,Compare,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_SET_HPP */
