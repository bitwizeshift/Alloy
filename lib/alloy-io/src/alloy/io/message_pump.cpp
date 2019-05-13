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

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

void alloy::io::message_pump::pump()
{
  for (auto& source : m_sources) {
    source->pump(*this);
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

void alloy::io::message_pump::do_post_event(const event& e)
{
  // TODO(bitwizeshift): Handle concurrency
  for (auto& listener : m_listeners) {
    listener->handle_message(e);
  }
}

void alloy::io::message_pump::do_post_immediate_event(const event& e)
{
  // TODO(bitwizeshift): Handle concurrency
  for (auto& listener : m_listeners) {
    listener->handle_immediate_message(e);
  }
}
//==============================================================================
// class : message_pump::listener
//==============================================================================

//------------------------------------------------------------------------------
// Hooks
//------------------------------------------------------------------------------

void alloy::io::message_pump::listener::handle_message(const event& e)
  noexcept
{
  (void) e;
}

void alloy::io::message_pump::listener::handle_immediate_message(const event& e)
  noexcept
{
  (void) e;
}
