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
// Constructors
//------------------------------------------------------------------------------

alloy::extra::logger::logger(alloy::io::mutable_buffer buffer)
  noexcept
  : m_head{nullptr},
    m_buffer{buffer}
{

}

//------------------------------------------------------------------------------
// Binding
//------------------------------------------------------------------------------

void alloy::extra::logger::attach(core::not_null<log_stream*> log)
{
  if (m_head == nullptr) {
    m_head = log.get();
  } else {
    auto* const new_link = log.get();

    new_link->m_next = m_head;
    m_head = new_link;
  }
  log->m_detach = log_stream::detach_function::bind<&logger::detach>(this);
}


void alloy::extra::logger::detach(core::not_null<log_stream*> log)
{
  log->m_detach = log_stream::detach_function{};
  if (log.get() == m_head) {
    m_head = log->m_next;
    return;
  }
  auto* prev = m_head;
  auto* current = prev + 1;

  while (current != nullptr) {
    if (current == log.get()) {
      break;
    }
    prev = current;
    current = current->m_next;
  }
  ALLOY_ASSERT(current != nullptr);

  prev->m_next = current->m_next;
}
