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
#include "alloy/core/utilities/delegate.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <type_traits> // std::enable_if_t, etc
#include <algorithm>   // std::find, std::iter_swap

namespace alloy::core {

  //===========================================================================
  // class : event
  //===========================================================================

  template <typename Fn>
  class signal;

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A class for encapsulating an event emitting system.
  ///
  /// This type is used for the primary event source, which will be subscribed
  /// to be event::sink instances, which are effectively arrays of handlers
  /// supplied by the client.
  ///
  /// Events are triggered by a 3rd type, event::trigger -- which enables
  /// encapsulating the source of the event better than the typical
  /// slots/signals approach (wherein a public signal can be emitted by anyone
  /// with an instance to the type that exposes the signal).
  ///
  /// Instances of this type are immovable and uncopyable. Consumers of this
  /// type are expected to define their own semantics for copying and moving,
  /// if this is desired.
  ///
  /// Example usage:
  ///
  /// \code
  /// using example_event = alloy::core::signal<void(std::string_view)>;
  ///
  /// class example
  /// {
  /// public:
  ///
  ///   example_event file_event; // used to watch events
  ///
  ///   example()
  ///     : event{},
  ///       m_trigger{}
  ///   {
  ///      // uniquely acquire the trigger
  ///      event.acquire_trigger(&m_trigger);
  ///   }
  ///
  ///   void emit(std::string_view x)
  ///   {
  ///     m_trigger.emit(x);
  ///   }
  ///
  /// private:
  ///   example_event::trigger m_trigger; // used to trigger events
  /// };
  ///
  /// auto e = example{};
  /// auto sink = example_event::sink{2u, allocator{}};
  ///
  /// sink.add_listener<&::some_event_handler>();
  /// sink.add_listener<&::some_other_event_handler>();
  /// e.file_event.bind(&sink);
  ///
  /// // emit "hello world" to 'some_event_handler' and 'some_other_event_handler'
  /// e.emit("hello world");
  ///
  /// e.file_event.unbind(); // events will no longer be handled
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

    class sink;

    class trigger;

    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a event that does not have a bound function
    signal() noexcept;
    signal(signal&&) = delete;
    signal(const signal&) = delete;

    //-------------------------------------------------------------------------

    signal& operator=(signal&&) = delete;
    signal& operator=(const signal&) = delete;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Binds a new sink to this event, returning the previous event
    ///
    /// \pre \p s cannot be \c nullptr
    ///
    /// \param s the sink to bind
    /// \return the pointer to the previously bound sink
    sink* bind(not_null<sink*> s) noexcept;

    /// \brief Unbinds the currently active sink, returning a pointer to the
    ///        previous sink
    ///
    /// \return the pointer to the previously bound sink
    sink* unbind() noexcept;

    /// \brief Acquires the trigger for this event
    ///
    /// \note This action is transactional; the first invocation to call this
    ///       will be assigned the trigger, and future invocations will not
    ///
    /// \param t the pointer to the trigger to be associated with this event
    void acquire_trigger(not_null<trigger*> t) noexcept;

    //-------------------------------------------------------------------------
    // Private Friend Hooks
    //-------------------------------------------------------------------------
  private:

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
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>,
              typename=std::enable_if_t<!std::is_void_v<R2>>>
    void emit(CollectorFn&& collector, UArgs&&...args);

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    sink* m_sink;
    bool m_trigger_acquired;

