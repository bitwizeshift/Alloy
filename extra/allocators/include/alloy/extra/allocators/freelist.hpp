/*****************************************************************************
 * \file freelist.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019, 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_EXTRA_ALLOCATORS_FREELIST_HPP
#define ALLOY_EXTRA_ALLOCATORS_FREELIST_HPP

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
    freelist& operator=(freelist&& other) noexcept = default;

    // Deleted copy construction
    freelist& operator=(const freelist& other) = delete;

    //----------------------------------------------------------------------
    // Observers
    //----------------------------------------------------------------------
  public:

    /// \brief Returns whether or not this memory_block_cache is empty
    ///
    /// \return `true` if this memory_block_cache is empty, `false` otherwise
    bool empty() const noexcept;

    /// \brief Returns the number of entries in this freelist
    ///
    /// \return the number of entries in this freelist
    size_type size() const noexcept;

    //----------------------------------------------------------------------
    // Modifiers
    //----------------------------------------------------------------------
  public:

    /// \brief Empties the freelist cache
    void clear();

    //----------------------------------------------------------------------
    // Caching
    //----------------------------------------------------------------------
  public:

    /// \brief Requests raw memory from the freelist, if any exists
    ///
    /// The size of the returned instance is not known to the freelist
    ///
    /// \return pointer to memory, if it exists
    void* request() noexcept;

    /// \brief Steals raw memory from an existing freelist
    ///
    /// \param other the other freelist to steal an entry from
    void steal(freelist& other) noexcept;

    /// \brief Stores raw memory into this freelist
    ///
    /// \pre The pointer \p p must point to memory of at least
    ///      `sizeof(void*)` bytes, and must be suitably aligned to support
    ///      pointer types.
    ///
    /// \param p pointer to the raw memory to store
    void store(pointer p) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    pointer m_head;

    friend bool operator==(const freelist&, const freelist&) noexcept;
  };

  //============================================================================
  // non-member functions : class : freelist
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  bool operator==(const freelist& lhs, const freelist& rhs) noexcept;
  bool operator!=(const freelist& lhs, const freelist& rhs) noexcept;

} // namespace alloy::extra

//==============================================================================
// inline definitions : class : freelist
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline constexpr alloy::extra::freelist::freelist()
  noexcept
  : m_head{nullptr}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

inline bool alloy::extra::freelist::empty()
  const noexcept
{
  return m_head == nullptr;
}


inline alloy::extra::freelist::size_type alloy::extra::freelist::size()
  const noexcept
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

inline void alloy::extra::freelist::clear()
{
  m_head = nullptr;
}

//------------------------------------------------------------------------------
// Caching
//------------------------------------------------------------------------------

inline void* alloy::extra::freelist::request()
  noexcept
{
  auto* const p = m_head;

  if (m_head != nullptr) {
    m_head = (*static_cast<void**>(m_head));
  }
  return p;
}


inline void alloy::extra::freelist::steal(freelist& other)
  noexcept
{
  auto* const p = other.request();

  if (p != nullptr) {
    store(p);
  }
}


inline void alloy::extra::freelist::store(pointer p)
  noexcept
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

inline bool alloy::extra::operator==(const freelist& lhs, const freelist& rhs)
  noexcept
{
  return lhs.m_head == rhs.m_head;
}


inline bool alloy::extra::operator!=(const freelist& lhs, const freelist& rhs)
  noexcept
{
  return !(lhs==rhs);
}

#endif /* ALLOY_EXTRA_ALLOCATORS_FREELIST_HPP */

