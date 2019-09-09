#include "alloy/io/message_pump.hpp"

#include <algorithm> // std::find

//==============================================================================
// class : event
//==============================================================================

//------------------------------------------------------------------------------
// Public Static Members
//------------------------------------------------------------------------------

// ID defaults to '1', since '0' is reserved for null events.
unsigned alloy::io::event::s_current_id = 1;

//------------------------------------------------------------------------------
// Constructors / Destructor
//------------------------------------------------------------------------------

alloy::io::event::event()
  noexcept
  : m_storage{},
    m_handler{nullptr}
{

}

alloy::io::event::event( event&& other )
  noexcept
  : m_storage{},
    m_handler{other.m_handler}
{
  m_handler(operation::move, &m_storage, &other.m_storage);
}

alloy::io::event::event( const event& other )
  noexcept
  : m_storage{},
    m_handler{other.m_handler}
{
  m_handler(operation::move, &m_storage, &other.m_storage);
}

//------------------------------------------------------------------------------

alloy::io::event::~event()
  noexcept
{
  reset();
}

//------------------------------------------------------------------------------

alloy::io::event& alloy::io::event::operator=( event&& other )
  noexcept
{
  reset();

  if (other.m_handler != nullptr) {
    m_handler = other.m_handler;
    m_handler( operation::move, &m_storage, &other.m_storage );
  }

  return (*this);
}

alloy::io::event& alloy::io::event::operator=( const event& other )
  noexcept
{
  reset();

  if (other.m_handler != nullptr) {
    m_handler = other.m_handler;
    m_handler( operation::move, &m_storage, &other.m_storage );
  }

  return (*this);
}

//------------------------------------------------------------------------------
// Private Members
//------------------------------------------------------------------------------

void alloy::io::event::reset()
  noexcept
{
  if (m_handler != nullptr) {
    (*m_handler)( operation::destroy, &m_storage, nullptr );
    m_handler = nullptr;
  }
}

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
  for (auto& source : m_sources) {
    source->poll(*this);
  }
}

void alloy::io::message_pump::pump()
{
  poll();
  dispatch();
}

void alloy::io::message_pump::dispatch()
{
  // The event queues are moved and cleared prior to dispatching the events
  // to avoid any listener from pushing a new event, and corrupting the queue
  // during iteration.
  //
  // The 'clear' call is necessary to put the vectors into a valid state after
  // being moved-from -- otherwise it would be undefined behavior.

  auto normal_events = std::move(m_normal_events);
  auto immediate_events = std::move(m_immediate_events);
  m_normal_events.clear();
  m_immediate_events.clear();

  for (const auto& e : normal_events) {
    for (auto& listener : m_listeners) {
      ALLOY_ASSERT_AND_ASSUME(listener != nullptr);

      listener->handle_immediate_message(e);
    }
  }

  for (const auto& e : immediate_events) {
    for (auto& listener : m_listeners) {
      ALLOY_ASSERT_AND_ASSUME(listener != nullptr);

      listener->handle_message(e);
    }
  }

}

//------------------------------------------------------------------------------

void alloy::io::message_pump::register_listener( core::not_null<listener*> l )
{
  m_listeners.push_back(l.get());
}

void alloy::io::message_pump::unregister_listener( core::not_null<listener*> l )
{
  auto it = std::find(m_listeners.begin(),m_listeners.end(),l.get());

  ALLOY_ASSERT(it != m_listeners.end());

  m_listeners.erase(it);
}

void alloy::io::message_pump::register_pump_source( core::not_null<source*> s )
{
  m_sources.push_back(s.get());
}

void alloy::io::message_pump::unregister_pump_source( core::not_null<source*> s )
{
  auto it = std::find(m_sources.begin(),m_sources.end(),s.get());

  ALLOY_ASSERT(it != m_sources.end());

  m_sources.erase(it);
}

//------------------------------------------------------------------------------
// Event Posting
//------------------------------------------------------------------------------

void alloy::io::message_pump::do_post_event( event e )
{
  m_normal_events.emplace_back(std::move(e));
}

void alloy::io::message_pump::do_post_immediate_event( event e )
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
