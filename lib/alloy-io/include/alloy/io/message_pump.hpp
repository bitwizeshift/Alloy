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

#include "alloy/core/utilities/not_null.hpp" // core::not_null
#include "alloy/core/assert.hpp"             // ALLOY_ASSERT

#include <vector>      // std::vector
#include <cstddef>     // std::size_t
#include <type_traits> // std::aligned_storage_t
#include <utility>     // std::forward
#include <new>         // placement new

namespace alloy::io {

  //============================================================================
  // class : event
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event in a message pump
  ///
  /// All event types are type-erased to remove any dependencies between events
  /// and this class -- which provides an easy extension mechanism for
  /// introducing custom event types.
  ///
  /// A noted consequence of this design is that ids are not suitable for
  /// switch/cases, since 'id_of' uses runtime values rather than compile-time
  /// values for the case labels.
  ///
  /// All events must fit in \c event::max_size, and are required to fit
  /// in \c event::max_align.
  //////////////////////////////////////////////////////////////////////////////
  class event
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    /// \brief A type-safe integral value representing a unique event type
    enum class id_type : unsigned{};

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    static constexpr std::size_t max_size = 64;
    static constexpr std::size_t max_align = 64;

    //--------------------------------------------------------------------------
    // Public Static Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the id of the specified \p Event
    ///
    /// \note The returned ID is guaranteed to be consistent for the entirety of
    ///       the application's runtime, but is NOT guaranteed to be consistent
    ///       across different invocations/compilations.
    ///       Under no circumstance is the id to be serialized across different
    ///       invocations, otherwise you may experience undefined behavior.
    ///
    /// \tparam Event the event to get the id of
    /// \return the id of the event type
    template<typename Event>
    static id_type id_of();

    /// \brief Constructs an \p Event from the given \p args
    ///
    /// \param args... the arguments to forward to Event's constructor
    /// \return an event that contains the type-erased Event
    template<typename Event, typename...Args>
    static event make_event( Args&&...args )
      noexcept(std::is_nothrow_constructible<Event,Args...>::value);

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a null-event with no id
    event() noexcept;

    /// \brief Constucts an event by moving the contents of an existing event
    ///
    /// \param other the other event to move
    event(event&& other) noexcept;

    /// \brief Constucts an event by copying the contents of an existing event
    ///
    /// \param other the other event to copy
    event(const event& other) noexcept;

    //--------------------------------------------------------------------------

    ~event() noexcept;

    //--------------------------------------------------------------------------

    /// \brief Assigns an event by moving the contents of an existing event
    ///
    /// \param other the other event to move
    /// \return reference to \c (*this)
    event& operator=(event&& other) noexcept;

    /// \brief Assigns an event by copying the contents of an existing event
    ///
    /// \param other the other event to copy
    /// \return reference to \c (*this)
    event& operator=(const event& other) noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the id of this event
    ///
    /// \note Under no circumstance is the id to be serialized across different
    ///       invocations, otherwise you may experience undefined behavior.
    ///
    /// \return the id
    id_type id() const noexcept;

    /// \brief Checks if this event is of type \p Event
    ///
    /// \tparam Event the event to compare against
    /// \return \c true if this event is of type \p Event
    template<typename Event>
    bool is() const noexcept;

    /// \brief Converts this event to the specified \p Event type
    ///
    /// \pre \c id() matches the result of \c event::is_of<Event>()
    /// \tparam Event the event type to convert to
    /// \return the event
    template<typename Event>
    const Event& as() const noexcept;

    /// \brief Attempts to convert this event to the specified \p Event type
    ///
    /// If the underlying event is of type \p Event, this function returns a
    /// pointer to the underlying event. If it is not valid, it returns
    /// \c nullptr
    ///
    /// \tparam Event the event type to attempt to convert to
    /// \return a pointer to the event if successful, \c nullptr otherwise
    template<typename Event>
    const Event* try_as() const noexcept;

    //--------------------------------------------------------------------------
    // Private Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Clears the state of this event, calling the destructor on the
    ///        underlying type-erased object
    void reset() noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    enum class operation
    {
      destroy, ///< Operation for calling the underlying's destructor
      copy,    ///< Operation for copying the underlying value
      move,    ///< Operation for moving the underlying value
      id,      ///< Operation for retrieving the underlying id
    };

    using storage_type    = std::aligned_storage_t<max_size, max_align>;
    using storage_handler = id_type(*)(operation, const storage_type*,const storage_type*);

    storage_type    m_storage;
    storage_handler m_handler;

    friend class message_pump;

    static unsigned s_current_id;

    //--------------------------------------------------------------------------
    // Private Static Members
    //--------------------------------------------------------------------------
  private:

