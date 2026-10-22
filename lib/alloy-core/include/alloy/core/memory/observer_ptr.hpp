// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file observer_ptr.hpp
 *
 * \brief This header contains the definition for observer_ptr
 *****************************************************************************/

#ifndef ALLOY_CORE_MEMORY_OBSERVER_PTR_HPP
#define ALLOY_CORE_MEMORY_OBSERVER_PTR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/utilities/not_null.hpp"

#include <concepts>    // std::convertible_to
#include <cstddef>     // std::nullptr_t
#include <type_traits> // type traits
#include <utility>     // std::move

namespace alloy::core {

  //===========================================================================
  // Forward Declarations
  //===========================================================================

  namespace detail {
    class observer_ptr_base;
  } // namespace detail

  class observable;

  template <typename T>
  class observer_ptr;

  //===========================================================================
  // class : observable
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An object in the system that may be observed by an observer_ptr
  ///
  /// Observerables contain extra data to be aware of their own observables,
  /// which allows the observing observer_ptr's to be nulled-out after
  /// destruction.
  ///
  /// Example usage:
  /// \code
  /// class some_lifetime_object : public observable
  /// {
  /// public:
  ///   some_lifetime_object() = default;
  ///   ...
  /// };
  /// \endcode
  /////////////////////////////////////////////////////////////////////////////
  class observable
  {
    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Checks whether this observable is being referenced by any
    ///        observer_ptrs
    ///
    /// \return `true` if this observable is referenced
    auto is_referenced() const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this observable
    observable() noexcept;

    /// \brief Constructs this observable by move-constructing from the
    ///        contents of \p other
    ///
    /// All intrinsic linked pointers are rebound from \p other to `this`
    ///
    /// \note Due to the pointer-rebinding, move-assignment may be more
    ///       expensive than typical copy-construction in order to preserve the
    ///       typical move semantics
    ///
    /// \param other the other observable to move
    observable(observable&& other) noexcept;

    /// \brief Constructs this observable by copy-construction
    ///
    /// \note No pointers are rebound when copying. Copying is effectively
    ///       just default-construction
    ///
    /// \param other the other observable to copy
    observable(const observable& other) noexcept;

    //-------------------------------------------------------------------------

    // Deleted copy and move assignment, since semantics for pointer
    // re-assignment would be complicated. Do observer_ptr's to 'other' get
    // rebound? Merged with current observer_ptr's? Ignored?
    // This is left to derived implementations to figure out.
    auto operator=(observable&& other) -> observable& = delete;
    auto operator=(const observable& other) -> observable& = delete;

    //-------------------------------------------------------------------------
    // Protected : Destructor
    //-------------------------------------------------------------------------
  public:

    /// \brief Destructs this observable, clearing all references to this
    ///        observable
    ~observable();

    //-------------------------------------------------------------------------
    // Protected : Modifiers
    //-------------------------------------------------------------------------
  protected:

    /// \brief Removes all references to this observable
    ///
    /// \post All observer_ptr instances that were pointing to this are now
    ///       pointing to `nullptr`
    auto clear_references() noexcept -> void;

    /// \brief Rebinds all references to the new observable
    ///
    /// \param other the observable to rebind all references to
    auto rebind_references(observable& other) noexcept -> void;

    //-------------------------------------------------------------------------
    // Friend Hooks : Modifiers
    //-------------------------------------------------------------------------
  private:

    /// \brief Adds \p p to this observable's record of all pointers observing
    ///        it
    ///
    /// \param p the base observer_ptr that is referencing this
    auto add(not_null<detail::observer_ptr_base*> p) const noexcept -> void;

    /// \brief Removes \p p from this observable's record of all pointers
    ///        observing it
    ///
    /// \param p the base observer_ptr that is no longer referencing this
    auto remove(not_null<detail::observer_ptr_base*> p) const noexcept -> void;

    /// \brief Rebinds the pointer from \p old_p to \p new_p
    ///
    /// \param old_p the old pointer
    /// \param new_p the new pointer
    auto rebind(
      not_null<detail::observer_ptr_base*> old_p,
      not_null<detail::observer_ptr_base*> new_p
    ) const noexcept -> void;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    mutable detail::observer_ptr_base* m_head; ///< Head of the intrinsic list

    friend class detail::observer_ptr_base;
  };

