/*****************************************************************************
 * \file event.hpp
 *
 * \brief This header contains a generic event handling system
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
#ifndef ALLOY_CORE_UTILITIES_SIGNAL_HPP
#define ALLOY_CORE_UTILITIES_SIGNAL_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/assert.hpp" // ALLOY_ASSERT
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/utilities/delegate.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <type_traits> // std::enable_if_t, etc
#include <algorithm>   // std::remove

namespace alloy::core {

  //===========================================================================
  // forward-declarations
  //===========================================================================

  template <typename Fn>
  class signal;

  template <typename Fn>
  class sink;

  class connection;

  class scoped_connection;

  //===========================================================================
  // class : signal
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A class for encapsulating an event emitting system.
  ///
  /// This type is used for the primary event source, which will be subscribed
  /// to through sink instances -- which is effectively the registrar for
  /// signals.
  ///
  /// Instances of this type are immovable and uncopyable. Consumers of this
  /// type are expected to define their own semantics for copying and moving,
  /// if this is desired.
  ///
  /// Example usage:
  ///
  /// \code
  ///
  /// class example
  /// {
  /// public:
  ///
  ///   example()
  ///     : m_signal{}
  ///   {
  ///   }
  ///
  ///   sink<void(std::string_view)> on_string_event()
  ///   {
  ///     return sink{&m_signal};
  ///   }
  ///
  ///   void something_that_emits()
  ///   {
  ///     m_file_event.emit("hello world");
  ///   }
  ///
  /// private:
  ///   signal<void(std::string_view)> m_signal;
  /// };
  ///
  /// auto e = example{};
  ///
  /// auto conn1 = e.on_string_event.connect<&::some_event_handler>();
  /// auto conn2 = e.on_string_event.connect<&::some_other_event_handler>();
  ///
  /// // emit "hello world" to 'some_event_handler' and 'some_other_event_handler'
  /// e.something_that_emits();
  ///
  /// conn2.disconnect();
  /// conn1.disconnect();
  /// \endcode
  ///
  /// \tparam R the result type
  /// \tparam Args the argument types
  /////////////////////////////////////////////////////////////////////////////
  template <typename R, typename...Args>
  class signal<R(Args...)>
  {
    template <typename F, typename...Ts>
    using enable_if_invocable_t = std::enable_if_t<
      std::conjunction_v<
        std::is_invocable<F, Ts...>,
        std::disjunction<
          std::is_void<R>,
          std::is_convertible<std::invoke_result_t<F, Ts...>,R>
        >
      >
    >;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using callback_type = delegate<R(Args...)>;
    using connection_type = connection;
    using sink_type = sink<R(Args...)>;
    using size_type = std::size_t;

    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a event that does not have a bound function
    signal() noexcept;

    /// \brief Constructs a signal that can hold at least \p size listeners
    ///
    /// \param size the number of listeners to be lsitened to
    /// \param alloc the allocator to use for the listeners
    signal(size_type size, allocator alloc) noexcept;
    signal(signal&&) = delete;
    signal(const signal&) = delete;

    //-------------------------------------------------------------------------

    signal& operator=(signal&&) = delete;
    signal& operator=(const signal&) = delete;

    //-------------------------------------------------------------------------
    // Emition
    //-------------------------------------------------------------------------
  public:

    /// \brief Emits a signal to all handlers of the event sink
    ///
    /// \param args the arguments to forward to the handlers
    template <typename...UArgs,
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>>
    void emit(UArgs&&...args);

    /// \brief Emits a signal to all handlers of the event sink, collecting the
    ///        results
    ///
    /// \param collector a function to invoke on each return argument
    /// \param args the arguments to forward to the handlers
    template <typename CollectorFn,
              typename...UArgs,
              typename R2=R,
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>>
    void emit(CollectorFn&& collector, UArgs&&...args);

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    vector<callback_type> m_callbacks;

    friend class sink<R(Args...)>;
  };

  //===========================================================================
  // class : sink<R(Args...)>
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A sink which holds all listeners for a given event
  ///
  /// The sink acts as the storage container to house all event listeners for
  /// a given event. Sinks are allocator-aware, and store a fixed container
  /// of all possible event handlers.
  ///
  /// This inversion of control allows for the consumer, who is more aware of
  /// the lifetime concerns, to be in control of the allocation mechanism for
  /// the given listeners.
  ///
  /// Currently, an event can only bind a single event sink. This topic may be
  /// revisited in the future if it is deemed that more than one event sink is
  /// necessary for a given event.
  ///
  /// \note The lifetime of a given sink must outlive the length of time in
  ///       which it is bound to a given event.
  ///       This is a concern that is to be managed by the consumer directly.
  /////////////////////////////////////////////////////////////////////////////
  template <typename R, typename...Args>
  class sink<R(Args...)>
  {
    template <typename F, typename...Ts>
    using enable_if_invocable_t = std::enable_if_t<
      std::conjunction_v<
        std::is_invocable<F, Ts...>,
        std::disjunction<
          std::is_void<R>,
          std::is_convertible<std::invoke_result_t<F, Ts...>,R>
        >
      >
    >;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using callback_type = delegate<R(Args...)>;
    using connection_type = connection;
    using signal_type = signal<R(Args...)>;
    using size_type = std::size_t;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a sink that connects to the specified \p signal
    ///
    /// \pre signal is not null
    ///
    /// \param signal a pointer ot the signal that this sink connects to
    explicit sink(signal_type* signal) noexcept;

    sink() = delete;
    sink(sink&&) = delete;
    sink(const sink&) = delete;

    //-------------------------------------------------------------------------

    sink& operator=(sink&&) = delete;
    sink& operator=(const sink&) = delete;

    //-------------------------------------------------------------------------
    // Event Handlers
    //-------------------------------------------------------------------------
  public:

    /// \brief Adds a non-member function pointer listener to this event
    ///
    /// \tparam Fn the function pointer to bind
    /// \return a connection object
    template <auto Fn,
              typename=enable_if_invocable_t<decltype(Fn),Args...>>
    connection connect() noexcept;

    /// \brief Adds a member function pointer listener to this event
    ///
    /// \pre c must not be null
    ///
    /// \tparam MemberFn the member function pointer to bind
    /// \param c a pointer to the underlying instance
    /// \return a connection object
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),C*,Args...>>
    connection connect(C* c) noexcept;

    /// \brief Adds a const member function pointer listener to this event
    ///
    /// \pre c must not be null
    ///
    /// \tparam MemberFn the const member function pointer to bind
    /// \param c a pointer to the underlying instance
    /// \return a connection object
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),const C*,Args...>>
    connection connect(const C* c) noexcept;

    /// \{
    /// \brief Adds a callable listener to this event
    ///
    /// \param callable the callable to bind
    /// \return a connection object
    template <typename Callable,
              typename=enable_if_invocable_t<Callable&,Args...>>
    connection connect(Callable* callable) noexcept;
    template <typename Callable,
              typename=enable_if_invocable_t<const Callable&,Args...>>
    connection connect(const Callable* callable) noexcept;
    /// \}

    // Disallow binding nulls
    template <auto MemberFn>
    connection connect(std::nullptr_t) = delete;
    connection connect(std::nullptr_t) = delete;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Reserves space for \p size handlers
    ///
    /// \param size the number of handlers to reserve space for
    void reserve(size_type size);

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the capacity of this event sink
    ///
    /// \return the number of entries this sink can store
    size_type capacity() const noexcept;

    /// \brief Gets the current number of entries this sink stores
    ///
    /// \return the number of entries this sink stores
    size_type size() const noexcept;

    /// \brief Queries whether this sink has any handlers stored
    ///
    /// \return \c true if this event has no handlers
    bool empty() const noexcept;

    //-------------------------------------------------------------------------
    // Private Static Functions
    //-------------------------------------------------------------------------
  private:

    template <auto Fn>
    static void disconnect(void* signal) noexcept;

    template <auto MemberFn, typename C>
    static void disconnect(C* c, void* signal) noexcept;

    template <typename Callable>
    static void disconnect(Callable* callable, void* signal) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    signal_type* m_signal;
  };

  template <typename R, typename...Args>
  sink(signal<R(Args...)>*) -> sink<R(Args...)>;

  //===========================================================================
  // class : connection
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An object that represents the connection between a signal and a
  ///        handler.
  ///
  /// This object can be used to monitor or disconnect a connection.
  ///
  /// \note The connection object is only useful for indicating whether a
  ///       connection to a signal that is within scope is alive. If a signal
  ///       has left the connection scope, then attempting to disconnect will
  ///       result in undefined behavior.
  /////////////////////////////////////////////////////////////////////////////
  class connection final
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a null connection
    connection() noexcept;

    /// \brief Constructs a connection by moving \p other to this
    ///
    /// \param other the other connection to move
    connection(connection&& other) noexcept;
    connection(const connection&) = delete;

    //-------------------------------------------------------------------------

    /// \brief Move-assigns a connection by moving \p other to this
    ///
    /// \param other the other connection to move
    /// \return reference to this
    connection& operator=(connection&& other) noexcept;
    connection& operator=(const connection&) = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Queries whether the associated slot is still connected
    ///
    /// \return \c true if connected
    bool connected() const noexcept;
    explicit operator bool() const noexcept;
    /// \}

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Disconnects the sink from the signal
    void disconnect() noexcept;

    //-------------------------------------------------------------------------
    // Private Member Types
    //-------------------------------------------------------------------------
  private:

    using disconnect_callback = delegate<void(void*)>;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    /// \brief Creates a connection that uses the specified disconnect_callback
    ///        to disconnect
    ///
    /// \param disconnect the callback to do the disconnecting
    /// \param signal the signal to connect
    template <typename R, typename...Args>
    connection(disconnect_callback disconnect,
               signal<R(Args...)>* signal) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    disconnect_callback m_disconnect;
    void* m_signal;

    template <typename Fn>
    friend class sink;
  };

  //===========================================================================
  // class : scoped_connection
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An RAII wrapper around a connection object.
  ///
  /// This type will automatically disconnect a connection at the end of the
  /// scope.
  /////////////////////////////////////////////////////////////////////////////
  class scoped_connection final
  {
    //-------------------------------------------------------------------------
    // Constructors / Destructors / Assignment
    //-------------------------------------------------------------------------
  public:

    scoped_connection() = delete;

    /// \brief Creates a scoped connection from the underlying connection
    ///
    /// \param conn the connection to scope
    scoped_connection(connection conn) noexcept;
    scoped_connection(scoped_connection&&) = delete;
    scoped_connection(const scoped_connection&) = delete;

    //-------------------------------------------------------------------------

    ~scoped_connection();

    //-------------------------------------------------------------------------

    scoped_connection& operator=(scoped_connection&&) = delete;
    scoped_connection& operator=(const scoped_connection&) = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Queries whether the associated slot is still connected
    ///
    /// \return \c true if connected
    bool connected() const noexcept;
    explicit operator bool() const noexcept;
    /// \}

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Disconnects the sink from the signal
    void disconnect() noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    connection m_connection;
  };

} // namespace alloy::core

//=============================================================================
// definition : class : signal<R(Args...)>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline alloy::core::signal<R(Args...)>::signal()
  noexcept
  : m_callbacks{}
{

}

template <typename R, typename...Args>
inline alloy::core::signal<R(Args...)>::signal(size_type size, allocator alloc)
  noexcept
  : m_callbacks{size, alloc}
{

}

//-----------------------------------------------------------------------------
// Private Friend Hooks
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <typename...UArgs, typename>
inline void alloy::core::signal<R(Args...)>::emit(UArgs&&...args)
{
  for (const auto& callback : m_callbacks) {
    callback(std::forward<UArgs>(args)...);
  }
}


template <typename R, typename...Args>
template <typename CollectorFn, typename...UArgs, typename, typename>
inline void alloy::core::signal<R(Args...)>::emit(CollectorFn&& collector,
                                                  UArgs&&...args)
{
  for (const auto& callback : m_callbacks) {
    if constexpr (std::is_void_v<R>) {
      if constexpr (std::is_invocable_r_v<bool, CollectorFn>) {
        callback(std::forward<UArgs>(args)...);
        if (std::forward<CollectorFn>(collector)()) {
          return;
        }
      } else {
        callback(std::forward<UArgs>(args)...);
        std::forward<CollectorFn>(collector)();
      }
    } else {
      if constexpr (std::is_invocable_r_v<bool, CollectorFn, R>) {
        const auto r = std::forward<CollectorFn>(collector)(
          callback(std::forward<UArgs>(args)...)
        );
        if (r) {
          return;
        }
      } else {
        std::forward<CollectorFn>(collector)(
          callback(std::forward<UArgs>(args)...)
        );
      }
    }
  }
}

//=============================================================================
// inline definition : class : sink<R(Args...)>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline alloy::core::sink<R(Args...)>::sink(signal_type* signal)
  noexcept
  : m_signal{signal}
{
  ALLOY_ASSERT(signal != nullptr);
}

//-----------------------------------------------------------------------------
// Event Handlers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn, typename>
inline alloy::core::connection
  alloy::core::sink<R(Args...)>::connect()
  noexcept
{
  ALLOY_ASSERT(m_signal != nullptr);

  using disconnect_type = connection::disconnect_callback;

  auto& callbacks = m_signal->m_callbacks;

  callbacks.emplace_back(
    callback_type::template make<Fn>()
  );

  return connection{
    disconnect_type::template make<&disconnect<Fn>>(),
    m_signal
  };
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline alloy::core::connection
  alloy::core::sink<R(Args...)>::connect(C* c)
  noexcept
{
  ALLOY_ASSERT(c != nullptr);
  ALLOY_ASSERT(m_signal != nullptr);

  using disconnect_type = connection::disconnect_callback;

  auto& callbacks = m_signal->m_callbacks;

  callbacks.emplace_back(
    callback_type::template make<MemberFn>(c)
  );

  return connection{
    disconnect_type::make<&disconnect<MemberFn, C>>(c),
    m_signal
  };
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline alloy::core::connection
  alloy::core::sink<R(Args...)>::connect(const C* c)
  noexcept
{
  ALLOY_ASSERT(c != nullptr);
  ALLOY_ASSERT(m_signal != nullptr);

  using disconnect_type = connection::disconnect_callback;

  auto& callbacks = m_signal->m_callbacks;

  callbacks.emplace_back(
    callback_type::template make<MemberFn>(c)
  );

  return connection{
    disconnect_type::make<&disconnect<MemberFn, const C>>(c),
    m_signal
  };
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline alloy::core::connection
  alloy::core::sink<R(Args...)>::connect(Callable* callable)
  noexcept
{
  ALLOY_ASSERT(callable != nullptr);
  ALLOY_ASSERT(m_signal != nullptr);

  using disconnect_type = connection::disconnect_callback;

  auto& callbacks = m_signal->m_callbacks;

  callbacks.emplace_back(
    callback_type::make(callable)
  );

  return connection{
    disconnect_type::make<&disconnect<Callable>>(callable),
    m_signal
  };
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline alloy::core::connection
  alloy::core::sink<R(Args...)>::connect(const Callable* callable)
  noexcept
{
  using disconnect_type = connection::disconnect_callback;

  ALLOY_ASSERT(callable != nullptr);
  ALLOY_ASSERT(m_signal != nullptr);
  auto& callbacks = m_signal->m_callbacks;

  callbacks.emplace_back(
    callback_type::make(callable)
  );

  return connection{
    disconnect_type::make<&disconnect<const Callable>>(callable),
    m_signal
  };
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline void alloy::core::sink<R(Args...)>::reserve(size_type size)
{
  ALLOY_ASSERT(m_signal != nullptr);
  auto& callbacks = m_signal->m_callbacks;

  callbacks.reserve(size);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline typename alloy::core::sink<R(Args...)>::size_type
  alloy::core::sink<R(Args...)>::capacity()
  const noexcept
{
  ALLOY_ASSERT(m_signal != nullptr);
  return m_signal->m_callbacks.capacity();
}


template <typename R, typename...Args>
inline typename alloy::core::sink<R(Args...)>::size_type
  alloy::core::sink<R(Args...)>::size()
  const noexcept
{
  ALLOY_ASSERT(m_signal != nullptr);
  return m_signal->m_callbacks.size();
}


template <typename R, typename...Args>
inline bool alloy::core::sink<R(Args...)>::empty()
  const noexcept
{
  ALLOY_ASSERT(m_signal != nullptr);
  return m_signal->m_callbacks.empty();
}

//-----------------------------------------------------------------------------
// Private Static Functions
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn>
inline void alloy::core::sink<R(Args...)>::disconnect(void* signal)
  noexcept
{
  ALLOY_ASSERT(signal != nullptr);
  auto& callbacks = static_cast<signal_type*>(signal)->m_callbacks;

  const auto it = std::remove(
    callbacks.begin(),
    callbacks.end(),
    callback_type::template make<Fn>()
  );
  callbacks.erase(it, callbacks.cend());
}


template <typename R, typename...Args>
template <auto MemberFn, typename C>
inline void alloy::core::sink<R(Args...)>::disconnect(C* c, void* signal)
  noexcept
{
  ALLOY_ASSERT(c != nullptr);
  ALLOY_ASSERT(signal != nullptr);
  auto& callbacks = static_cast<signal_type*>(signal)->m_callbacks;

  const auto it = std::remove(
    callbacks.begin(),
    callbacks.end(),
    callback_type::template make<MemberFn>(c)
  );
  callbacks.erase(it, callbacks.cend());
}


template <typename R, typename...Args>
template <typename Callable>
inline void alloy::core::sink<R(Args...)>::disconnect(Callable* callable, void* signal)
  noexcept
{
  ALLOY_ASSERT(callable != nullptr);
  ALLOY_ASSERT(signal != nullptr);
  auto& callbacks = static_cast<signal_type*>(signal)->m_callbacks;

  const auto it = std::remove(
    callbacks.begin(),
    callbacks.end(),
    callback_type::template make(callable)
  );
  callbacks.erase(it, callbacks.cend());
}

//=============================================================================
// definitions : class : connection
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

inline alloy::core::connection::connection()
  noexcept
  : m_disconnect{},
    m_signal{nullptr}
{

}


inline alloy::core::connection::connection(connection&& other)
  noexcept
  : m_disconnect{other.m_disconnect},
    m_signal{other.m_signal}
{
  other.m_disconnect.reset();
}


inline alloy::core::connection&
  alloy::core::connection::operator=(connection&& other)
  noexcept
{
  m_disconnect = other.m_disconnect;
  m_signal = other.m_signal;

  other.m_disconnect.reset();

  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline bool alloy::core::connection::connected()
  const noexcept
{
  return static_cast<bool>(m_disconnect);
}


inline alloy::core::connection::operator bool()
  const noexcept
{
  return connected();
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

inline void alloy::core::connection::disconnect()
  noexcept
{
  if (m_disconnect) {
    m_disconnect(m_signal);
    m_disconnect.reset();
  }
}

//-----------------------------------------------------------------------------
// Private Constructors
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline alloy::core::connection::connection(disconnect_callback disconnect,
                                           signal<R(Args...)>* signal)
  noexcept
  : m_disconnect{disconnect},
    m_signal{signal}
{

}

//=============================================================================
// definitions : class : scoped_connection
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

inline alloy::core::scoped_connection::scoped_connection(connection conn)
  noexcept
  : m_connection{std::move(conn)}
{

}

//-----------------------------------------------------------------------------

inline alloy::core::scoped_connection::~scoped_connection()
{
  m_connection.disconnect();
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline bool alloy::core::scoped_connection::connected()
  const noexcept
{
  return m_connection.connected();
}


inline alloy::core::scoped_connection::operator bool()
  const noexcept
{
  return connected();
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

inline void alloy::core::scoped_connection::disconnect()
  noexcept
{
  m_connection.disconnect();
}

#endif /* ALLOY_CORE_UTILITIES_SIGNAL_HPP */