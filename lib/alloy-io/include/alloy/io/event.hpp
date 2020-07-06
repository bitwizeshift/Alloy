/*****************************************************************************
 * \file event.hpp
 *
 * \todo description
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
#ifndef ALLOY_IO_EVENT_HPP
#define ALLOY_IO_EVENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/utilities/aligned_storage.hpp"

#include "alloy/io/config.hpp"

#include <cstdint>     // std::uint32_t
#include <cstddef>     // std::size_t
#include <type_traits> // std::enable_if
#include <utility>     // std::forward, std::move
#include <new>         // placement ::new

namespace alloy::io {

  //============================================================================
  // implementation detail : trait : event_traits
  //============================================================================

  namespace detail {

    template <typename T> struct always_false : std::false_type{};
    template <typename T>
    inline constexpr bool always_false_v = always_false<T>::value;

    template <typename T>
    struct event_traits
    {
      static_assert(
          always_false_v<T>,
          "Unable to use event that was not defined with "
          "ALLOY_DEFINE_[USER|SYSTEM]_EVENT"
      );
    };

  } // namespace detail

  //============================================================================
  // enum class : event_priority
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The priority for a given event
  //////////////////////////////////////////////////////////////////////////////
  enum class event_priority
  {
    none,   ///< No priority (default)
    normal, ///< Normal priority event
    high,   ///< High priority event
  };

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
  /// In order to be used as an event, either \c ALLOY_DEFINE_<USER|SYSTEM>_EVENT
  /// or \c ALLOY_DEFINE_HIGH_PRIORITY_<USER|SYSTEM>_EVENT must be used to
  /// assign a **system-wide unique** id to the event type. This allows these
  /// events to be usable in a switch case, or serialized/deserialized in a
  /// consistent form.
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
    enum class id_type : std::uint32_t{};

    //--------------------------------------------------------------------------
    // Public Static Members
    //--------------------------------------------------------------------------
  public:

    static constexpr std::size_t max_size = ALLOY_IO_EVENT_SIZE;
    static constexpr std::size_t max_align = ALLOY_IO_EVENT_ALIGN;

    //--------------------------------------------------------------------------
    // Public Static Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the id of the specified \p Event
    ///
    /// \tparam Event the event to get the id of
    /// \return the id of the event type
    template<typename Event>
    static constexpr id_type id_of() noexcept;

    /// \brief Gets the priority of the specified \p Event
    ///
    /// \tparam Event the event to get the priority of
    /// \return the priority of the event type
    template<typename Event>
    static constexpr event_priority priority_of() noexcept;

    /// \brief Constructs an \p Event from the given \p args
    ///
    /// \param args the arguments to forward to Event's constructor
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

    /// \brief Make events implicitly constructible from the underlying event
    ///        type
    ///
    /// \param e the underlying event to construct this event from
    template <typename Event,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<Event>,event>>>
    /* IMPLICIT */ event(Event&& e)
      noexcept(std::is_nothrow_constructible<std::decay_t<Event>,Event>::value);

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
    /// \return the id
    id_type id() const noexcept;

    /// \brief Gets the priority of this event
    ///
    /// \return the priority of this event
    event_priority priority() const noexcept;

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
      destroy,  ///< Operation for calling the underlying's destructor
      copy,     ///< Operation for copying the underlying value
      move,     ///< Operation for moving the underlying value
      id,       ///< Operation for retrieving the underlying id
      priority, ///< Operation for retrieving the priority of the event
    };

    using storage_type    = core::aligned_storage<max_size, max_align>;
    using storage_handler = std::uint32_t(*)(operation, const storage_type*,const storage_type*);

    storage_type    m_storage;
    storage_handler m_handler;

    //--------------------------------------------------------------------------
    // Private Static Members
    //--------------------------------------------------------------------------
  private:

    /// \brief The handler used for managing events with a T instance
    ///
    /// \tparam T the type of the handler
    /// \param op the operation
    /// \param self the self pointer, used for 'copy', 'move', and 'destroy'
    /// \param other the other pointer, used for 'copy', and 'move'
    /// \return an integral value, only valid for "id" and "priority" ops
    template<typename T>
    static std::uint32_t handler(operation op,
                                 const storage_type* self,
                                 const storage_type* other);

    /// \brief The handler used for managing events with no instance
    ///
    /// This exists to prevent the need to perform branches to check for null
    /// state, when the common case will never contain a null state.
    ///
    /// \param op the operation
    /// \param self the self pointer (unused)
    /// \param other the other pointer (unused)
    /// \return an integral value, only valid for "id" and "priority" ops
    static std::uint32_t null_handler(operation op,
                                      const storage_type* self,
                                      const storage_type* other);

  };

  //============================================================================
  // traits : is_event
  //============================================================================

  /// \brief A simple trait for checking if the type T is a valid event type
  ///
  /// The result is aliased as ::value
  ///
  /// \tparam T the type to check
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

  /// \brief A convenience template variable for extracting is_valid_event's
  ///        ::value parameter
  template <typename T>
  inline constexpr bool is_valid_event_v = is_valid_event<T>::value;

} // namespace alloy::io