  namespace detail {
    ///////////////////////////////////////////////////////////////////////////
    /// \brief A non-templated base class for the observer_ptr
    ///
    /// \note This is an internal type, not intended for direct consumption
    ///////////////////////////////////////////////////////////////////////////
    class observer_ptr_base
    {
      //-----------------------------------------------------------------------
      // Protected : Constructors / Destructor
      //-----------------------------------------------------------------------
    protected:

      observer_ptr_base() noexcept;
      explicit observer_ptr_base(const observable* p) noexcept;
      observer_ptr_base(observer_ptr_base&& other) noexcept;
      observer_ptr_base(const observer_ptr_base& other) noexcept;

      //-----------------------------------------------------------------------

      ~observer_ptr_base();

      //-----------------------------------------------------------------------

      auto operator=(observer_ptr_base&&) -> observer_ptr_base& = delete;
      auto operator=(const observer_ptr_base&) -> observer_ptr_base& = delete;

      //-----------------------------------------------------------------------
      // Protected Modifiers
      //-----------------------------------------------------------------------
    protected:

      /// Make this point to pObject and enlist.
      auto do_reset(const observable* p) noexcept -> void;

      /// Make this point to NULL and delist.
      auto do_reset() noexcept -> void;

      /// Rebinds to \p other
      auto do_rebind(observer_ptr_base& other) noexcept -> void;

      //-----------------------------------------------------------------------
      // Protected Members
      //-----------------------------------------------------------------------
    protected:

      const observable* m_object;

      //-----------------------------------------------------------------------
      // Private Members
      //-----------------------------------------------------------------------
    private:

      observer_ptr_base* m_next;

      friend class alloy::core::observable;
    };
  } // namespace deatil

  /////////////////////////////////////////////////////////////////////////////
  /// \brief observer_ptr is a simple, lightweight, non-thread-safe solution
  ///        to viewing pointers that might dangle.
  ///
  /// This class is intended to be used in places where ownership may
  /// eventually go out of scope, and where lifetime may eventually exit
  /// scope -- for example in the use of listeners/observer code that will be
  /// executed by clients.
  ///
  /// The observer_ptr has little overhead -- requiring only a few pointers
  /// using an intrinsicly linked list to operate.
  ///
  /// Example usage:
  /// \code
  /// class some_lifetime_object : public observable
  /// {
  /// public:
  ///     some_lifetime_object() = default;
  ///     ...
  /// };
  ///
  /// auto observer_object = observer_ptr<some_lifetime_object>{};
  /// {
  ///   auto object = some_lifetime_object{}
  ///   observer_object = observer_ptr<some_lifetime_object>{&object};
  ///
  ///   observer_object->do_something();
  /// } // end of scope
  /// assert(observer_ptr == nullptr) // observer_ptr is now null
  /// \endcode
  ///
  /// \tparam T the underlying type of the observer_ptr
  /////////////////////////////////////////////////////////////////////////////
  template<typename T>
  class observer_ptr
    : public detail::observer_ptr_base
  {
    static_assert(std::is_base_of<observable,T>::value);

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using element_type = T;
    using pointer   = std::add_pointer_t<element_type>;
    using reference = std::add_lvalue_reference_t<element_type>;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Constructs an observer_ptr pointing to null
    observer_ptr() noexcept;
    observer_ptr(std::nullptr_t) noexcept;
    /// \}

    /// \brief Constructs this observer_ptr from a pointer that is
    ///        convertible to T*
    ///
    /// \param p the pointer to observe
    template <typename U>
      requires std::convertible_to<U*, T*>
    explicit observer_ptr(U* p) noexcept;

    /// \brief Constructs this observer_ptr through move-construction
    ///
    /// \param other the other observer_ptr to move
    observer_ptr(observer_ptr&& other) noexcept;

    /// \brief Constructs this observer_ptr through copy-construction
    ///
    /// \param other the other observer_ptr to copy
    observer_ptr(const observer_ptr& other) noexcept;

    /// \brief Constructs this observer_ptr through move-construction of a
    ///        similar observer_ptr
    ///
    /// \param other the other observer_ptr to move
    template <typename U>
      requires std::convertible_to<U*, T*>
    observer_ptr(observer_ptr<U>&& other) noexcept;

    /// \brief Constructs this observer_ptr through copy-construction of a
    ///        similar observer_ptr
    ///
    /// \param other the other observer_ptr to copy
    template <typename U>
      requires std::convertible_to<U*, T*>
    observer_ptr(const observer_ptr<U>& other) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this observer_ptr
    ///
    /// \param other the other observer_ptr to move
    /// \return a reference to `(*this)`
    auto operator=(observer_ptr&& other) noexcept -> observer_ptr&;

    /// \brief Assigns the contents of \p other to this observer_ptr
    ///
    /// \param other the other observer_ptr to copy
    /// \return a reference to `(*this)`
    auto operator=(const observer_ptr& other) noexcept -> observer_ptr&;

    /// \brief Assigns the contents of \p p to this observer_ptr
    ///
    /// \param p the pointer to observe
    /// \return a reference to `(*this)`
    template <typename U>
      requires std::convertible_to<U*, T*>
    auto operator=(U* p) noexcept -> observer_ptr&;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Swaps the contents of \p other and this
    ///
    /// \param other the other observer_ptr to swap
    auto swap(observer_ptr& other) noexcept -> void;

    /// \{
    /// \brief Resets the underlying pointer of this observer_ptr to null
    auto reset() noexcept -> void;
    auto reset(std::nullptr_t) noexcept -> void;
    /// \}

    /// \brief Resets the underlying pointer of this observer_ptr to \p p
    ///
    /// \param p the new pointer to observe
    auto reset(pointer p) noexcept -> void;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying pointer from this observer_ptr
    ///
    /// \return the underlying pointer
    auto get() const noexcept -> pointer;

    auto operator*() const noexcept -> reference;
    auto operator->() const noexcept -> pointer;

    /// \brief Convertible to `true` when this pointer is null, `false`
    ///        otherwise
    explicit operator bool() const noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    template <typename> friend class observer_ptr;
  };

