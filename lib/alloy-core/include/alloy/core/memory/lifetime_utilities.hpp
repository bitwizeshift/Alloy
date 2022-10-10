/*****************************************************************************
 * \file
 *
 * \brief
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_MEMORY_LIFETIME_UTILITIES_HPP
#define ALLOY_CORE_MEMORY_LIFETIME_UTILITIES_HPP

#include "alloy/core/utilities/not_null.hpp"
#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"

#include <iterator>
#include <tuple>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <new>

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A collection of utilities for creating and destroying objects
  /////////////////////////////////////////////////////////////////////////////
  class lifetime_utilities
  {
    lifetime_utilities() = delete;
    ~lifetime_utilities() = delete;

    //-------------------------------------------------------------------------
    // Construction
    //-------------------------------------------------------------------------
  public:

    /// \brief
    template <typename T, typename...Args>
    [[nodiscard]]
    static auto make_at(not_null<void*> where, Args&&...args)
      noexcept(std::is_nothrow_constructible_v<T,Args...>) -> not_null<T*>;

    template <typename T>
    [[nodiscard]]
    static auto make_array_at(not_null<void*> where, std::size_t length)
      noexcept(std::is_nothrow_constructible_v<T>) -> not_null<T*>;

    template <typename T>
    [[nodiscard]]
    static auto make_array_at(not_null<void*> where, std::size_t length, const T& copy)
      noexcept(std::is_nothrow_copy_constructible_v<T>) -> not_null<T*>;

    template <typename T, typename Tuple>
    [[nodiscard]]
    static auto make_from_tuple_at(not_null<void*> where, Tuple&& args) -> not_null<T*>;

    //-------------------------------------------------------------------------
    // Destruction
    //-------------------------------------------------------------------------
  public:

    template <typename T>
    static auto destroy(not_null<T*> p)
      noexcept(std::is_nothrow_destructible_v<T>) -> void;

    template <typename T>
    static auto destroy_array(not_null<T*> p, std::size_t length)
      noexcept(std::is_nothrow_destructible_v<T>) -> void;

    template <typename It>
    static auto destroy_range(It begin, It end)
      noexcept(std::is_nothrow_destructible_v<typename std::iterator_traits<It>::value_type>) -> void;

    //-------------------------------------------------------------------------
    // Private Construction
    //-------------------------------------------------------------------------
  private:

    template <typename T, typename...Args>
    static auto make_array_impl(not_null<void*> where, std::size_t lenght, const Args&...args)
      noexcept(std::is_nothrow_constructible_v<T,const Args&...>) -> not_null<T*>;

    template <typename T, typename Tuple, std::size_t...Idxs>
    static auto make_from_tuple_impl(
      not_null<void*> where,
      Tuple&& tuple,
      std::index_sequence<Idxs...>
    ) -> not_null<T*>;

  }; // class lifetime_utilities

} // namespace alloy::core

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

template <typename T, typename...Args>
ALLOY_FORCE_INLINE
auto alloy::core::lifetime_utilities::make_at(not_null<void*> where, Args&&...args)
  noexcept(std::is_nothrow_constructible_v<T,Args...>) -> not_null<T*>
{
  return assume_not_null(new(where.get()) T(std::forward<Args>(args)...));
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::lifetime_utilities::make_array_at(
  not_null<void*> where,
  std::size_t length
) noexcept(std::is_nothrow_constructible_v<T>) -> not_null<T*>
{
  return make_array_impl<T>(where, length);
}

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::lifetime_utilities::make_array_at(
  not_null<void*> where,
  std::size_t length,
  const T& copy
) noexcept(std::is_nothrow_copy_constructible_v<T>) -> not_null<T*>
{
  return make_array_impl<T>(where, length, copy);
}

template <typename T, typename Tuple>
ALLOY_FORCE_INLINE
auto alloy::core::lifetime_utilities::make_from_tuple_at(
  not_null<void*> where,
  Tuple&& args
) -> not_null<T*>
{
  return make_from_tuple_impl<T>(
    where,
    std::forward<Tuple>(args),
    std::make_index_sequence<std::tuple_size_v<Tuple>>{}
  );
}

//-----------------------------------------------------------------------------
// Destruction
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::lifetime_utilities::destroy(not_null<T*> p)
  noexcept(std::is_nothrow_destructible_v<T>) -> void
{
  p->~T();
}

template <typename T>
inline
auto alloy::core::lifetime_utilities::destroy_array(
  not_null<T*> p,
  std::size_t length
) noexcept(std::is_nothrow_destructible_v<T>) -> void
{
  destroy_range(p.get(), p.get() + length);
}

template <typename It>
inline
auto alloy::core::lifetime_utilities::destroy_range(It begin, It end)
  noexcept(std::is_nothrow_destructible_v<typename std::iterator_traits<It>::value_type>) -> void
{
  const auto rbegin = std::make_reverse_iterator(end);
  const auto rend   = std::make_reverse_iterator(begin);

  using value_type = typename std::iterator_traits<It>::value_type;
  for (auto it = rbegin; it != rend; ++it) {
    it->~value_type();
  }
}

//-----------------------------------------------------------------------------
// Private Construction
//-----------------------------------------------------------------------------

template <typename T, typename...Args>
inline
auto alloy::core::lifetime_utilities::make_array_impl(
  not_null<void*> where,
  std::size_t length,
  const Args&...args
) noexcept(std::is_nothrow_constructible_v<T,const Args&...>) -> not_null<T*>
{
  auto* const begin = static_cast<T*>(where.get());
  auto* const end   = begin + length;
  auto* current     = begin;
  auto* result      = begin;

  if constexpr (std::is_nothrow_constructible_v<T,const Args&...>) {
    result = new(static_cast<void*>(current++)) T(args...);
    for (; current < end; ++current) {
      new (current) T(args...);
    }
    return result;
  } else {
    try {
      result = new(static_cast<void*>(current++)) T(args...);
      for (; current < end; ++current) {
        new (current) T(args...);
      }
    } catch (...) {
      destroy_array(result, current - result);
      throw;
    }
  }
  return assume_not_null(result);
}

template <typename T, typename Tuple, std::size_t...Idxs>
ALLOY_FORCE_INLINE
auto alloy::core::lifetime_utilities::make_from_tuple_impl(
  not_null<void*> where,
  Tuple&& tuple,
  std::index_sequence<Idxs...>
) -> not_null<T*>
{
  return assume_not_null(
    new(where.get()) T(std::get<Idxs>(std::forward<Tuple>(tuple))...)
  );
}

#endif /* ALLOY_CORE_MEMORY_LIFETIME_UTILITIES_HPP */

