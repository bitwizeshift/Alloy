#include "alloy/extra/logging/logger.hpp"

#include "alloy/core/assert.hpp"

//==============================================================================
// class : log_stream
//==============================================================================

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

alloy::extra::log_stream::log_stream()
  noexcept
  : m_next{nullptr},
    m_detach{}
{

}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------

alloy::extra::log_stream::~log_stream()
{
  if (m_detach) {
    m_detach(this);
  }
}

//==============================================================================
// class : logger
//==============================================================================

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::extra::logger::logger(alloy::io::mutable_buffer buffer)
  noexcept
  : m_head{nullptr},
    m_buffer{buffer}
{

}

alloy::extra::logger::logger(logger&& other)
  noexcept
  : m_head{other.m_head},
    m_buffer{other.m_buffer}
{
  other.m_head = nullptr;
}

//------------------------------------------------------------------------------

alloy::extra::logger::~logger()
{
  detach_all();
}

//------------------------------------------------------------------------------

alloy::extra::logger& alloy::extra::logger::operator=(logger&& other)
  noexcept
{
  detach_all();
  m_head = other.m_head;
  m_buffer = other.m_buffer;
  other.m_head = nullptr;

  return (*this);
}


//------------------------------------------------------------------------------
// Binding
//------------------------------------------------------------------------------

void alloy::extra::logger::attach(core::not_null<log_stream*> log)
  noexcept
{
  ALLOY_ASSERT(
    log->m_detach == log_stream::detach_function{},
    "log_stream cannot be attached to multiple logger instances!"
  );

  if (m_head == nullptr) {
    m_head = log.get();
  } else {
    auto* const new_link = log.get();

    new_link->m_next = m_head;
    m_head = new_link;
  }

  log->m_detach = log_stream::detach_function::make<&logger::detach>(this);
}


void alloy::extra::logger::detach(core::not_null<log_stream*> log)
  noexcept
{
  ALLOY_ASSERT(
    log->m_detach != log_stream::detach_function{},
    "log_stream must be attached to logger before detaching!"
  );

  log->m_detach = log_stream::detach_function{};
  if (log.get() == m_head) {
    m_head = log->m_next;
    return;
  }
  auto* prev = m_head;
  auto* current = prev->m_next;

  while (current != nullptr) {
    if (current == log.get()) {
      break;
    }
    prev = current;
    current = current->m_next;
  }
  ALLOY_ASSERT(current != nullptr); // invariant

  prev->m_next = current->m_next;
}

void alloy::extra::logger::detach_all()
  noexcept
{
  // detach all log streams from this logger to ensure there are no lifetime
  // issues
  auto* current = m_head;
  while (current != nullptr) {
    auto* prev = current;
    current = current->m_next;

    prev->m_detach = log_stream::detach_function{};
    prev->m_next = nullptr;
  }
}