    template<typename T>
    static id_type handler( operation op,
                            const storage_type* self,
                            const storage_type* other );
  };

  //============================================================================
  // traits : is_event
  //============================================================================

  template<typename T>
  struct is_valid_event : std::bool_constant<(
    (event::max_size >= sizeof(T)) &&
    (event::max_align >= alignof(T)) &&
    (std::is_nothrow_destructible<T>::value) &&
    (std::is_copy_constructible<T>::value) &&
    (!std::is_pointer<T>::value) &&
    (!std::is_reference<T>::value) &&
    (!std::is_const<T>::value)
  )>{};

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

    /// \brief Pumps for an event from all pump-sources
    void pump();

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
    void do_post_event(const event& e);

    /// \brief Posts an immediate event to all listeners
    ///
    /// \param e the event
    void do_post_immediate_event(const event& e);

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    std::vector<listener*> m_listeners;
    std::vector<source*> m_sources;
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

    virtual void handle_message(const event& e) noexcept;
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

    /// \brief Virtual hook for handling the message pump
    ///
    /// \param p the message_pump to handle the message
    virtual void pump(message_pump& p) noexcept = 0;
  };

} // namespace alloy::io

//==============================================================================
// definitions : class : event
//==============================================================================

//------------------------------------------------------------------------------
// Static Member Functions
//------------------------------------------------------------------------------

template<typename Event>
inline alloy::io::event::id_type
  alloy::io::event::id_of()
{
  static_assert(is_valid_event<Event>::value);
  static const auto s_id = s_current_id++;

  return static_cast<id_type>(s_id);
}

template<typename Event, typename...Args>
inline alloy::io::event alloy::io::event::make_event( Args&&...args )
  noexcept(std::is_nothrow_constructible<Event,Args...>::value)
{
  static_assert(is_valid_event<Event>::value);

  auto e = event{};
  e.m_handler = &event::handler<Event>;
  ::new (&e.m_storage) Event{ std::forward<Args>(args)... };

  return e;
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline alloy::io::event::id_type
  alloy::io::event::id()
  const noexcept
{
  if (m_handler == nullptr) {
    return static_cast<id_type>(0u);
  }
  return (*m_handler)(operation::id,nullptr,nullptr);
}

template<typename Event>
inline bool alloy::io::event::is()
  const noexcept
{
  static_assert(is_valid_event<Event>::value);

  return id() == event::id_of<Event>();
}

template<typename Event>
inline const Event& alloy::io::event::as()
  const noexcept
{
  static_assert(is_valid_event<Event>::value);

  ALLOY_ASSERT(id() == event::id_of<Event>(), "Invalid type conversion");

  return reinterpret_cast<const Event&>(m_storage);
}

template<typename Event>
inline const Event* alloy::io::event::try_as()
  const noexcept
{
  static_assert(is_valid_event<Event>::value);

  if (id() != event::id_of<Event>()) {
    return nullptr;
  }
  return reinterpret_cast<const Event*>(&m_storage);
}

//------------------------------------------------------------------------------
// Private Static Functions
//------------------------------------------------------------------------------

template<typename T>
alloy::io::event::id_type alloy::io::event::handler(operation op,
                                                    const storage_type* self,
                                                    const storage_type* other)
{
  switch (op) {
    case operation::destroy: {
      ALLOY_ASSERT_AND_ASSUME( self != nullptr );
      (void) other;

      auto* const mself = const_cast<storage_type*>(self);
      auto* const t = reinterpret_cast<T*>(mself);
      t->~T();
      break;
    }

    case operation::copy: {
      ALLOY_ASSERT_AND_ASSUME( self != nullptr );
      ALLOY_ASSERT_AND_ASSUME( other != nullptr );

      auto* const mself = const_cast<storage_type*>(self);
      const auto* const t = reinterpret_cast<const T*>(other);
      ::new(static_cast<void*>(mself)) T{*t};
      break;
    }

    case operation::move: {
      ALLOY_ASSERT_AND_ASSUME( self != nullptr );
      ALLOY_ASSERT_AND_ASSUME( other != nullptr );

      // Move construct from the internal storage. '
      auto* const mself = const_cast<storage_type*>(self);
      const auto* const t = reinterpret_cast<const T*>(other);
      auto* const mt = const_cast<T*>(t);

      ::new(static_cast<void*>(mself)) T{std::move(*mt)};
      break;
    }

    case operation::id: {
      (void) self;
      (void) other;

      return id_of<T>();
    }

    default:
      ALLOY_UNREACHABLE();
  }
  return static_cast<id_type>(0u);
}

//==============================================================================
// definitions : class : message_pump
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template<typename Event>
inline void alloy::io::message_pump::post_event(Event&& e)
{
  const auto erased_event = event::make_event<std::decay_t<Event>>( std::forward<Event>(e) );

  do_post_event(erased_event);
}

template<typename Event>
inline void alloy::io::message_pump::post_immediate_event(Event&& e)
{
  const auto erased_event = event::make_event<std::decay_t<Event>>( std::forward<Event>(e) );

  do_post_immediate_event(erased_event);
}

//==============================================================================
// definitions : class : message_pump::listener
//==============================================================================

inline alloy::io::message_pump::listener::~listener() noexcept = default;

#endif /* ALLOY_IO_MESSAGE_PUMP_HPP */