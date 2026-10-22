// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/extra/allocators/memory_arena_cache.hpp"

#include "alloy/extra/allocators/pointer_util.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

#include <cstddef>

//==============================================================================
// definitions : class : memory_arena_cache
//==============================================================================

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

alloy::extra::memory_arena_cache::memory_arena_cache() noexcept
  : m_head{}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::extra::memory_arena_cache::empty() const noexcept -> bool
{
  return m_head.data() == nullptr;
}


auto alloy::extra::memory_arena_cache::size() const noexcept -> std::size_t
{
  auto result = std::size_t{0u};

  for_each_arena([&](const memory_arena& arena){
    core::compiler::unused(arena);

    ++result;
  });

  return result;
}


auto alloy::extra::memory_arena_cache::size_bytes()
  const noexcept -> std::size_t
{
  auto result = std::size_t{0u};

  for_each_arena([&](const memory_arena& arena){
    result += arena.size();
  });

  return result;
}


auto alloy::extra::memory_arena_cache::contains(const void* p)
  const noexcept -> bool
{
  auto found = false;

  for_each_arena([&](const memory_arena& arena){
    found = found || arena.contains(p);
  });

  return found;
}

//------------------------------------------------------------------------------
// Element Access
//------------------------------------------------------------------------------

auto alloy::extra::memory_arena_cache::peek()
  const noexcept -> const memory_arena&
{
  ALLOY_ASSERT(m_head.data() != nullptr);

  return m_head;
}


auto alloy::extra::memory_arena_cache::request() noexcept -> memory_arena
{
  auto arena = m_head;
  if (m_head.data() != nullptr) {
    auto* p = arena.data();
    p = core::compiler::assume_aligned<alignof(memory_arena)>(p);

    m_head = (*static_cast<memory_arena*>(p));
  }
  return arena;
}


auto alloy::extra::memory_arena_cache::steal(memory_arena_cache& other)
  noexcept -> void
{
  const auto arena = other.request();

  if (arena) {
    store(arena);
  }
}

template <typename Fn>
auto alloy::extra::memory_arena_cache::for_each_arena(Fn&& fn)
  const noexcept -> void
{
  auto* arena = &m_head;
  while (arena->data() != nullptr) {

    fn(*arena);

    auto* p = arena->data();
    p = core::compiler::assume_aligned<alignof(memory_arena)>(p);

    arena = static_cast<const memory_arena*>(p);
  }
}


auto alloy::extra::memory_arena_cache::store(memory_arena arena)
  noexcept -> void
{
  ALLOY_ASSERT(arena.data() != nullptr);
  ALLOY_ASSERT(arena.size() >= sizeof(memory_arena));
  ALLOY_ASSERT(pointer_util::align_of(arena.data()) >= alignof(memory_arena));

  auto* p = arena.data();
  p = core::compiler::assume_aligned<alignof(memory_arena)>(p);

  // Link entries by storing the contents of the previous arena in this new
  // arena
  ::new (p) memory_arena{m_head};

  m_head = arena;
}



