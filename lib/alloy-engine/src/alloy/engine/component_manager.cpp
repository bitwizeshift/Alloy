#include "alloy/engine/component_manager.hpp"

void alloy::engine::component_manager::detach_all_components(entity e)
{
  for (const auto& storage : m_component_storages) {
    if (storage->has_component(e)) {
      storage->detach_component(e);
    }
  }
}
