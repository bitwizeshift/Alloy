// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file memory_arena_cache.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_EXTRA_ALLOCATORS_MEMORY_ARENA_CACHE_HPP
#define ALLOY_EXTRA_ALLOCATORS_MEMORY_ARENA_CACHE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/allocators/memory_arena.hpp"

#include <cstddef> // std::size_t

namespace alloy::extra {

  ///////////////////////////////////////////////////////////////////////////
  /// \brief A cache containing an intrinsically linked list of
  ///        memory_arenas
  ///
  /// Blocks in this cache are not guaranteed to all be of the same size.
  /// Memory arenas may originate from different allocators, and represent
  /// different regions of memory -- however this is not the recommended
  /// practice.
  ///
  /// Every memory_arena in the memory_arena_cache must be aligned to at
  /// least `alignof(memory_arena)` bytes -- otherwise it is undefined
  /// behavior
  ///////////////////////////////////////////////////////////////////////////
  class memory_arena_cache
  {
    //--------------------------------------------------------------------------
    // Constructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Default constructs a arena cache
    memory_arena_cache() noexcept;

    /// \brief Move-constructs a memory_arena_cache from an existing one
    ///
    /// \param other the other memory_arena_cache to move
    memory_arena_cache(memory_arena_cache&& other) noexcept = default;

    // Deleted copy constructor
    memory_arena_cache(const memory_arena_cache& other) = delete;

    //--------------------------------------------------------------------------

    /// \brief Moves the contents of \p other to this
    ///
    /// \param other the other arena to move
    /// \return reference to `(*this)`
    memory_arena_cache& operator=(memory_arena_cache&& other) noexcept = default;

    // Deleted copy assignment
    memory_arena_cache& operator=(const memory_arena_cache& other) = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Returns whether or not this memory_arena_cache is empty
    ///
    /// \return `true` if this memory_arena_cache is empty, `false` otherwise
    bool empty() const noexcept;

    /// \brief Returns the number of memory_arenas in this cache
    ///
    /// This function is lazily computed, and is written with `O(n)`
    /// complexity
    ///
    /// \return the number of memory_arenas in this cache
    std::size_t size() const noexcept;

    /// \brief Returns the size in bytes of all the memory_arenas in the
    ///        cache
    /// \return the size in bytes
    std::size_t size_bytes() const noexcept;

    /// \brief Evaluates whether \p p exists within this memory_arena_cache
    ///
    /// \param p the pointer to evaluate
    /// \return true whether \p p
    bool contains(const void* p) const noexcept;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Views the front memory arena of this cache
    ///
    /// \pre !empty()
    ///
    /// \note It is undefined behaviour to invoke this function if the cache
    ///       is empty
    ///
    /// \return the front cache entry
    const memory_arena& peek() const noexcept;

    /// \brief Requests an arena from the current arena cache.
    ///
    /// If one can be provided, the arena is released from this cache and is
    /// now owned by the accessor
    ///
    /// \return a memory arena, if available, or a null arena if unable
    memory_arena request() noexcept;

    /// \brief Acquires an entry from another cache
    ///
    /// \param other the cache to acquire the entry from
    void steal(memory_arena_cache& other) noexcept;

    /// \brief Stores an arena inside this memory_arena_cache
    ///
    /// \pre `arena.data()` points to memory that is aligned to at least
    ///      `alignof(memory_arena)` bytes.
    ///
    /// \pre `arena` points to a valid memory_arena
    ///
    /// \param arena the arena to store
    void store(memory_arena arena) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    memory_arena m_head; ///< The head of the cache

    template <typename Fn>
    void for_each_arena(Fn&& fn) const noexcept;

    friend bool operator==(const memory_arena_cache&,
                           const memory_arena_cache&) noexcept;
  };

  //============================================================================
  // non-member functions : class : memory_arena_cache
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  bool operator==(const memory_arena_cache& lhs,
                  const memory_arena_cache& rhs) noexcept;
  bool operator!=(const memory_arena_cache& lhs,
                  const memory_arena_cache& rhs) noexcept;

} // namespace alloy::extra

//==============================================================================
// inline definition : non-member functions : class : memory_arena_cache
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline bool alloy::extra::operator==(const memory_arena_cache& lhs,
                                     const memory_arena_cache& rhs)
  noexcept
{
  return lhs.m_head == rhs.m_head;
}


inline bool alloy::extra::operator!=(const memory_arena_cache& lhs,
                                     const memory_arena_cache& rhs)
  noexcept
{
  return !(lhs == rhs);
}


#endif /* ALLOY_EXTRA_ALLOCATORS_MEMORY_ARENA_CACHE_HPP */

