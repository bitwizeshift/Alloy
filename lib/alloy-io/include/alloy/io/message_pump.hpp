/*****************************************************************************
 * \file message_pump.hpp
 *
 * \todo
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef ALLOY_IO_MESSAGE_PUMP_HPP
#define ALLOY_IO_MESSAGE_PUMP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/io/event.hpp"

#include "alloy/core/utilities/not_null.hpp" // core::not_null
#include "alloy/core/assert.hpp"             // ALLOY_ASSERT

#include <vector>      // std::vector
#include <cstddef>     // std::size_t
#include <type_traits> // std::aligned_storage_t
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
  /// The pump does not store event state directly -- it's forwarded on to
  /// all listeners, if any exist at the time of the event being pushed.
  ///
  /// Subsystems are responsible for storing events if handling of the event
  /// is deferred.
  //////////////////////////////////////////////////////////////////////////////
  class message_pump final
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

    /// \brief Constructs a message_pump by moving the contents from \p other
    ///
    /// \param other the other message_pump to move
    message_pump(message_pump&& other) noexcept = default;
    message_pump(const message_pump& other) = delete;

    //--------------------------------------------------------------------------

    /// \brief Move-assigns the contents from \p other to this message_pump
    ///
    /// \param other the other message_pump to move
    /// \return reference to \c (*this)
    message_pump& operator=(message_pump&& other) noexcept = default;
    message_pump& operator=(const message_pump& other) = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Pushes an event into the end of the message pump
    ///
    /// \param e the event to push
    template<typename Event>
    void post_event(Event&& e);

    /// \brief Pushes an immediate event into the end of the message pump
    ///
    /// \param e the event to push
    template<typename Event>
    void post_immediate_event(Event&& e);

    //--------------------------------------------------------------------------

    /// \brief Polls for events and dispatches accumulated events to all
    ///        listeners
    void pump();

    /// \brief Polls for events from all message_pump event sources
    ///
    /// Events are accumulated, but will not be dispatched until \c dispatch
    /// is called
    void poll();

    /// \brief Dispatches all accumulated events
    void dispatch();

    //--------------------------------------------------------------------------

    /// \brief Registers a listener to handle the message pump
    ///
    /// \note \p l is not owned by this message_pump
    /// \pre \p l is not-null
    /// \param l the listener to register
    void register_listener(core::not_null<listener*> l);

    /// \brief Unregisters a listener to handle the message pump
    ///
    /// \note \p l is not owned by this message_pump
    /// \pre l is not-null
    /// \param l the listener to unregister
    void unregister_listener(core::not_null<listener*> l);

    /// \brief Registers a message pump source
    ///
    /// \note \p l is not owned by this message_pump
    /// \pre s is not-null
    /// \param l the pump source to register
    void register_pump_source(core::not_null<source*> s);

    /// \brief Unregisters a message pump source
    ///
    /// \note \p l is not owned by this message_pump
    /// \pre s is not-null
    /// \param l the pump source to unregister
    void unregister_pump_source(core::not_null<source*> s);

    //--------------------------------------------------------------------------
    // Event Posting
    //--------------------------------------------------------------------------
  private:

    /// \brief Posts an event to all listeners
    ///
    /// \param e the event
    void do_post_event(event e);

    /// \brief Posts an immediate event to all listeners
    ///
    /// \param e the event
    void do_post_immediate_event(event e);

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    // TODO(bitwizeshift):
    //   Consider replacing these vectors with fixed sized containers which
    //   use circular buffers

    std::vector<listener*> m_listeners;
    std::vector<source*> m_sources;

    // This could also be done with a priority queue, but that introduces
    // logarithmic comparisons on pushing -- and this would be unnecessary
    std::vector<event> m_normal_events;
    std::vector<event> m_immediate_events;
  };

  //============================================================================
  // class : message_pump::listener
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A listener for a message_pump
  //////////////////////////////////////////////////////////////////////////////
  class message_pump::listener
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

    virtual ~listener() noexcept = 0;

    //--------------------------------------------------------------------------

    listener& operator=(listener&& other) noexcept = default;
    listener& operator=(const listener& other) noexcept = default;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    virtual void handle_message(const event& e) noexcept = 0;
    virtual void handle_immediate_message(const event& e) noexcept;
  };

  //============================================================================
  // class : message_pump::source
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A source for a message_pump
  //////////////////////////////////////////////////////////////////////////////
  class message_pump::source
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

    source& operator=(source&& other) noexcept = default;
    source& operator=(const source& other) noexcept = default;

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
    virtual void poll(message_pump& p) noexcept = 0;
  };

} // namespace alloy::io

//==============================================================================
// definitions : class : message_pump
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template<typename Event>
inline void alloy::io::message_pump::post_event(Event&& e)
{
  auto erased_event = event::make_event<std::decay_t<Event>>(
    std::forward<Event>(e)
  );

  do_post_event(std::move(erased_event));
}

template<typename Event>
inline void alloy::io::message_pump::post_immediate_event(Event&& e)
{
  auto erased_event = event::make_event<std::decay_t<Event>>(
    std::forward<Event>(e)
  );

  do_post_immediate_event(std::move(erased_event));
}

//==============================================================================
// definitions : class : message_pump::listener
//==============================================================================

inline alloy::io::message_pump::listener::~listener() noexcept = default;

#endif /* ALLOY_IO_MESSAGE_PUMP_HPP */