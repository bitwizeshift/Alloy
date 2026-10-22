// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/engine/component_manager.hpp"

#include "alloy/engine/entity.hpp"

auto alloy::engine::component_manager::detach_all_components(entity e) -> void
{
  for (const auto& storage : m_component_storages) {
    if (storage->has_component(e)) {
      storage->detach_component(e);
    }
  }
}
