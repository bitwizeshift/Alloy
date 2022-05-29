/*****************************************************************************
 * \file memory_resource.hpp
 *
 * \brief This header contains the definition for memory_resource
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019-2020, 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_MEMORY_MEMORY_RESOURCE_HPP
#define ALLOY_CORE_MEMORY_MEMORY_RESOURCE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp"

#include <cstddef> // std::max_align_t, std::size_t, etc
#include <limits>  // std::numeric_limits
#include <new>     // std::align_val_t
#include <string_view> // std::string_view

namespace alloy::core {

  //===========================================================================
  // class : memory_resource
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A resource that is capable of allocating memory as bytes for
  ///        consumption
  ///
  /// Memory resources expose a way to allocate memory with offsets to allow
  /// for strong composability with other resources.
  ///
  /// Memory resources are immovable and uncopyable, since they are the primary
  /// owners of the memory in which they distribute. This contrasts the concept
  /// of a memory allocator, which is actually just a handle to a resource that
  /// distributes the memory.
  ///
  /// This concept is similar to std::memory_resource, but deviates in
  /// underlying philosophies in the following ways:
  ///
  /// * All allocations may be offset by a number of bytes
  /// * Resources have the ability to expand the number of bytes it has
  ///   allocated, without requiring a new allocation invocation
  /// * Resources can convey their constraints, and recommended inputs
  /////////////////////////////////////////////////////////////////////////////
  class memory_resource
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    static constexpr auto default_align  = std::align_val_t{
      alignof(std::max_align_t)
    };

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    memory_resource() noexcept = default;
    memory_resource(memory_resource&&) = delete;
    memory_resource(const memory_resource&) = delete;

    memory_resource& operator=(memory_resource&&) = delete;
    memory_resource& operator=(const memory_resource&) = delete;

    //-------------------------------------------------------------------------
    // Allocation
    //-------------------------------------------------------------------------
  public:

    /// \brief Allocates \p bytes at the desired alignment
    ///
    /// \pre \p bytes must be non-zero
    /// \pre \p align must be power-of-two
    ///
    /// \param bytes the number of bytes to allocate
    /// \param align the alignment for the memory to be aligned to
    /// \return a pointer to memory of size \p bytes aligned to \p align
    [[nodiscard]]
    void* allocate(std::size_t bytes,
                   std::align_val_t align = default_align) noexcept;

    /// \brief Deallocates memory previously allocated with \p allocate
    ///
    /// \note deallocate does not require the offset previously requested in
    ///       the call to allocate, since the pointer \p p must be readjusted
    ///       to be the value previously returned to by a call to allocate,
    ///       which contains that offset.
    ///
    /// \pre \p p must be non-null
    /// \pre \p bytes must be non-zero
    /// \pre \p align must be power-of-two
    ///
    /// \param p the pointer to memory to deallocate
    /// \param bytes the number of bytes previously requested
    /// \param align the alignment previously requested
    void deallocate(void* p,
                    std::size_t bytes,
                    std::align_val_t align = default_align) noexcept;

    /// \brief Resizes an allocation previously allocated with \p allocate
    ///
    /// \pre \p p must be non-null
    /// \pre \p bytes must be non-zero
    ///
    /// \param p the pointer to memory previously allocated with allocate
    /// \param bytes the new size for the memory allocation, in bytes
    /// \return `true` if \p p was able to be resized
    bool resize_allocation(void* p,
                           std::size_t bytes) noexcept;

    //-------------------------------------------------------------------------
    // Helpers
    //-------------------------------------------------------------------------
  public:

    /// \brief Computes the recommended allocation size when requesting
    ///        \p bytes with the specified \p alignment
    ///
    /// This provides a hook to allow the memory resource to tell the upstream
    /// what an optimal size should be for a given request. For example, a pool
    /// resource can recommend the pool size, since that would be the
    /// fixed size of allocation that it can produce.
    ///
    /// The default implementation will always round \p bytes up to an
    /// increment of \p align
    ///
    /// \note Consumers of this function should retain the returned recommended
    ///       allocation size for use in the deallocate function at a later time.
    ///
    /// \param bytes the number of bytes being requested
    /// \param align the default alignment
    /// \return the optimal number of bytes to request
    [[nodiscard]]
    std::size_t recommended_allocation_size_for(std::size_t bytes,
                                                std::align_val_t align = default_align) const noexcept;

    /// \brief Returns the minimum size allocation that this memory resource
    ///        can allocate
    ///
    /// \note The default for this is '1'
    ///
    /// \return the minimum size
    [[nodiscard]]
    std::size_t min_allocation_size() const noexcept;

    /// \brief Returns the maximum size allocation that this memory resource
    ///        can allocate
    ///
    /// \note The default for this is std::numeric_limits<std::size_t>::max()
    ///
    /// \return the maximum size
    [[nodiscard]]
    std::size_t max_allocation_size() const noexcept;

    /// \brief Retrieves the name of this resource
    ///
    /// \return The name of this resource
    [[nodiscard]]
    std::string_view name() const noexcept;

    //-------------------------------------------------------------------------
    // Protected Destructor
    //-------------------------------------------------------------------------
  protected:

    ~memory_resource() = default;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Allocation
    //-------------------------------------------------------------------------
  private:

    virtual void* do_allocate(std::size_t bytes,
                              std::align_val_t align) noexcept = 0;

    virtual void do_deallocate(void* p,
                               std::size_t bytes,
                               std::align_val_t align) noexcept = 0;

    virtual bool do_resize_allocation(void* p,
                                      std::size_t bytes) noexcept;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Helpers
    //-------------------------------------------------------------------------
  private:

    virtual std::size_t
      get_recommended_allocation_size_for(std::size_t bytes,
                                          std::align_val_t align) const noexcept;

    virtual std::size_t
      get_min_allocation_size() const noexcept;

    virtual std::size_t
      get_max_allocation_size() const noexcept;

    virtual std::string_view get_name() const noexcept;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : memory_resource
//=============================================================================

//-----------------------------------------------------------------------------
// Allocations
//-----------------------------------------------------------------------------


inline void*
  alloy::core::memory_resource::allocate(std::size_t bytes,
                                         std::align_val_t align)
  noexcept
{
  ALLOY_ASSERT(bytes != 0, "Allocation size must be non-zero");
  ALLOY_ASSERT(
    (static_cast<std::size_t>(align) & ~(static_cast<std::size_t>(align) - 1)) == static_cast<std::size_t>(align),
    "Alignment must be power-of-two"
  );

  return do_allocate(bytes, align);
}


inline void
  alloy::core::memory_resource::deallocate(void* p,
                                           std::size_t bytes,
                                           std::align_val_t align)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(bytes != 0, "Allocation size must be non-zero");
  ALLOY_ASSERT(
    (static_cast<std::size_t>(align) & ~(static_cast<std::size_t>(align) - 1)) == static_cast<std::size_t>(align),
    "Alignment must be power-of-two"
  );

  return do_deallocate(p, bytes, align);
}


inline bool
  alloy::core::memory_resource::resize_allocation(void* p,
                                                  std::size_t bytes)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(bytes != 0, "Resize bytes must be non-zero");

  return do_resize_allocation(p, bytes);
}


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------


inline std::size_t
  alloy::core::memory_resource::recommended_allocation_size_for(std::size_t bytes,
                                                                std::align_val_t align)
  const noexcept
{
  return get_recommended_allocation_size_for(bytes, align);
}


inline std::size_t
  alloy::core::memory_resource::min_allocation_size()
  const noexcept
{
  return get_min_allocation_size();
}


inline std::size_t
  alloy::core::memory_resource::max_allocation_size()
  const noexcept
{
  return get_max_allocation_size();
}


inline std::string_view
  alloy::core::memory_resource::name()
  const noexcept
{
  return get_name();
}


//-----------------------------------------------------------------------------
// Virtual Hooks : Allocation
//-----------------------------------------------------------------------------


inline bool
  alloy::core::memory_resource::do_resize_allocation(void* p,
                                                     std::size_t bytes)
  noexcept
{
  (void) p;
  (void) bytes;

  return false;
}


//-----------------------------------------------------------------------------
// Virtual Hooks : Helpers
//-----------------------------------------------------------------------------


inline std::size_t
  alloy::core::memory_resource::get_recommended_allocation_size_for(std::size_t bytes,
                                                                    std::align_val_t align)
  const noexcept
{
  const auto step = bytes + static_cast<std::size_t>(align) - 1;
  const auto mask = ~(static_cast<std::size_t>(align) - 1);
  return step & mask;
}


inline std::size_t
  alloy::core::memory_resource::get_min_allocation_size()
  const noexcept
{
  return 1u;
}


inline std::size_t
  alloy::core::memory_resource::get_max_allocation_size()
  const noexcept
{
  return std::numeric_limits<std::size_t>::max();
}

inline std::string_view
  alloy::core::memory_resource::get_name()
  const noexcept
{
  return "unnamed memory resource";
}

#endif /* ALLOY_CORE_MEMORY_MEMORY_RESOURCE_HPP */