    friend class signal<R(Args...)>::trigger;
  };

  //===========================================================================
  // class : signal<R(Args...)>::sink
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
  class signal<R(Args...)>::sink
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
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a sink of the specified \p size using the underlying
    ///        \p alloc
    ///
    /// \pre \p size is non-zero
    /// \pre \p alloc has storage for \p size handlers
    ///
    /// \param size the number of handlers this sink can manage
    /// \param alloc the allocator
    sink(std::size_t size, allocator alloc) noexcept;

    sink() = delete;
    sink(sink&&) = delete;
    sink(const sink&) = delete;

    //-------------------------------------------------------------------------

    ~sink();

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
    template <auto Fn,
              typename=enable_if_invocable_t<decltype(Fn),Args...>>
    void add_listener() noexcept;

    /// \brief Adds a member function pointer listener to this event
    ///
    /// \tparam MemberFn the member function pointer to bind
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),C*,Args...>>
    void add_listener(C* c) noexcept;

    /// \brief Adds a const member function pointer listener to this event
    ///
    /// \tparam MemberFn the const member function pointer to bind
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),const C*,Args...>>
    void add_listener(const C* c) noexcept;

    /// \{
    /// \brief Adds a callable listener to this event
    ///
    /// \param callable the callable to bind
    template <typename Callable,
              typename=enable_if_invocable_t<Callable&,Args...>>
    void add_listener(Callable* callable) noexcept;
    template <typename Callable,
              typename=enable_if_invocable_t<const Callable&,Args...>>
    void add_listener(const Callable* callable) noexcept;
    /// \}

    // Disallow binding nulls
    template <auto MemberFn>
    void add_listener(std::nullptr_t) = delete;
    void add_listener(std::nullptr_t) = delete;

    //-------------------------------------------------------------------------

    /// \brief Adds a non-member function pointer listener to this event
    ///
    /// \tparam Fn the function pointer to bind
    template <auto Fn,
              typename=enable_if_invocable_t<decltype(Fn),Args...>>
    void remove_listener() noexcept;

    /// \brief Adds a member function pointer listener to this event
    ///
    /// \tparam MemberFn the member function pointer to bind
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),C*,Args...>>
    void remove_listener(C* c) noexcept;

    /// \brief Adds a const member function pointer listener to this event
    ///
    /// \tparam MemberFn the const member function pointer to bind
    template <auto MemberFn, typename C,
              typename=enable_if_invocable_t<decltype(MemberFn),const C*,Args...>>
    void remove_listener(const C* c) noexcept;

    /// \{
    /// \brief Adds a callable listener to this event
    ///
    /// \param callable the callable to bind
    template <typename Callable,
              typename=enable_if_invocable_t<Callable&,Args...>>
    void remove_listener(Callable* callable) noexcept;
    template <typename Callable,
              typename=enable_if_invocable_t<const Callable&,Args...>>
    void remove_listener(const Callable* callable) noexcept;
    /// \}

    // Disallow binding nulls
    template <auto MemberFn>
    void remove_listener(std::nullptr_t) = delete;
    void remove_listener(std::nullptr_t) = delete;

    //-------------------------------------------------------------------------
    // Triggers
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
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>,
              typename=std::enable_if_t<!std::is_void_v<R2>>>
    void emit(CollectorFn&& collector, UArgs&&...args);

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the capacity of this event sink
    ///
    /// \return the number of entries this sink can store
    std::size_t capacity() const noexcept;

    /// \brief Gets the current number of entries this sink stores
    ///
    /// \return the number of entries this sink stores
    std::size_t size() const noexcept;

    /// \brief Queries whether this sink has any handlers stored
    ///
    /// \return \c true if this event has no handlers
    bool empty() const noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    using event_handler = delegate<R(Args...)>;

    event_handler* m_handlers;
    allocator m_allocator;
    std::size_t m_capacity;
    std::size_t m_size;

    friend class signal<R(Args...)>;
  };

  //===========================================================================
  // class : signal<R(Args...)>::trigger
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A disjoint class that can be used to trigger events from an
  ///        \c signal<R(Args...)>
  ///
  /// This decomposition is to prevent types from being able to trigger events
  /// that otherwise should not. This is typically an inherent problem in the
  /// 'slot-signal' approach, wherein any public 'signal' is capable of
  /// being emitted by any consumer of the class, even if the class itself is
  /// meant to be the sole emitter of that event.
  ///
  /// By decoupling it here in this manner, the trigger can be encapsulated
  /// in the class that does the signal emition. This comes at the cost of two
  /// separate pointers; one in the 'event' class to keep track of the trigger
  /// (necessary for rebinding on moves and copies), and one in this trigger
  /// class to keep track of the original event.
  ///
  /// \note No effort has been made to ensure that triggers cannot outlive
  ///       the event in which they are associated to. This is a concern that
  ///       should be managed by the consumer. Typical usage is for the trigger
  ///       to be a private member of the same class in which 'event' is a
  ///       public member.
  /////////////////////////////////////////////////////////////////////////////
  template <typename R, typename...Args>
  class signal<R(Args...)>::trigger
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
    // Constructor / Assignment
    //-------------------------------------------------------------------------
  public:

    trigger() noexcept;
    trigger(trigger&& other) = default;
    trigger(const trigger& other) = default;

    //-------------------------------------------------------------------------

    trigger& operator=(trigger&& other) = default;
    trigger& operator=(const trigger& other) = default;

    //-------------------------------------------------------------------------
    // Emition
    //-------------------------------------------------------------------------
  public:

    template <typename...UArgs,
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>>
    void emit(UArgs&&...args);

    template <typename CollectorFn,
              typename...UArgs,
              typename R2=R,
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>,
              typename=std::enable_if_t<!std::is_void_v<R2>>>
    void emit(CollectorFn&& collector, UArgs&&...args);

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    signal<R(Args...)>* m_source;

    friend class signal<R(Args...)>;
  };

} // namespace alloy::core