//==============================================================================
// macros : class : event
//==============================================================================

// Internal
#define ALLOY_INTERNAL_DEFINE_EVENT(event,id_value,offset,p)                   \
  template <>                                                                  \
  struct alloy::io::detail::event_traits<event> final                          \
  {                                                                            \
      static_assert(::alloy::io::is_valid_event<event>::value);                \
      static constexpr auto id = std::uint32_t{(id_value << offset)};          \
      static constexpr auto priority = ::alloy::io::event_priority::p;         \
  }

//! \def ALLOY_DEFINE_USER_EVENT(event,id)
//! \brief Defines a user event named \p event with the given \p id
//!
//! This event will have normal priority. All event IDs given to user events
//! must be unique across the entire system. By default, no events have a
//! user ID -- so this is entirely up to the consumer of this library to decide
//! on a numbering scheme
//!
//! \note \p event must be the fully-qualified path to the event type.
//!
//! \note This macro must be expanded outside of any namespace to avoid
//!       potential collisions.
//!
//! \param event the event to define
//! \param id the id to associate to the event -- must fit into 16 bits.
#define ALLOY_DEFINE_USER_EVENT(event,id)                                      \
  ALLOY_INTERNAL_DEFINE_EVENT(event,id,16,normal)

//! \def ALLOY_DEFINE_HIGH_PRIORITY_USER_EVENT(event,id)
//! \brief Defines a user event named \p event with the given \p id that is
//!        always treated as a higher priority.
//!
//! This event will have high priority. All event IDs given to user events
//! must be unique across the entire system. By default, no events have a
//! user ID -- so this is entirely up to the consumer of this library to decide
//! on a numbering scheme
//!
//! \note \p event must be the fully-qualified path to the event type.
//!
//! \note This macro must be expanded outside of any namespace to avoid
//!       potential collisions.
//!
//! \param event the event to define
//! \param id the id to associate to the event -- must fit into 16 bits.
#define ALLOY_DEFINE_HIGH_PRIORITY_USER_EVENT(event,id)                        \
  ALLOY_INTERNAL_DEFINE_EVENT(event,id,16,high)

//! \def ALLOY_DEFINE_SYSTEM_EVENT(event,id)
//! \brief Defines a system event named \p event with the given \p id
//!
//! This event will have normal priority. All event Ids must be unique across
//! the entire system. Unlike user events, system events have reserved ranges
//! for the alloy core event system. Use this at your own risk; if collisions
//! occur, there will be no support from maintainers.
//!
//! System events are effectively just a reserved range of user events; and so
//! there should not be any real reason to need to define a system event over
//! a user event as a consumer. This macro is still provided for completeness
//! in case this is ever needed for workarounds.
//!
//! \note \p event must be the fully-qualified path to the event type.
//!
//! \note This macro must be expanded outside of any namespace to avoid
//!       potential collisions.
//!
//! \note This is a macro intended to be used only for Alloy's internal
//!       system messages. Use at your own risk
//! \param event the event to define
//! \param id the id to associate to the event
#define ALLOY_DEFINE_SYSTEM_EVENT(event,id) \
  ALLOY_INTERNAL_DEFINE_EVENT(event,id,0,normal)

