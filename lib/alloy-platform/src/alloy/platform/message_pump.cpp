// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/platform/message_pump.hpp"
#include "alloy/core/utilities/signal.hpp"
#include "alloy/platform/event.hpp"

#include <utility>

//==============================================================================
// class : message_pump
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

alloy::platform::message_pump::message_pump() noexcept
{

}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::platform::message_pump::poll() -> void
{
  m_sources.emit<&source::poll>(*this);
}

auto alloy::platform::message_pump::pump() -> void
{
  poll();
  dispatch();
}

auto alloy::platform::message_pump::dispatch() -> void
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

auto alloy::platform::message_pump::on_event() noexcept -> core::sink<listener>
{
  return core::sink{&m_listeners};
}

auto alloy::platform::message_pump::on_poll() noexcept -> core::sink<source>
{
  return core::sink{&m_sources};
}

//------------------------------------------------------------------------------
// Event Posting
//------------------------------------------------------------------------------

auto alloy::platform::message_pump::do_post_event(event&& e) -> void
{
  // always dispatch high-priority events as immediate events
  if (e.priority() == event_priority::high) {
    m_immediate_events.emplace_back(std::move(e));
  } else {
    m_normal_events.emplace_back(std::move(e));
  }
}

auto alloy::platform::message_pump::do_post_immediate_event(event&& e) -> void
{
  m_immediate_events.emplace_back(std::move(e));
}

//==============================================================================
// class : message_pump::listener
//==============================================================================

//------------------------------------------------------------------------------
// Hooks
//------------------------------------------------------------------------------

auto alloy::platform::message_pump::listener::handle_immediate_message(const event& e)
  noexcept -> void
{
  handle_message(e);
}