//=============================================================================
// inline definition : class : signal<R(Args...)>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline alloy::core::signal<R(Args...)>::signal()
  noexcept
  : m_sink{nullptr},
    m_trigger_acquired{false}
{

}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline typename alloy::core::signal<R(Args...)>::sink*
  alloy::core::signal<R(Args...)>::bind(not_null<sink*> s)
  noexcept
{
  auto* result = m_sink;
  m_sink = s.get();
  return result;
}


template <typename R, typename...Args>
inline typename alloy::core::signal<R(Args...)>::sink*
  alloy::core::signal<R(Args...)>::unbind()
  noexcept
{
  auto* result = m_sink;
  m_sink = nullptr;
  return result;
}


template <typename R, typename...Args>
inline void
  alloy::core::signal<R(Args...)>::acquire_trigger(not_null<trigger*> t)
  noexcept
{
  if (m_trigger_acquired) {
    t->m_source = this;
    m_trigger_acquired = true;
  }
}

//-----------------------------------------------------------------------------
// Private Friend Hooks
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <typename...UArgs, typename>
inline void alloy::core::signal<R(Args...)>::emit(UArgs&&...args)
{
  ALLOY_ASSERT(m_sink != nullptr);

  m_sink->emit(std::forward<UArgs>(args)...);
}


template <typename R, typename...Args>
template <typename CollectorFn, typename...UArgs, typename, typename, typename>
inline void alloy::core::signal<R(Args...)>::emit(CollectorFn&& collector,
                                                   UArgs&&...args)
{
  ALLOY_ASSERT(m_sink != nullptr);

  m_sink->emit(
    std::forward<CollectorFn>(collector),
    std::forward<UArgs>(args)...
  );
}

//=============================================================================
// inline definition : class : signal<R(Args...)>::sink
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline alloy::core::signal<R(Args...)>::sink::sink(std::size_t size,
                                                  allocator alloc)
  noexcept
  : m_handlers{alloc.make_array<event_handler>(size)},
    m_allocator{alloc},
    m_capacity{size},
    m_size{0u}
{
  ALLOY_ASSERT(size != 0u);
  ALLOY_ASSERT(m_handlers != nullptr);
}

//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline alloy::core::signal<R(Args...)>::sink::~sink()
{
  m_allocator.dispose_array<event_handler>(m_handlers, m_capacity);
}

