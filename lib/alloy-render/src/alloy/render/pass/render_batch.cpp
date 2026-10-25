// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/pass/render_batch.hpp"

#include "alloy/render/model/model_spec.hpp"

#include <cstddef> // std::byte
#include <cstdint> // std::uint32_t
#include <span>    // std::span

//==============================================================================
// class : render_batch
//==============================================================================

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::render_batch::entries()
  const noexcept -> std::span<const batch_entry>
{
  return m_entries;
}

auto alloy::render::render_batch::instance_data()
  const noexcept -> std::span<const std::byte>
{
  return m_instance_data;
}

auto alloy::render::render_batch::empty() const noexcept -> bool
{
  return m_entries.empty();
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::render::render_batch::add(
  model_handle model,
  std::span<const std::byte> instances,
  std::uint32_t count
) -> void
{
  if (count == 0u) {
    return;
  }
  m_entries.push_back(
    batch_entry{
      .model = model,
      .offset = m_instance_data.size(),
      .instance_count = count,
    }
  );
  m_instance_data.insert(
    m_instance_data.end(),
    instances.begin(),
    instances.end()
  );
}

auto alloy::render::render_batch::clear() noexcept -> void
{
  m_entries.clear();
  m_instance_data.clear();
}
