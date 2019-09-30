/*****************************************************************************
 * \file map.hpp
 *
 * \todo
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef ALLOY_CORE_CONTAINERS_MAP_HPP
#define ALLOY_CORE_CONTAINERS_MAP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/explicitly_copyable.hpp"

#include <map>        // std::map
#include <memory>     // std::allocator
#include <functional> // std::equal_to, std::hash
#include <utility>    // std::pair

namespace alloy::core {

  template <typename Key,
            typename T,
            typename Compare = std::less<Key>,
            typename Allocator = stl_allocator_adapter<std::pair<const Key, T>>>
  using map = explicitly_copyable<std::map<Key,T,Compare,Allocator>>;

} // namespace alloy::core

#endif /* ALLOY_CORE_CONTAINERS_MAP_HPP */