//! \def ALLOY_DEFINE_HIGH_PRIORITY_SYSTEM_EVENT(event,id)
//! \brief Defines a system event named \p event with the given \p id that is
//!        always treated as high priority
//!
//! This event will have high priority. All event Ids must be unique across
//! the entire system. Unlike user events, system events have reserved ranges
//! for the alloy core event system. Use this at your own risk; if collisions
//! occur, there will be no support from maintainers.
//!
//! System events are effectively just a reserved range of user events; and so
//! there should not be any real reason to need to define a system event over
//! a user event as a consumer. This macro is still provided for completeness
//! in case this is ever needed for workarounds.
//!
//! \note \p event must be the fully-qualified path to the event type.
//!
//! \note This macro must be expanded outside of any namespace to avoid
//!       potential collisions.
//!
//! \note This is a macro intended to be used only for Alloy's internal
//!       system messages. Use at your own risk
//! \param event the event to define
//! \param id the id to associate to the event
#define ALLOY_DEFINE_HIGH_PRIORITY_SYSTEM_EVENT(event,id) \
  ALLOY_INTERNAL_DEFINE_EVENT(event,id,0,high)

//==============================================================================
// definitions : class : event
//==============================================================================

//------------------------------------------------------------------------------
// Static Member Functions
//------------------------------------------------------------------------------

template<typename Event>
inline constexpr alloy::io::event::id_type
  alloy::io::event::id_of()
  noexcept
{
  static_assert(is_valid_event<Event>::value);

  return id_type{detail::event_traits<Event>::id};
}


template<typename Event>
inline constexpr alloy::io::event_priority
  alloy::io::event::priority_of()
  noexcept
{
  static_assert(is_valid_event<Event>::value);

  return detail::event_traits<Event>::priority;
}


template<typename Event, typename...Args>
inline alloy::io::event alloy::io::event::make_event( Args&&...args )
  noexcept(std::is_nothrow_constructible<Event,Args...>::value)
{
  static_assert(is_valid_event<Event>::value);

  auto e = event{};
  ::new (&e.m_storage) Event{ std::forward<Args>(args)... };
  e.m_handler = &event::handler<Event>;

  return e;
}

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------

inline alloy::io::event::event()
  noexcept
  : m_handler{&event::null_handler}
{

}


template <typename Event, typename>
inline alloy::io::event::event(Event&& e)
  noexcept(std::is_nothrow_constructible<std::decay_t<Event>,Event>::value)
  : event{}
{
  static_assert(is_valid_event<std::decay_t<Event>>::value);

  ::new(&e.m_storage) std::decay_t<Event>(std::forward<Event>(e));
  m_handler = &event::handler<std::decay_t<Event>>;
}


inline alloy::io::event::event(event&& other)
  noexcept
  : event{}
{
  auto* const handler = core::compiler::assume_not_null(other.m_handler);

  (*handler)(operation::move, &m_storage, &other.m_storage);
  m_handler = handler;
}


inline alloy::io::event::event(const event& other)
  noexcept
  : event{}
{
  auto* const handler = core::compiler::assume_not_null(other.m_handler);

  (*handler)(operation::copy, &m_storage, &other.m_storage);
  m_handler = handler;
}

//------------------------------------------------------------------------------

inline alloy::io::event::~event()
  noexcept
{
  reset();
}

//------------------------------------------------------------------------------

