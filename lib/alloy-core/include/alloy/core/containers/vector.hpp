// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file vector.hpp
 *
 * \brief This header provides a vector container wrapper around the
 *        `std::vector`, which makes it explicitly copyable.
 *****************************************************************************/

#ifndef ALLOY_CORE_CONTAINERS_VECTOR_HPP
#define ALLOY_CORE_CONTAINERS_VECTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <vector>

namespace alloy::core {

  template <typename T, typename Allocator = stl_allocator_adapter<T>>
  using vector = explicitly_copyable<std::vector<T,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_VECTOR_HPP */