//-----------------------------------------------------------------------------
// Event Handlers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn, typename>
inline void alloy::core::signal<R(Args...)>::sink::add_listener()
  noexcept
{
  ALLOY_ASSERT(m_handlers != nullptr);
  ALLOY_ASSERT(m_size < m_capacity);

  m_handlers[m_size++] = event_handler::template bind<Fn>();
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline void alloy::core::signal<R(Args...)>::sink::add_listener(C* c)
  noexcept
{
  ALLOY_ASSERT(m_handlers != nullptr);
  ALLOY_ASSERT(m_size < m_capacity);

  m_handlers[m_size++] = event_handler::bind<MemberFn>(c);
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline void alloy::core::signal<R(Args...)>::sink::add_listener(const C* c)
  noexcept
{
  ALLOY_ASSERT(m_handlers != nullptr);
  ALLOY_ASSERT(m_size < m_capacity);

  m_handlers[m_size++] = event_handler::bind<MemberFn>(c);
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline void alloy::core::signal<R(Args...)>::sink::add_listener(Callable* callable)
  noexcept
{
  ALLOY_ASSERT(m_handlers != nullptr);
  ALLOY_ASSERT(m_size < m_capacity);

  m_handlers[m_size++] = event_handler::bind(callable);
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline void alloy::core::signal<R(Args...)>::sink::add_listener(const Callable* callable)
  noexcept
{
  ALLOY_ASSERT(m_handlers != nullptr);
  ALLOY_ASSERT(m_size < m_capacity);

  m_handlers[m_size++] = event_handler::bind(callable);
}

//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <auto Fn, typename>
inline void alloy::core::signal<R(Args...)>::sink::remove_listener()
  noexcept
{
  const auto val = event_handler::template bind<Fn>();
  const auto* begin = &m_handlers[0];
  const auto* end   = &m_handlers[m_size];
  const auto* last  = &m_handlers[m_size-1];

  const auto* it = std::find(begin, end, val);

  ALLOY_ASSERT(it != end, "Listener was never registered");

  std::iter_swap(it, last);
  --m_size;
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline void alloy::core::signal<R(Args...)>::sink::remove_listener(C* c)
  noexcept
{
  const auto val = event_handler::bind<MemberFn>(c);
  const auto* begin = &m_handlers[0];
  const auto* end   = &m_handlers[m_size];
  const auto* last  = &m_handlers[m_size-1];

  const auto* it = std::find(begin, end, val);

  ALLOY_ASSERT(it != end, "Listener was never registered");

  std::iter_swap(it, last);
  --m_size;
}


template <typename R, typename...Args>
template <auto MemberFn, typename C, typename>
inline void alloy::core::signal<R(Args...)>::sink::remove_listener(const C* c)
  noexcept
{
  const auto val = event_handler::bind<MemberFn>(c);
  const auto* begin = &m_handlers[0];
  const auto* end   = &m_handlers[m_size];
  const auto* last  = &m_handlers[m_size-1];

  const auto* it = std::find(begin, end, val);

  ALLOY_ASSERT(it != end, "Listener was never registered");

  std::iter_swap(it, last);
  --m_size;
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline void alloy::core::signal<R(Args...)>::sink::remove_listener(Callable* callable)
  noexcept
{
  const auto val = event_handler::bind(callable);
  const auto* begin = &m_handlers[0];
  const auto* end   = &m_handlers[m_size];
  const auto* last  = &m_handlers[m_size-1];

  const auto* it = std::find(begin, end, val);

  ALLOY_ASSERT(it != end, "Listener was never registered");

  std::iter_swap(it, last);
  --m_size;
}


template <typename R, typename...Args>
template <typename Callable, typename>
inline void alloy::core::signal<R(Args...)>::sink::remove_listener(const Callable* callable)
  noexcept
{
  const auto val = event_handler::bind(callable);
  const auto* begin = &m_handlers[0];
  const auto* end   = &m_handlers[m_size];
  const auto* last  = &m_handlers[m_size-1];

  const auto* it = std::find(begin, end, val);

  ALLOY_ASSERT(it != end, "Listener was never registered");

  std::iter_swap(it, last);
  --m_size;
}

//-----------------------------------------------------------------------------
// Triggers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <typename...UArgs, typename>
inline void alloy::core::signal<R(Args...)>::sink::emit(UArgs&&...args)
{
  const auto* begin = &m_handlers[0];
  const auto* end   = &m_handlers[m_size];

  for (const auto* current = begin; begin < end; ++begin) {
    const auto& handler = *current;

    handler(std::forward<UArgs>(args)...);
  }
}


template <typename R, typename...Args>
template <typename CollectorFn, typename...UArgs, typename, typename, typename>
inline void alloy::core::signal<R(Args...)>::sink::emit(CollectorFn&& collector,
                                                         UArgs&&...args)
{
  const auto* begin = &m_handlers[0];
  const auto* end   = &m_handlers[m_size];

  for (const auto* current = begin; begin < end; ++begin) {
    const auto& handler = *current;

    std::forward<CollectorFn>(collector)(
      handler(std::forward<UArgs>(args)...)
    );
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline std::size_t alloy::core::signal<R(Args...)>::sink::capacity()
  const noexcept
{
  return m_capacity;
}


template <typename R, typename...Args>
inline std::size_t alloy::core::signal<R(Args...)>::sink::size()
  const noexcept
{
  return m_size;
}


template <typename R, typename...Args>
inline bool alloy::core::signal<R(Args...)>::sink::empty()
  const noexcept
{
  return m_size == 0u;
}

//=============================================================================
// inline definition : class : signal<R(Args...)>::trigger
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
inline alloy::core::signal<R(Args...)>::trigger::trigger()
  noexcept
  : m_source{nullptr}
{

}

//-----------------------------------------------------------------------------
// Emition
//-----------------------------------------------------------------------------

template <typename R, typename...Args>
template <typename...UArgs, typename>
inline void alloy::core::signal<R(Args...)>::trigger::emit(UArgs&&...args)
{
  ALLOY_ASSERT(m_source != nullptr);

  m_source->emit(std::forward<UArgs>(args)...);
}


template <typename R, typename...Args>
template <typename CollectorFn, typename...UArgs, typename, typename, typename>
inline void alloy::core::signal<R(Args...)>::trigger::emit(CollectorFn&& collector,
                                                            UArgs&&...args)
{
  ALLOY_ASSERT(m_source != nullptr);

  m_source->emit(
    std::forward<CollectorFn>(collector),
    std::forward<UArgs>(args)...
  );
}

#endif /* ALLOY_CORE_UTILITIES_SIGNAL_HPP */