inline alloy::io::event& alloy::io::event::operator=(event&& other)
  noexcept
{
  reset();

  auto* const handler = core::compiler::assume_not_null(other.m_handler);

  (*handler)(operation::move, &m_storage, &other.m_storage);
  m_handler = handler;

  return (*this);
}


inline alloy::io::event& alloy::io::event::operator=(const event& other)
  noexcept
{
  reset();

  auto* const handler = core::compiler::assume_not_null(other.m_handler);

  (*handler)(operation::copy, &m_storage, &other.m_storage);
  m_handler = handler;

  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

inline alloy::io::event::id_type
  alloy::io::event::id()
  const noexcept
{
  auto* const handler = core::compiler::assume_not_null(m_handler);

  const auto value = (*handler)(operation::id,nullptr,nullptr);
  return static_cast<id_type>(value);
}

inline alloy::io::event_priority
  alloy::io::event::priority()
  const noexcept
{
  auto* const handler = core::compiler::assume_not_null(m_handler);

  const auto value = (*handler)(operation::priority,nullptr,nullptr);
  return static_cast<event_priority>(value);
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

  // TODO: std::launder this reference
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

  // TODO: std::launder this pointer
  return reinterpret_cast<const Event*>(&m_storage);
}

//------------------------------------------------------------------------------
// Private Members
//------------------------------------------------------------------------------

inline void alloy::io::event::reset()
  noexcept
{
  auto* const handler = core::compiler::assume_not_null(m_handler);

  (*handler)(operation::destroy, &m_storage, nullptr);
  m_handler = &event::null_handler;
}

//------------------------------------------------------------------------------
// Private Static Functions
//------------------------------------------------------------------------------

template<typename T>
std::uint32_t alloy::io::event::handler(operation op,
                                        const storage_type* self,
                                        const storage_type* other)
{
  switch (op) {
    case operation::destroy: {
      ALLOY_ASSERT_AND_ASSUME( self != nullptr );
      core::compiler::unused(other);

      auto* const mself = const_cast<storage_type*>(self);
      auto* const t = reinterpret_cast<T*>(mself);
      t->~T();
      return 0u;
    }

    case operation::copy: {
      ALLOY_ASSERT_AND_ASSUME( self != nullptr );
      ALLOY_ASSERT_AND_ASSUME( other != nullptr );

      auto* const mself = const_cast<storage_type*>(self);
      const auto* const t = reinterpret_cast<const T*>(other);
      ::new(static_cast<void*>(mself)) T(*t);
      return 0u;
    }

    case operation::move: {
      ALLOY_ASSERT_AND_ASSUME( self != nullptr );
      ALLOY_ASSERT_AND_ASSUME( other != nullptr );

      // Move construct from the internal storage. '
      auto* const mself = const_cast<storage_type*>(self);
      const auto* const t = reinterpret_cast<const T*>(other);
      auto* const mt = const_cast<T*>(t);

      ::new(static_cast<void*>(mself)) T(std::move(*mt));
      return 0u;
    }

    case operation::id: {
      core::compiler::unused(self);
      core::compiler::unused(other);

      return static_cast<std::uint32_t>(id_of<T>());
    }

    case operation::priority: {
      core::compiler::unused(self);
      core::compiler::unused(other);

      return static_cast<std::uint32_t>(priority_of<T>());
    }
  }
  core::compiler::unreachable();
}

inline std::uint32_t alloy::io::event::null_handler(operation op,
                                                    const storage_type* self,
                                                    const storage_type* other)
{
  core::compiler::unused(self, other);

  switch (op) {
    case operation::destroy: {
      return 0u;
    }

    case operation::copy: {
      // No operation required for 'copy'
      return 0u;
    }

    case operation::move: {
      // No operation required for 'move'
      return 0u;
    }

    case operation::id: {
      // ID of null is '0'
      return 0u;
    }

    case operation::priority: {
      return static_cast<std::uint32_t>(event_priority::none);
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_IO_EVENT_HPP */
