// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file thread_group.hpp
 *
 * \brief This header defines a grouping utility for thread management
 *****************************************************************************/

#ifndef ALLOY_CORE_THREAD_THREAD_GROUP_HPP
#define ALLOY_CORE_THREAD_THREAD_GROUP_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/utilities/quantity.hpp"

#include <concepts>
#include <thread>

#if defined(_MSC_VER)
# pragma once
#endif

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A group collection of threads
  ///
  /// This class simplifies the joining and collection of threads in a group.
  //////////////////////////////////////////////////////////////////////////////
  class thread_group
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using size_type = uquantity<std::thread>;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this thread group
    thread_group() = default;

    /// \brief Constructs this thread group using the specified allocator
    ///
    /// \param alloc the allocator to use
    explicit thread_group(allocator alloc) noexcept;

    thread_group(thread_group&& other) noexcept;
    thread_group(const thread_group&) = delete;

    auto operator=(thread_group&& other) noexcept -> thread_group&;
    auto operator=(const thread_group& other) noexcept -> thread_group&;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs and stores a thraed in this group
    ///
    /// \param args the arguments to forward to the thread
    template <typename...Args>
      requires std::constructible_from<std::thread, Args...>
    auto emplace_thread(Args&&...args) -> void;

    /// \brief Attaches a thread to this group
    ///
    /// \param thread the thread to attach
    auto add_thread(std::thread thread) -> void;

    /// \brief Reserves space for \p n threads
    ///
    /// \param n the number of threads to reserve
    auto reserve(size_type n) -> void;

    //--------------------------------------------------------------------------
    // Operations
    //--------------------------------------------------------------------------

    /// \brief Joins all threads and clears this group
    ///
    /// \post all threads are joined, and `this->size()` is `0`
    auto join_all() noexcept -> void;

    /// \brief Detaches all threads and clears this group
    ///
    /// \post all threads are detached, and `this->empty()` is `true`
    auto detach_all() noexcept -> void;

    /// \brief Swaps the contents of `(*this)` with \p group
    ///
    /// \param group the other group to swap with
    auto swap(thread_group& group) noexcept -> void;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of threads in this group
    ///
    /// \return the size
    auto size() const noexcept -> size_type;

    /// \brief Queries whether this group is empty
    ///
    /// \return `true` if empty
    auto empty() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    vector<std::thread> m_threads;
  };

} // namespace alloy::core

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

ALLOY_FORCE_INLINE
alloy::core::thread_group::thread_group(allocator alloc)
  noexcept
  : m_threads{stl_allocator_adapter<std::thread>{alloc}}
{

}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename...Args>
  requires std::constructible_from<std::thread, Args...>
ALLOY_FORCE_INLINE
auto alloy::core::thread_group::emplace_thread(Args&&...args)
  -> void
{
  m_threads.emplace_back(std::forward<Args>(args)...);
}

ALLOY_FORCE_INLINE
auto alloy::core::thread_group::add_thread(std::thread thread)
  -> void
{
  ALLOY_ASSERT(thread.joinable());

  m_threads.push_back(std::move(thread));
}

ALLOY_FORCE_INLINE
auto alloy::core::thread_group::reserve(size_type n)
  -> void
{
  m_threads.reserve(n.count());
}

//------------------------------------------------------------------------------
// Operations
//------------------------------------------------------------------------------

inline
auto alloy::core::thread_group::join_all()
  noexcept -> void
{
  for (auto& thread : m_threads) {
    thread.join();
  }
  m_threads.clear();
}

inline
auto alloy::core::thread_group::detach_all()
  noexcept -> void
{
  for (auto& thread : m_threads) {
    thread.detach();
  }
  m_threads.clear();
}

ALLOY_FORCE_INLINE
auto alloy::core::thread_group::swap(thread_group& group)
  noexcept -> void
{
  m_threads.swap(group.m_threads);
}

ALLOY_FORCE_INLINE
auto alloy::core::thread_group::size()
  const noexcept -> size_type
{
  return size_type{m_threads.size()};
}

ALLOY_FORCE_INLINE
auto alloy::core::thread_group::empty()
  const noexcept -> bool
{
  return m_threads.empty();
}

#endif /* ALLOY_CORE_THREAD_THREAD_GROUP_HPP */
