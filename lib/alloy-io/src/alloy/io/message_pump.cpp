#include "alloy/io/message_pump.hpp"

#include <algorithm> // std::find

//==============================================================================
// class : message_pump
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

alloy::io::message_pump::message_pump()
  noexcept
{

}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

void alloy::io::message_pump::poll()
{
  m_sources.emit<&source::poll>(*this);
}

void alloy::io::message_pump::pump()
{
  poll();
  dispatch();
}

void alloy::io::message_pump::dispatch()
{
  for (const auto& e : m_immediate_events) {
    m_listeners.emit<&listener::handle_immediate_message>(e);
  }

  for (const auto& e : m_normal_events) {
    m_listeners.emit<&listener::handle_message>(e);
  }

  // Reset events to prevent duplicate dispatching
  m_normal_events.clear();
  m_immediate_events.clear();
}

//------------------------------------------------------------------------------

alloy::core::sink<alloy::io::message_pump::listener>
  alloy::io::message_pump::on_event()
  noexcept
{
  return core::sink{&m_listeners};
}

alloy::core::sink<alloy::io::message_pump::source>
  alloy::io::message_pump::on_poll()
  noexcept
{
  return core::sink{&m_sources};
}

//------------------------------------------------------------------------------
// Event Posting
//------------------------------------------------------------------------------

void alloy::io::message_pump::do_post_event(event&& e)
{
  // always dispatch high-priority events as immediate events
  if (e.priority() == event_priority::high) {
    m_immediate_events.emplace_back(std::move(e));
  } else {
    m_normal_events.emplace_back(std::move(e));
  }
}

void alloy::io::message_pump::do_post_immediate_event(event&& e)
{
  m_immediate_events.emplace_back(std::move(e));
}

//==============================================================================
// class : message_pump::listener
//==============================================================================

//------------------------------------------------------------------------------
// Hooks
//------------------------------------------------------------------------------

void alloy::io::message_pump::listener::handle_immediate_message(const event& e)
  noexcept
{
  handle_message(e);
}
