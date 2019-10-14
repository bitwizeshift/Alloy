#include "alloy/extra/allocators/memory_arena_cache.hpp"

#include "alloy/extra/allocators/pointer_util.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"

#include <new>

//==============================================================================
// definitions : class : memory_arena_cache
//==============================================================================

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------

alloy::extra::memory_arena_cache::memory_arena_cache()
  noexcept
  : m_head{}
{

}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

bool alloy::extra::memory_arena_cache::empty()
  const noexcept
{
  return m_head.data() == nullptr;
}


std::size_t alloy::extra::memory_arena_cache::size()
  const noexcept
{
  auto result = std::size_t{0u};

  for_each_arena([&](const memory_arena& arena){
    core::compiler::unused(arena);

    ++result;
  });

  return result;
}


std::size_t alloy::extra::memory_arena_cache::size_bytes()
  const noexcept
{
  auto result = std::size_t{0u};

  for_each_arena([&](const memory_arena& arena){
    result += arena.size();
  });

  return result;
}


bool alloy::extra::memory_arena_cache::contains(const void* p)
  const noexcept
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

const alloy::extra::memory_arena& alloy::extra::memory_arena_cache::peek()
  const noexcept
{
  ALLOY_ASSERT(m_head.data() != nullptr);

  return m_head;
}


alloy::extra::memory_arena alloy::extra::memory_arena_cache::request()
  noexcept
{
  auto arena = m_head;
  if (m_head.data() != nullptr) {
    auto* p = arena.data();
    p = core::compiler::assume_aligned<alignof(memory_arena)>(p);

    m_head = (*static_cast<memory_arena*>(p));
  }
  return arena;
}


void alloy::extra::memory_arena_cache::steal(memory_arena_cache& other)
  noexcept
{
  const auto arena = other.request();

  if (arena) {
    store(arena);
  }
}

template <typename Fn>
void alloy::extra::memory_arena_cache::for_each_arena(Fn&& fn)
  const noexcept
{
  auto* arena = &m_head;
  while (arena->data() != nullptr) {

    fn(*arena);

    auto* p = arena->data();
    p = core::compiler::assume_aligned<alignof(memory_arena)>(p);

    arena = static_cast<const memory_arena*>(p);
  }
}


void alloy::extra::memory_arena_cache::store(memory_arena arena)
  noexcept
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



