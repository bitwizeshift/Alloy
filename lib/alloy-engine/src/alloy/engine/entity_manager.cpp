#include "alloy/engine/entity_manager.hpp"

#include "alloy/core/assert.hpp"

//==============================================================================
// definitions : class : entity_manager
//==============================================================================

alloy::engine::entity_manager::entity_manager()
  : m_listeners{},
    m_available_entities{},
    m_entity_signatures{},
    m_current_id{0},
    m_total_entities{0}
{

}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

alloy::engine::entity alloy::engine::entity_manager::create()
  noexcept
{
  ALLOY_ASSERT(
    m_current_id < max_entities,
    "Too many entities in existence."
  );

  auto e = entity{};

  // Attempt to reuse available entity
  if (!m_available_entities.empty()) {
    e = m_available_entities.front();
    m_available_entities.pop();
  } else {
    e = entity{static_cast<entity::id_type>(m_total_entities++)};
    m_entity_signatures.emplace_back();
  }
  ++m_total_entities;

  // Notify listeners of the newly created entity
  for (auto& listener : m_listeners) {
    ALLOY_ASSERT_AND_ASSUME(listener != nullptr);

    listener->on_entity_created(e);
  }

  return e;
}

void alloy::engine::entity_manager::destroy(entity e)
  noexcept
{
  ALLOY_ASSERT(
    e.index < max_entities,
    "Entity is out of range."
  );

  m_entity_signatures[e.index].reset();

  m_available_entities.push(std::move(e));
  --m_total_entities;
}

void alloy::engine::entity_manager::set_signature(entity e, signature sig)
  noexcept
{
  ALLOY_ASSERT(
    e.index < max_entities,
    "Entity is out of range."
  );
  ALLOY_ASSERT(
    e.index < static_cast<entity::id_type>(m_entity_signatures.size()),
    "Entity does not contain a signature"
  );

  m_entity_signatures[e.index] = sig;

  // Notify listeners of the newly created entity
  for (auto& listener : m_listeners) {
    ALLOY_ASSERT_AND_ASSUME(listener != nullptr);

    listener->on_signature_change(e, sig);
  }
}

alloy::engine::signature
alloy::engine::entity_manager::get_signature(entity e)
  const noexcept
{
  ALLOY_ASSERT(
    e.index < max_entities,
    "Entity is out of range."
  );
  ALLOY_ASSERT(
    e.index < static_cast<entity::id_type>(m_entity_signatures.size()),
    "Entity does not contain a signature"
  );

  auto signature = m_entity_signatures[e.index];

  for (auto& listener : m_listeners) {
    ALLOY_ASSERT_AND_ASSUME(listener != nullptr);

    listener->on_signature_change(e, signature);
  }

  return signature;
}

void alloy::engine::entity_manager::register_listener(core::not_null<listener*> l)
{
  m_listeners.push_back(l.get());
}

void alloy::engine::entity_manager::unregister_listener(core::not_null<listener*> l)
{
  auto it = std::find(m_listeners.begin(),m_listeners.end(),l.get());

  ALLOY_ASSERT(it != m_listeners.end());

  m_listeners.erase(it);
}

void alloy::engine::entity_manager::listener::on_entity_created(entity e)
  noexcept
{
  (void) e;
}

void alloy::engine::entity_manager::listener::on_entity_destroyed(entity e)
  noexcept
{
  (void) e;
}

void alloy::engine::entity_manager::listener
  ::on_signature_change(entity e, signature sig)
  noexcept
{
  (void) e;
  (void) sig;
}