  //===========================================================================
  // non-member functions : class : observer_ptr
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  template <typename T, typename U>
  auto operator==(const observer_ptr<T>& lhs, const observer_ptr<U>& rhs)
    noexcept -> bool;
  template <typename T>
  auto operator==(const observer_ptr<T>& lhs, std::nullptr_t) noexcept -> bool;
  template <typename T>
  auto operator==(std::nullptr_t, const observer_ptr<T>& rhs) noexcept -> bool;

  //---------------------------------------------------------------------------

  template <typename T, typename U>
  auto operator!=(const observer_ptr<T>& lhs, const observer_ptr<U>& rhs)
    noexcept -> bool;
  template <typename T>
  auto operator!=(const observer_ptr<T>& lhs, std::nullptr_t) noexcept -> bool;
  template <typename T>
  auto operator!=(std::nullptr_t, const observer_ptr<T>& rhs) noexcept -> bool;

} // namespace alloy::core

//=============================================================================
// inline definition : class : observable
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline
auto alloy::core::observable::is_referenced() const noexcept -> bool
{
  return m_head != nullptr;
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline
alloy::core::observable::observable() noexcept
  : m_head{nullptr}
{

}


inline
alloy::core::observable::observable(observable&& other) noexcept
  : m_head{other.m_head}
{
  other.m_head = nullptr;

  rebind_references(*this);
}


inline
alloy::core::observable::observable(const observable& other) noexcept
  : m_head{nullptr}
{
  (void) other;
}


//-----------------------------------------------------------------------------
// Protected : Destructor
//-----------------------------------------------------------------------------

inline
alloy::core::observable::~observable()
{
  clear_references();
}

//-----------------------------------------------------------------------------
// Protected : Modifier
//-----------------------------------------------------------------------------

inline
auto alloy::core::observable::clear_references() noexcept -> void
{
  auto* p = m_head;

  // Rebind all observer_ptr instances to nullptr
  while (p != nullptr) {
    auto* const next = p->m_next;
    p->m_next = nullptr;
    p->m_object = nullptr;
    p = next;
  }
}


inline
auto alloy::core::observable::rebind_references(observable& other)
  noexcept -> void
{
  auto* p = m_head;

  // Rebind all observer_ptr instances to this new one
  while (p != nullptr) {
    p->m_object = &other;
    p = p->m_next;
  }
}

//-----------------------------------------------------------------------------
// Friend Hooks : Modifiers
//-----------------------------------------------------------------------------

inline
auto alloy::core::observable::add(not_null<detail::observer_ptr_base*> p)
  const noexcept -> void
{
  p->m_next = m_head;
  m_head = p.get();
}


inline
auto alloy::core::observable::remove(not_null<detail::observer_ptr_base*> p)
  const noexcept -> void
{
  // If 'p' is the head, change the head pointer
  if (p.get() == m_head) {
    m_head = m_head->m_next;
    return;
  }

  // If 'p' is not the head, iterate to find the current entry and rebind
  auto* prev = m_head;
  auto* current = m_head->m_next;

  while (current != nullptr) {
    if (current == p.get()) {
      prev->m_next = current->m_next;
      return;
    }
    prev = current;
    current = current->m_next;
  }
}


inline
auto alloy::core::observable::rebind(
  not_null<detail::observer_ptr_base*> old_p,
  not_null<detail::observer_ptr_base*> new_p
) const noexcept -> void
{
  // If 'old_p' is the head, rebind to new_p
  if (old_p.get() == m_head) {
    new_p->m_next = m_head->m_next;
    m_head = new_p.get();
    old_p->m_next = nullptr;
    return;
  }

  // If 'old_p' is not the head, iterate to find the current entry and rebind
  auto* prev = m_head;
  auto* current = m_head->m_next;

  while (current != nullptr) {
    if (current == old_p.get()) {
      prev->m_next = new_p.get();
      new_p->m_next = current->m_next;
      old_p->m_next = nullptr;
      return;
    }
    prev = current;
    current = current->m_next;
  }
}

//=============================================================================
// class : detail::observer_ptr_base
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor
//-----------------------------------------------------------------------------

inline
alloy::core::detail::observer_ptr_base::observer_ptr_base() noexcept
  : m_object{nullptr},
    m_next{nullptr}
{

}


inline
alloy::core::detail::observer_ptr_base::observer_ptr_base(const observable* p)
  noexcept
  : m_object{p},
    m_next{nullptr}
{
  if (m_object) {
    m_object->add(this);
  }
}


inline
alloy::core::detail::observer_ptr_base::observer_ptr_base(
  observer_ptr_base&& other
) noexcept
  : m_object{other.m_object},
    m_next{nullptr}
{
  if (m_object != nullptr) {
    m_object->rebind(&other, this);
  }
  other.m_next = nullptr;
}


inline
alloy::core::detail::observer_ptr_base::observer_ptr_base(
  const observer_ptr_base& other
) noexcept
  : m_object{other.m_object},
    m_next{nullptr}
{
  if (m_object != nullptr) {
    m_object->add(this);
  }
}


//-----------------------------------------------------------------------------

inline
alloy::core::detail::observer_ptr_base::~observer_ptr_base()
{
  if (m_object != nullptr) {
    m_object->remove(this);
  }
}


//-----------------------------------------------------------------------------
// Protected : Modifiers
//-----------------------------------------------------------------------------

inline
auto alloy::core::detail::observer_ptr_base::do_reset(const observable* p)
  noexcept -> void
{
  if (p == m_object) {
    return;
  }

  if (m_object != nullptr) {
    m_object->remove(this);
  }

  m_object = p;

  if (m_object != nullptr) {
    m_object->add(this);
  }
}


inline
auto alloy::core::detail::observer_ptr_base::do_reset() noexcept -> void
{
  if (m_object != nullptr) {
    m_object->remove(this);
    m_object = nullptr;
  }
}

inline
auto alloy::core::detail::observer_ptr_base::do_rebind(observer_ptr_base& other)
  noexcept -> void
{
  if (other.m_object != nullptr) {
    other.m_object->rebind(&other, this);
    m_object = other.m_object;
    other.m_object = nullptr;
  }

}

//=============================================================================
// class : observer_ptr<T>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename T>
inline
alloy::core::observer_ptr<T>::observer_ptr() noexcept
  : observer_ptr{nullptr}
{

}


template <typename T>
inline
alloy::core::observer_ptr<T>::observer_ptr(std::nullptr_t) noexcept
  : observer_ptr_base{}
{

}


template <typename T>
template <typename U>
  requires std::convertible_to<U*, T*>
inline
alloy::core::observer_ptr<T>::observer_ptr(U* p) noexcept
  : observer_ptr_base{p}
{

}


template <typename T>
inline
alloy::core::observer_ptr<T>::observer_ptr(observer_ptr&& other) noexcept
  : observer_ptr_base{std::move(other)}
{

}


template <typename T>
inline
alloy::core::observer_ptr<T>::observer_ptr(const observer_ptr& other) noexcept
  : observer_ptr_base{other}
{

}


template <typename T>
template <typename U>
  requires std::convertible_to<U*, T*>
inline
alloy::core::observer_ptr<T>::observer_ptr(observer_ptr<U>&& other) noexcept
  : observer_ptr_base{std::move(other)}
{

}


template <typename T>
template <typename U>
  requires std::convertible_to<U*, T*>
inline
alloy::core::observer_ptr<T>::observer_ptr(const observer_ptr<U>& other)
  noexcept
  : observer_ptr_base{other}
{

}

//-----------------------------------------------------------------------------

template <typename T>
inline
auto alloy::core::observer_ptr<T>::operator=(observer_ptr&& other)
  noexcept -> observer_ptr<T>&
{
  do_rebind(other);
  return (*this);
}


template <typename T>
inline
auto alloy::core::observer_ptr<T>::operator=(const observer_ptr& other)
  noexcept -> observer_ptr<T>&
{
  do_reset(other.m_object);
  return (*this);
}


template <typename T>
template <typename U>
  requires std::convertible_to<U*, T*>
inline
auto alloy::core::observer_ptr<T>::operator=(U* p) noexcept -> observer_ptr<T>&
{
  reset(p);
  return (*this);
}


//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename T>
inline
auto alloy::core::observer_ptr<T>::swap(observer_ptr& other) noexcept -> void
{
  // TODO: optimize
  using std::swap;

  swap(other,*this);
}


template <typename T>
inline
auto alloy::core::observer_ptr<T>::reset() noexcept -> void
{
  do_reset();
}


template <typename T>
inline
auto alloy::core::observer_ptr<T>::reset(std::nullptr_t) noexcept -> void
{
  do_reset();
}


template <typename T>
inline
auto alloy::core::observer_ptr<T>::reset(pointer p) noexcept -> void
{
  do_reset(p);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T>
inline
auto alloy::core::observer_ptr<T>::get()
  const noexcept -> typename observer_ptr<T>::pointer
{
  return static_cast<T*>(const_cast<observable*>(m_object));
}


template <typename T>
inline
auto alloy::core::observer_ptr<T>::operator*()
  const noexcept -> typename observer_ptr<T>::reference
{
  return *get();
}


template <typename T>
inline
auto alloy::core::observer_ptr<T>::operator->()
  const noexcept -> typename observer_ptr<T>::pointer
{
  return get();
}


template <typename T>
inline
alloy::core::observer_ptr<T>::operator bool() const noexcept
{
  return m_object != nullptr;
}

//=============================================================================
// inline-definitions : non-member functions : class : observer_ptr
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

template <typename T, typename U>
inline
auto alloy::core::operator==(
  const observer_ptr<T>& lhs,
  const observer_ptr<U>& rhs
) noexcept -> bool
{
  return lhs.get() == rhs.get();
}


template <typename T>
inline
auto alloy::core::operator==(const observer_ptr<T>& lhs, std::nullptr_t)
  noexcept -> bool
{
  return lhs.get() == nullptr;
}


template <typename T>
inline
auto alloy::core::operator==(std::nullptr_t, const observer_ptr<T>& rhs)
  noexcept -> bool
{
  return nullptr == rhs.get();
}

//-----------------------------------------------------------------------------

template <typename T, typename U>
inline
auto alloy::core::operator!=(
  const observer_ptr<T>& lhs,
  const observer_ptr<U>& rhs
) noexcept -> bool
{
  return lhs.get() != rhs.get();
}


template <typename T>
inline
auto alloy::core::operator!=(const observer_ptr<T>& lhs, std::nullptr_t)
  noexcept -> bool
{
  return lhs.get() != nullptr;
}


template <typename T>
inline
auto alloy::core::operator!=(std::nullptr_t, const observer_ptr<T>& rhs)
  noexcept -> bool
{
  return nullptr != rhs.get();
}

#endif /* ALLOY_CORE_MEMORY_OBSERVER_PTR_HPP */

