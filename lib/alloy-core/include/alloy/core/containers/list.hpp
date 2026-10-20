// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file list.hpp
 *
 * \brief This header provides a list container wrapper around the
 *        `std::list`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_LIST_HPP
#define ALLOY_CORE_CONTAINERS_LIST_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <list>

namespace alloy::core {

  template <typename T, typename Allocator = stl_allocator_adapter<T>>
  using list = explicitly_copyable<std::list<T,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_LIST_HPP */
