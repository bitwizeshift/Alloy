/*****************************************************************************
 * \file observer_ptr.hpp
 *
 * \brief This header contains the definition for observer_ptr
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
#ifndef ALLOY_CORE_UTILITIES_OBSERVER_PTR_HPP
#define ALLOY_CORE_UTILITIES_OBSERVER_PTR_HPP

#include "alloy/core/utilities/not_null.hpp"

#include <type_traits> // std::enable_if_t, etc
#include <cstddef> // std::nullptr_t
#include <utility> // std::move

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
    /// \return \c true if this observable is referenced
    bool is_referenced() const noexcept;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this observable
    observable() noexcept;

    /// \brief Constructs this observable by move-constructing from the
    ///        contents of \p other
    ///
    /// All intrinsic linked pointers are rebound from \p other to \c this
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
    observable& operator=(observable&& other) = delete;
    observable& operator=(const observable& other) = delete;

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
    ///       pointing to \c nullptr
    void clear_references() noexcept;

    /// \brief Rebinds all references to the new observable
    ///
    /// \param other the observable to rebind all references to
    void rebind_references(observable& other) noexcept;

    //-------------------------------------------------------------------------
    // Friend Hooks : Modifiers
    //-------------------------------------------------------------------------
  private:

    /// \brief Adds \p p to this observable's record of all pointers observing
    ///        it
    ///
    /// \param p the base observer_ptr that is referencing this
    void add(not_null<detail::observer_ptr_base*> p) const noexcept;

    /// \brief Removes \p p from this observable's record of all pointers
    ///        observing it
    ///
    /// \param p the base observer_ptr that is no longer referencing this
    void remove(not_null<detail::observer_ptr_base*> p) const noexcept;

    /// \brief Rebinds the pointer from \p old_p to \p new_p
    ///
    /// \param old_p the old pointer
    /// \param new_p the new pointer
    void rebind(not_null<detail::observer_ptr_base*> old_p,
                not_null<detail::observer_ptr_base*> new_p) const noexcept;

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

      observer_ptr_base& operator=(observer_ptr_base&&) = delete;
      observer_ptr_base& operator=(const observer_ptr_base&) = delete;

      //-----------------------------------------------------------------------
      // Protected Modifiers
      //-----------------------------------------------------------------------
    protected:

      /// Make this point to pObject and enlist.
      void do_reset(const observable* p) noexcept;

      /// Make this point to NULL and delist.
      void do_reset() noexcept;

      /// Rebinds to \p other
      void do_rebind(observer_ptr_base& other) noexcept;

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
    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U*,T*>>>
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
    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U*,T*>>>
    observer_ptr(observer_ptr<U>&& other) noexcept;

    /// \brief Constructs this observer_ptr through copy-construction of a
    ///        similar observer_ptr
    ///
    /// \param other the other observer_ptr to copy
    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U*,T*>>>
    observer_ptr(const observer_ptr<U>& other) noexcept;

    //-------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this observer_ptr
    ///
    /// \param other the other observer_ptr to move
    /// \return a reference to \c (*this)
    observer_ptr& operator=(observer_ptr&& other) noexcept;

    /// \brief Assigns the contents of \p other to this observer_ptr
    ///
    /// \param other the other observer_ptr to copy
    /// \return a reference to \c (*this)
    observer_ptr& operator=(const observer_ptr& other) noexcept;

    /// \brief Assigns the contents of \p p to this observer_ptr
    ///
    /// \param p the pointer to observe
    /// \return a reference to \c (*this)
    template <typename U,
              typename = std::enable_if_t<std::is_convertible_v<U*,T*>>>
    observer_ptr& operator=(U* p) noexcept;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Swaps the contents of \p other and this
    ///
    /// \param other the other observer_ptr to swap
    void swap(observer_ptr& other) noexcept;

    /// \{
    /// \brief Resets the underlying pointer of this observer_ptr to null
    void reset() noexcept;
    void reset(std::nullptr_t) noexcept;
    /// \}

    /// \brief Resets the underlying pointer of this observer_ptr to \p p
    ///
    /// \param p the new pointer to observe
    void reset(pointer p) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the underlying pointer from this observer_ptr
    ///
    /// \return the underlying pointer
    pointer get() const noexcept;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;

    /// \brief Convertible to \c true when this pointer is null, \c false
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
  bool operator==(const observer_ptr<T>& lhs,
                  const observer_ptr<U>& rhs) noexcept;
  template <typename T>
  bool operator==(const observer_ptr<T>& lhs, std::nullptr_t) noexcept;
  template <typename T>
  bool operator==(std::nullptr_t, const observer_ptr<T>& rhs) noexcept;

  //---------------------------------------------------------------------------

  template <typename T, typename U>
  bool operator!=(const observer_ptr<T>& lhs,
                  const observer_ptr<U>& rhs) noexcept;
  template <typename T>
  bool operator!=(const observer_ptr<T>& lhs, std::nullptr_t) noexcept;
  template <typename T>
  bool operator!=(std::nullptr_t, const observer_ptr<T>& rhs) noexcept;

} // namespace alloy::core

//=============================================================================
// inline definition : class : observable
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline bool alloy::core::observable::is_referenced()
  const noexcept
{
  return m_head != nullptr;
}

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline alloy::core::observable::observable()
  noexcept
  : m_head{nullptr}
{

}


inline alloy::core::observable::observable(observable&& other)
  noexcept
  : m_head{other.m_head}
{
  other.m_head = nullptr;

  rebind_references(*this);
}


inline alloy::core::observable::observable(const observable& other)
  noexcept
  : m_head{nullptr}
{
  (void) other;
}


//-----------------------------------------------------------------------------
// Protected : Destructor
//-----------------------------------------------------------------------------

inline alloy::core::observable::~observable()
{
  clear_references();
}

//-----------------------------------------------------------------------------
// Protected : Modifier
//-----------------------------------------------------------------------------

inline void alloy::core::observable::clear_references()
  noexcept
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


inline void alloy::core::observable::rebind_references(observable& other)
  noexcept
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

inline void
  alloy::core::observable::add(not_null<detail::observer_ptr_base*> p)
  const noexcept
{
  p->m_next = m_head;
  m_head = p.get();
}


inline void
  alloy::core::observable::remove(not_null<detail::observer_ptr_base*> p)
  const noexcept
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


inline void
  alloy::core::observable::rebind(not_null<detail::observer_ptr_base*> old_p,
                                  not_null<detail::observer_ptr_base*> new_p)
  const noexcept
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

inline alloy::core::detail::observer_ptr_base::observer_ptr_base()
  noexcept
  : m_object{nullptr},
    m_next{nullptr}
{

}


inline alloy::core::detail::observer_ptr_base
  ::observer_ptr_base(const observable* p)
  noexcept
  : m_object{p},
    m_next{nullptr}
{
  if (m_object) {
    m_object->add(this);
  }
}


inline alloy::core::detail::observer_ptr_base
  ::observer_ptr_base(observer_ptr_base&& other)
  noexcept
  : m_object{other.m_object},
    m_next{nullptr}
{
  if (m_object != nullptr) {
    m_object->rebind(&other, this);
  }
  other.m_next = nullptr;
}


inline alloy::core::detail::observer_ptr_base
  ::observer_ptr_base(const observer_ptr_base& other)
  noexcept
  : m_object{other.m_object},
    m_next{nullptr}
{
  if (m_object != nullptr) {
    m_object->add(this);
  }
}


//-----------------------------------------------------------------------------

inline alloy::core::detail::observer_ptr_base::~observer_ptr_base()
{
  if (m_object != nullptr) {
    m_object->remove(this);
  }
}


//-----------------------------------------------------------------------------
// Protected : Modifiers
//-----------------------------------------------------------------------------

inline void
  alloy::core::detail::observer_ptr_base::do_reset(const observable* p)
  noexcept
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


inline void alloy::core::detail::observer_ptr_base::do_reset()
  noexcept
{
  if (m_object != nullptr) {
    m_object->remove(this);
    m_object = nullptr;
  }
}

inline void
  alloy::core::detail::observer_ptr_base::do_rebind(observer_ptr_base& other)
  noexcept
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
inline alloy::core::observer_ptr<T>::observer_ptr()
  noexcept
  : observer_ptr{nullptr}
{

}


template <typename T>
inline alloy::core::observer_ptr<T>::observer_ptr(std::nullptr_t)
  noexcept
  : observer_ptr_base{}
{

}


template <typename T>
template <typename U, typename>
inline alloy::core::observer_ptr<T>::observer_ptr(U* p)
  noexcept
  : observer_ptr_base{p}
{

}


template <typename T>
inline alloy::core::observer_ptr<T>::observer_ptr(observer_ptr&& other)
  noexcept
  : observer_ptr_base{std::move(other)}
{

}


template <typename T>
inline alloy::core::observer_ptr<T>::observer_ptr(const observer_ptr& other)
  noexcept
  : observer_ptr_base{other}
{

}


template <typename T>
template <typename U, typename>
inline alloy::core::observer_ptr<T>::observer_ptr(observer_ptr<U>&& other)
  noexcept
  : observer_ptr_base{std::move(other)}
{

}


template <typename T>
template <typename U, typename>
inline alloy::core::observer_ptr<T>::observer_ptr(const observer_ptr<U>& other)
  noexcept
  : observer_ptr_base{other}
{

}

//-----------------------------------------------------------------------------

template <typename T>
inline alloy::core::observer_ptr<T>&
  alloy::core::observer_ptr<T>::operator=(observer_ptr&& other)
  noexcept
{
  do_rebind(other);
  return (*this);
}


template <typename T>
inline alloy::core::observer_ptr<T>&
  alloy::core::observer_ptr<T>::operator=(const observer_ptr& other)
  noexcept
{
  do_reset(other.m_object);
  return (*this);
}


template <typename T>
template <typename U, typename>
inline alloy::core::observer_ptr<T>&
  alloy::core::observer_ptr<T>::operator=(U* p)
  noexcept
{
  reset(p);
  return (*this);
}


//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename T>
inline void alloy::core::observer_ptr<T>::swap(observer_ptr& other)
  noexcept
{
  // TODO: optimize
  using std::swap;

  swap(other,*this);
}


template <typename T>
inline void alloy::core::observer_ptr<T>::reset()
  noexcept
{
  do_reset();
}


template <typename T>
inline void alloy::core::observer_ptr<T>::reset(std::nullptr_t)
  noexcept
{
  do_reset();
}


template <typename T>
inline void alloy::core::observer_ptr<T>::reset(pointer p)
  noexcept
{
  do_reset(p);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T>
inline typename alloy::core::observer_ptr<T>::pointer
  alloy::core::observer_ptr<T>::get()
  const noexcept
{
  return static_cast<T*>(const_cast<observable*>(m_object));
}


template <typename T>
inline typename alloy::core::observer_ptr<T>::reference
  alloy::core::observer_ptr<T>::operator*()
  const noexcept
{
  return *get();
}


template <typename T>
inline typename alloy::core::observer_ptr<T>::pointer
  alloy::core::observer_ptr<T>::operator->()
  const noexcept
{
  return get();
}


template <typename T>
inline alloy::core::observer_ptr<T>::operator bool()
  const noexcept
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
inline bool alloy::core::operator==(const observer_ptr<T>& lhs,
                                    const observer_ptr<U>& rhs)
  noexcept
{
  return lhs.get() == rhs.get();
}


template <typename T>
inline bool alloy::core::operator==(const observer_ptr<T>& lhs,
                                    std::nullptr_t)
  noexcept
{
  return lhs.get() == nullptr;
}


template <typename T>
inline bool alloy::core::operator==(std::nullptr_t,
                                    const observer_ptr<T>& rhs)
  noexcept
{
  return nullptr == rhs.get();
}

//-----------------------------------------------------------------------------

template <typename T, typename U>
inline bool alloy::core::operator!=(const observer_ptr<T>& lhs,
                                    const observer_ptr<U>& rhs)
  noexcept
{
  return lhs.get() != rhs.get();
}


template <typename T>
inline bool alloy::core::operator!=(const observer_ptr<T>& lhs,
                                    std::nullptr_t)
  noexcept
{
  return lhs.get() != nullptr;
}


template <typename T>
inline bool alloy::core::operator!=(std::nullptr_t,
                                    const observer_ptr<T>& rhs)
  noexcept
{
  return nullptr != rhs.get();
}

#endif /* ALLOY_CORE_UTILITIES_OBSERVER_PTR_HPP */