// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file freelist.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_EXTRA_ALLOCATORS_FREELIST_HPP
#define ALLOY_EXTRA_ALLOCATORS_FREELIST_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/allocators/pointer_util.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"

#include <cstddef> // std::size_t
#include <new>     // placement new

namespace alloy::extra {

  ///////////////////////////////////////////////////////////////////////////
  /// \brief A simple freelist that uses intrusive pointers inside raw memory
  ///
  /// A freelist is a non-owning intrusive linked-list of pointers without
  /// any associated data type
  ///////////////////////////////////////////////////////////////////////////
  class freelist
  {
    //-----------------------------------------------------------------------
    // Public Member Types
    //-----------------------------------------------------------------------
  public:

    using pointer = void*;
    using const_pointer = const void*;
    using size_type = std::size_t;

    //-----------------------------------------------------------------------
    // Constructors
    //-----------------------------------------------------------------------
  public:

    /// \brief Default constructs an empty freelist
    constexpr freelist() noexcept;

    /// \brief Move-constructs a freelist from an existing freelist
    ///
    /// \param other the other freelist to move
    freelist(freelist&& other) noexcept = default;

    // Deleted copy construction
    freelist(const freelist& other) = delete;

    //-----------------------------------------------------------------------

    /// \brief Move-assigns a freelist from an existing freelist
    ///
    /// \param other the other freelist to move
    /// \return reference to `(*this)`
    auto operator=(freelist&& other) noexcept -> freelist& = default;

    // Deleted copy construction
    auto operator=(const freelist& other) -> freelist& = delete;

    //----------------------------------------------------------------------
    // Observers
    //----------------------------------------------------------------------
  public:

    /// \brief Returns whether or not this memory_block_cache is empty
    ///
    /// \return `true` if this memory_block_cache is empty, `false` otherwise
    auto empty() const noexcept -> bool;

    /// \brief Returns the number of entries in this freelist
    ///
    /// \return the number of entries in this freelist
    auto size() const noexcept -> size_type;

    //----------------------------------------------------------------------
    // Modifiers
    //----------------------------------------------------------------------
  public:

    /// \brief Empties the freelist cache
    auto clear() -> void;

    //----------------------------------------------------------------------
    // Caching
    //----------------------------------------------------------------------
  public:

    /// \brief Requests raw memory from the freelist, if any exists
    ///
    /// The size of the returned instance is not known to the freelist
    ///
    /// \return pointer to memory, if it exists
    auto request() noexcept -> void*;

    /// \brief Steals raw memory from an existing freelist
    ///
    /// \param other the other freelist to steal an entry from
    auto steal(freelist& other) noexcept -> void;

    /// \brief Stores raw memory into this freelist
    ///
    /// \pre The pointer \p p must point to memory of at least
    ///      `sizeof(void*)` bytes, and must be suitably aligned to support
    ///      pointer types.
    ///
    /// \param p pointer to the raw memory to store
    auto store(pointer p) noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    pointer m_head;

    friend auto operator==(const freelist&, const freelist&) noexcept -> bool;
  };

  //============================================================================
  // non-member functions : class : freelist
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  auto operator==(const freelist& lhs, const freelist& rhs) noexcept -> bool;
  auto operator!=(const freelist& lhs, const freelist& rhs) noexcept -> bool;

} // namespace alloy::extra

//==============================================================================
// inline definitions : class : freelist
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr
alloy::extra::freelist::freelist() noexcept
  : m_head{nullptr}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

inline
auto alloy::extra::freelist::empty() const noexcept -> bool
{
  return m_head == nullptr;
}


inline
auto alloy::extra::freelist::size() const noexcept -> size_type
{
  auto size = size_type{0u};
  auto* p = m_head;
  while (p != nullptr) {
    ++size;
    p = (*static_cast<void**>(p));
  }
  return size;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

inline
auto alloy::extra::freelist::clear() -> void
{
  m_head = nullptr;
}

//------------------------------------------------------------------------------
// Caching
//------------------------------------------------------------------------------

inline
auto alloy::extra::freelist::request() noexcept -> void*
{
  auto* const p = m_head;

  if (m_head != nullptr) {
    m_head = (*static_cast<void**>(m_head));
  }
  return p;
}


inline
auto alloy::extra::freelist::steal(freelist& other) noexcept -> void
{
  auto* const p = other.request();

  if (p != nullptr) {
    store(p);
  }
}


inline
auto alloy::extra::freelist::store(pointer p) noexcept -> void
{
  ALLOY_ASSERT(p != nullptr);
  ALLOY_ASSERT(alignof(pointer) <= pointer_util::align_of(p));

  p = core::compiler::assume_aligned<alignof(pointer)>(p);

  ::new (p) pointer{m_head};
  m_head = p;
}

//==============================================================================
// inline definitions : class : freelist
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline
auto alloy::extra::operator==(const freelist& lhs, const freelist& rhs)
  noexcept -> bool
{
  return lhs.m_head == rhs.m_head;
}


inline
auto alloy::extra::operator!=(const freelist& lhs, const freelist& rhs)
  noexcept -> bool
{
  return !(lhs==rhs);
}

#endif /* ALLOY_EXTRA_ALLOCATORS_FREELIST_HPP */

