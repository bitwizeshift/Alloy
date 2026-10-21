// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file message_pump.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_IO_MESSAGE_PUMP_HPP
#define ALLOY_IO_MESSAGE_PUMP_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/io/api.hpp"
#include "alloy/io/event.hpp"

#include "alloy/core/utilities/not_null.hpp" // core::not_null
#include "alloy/core/assert.hpp"             // ALLOY_ASSERT
#include "alloy/core/utilities/signal.hpp"

#include <vector>      // std::vector
#include <cstddef>     // std::size_t
#include <utility>     // std::forward
#include <new>         // placement new

namespace alloy::io {

  //============================================================================
  // class : message_pump
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The primary form of communication between i/o subsystems in alloy
  ///
  /// The message pump communicates with other listeners through event objects.
  ///
  /// Event dispatching is deferred to when message_pump::dispatch is invoked;
  /// this requires the message_pump to be stateful.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API message_pump final
  {
    // TODO(bitwizeshift): Add allocator support

    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    class listener;
    class source;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a message_pump with a default allocator
    message_pump() noexcept;

    message_pump(message_pump&&) = delete;
    message_pump(const message_pump&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(message_pump&&) -> message_pump& = delete;
    auto operator=(const message_pump&) -> message_pump& = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Pushes an event into the end of the message pump
    ///
    /// This function will honor the priority of the underlying event. High
    /// priority events will be dispatched as immediate events, whereas normal
    /// priority events will be dispatched as normal events.
    ///
    /// \param e the event to push
    template<typename Event>
    auto post_event(Event&& e) -> void;

    /// \brief Pushes an immediate event into the end of the message pump
    ///
    /// This will always push immediate events regardless of the underyling
    /// priority of the event
    ///-
    /// \param e the event to push
    template<typename Event>
    auto post_immediate_event(Event&& e) -> void;

    //--------------------------------------------------------------------------

    /// \brief Polls for events and dispatches accumulated events to all
    ///        listeners
    ///
    /// This is equivalent to calling 'poll' followed by 'dispatch'
    auto pump() -> void;

    /// \brief Polls for events from all message_pump event sources
    ///
    /// Events are accumulated, but will not be dispatched until `dispatch`
    /// is called
    ///
    /// \note Polling must **not** be done from a dispatched event handler
    auto poll() -> void;

    /// \brief Dispatches all accumulated events to listeners
    auto dispatch() -> void;

    //--------------------------------------------------------------------------

    /// \brief Retrieves a sink for the on-event handler
    ///
    /// This event is invoked any time 'dispatch' is invoked. This is used to
    /// notify any attached listeners that an event has occurred.
    ///
    /// \return a sink for registering to the on-event handler
    auto on_event() noexcept -> core::sink<listener>;

    /// \brief Retrieves a sink for the on-poll handler
    ///
    /// This event is invoked any time 'poll' is invoked. This is used to
    /// retrieve any event notifications, and push it back to the message_pump
    ///
    /// \return a sink for registering to the on-poll handler
    auto on_poll() noexcept -> core::sink<source>;

    //--------------------------------------------------------------------------
    // Event Posting
    //--------------------------------------------------------------------------
  private:

    /// \brief Posts an event to all listeners
    ///
    /// \note This is an rvalue since it's an internal API to the message_pump.
    ///       This reduces the overhead of calling the event's type-erased
    ///       move-constructor.
    ///
    /// \param e an rvalue of the event
    auto do_post_event(event&& e) -> void;

    /// \brief Posts an immediate event to all listeners
    ///
    /// \note This is an rvalue since it's an internal API to the message_pump.
    ///       This reduces the overhead of calling the event's type-erased
    ///       move-constructor.
    ///
    /// \param e an rvalue of the event
    auto do_post_immediate_event(event&& e) -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    // TODO(bitwizeshift):
    //   Consider replacing these vectors with fixed sized containers which
    //   use circular buffers

    core::signal<listener> m_listeners;
    core::signal<source> m_sources;

    // This could also be done with a priority queue, but that introduces
    // logarithmic comparisons on insertion, and degrades in-order iteration
    // performance
    std::vector<event> m_normal_events;
    std::vector<event> m_immediate_events;
  };

  //============================================================================
  // class : message_pump::listener
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A listener for a message_pump
  ///
  /// This class gets registered to the message_pump and will listen to events
  /// dispatched through the pump.
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API message_pump::listener
  {
    friend class message_pump;

    //--------------------------------------------------------------------------
    // Constructors / Assignment / Destructor
    //--------------------------------------------------------------------------
  public:

    listener() noexcept = default;
    listener(listener&& other) noexcept = default;
    listener(const listener& other) noexcept = default;

    //--------------------------------------------------------------------------

    virtual ~listener() noexcept = default;

    //--------------------------------------------------------------------------

    auto operator=(listener&& other) noexcept -> listener& = default;
    auto operator=(const listener& other) noexcept -> listener& = default;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    virtual auto handle_message(const event& e) noexcept -> void = 0;
    virtual auto handle_immediate_message(const event& e) noexcept -> void;
  };

  //============================================================================
  // class : message_pump::source
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A source for a message_pump
  ///
  /// Sources produce the events that will be dispatched through the
  /// message_pump and listened to by listeners
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API message_pump::source
  {
    friend class message_pump;

    //--------------------------------------------------------------------------
    // Constructors / Assignment / Destructor
    //--------------------------------------------------------------------------
  public:

    source() noexcept = default;
    source(source&& other) noexcept = default;
    source(const source& other) noexcept = default;

    //--------------------------------------------------------------------------

    virtual ~source() noexcept = default;

    //--------------------------------------------------------------------------

    auto operator=(source&& other) noexcept -> source& = default;
    auto operator=(const source& other) noexcept -> source& = default;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    /// \brief Hook for the message pump source to implement
    ///
    /// This will poll the current source for events, pushing events back to the
    /// message_pump \p p
    ///
    /// \param p the message_pump to push new messages to
    virtual auto poll(message_pump& p) noexcept -> void = 0;
  };

} // namespace alloy::io

//==============================================================================
// definitions : class : message_pump
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template<typename Event>
inline
auto alloy::io::message_pump::post_event(Event&& e) -> void
{
  auto erased_event = event::make_event<std::decay_t<Event>>(
    std::forward<Event>(e)
  );

  do_post_event(std::move(erased_event));
}

template<typename Event>
inline
auto alloy::io::message_pump::post_immediate_event(Event&& e) -> void
{
  auto erased_event = event::make_event<std::decay_t<Event>>(
    std::forward<Event>(e)
  );

  do_post_immediate_event(std::move(erased_event));
}

#endif /* ALLOY_IO_MESSAGE_PUMP_HPP */

