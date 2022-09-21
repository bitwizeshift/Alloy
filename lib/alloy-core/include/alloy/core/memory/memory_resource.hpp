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
#include <new>     // std::size_t
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

    static constexpr auto default_align = alignof(std::max_align_t);

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    memory_resource() noexcept = default;
    memory_resource(memory_resource&&) = delete;
    memory_resource(const memory_resource&) = delete;

    auto operator=(memory_resource&&) -> memory_resource& = delete;
    auto operator=(const memory_resource&) -> memory_resource& = delete;

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
    auto allocate(std::size_t bytes, std::size_t align = default_align)
      noexcept -> void*;

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
    auto deallocate(void* p, std::size_t bytes, std::size_t align = default_align)
      noexcept -> void;

    /// \brief Resizes an allocation previously allocated with \p allocate
    ///
    /// \pre \p p must be non-null
    /// \pre \p bytes must be non-zero
    ///
    /// \param p the pointer to memory previously allocated with allocate
    /// \param bytes the new size for the memory allocation, in bytes
    /// \return `true` if \p p was able to be resized
    auto resize_allocation(void* p, std::size_t bytes)
      noexcept -> bool;

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
    auto recommended_allocation_size_for(
      std::size_t bytes,
      std::size_t align = default_align
    ) const noexcept -> std::size_t;

    /// \brief Returns the minimum size allocation that this memory resource
    ///        can allocate
    ///
    /// \note The default for this is '1'
    ///
    /// \return the minimum size
    [[nodiscard]]
    auto min_allocation_size() const noexcept -> std::size_t;

    /// \brief Returns the maximum size allocation that this memory resource
    ///        can allocate
    ///
    /// \note The default for this is std::numeric_limits<std::size_t>::max()
    ///
    /// \return the maximum size
    [[nodiscard]]
    auto max_allocation_size() const noexcept -> std::size_t;

    /// \brief Retrieves the name of this resource
    ///
    /// \return The name of this resource
    [[nodiscard]]
    auto name() const noexcept -> std::string_view;

    //-------------------------------------------------------------------------
    // Protected Destructor
    //-------------------------------------------------------------------------
  protected:

    ~memory_resource() = default;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Allocation
    //-------------------------------------------------------------------------
  private:
    virtual auto do_allocate(std::size_t bytes, std::size_t align)
      noexcept -> void* = 0;

    virtual auto do_deallocate(
      void* p,
      std::size_t bytes,
      std::size_t align
    ) noexcept -> void = 0;

    virtual auto do_resize_allocation(void* p, std::size_t bytes)
      noexcept -> bool;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Helpers
    //-------------------------------------------------------------------------
  private:
    [[nodiscard]]
    virtual auto get_recommended_allocation_size_for(
      std::size_t bytes,
      std::size_t align
    ) const noexcept -> std::size_t;

    [[nodiscard]]
    virtual auto get_min_allocation_size()
      const noexcept -> std::size_t;

    [[nodiscard]]
    virtual auto get_max_allocation_size()
      const noexcept -> std::size_t;

    [[nodiscard]]
    virtual auto get_name()
      const noexcept -> std::string_view;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : memory_resource
//=============================================================================

//-----------------------------------------------------------------------------
// Allocations
//-----------------------------------------------------------------------------

inline
auto alloy::core::memory_resource::allocate(std::size_t bytes, std::size_t align)
  noexcept -> void*
{
  ALLOY_ASSERT(bytes != 0, "Allocation size must be non-zero");
  ALLOY_ASSERT(
    (static_cast<std::size_t>(align) & ~(static_cast<std::size_t>(align) - 1)) == static_cast<std::size_t>(align),
    "Alignment must be power-of-two"
  );

  return do_allocate(bytes, align);
}


inline
auto alloy::core::memory_resource::deallocate(
  void* p,
  std::size_t bytes,
  std::size_t align
) noexcept -> void
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(bytes != 0, "Allocation size must be non-zero");
  ALLOY_ASSERT(
    (static_cast<std::size_t>(align) & ~(static_cast<std::size_t>(align) - 1)) == static_cast<std::size_t>(align),
    "Alignment must be power-of-two"
  );

  return do_deallocate(p, bytes, align);
}

inline
auto alloy::core::memory_resource::resize_allocation(void* p, std::size_t bytes)
  noexcept -> bool
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");
  ALLOY_ASSERT(bytes != 0, "Resize bytes must be non-zero");

  return do_resize_allocation(p, bytes);
}


//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

inline
auto alloy::core::memory_resource::recommended_allocation_size_for(
  std::size_t bytes,
  std::size_t align
) const noexcept -> std::size_t
{
  return get_recommended_allocation_size_for(bytes, align);
}

inline
auto alloy::core::memory_resource::min_allocation_size()
  const noexcept -> std::size_t
{
  return get_min_allocation_size();
}

inline
auto alloy::core::memory_resource::max_allocation_size()
  const noexcept -> std::size_t
{
  return get_max_allocation_size();
}

inline auto
  alloy::core::memory_resource::name() const noexcept -> std::string_view
{
  return get_name();
}


//-----------------------------------------------------------------------------
// Virtual Hooks : Allocation
//-----------------------------------------------------------------------------

inline
auto alloy::core::memory_resource::do_resize_allocation(void* p, std::size_t bytes)
  noexcept -> bool
{
  compiler::unused(p, bytes);

  return false;
}


//-----------------------------------------------------------------------------
// Virtual Hooks : Helpers
//-----------------------------------------------------------------------------

inline
auto alloy::core::memory_resource::get_recommended_allocation_size_for(
  std::size_t bytes,
  std::size_t align
) const noexcept -> std::size_t
{
  const auto step = bytes + static_cast<std::size_t>(align) - 1;
  const auto mask = ~(static_cast<std::size_t>(align) - 1);
  return step & mask;
}

inline
auto alloy::core::memory_resource::get_min_allocation_size()
  const noexcept -> std::size_t
{
  return 1u;
}

inline
auto alloy::core::memory_resource::get_max_allocation_size()
  const noexcept -> std::size_t
{
  return std::numeric_limits<std::size_t>::max();
}

inline
auto alloy::core::memory_resource::get_name()
  const noexcept -> std::string_view
{
  return "unnamed memory resource";
}

#endif /* ALLOY_CORE_MEMORY_MEMORY_RESOURCE_HPP */
