// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file handle_pool.hpp
///
/// \brief This header defines the storage that render devices use to map
///        resource handles to the objects they create
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_RESOURCE_HANDLE_POOL_HPP
#define ALLOY_RENDER_RESOURCE_HANDLE_POOL_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/resource/resource_handle.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/optional.hpp"

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint32_t
#include <limits>      // std::numeric_limits
#include <type_traits> // std::is_nothrow_move_constructible_v
#include <utility>     // std::move, std::exchange

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A generational pool that owns objects and identifies each of them
  ///        by a `resource_handle`
  ///
  /// Each handle encodes the slot holding its object along with the
  /// generation of that slot at the time of insertion. Erasing an object
  /// advances the generation of its slot, so any handle to an erased object
  /// stays stale even after the slot is reused.
  ///
  /// \tparam Tag the family of handle that identifies the objects
  /// \tparam Object the type of object stored for each handle
  //////////////////////////////////////////////////////////////////////////////
  template <typename Tag, typename Object>
  class handle_pool
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using handle_type = resource_handle<Tag>;

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    /// \brief The maximum number of objects that may be alive at once
    inline static constexpr auto max_size = std::size_t{1u} << 20u;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an empty pool that allocates from the default
    ///        allocator
    handle_pool() noexcept = default;

    /// \brief Constructs an empty pool that allocates from \p alloc
    ///
    /// \param alloc the allocator to allocate from
    explicit handle_pool(core::allocator alloc) noexcept;

    handle_pool(handle_pool&&) = default;
    handle_pool(const handle_pool&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(handle_pool&&) -> handle_pool& = default;
    auto operator=(const handle_pool&) -> handle_pool& = delete;

    //--------------------------------------------------------------------------
    // Capacity
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the number of objects alive in this pool
    ///
    /// \return the number of objects
    auto size() const noexcept -> std::size_t;

    /// \brief Queries whether this pool holds no objects
    ///
    /// \return `true` if no objects are alive in this pool
    auto empty() const noexcept -> bool;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Stores \p object in this pool
    ///
    /// \pre `size() < max_size`
    /// \param object the object to store
    /// \return the handle that identifies the stored object
    auto insert(Object object) -> handle_type;

    /// \brief Removes the object identified by \p handle from this pool
    ///
    /// Removal never allocates, so that it may be used where failure cannot
    /// be tolerated, such as when disposing of a resource.
    ///
    /// \param handle the handle of the object to remove
    /// \return the removed object, or null if \p handle is null or stale
    auto erase(handle_type handle)
      noexcept(std::is_nothrow_move_constructible_v<Object>)
      -> core::optional<Object>;

    //--------------------------------------------------------------------------
    // Element Access
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Finds the object identified by \p handle
    ///
    /// \param handle the handle of the object to find
    /// \return the object, or `nullptr` if \p handle is null or stale
    auto find(handle_type handle) noexcept -> Object*;
    auto find(handle_type handle) const noexcept -> const Object*;
    /// \}

    //--------------------------------------------------------------------------
    // Private Static Members
    //--------------------------------------------------------------------------
  private:

    inline static constexpr auto index_bits = 20u;
    inline static constexpr auto index_mask = (1u << index_bits) - 1u;
    inline static constexpr auto max_generation = (1u << (32u - index_bits)) - 1u;
    inline static constexpr auto no_free_slot = std::numeric_limits<std::uint32_t>::max();

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A slot of the pool, which holds an object when it is occupied
    ///        and links to the next free slot when it is not
    ///////////////////////////////////////////////////////////////////////////
    struct slot
    {
      std::uint32_t generation;
      std::uint32_t next_free;
      core::optional<Object> object;
    };

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::vector<slot> m_slots;
    std::uint32_t m_free_head = no_free_slot;
    std::size_t m_size = 0u;

    //--------------------------------------------------------------------------
    // Private Utilities
    //--------------------------------------------------------------------------
  private:

    static auto make_handle(std::uint32_t index, std::uint32_t generation)
      noexcept -> handle_type;

    auto find_slot(handle_type handle) noexcept -> slot*;
  };

} // namespace alloy::render

//==============================================================================
// definitions : class : handle_pool
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Assignment
//------------------------------------------------------------------------------

template <typename Tag, typename Object>
inline
alloy::render::handle_pool<Tag, Object>::handle_pool(core::allocator alloc)
  noexcept
  : m_slots{core::stl_allocator_adapter<slot>{alloc}}
{

}

//------------------------------------------------------------------------------
// Capacity
//------------------------------------------------------------------------------

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::size()
  const noexcept -> std::size_t
{
  return m_size;
}

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::empty() const noexcept -> bool
{
  return size() == 0u;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::insert(Object object)
  -> handle_type
{
  ALLOY_ASSERT(size() < max_size, "handle_pool exhausted");

  if (m_free_head == no_free_slot) {
    const auto index = static_cast<std::uint32_t>(m_slots.size());
    m_slots.push_back(slot{1u, no_free_slot, std::move(object)});
    ++m_size;
    return make_handle(index, 1u);
  }

  const auto index = m_free_head;
  auto& entry = m_slots[index];
  entry.object.emplace(std::move(object));
  m_free_head = entry.next_free;
  ++m_size;
  return make_handle(index, entry.generation);
}

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::erase(handle_type handle)
  noexcept(std::is_nothrow_move_constructible_v<Object>)
  -> core::optional<Object>
{
  auto* const entry = find_slot(handle);
  if (entry == nullptr || !entry->object.has_value()) {
    return core::null;
  }

  auto result = std::move(entry->object);
  entry->object.reset();

  // Generation 0 is never issued, so that no handle encodes the null value.
  entry->generation = (entry->generation == max_generation)
    ? 1u
    : entry->generation + 1u;
  entry->next_free = std::exchange(m_free_head, handle.m_value & index_mask);
  --m_size;

  return result;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::find(handle_type handle)
  noexcept -> Object*
{
  auto* const entry = find_slot(handle);
  if (entry == nullptr || !entry->object.has_value()) {
    return nullptr;
  }
  return &*entry->object;
}

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::find(handle_type handle)
  const noexcept -> const Object*
{
  return const_cast<handle_pool*>(this)->find(handle);
}

//------------------------------------------------------------------------------
// Private Utilities
//------------------------------------------------------------------------------

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::make_handle(
  std::uint32_t index,
  std::uint32_t generation
) noexcept -> handle_type
{
  return handle_type{(generation << index_bits) | index};
}

template <typename Tag, typename Object>
inline
auto alloy::render::handle_pool<Tag, Object>::find_slot(handle_type handle)
  noexcept -> slot*
{
  const auto index = handle.m_value & index_mask;
  const auto generation = handle.m_value >> index_bits;

  if (handle.is_null() || index >= m_slots.size()) {
    return nullptr;
  }
  auto& entry = m_slots[index];
  if (entry.generation != generation) {
    return nullptr;
  }
  return &entry;
}

#endif /* ALLOY_RENDER_RESOURCE_HANDLE_POOL_HPP */
