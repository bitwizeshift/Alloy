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
  ///   using example_event = alloy::core::signal<void(std::string_view)>;
  ///
  ///   example()
  ///     : file_event{}
  ///   {
  ///   }
  ///
  ///   void bind(alloy::core::not_null<example_event::sink*> sink)
  ///   {
  ///     m_file_event.bind(sink);
  ///   }
  ///
  ///   void unbind()
  ///   {
  ///     m_file_event.unbind();
  ///   }
  ///
  ///   void emit(std::string_view x)
  ///   {
  ///     m_file_event.emit(x);
  ///   }
  ///
  /// private:
  ///   example_event m_file_event; // used to watch events
  /// };
  ///
  /// auto e = example{};
  /// auto sink = example::example_event::sink{2u, allocator{}};
  ///
  /// sink.add_listener<&::some_event_handler>();
  /// sink.add_listener<&::some_other_event_handler>();
  /// e.bind(&sink);
  ///
  /// // emit "hello world" to 'some_event_handler' and 'some_other_event_handler'
  /// e.emit("hello world");
  ///
  /// e.unbind(); // events will no longer be handled
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
              typename=std::enable_if_t<std::is_invocable_v<R(*)(Args...),UArgs...>>,
              typename=std::enable_if_t<!std::is_void_v<R2>>>
    void emit(CollectorFn&& collector, UArgs&&...args);

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    sink* m_sink;
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
  : m_sink{nullptr}
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

#endif /* ALLOY_CORE_UTILITIES_SIGNAL_HPP */