/*****************************************************************************
 * \file allocator.hpp
 *
 * \brief This header contains the definition for allocator
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
#ifndef ALLOY_CORE_MEMORY_ALLOCATOR_HPP
#define ALLOY_CORE_MEMORY_ALLOCATOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/core/memory/memory_resource.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/scope_guard.hpp"

#include <cstring>     // std::memcpy
#include <cstddef>     // std::size_t, std::max_align_t
#include <type_traits> // std::enable_if, etc
#include <cstdlib>     // std::aligned_alloc, std::free
#include <new>         // std::launder

namespace alloy::core {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The basic allocator used in the engine
  ///
  /// This allocator deviates from the C++ standard allocator in the following
  /// ways:
  ///
  /// * Allocations always support over-aligned allocations
  /// * The allocator either creates raw bytes, *or* allocates and constructs
  ///   a full type (no broken 'allocate' + 'construct' calls)
  ///
  /// Conceptually, all allocators are handles to memory resources.
  /////////////////////////////////////////////////////////////////////////////
  class allocator
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    static constexpr auto default_align = std::align_val_t{
      alignof(std::max_align_t)
    };

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs this allocator using the default resource
    allocator() noexcept;

    /// \brief Constructs this allocator using the specified memory resource
    ///
    /// \pre \p resource cannot be null
    /// \param resource the resource to use for allocation
    explicit allocator(not_null<memory_resource*> resource) noexcept;

    allocator(allocator&&) = default;
    allocator(const allocator&) = default;

    //-------------------------------------------------------------------------

    allocator& operator=(allocator&&) = default;
    allocator& operator=(const allocator&) = default;

    //-------------------------------------------------------------------------
    // Allocation
    //-------------------------------------------------------------------------
  public:

    /// \brief Allocates bytes from the underlying memory resource
    ///
    /// \param bytes the number of bytes to allocate
    /// \param align the alignment of the allocation
    /// \return a pointer to the allocated bytes on success, nullptr on failure
    void* allocate_bytes(std::size_t bytes,
                         std::align_val_t align = default_align) noexcept;

    /// \brief Deallocates memory pointer to by \p p of size \p bytes
    ///
    /// The \p bytes and \p align parameters must be the same size as was
    /// requested of the original allocate, expand_allocate, or reallocate
    /// functions.
    ///
    /// \param p pointer to the memory to deallocate
    /// \param bytes the number of bytes to deallocate
    /// \param align the alignment of the allocation
    void deallocate_bytes(void* p,
                          std::size_t bytes,
                          std::align_val_t align = default_align) noexcept;

    /// \brief Attempts to resizes memory previously allocated
    ///
    /// \param p pointer to memory to attempt to resize
    /// \param bytes the new byte size
    /// \return \c true if the memory was able to be resized
    bool resize_allocation(void* p, std::size_t bytes) noexcept;

    /// \brief Attempts to resize memory previously allocated, and falls back
    ///        to allocating new memory instead
    ///
    /// \note Memory stored in \p p must be trivially destructible and
    ///       trivially copyable to attempt to reallocate
    ///
    /// \param p pointer to the memory to attempt to resize
    /// \param old_size the size that was previously requested
    /// \param new_size the new size that has been requested
    /// \param align the alignment previously requested when allocating
    /// \return a pointer to the allocated bytes
    void* reallocate_bytes(void* p,
                           std::size_t old_size,
                           std::size_t new_size,
                           std::align_val_t align = default_align) noexcept;

    //-------------------------------------------------------------------------
    // Make / Dispose
    //-------------------------------------------------------------------------
  public:

    /// \brief Allocates memory and constructs an instance of type T
    ///
    /// \tparam T the type to construct
    /// \param args the arguments to forward to T's constructor
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T on success, nullptr on failure
    template <typename T, typename...Args,
              typename = std::enable_if_t<std::is_constructible<T,Args...>::value>>
    T* make(Args&&...args)
      noexcept(std::is_nothrow_constructible<T,Args...>::value);

    /// \brief Allocates memory for an array of default-constructed T's
    ///
    /// \tparam T the type to construct
    /// \param n the size of the array to construct
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T array on success,
    ///         nullptr on failure
    template <typename T,
              typename = std::enable_if_t<std::is_constructible<T>::value>>
    T* make_array(std::size_t n)
      noexcept(std::is_nothrow_constructible<T>::value);

    /// \brief Allocates memory for an array of copy-constructed T's
    ///
    /// \tparam T the type to construct
    /// \param n the size of the array to construct
    /// \param u a type copy-convertable to T
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T array on success, nullptr on failure
    template <typename T, typename U,
              typename = std::enable_if_t<std::is_constructible<T,const U&>::value>>
    T* make_array(std::size_t n, const U& u)
      noexcept(std::is_nothrow_constructible<T, const U&>::value);

    /// \brief Deallocates memory previously allocated with a call to 'make'
    ///        for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    template <typename T>
    void dispose(not_null<T*> p) noexcept;

    /// \brief Deallocates memory previously allocated with a call to
    ///        'make_array' for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    /// \param n the size of the array to deallocate
    template <typename T>
    void dispose_array(not_null<T*> p, std::size_t n) noexcept;

    //-------------------------------------------------------------------------
    // Aligned Make / Dispose
    //-------------------------------------------------------------------------
  public:

    /// \brief Allocates over-aligned memory and constructs an instance of
    ///        type T
    ///
    /// \pre align must be at least alignof(T)
    ///
    /// \tparam T the type to construct
    /// \param align the alignment to construct the T with
    /// \param args the arguments to forward to T's constructor
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T on success, nullptr on failure
    template <typename T, typename...Args,
              typename = std::enable_if_t<std::is_constructible<T,Args...>::value>>
    T* aligned_make(std::align_val_t align, Args&&...args)
      noexcept(std::is_nothrow_constructible<T,Args...>::value);

    /// \brief Allocates over-aligned memory for an array of
    ///        default-constructed T's
    ///
    /// \pre align must be at least alignof(T)
    ///
    /// \tparam T the type to construct
    /// \param n the size of the array to construct
    /// \param align the alignment to align the array to
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T array on success,
    ///         nullptr on failure
    template <typename T,
              typename = std::enable_if_t<std::is_constructible<T>::value>>
    T* aligned_make_array(std::size_t n, std::align_val_t align)
      noexcept(std::is_nothrow_constructible<T>::value);


    /// \brief Allocates over-aligned memory for an array of
    ///        copy-constructed T's
    ///
    /// \pre align must be at least alignof(T)
    ///
    /// \tparam T the type to construct
    /// \param n the size of the array to construct
    /// \param align the alignment to align the array to
    /// \param u a type to copy-convert to T
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T array on success,
    ///         nullptr on failure
    template <typename T, typename U,
              typename = std::enable_if_t<std::is_constructible<T,const U&>::value>>
    T* aligned_make_array(std::size_t n, std::align_val_t align, const U& u)
      noexcept(std::is_nothrow_constructible<T,const U&>::value);


    /// \brief Deallocates over-aligned memory previously allocated with a
    ///        call to 'aligned_make' for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    /// \param align the alignment of the over-aligned type
    template <typename T>
    void aligned_dispose(not_null<T*> p, std::align_val_t align) noexcept;


    /// \brief Deallocates over-aligned memory previously allocated with a
    ///        call to 'aligned_make_array' for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    /// \param n the number of entries in the array
    /// \param align the alignment of the over-aligned type
    template <typename T>
    void aligned_dispose_array(not_null<T*> p,
                               std::size_t n,
                               std::align_val_t align) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets a pointer to the underlying memory resource
    ///
    /// \return a pointer to the underlying resource
    memory_resource* resource() const noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    class default_resource;

    /// \brief Gets the default resource
    ///
    /// \return the default resource
    static default_resource& get_default_resource() noexcept;

    memory_resource* m_resource;
  };

  //===========================================================================
  // class : allocator::default_resource
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The default memory resource for allocators
  ///
  /// This just calls basic aligned_offset_alloc and aligned_offset_free
  /// calls.
  /////////////////////////////////////////////////////////////////////////////
  class allocator::default_resource final
    : public memory_resource
  {
  public:

    default_resource() noexcept = default;

  private:

    void* do_allocate(std::size_t size,
                      std::align_val_t align) noexcept override;

    void do_deallocate(void* p,
                       std::size_t size,
                       std::align_val_t align) noexcept override;
  };

  //===========================================================================
  // class : stl_allocator_adapter<T>
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Adapts an alloy::core::allocator into a standard C++ allocator,
  ///        for use with standard containers.
  ///
  /// \tparam T the type for the allocator to allocate
  /////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class stl_allocator_adapter
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------

    stl_allocator_adapter() noexcept = default;
    explicit stl_allocator_adapter(allocator alloc) noexcept;

    stl_allocator_adapter(stl_allocator_adapter&&) = default;
    stl_allocator_adapter(const stl_allocator_adapter&) = default;

    template <typename U>
    stl_allocator_adapter(const stl_allocator_adapter<U>& other) noexcept;

    stl_allocator_adapter& operator=(stl_allocator_adapter&&) = default;
    stl_allocator_adapter& operator=(const stl_allocator_adapter&) = default;

    //-------------------------------------------------------------------------
    // Allocation
    //-------------------------------------------------------------------------

    T* allocate(std::size_t n) noexcept;

    void deallocate(T* p, std::size_t n) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    allocator m_allocator;

    template<typename> friend class stl_allocator_adapter;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : allocator
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------


inline alloy::core::allocator::allocator()
  noexcept
  : m_resource{&get_default_resource()}
{

}


inline alloy::core::allocator::allocator(not_null<memory_resource*> resource)
  noexcept
  : m_resource{resource.get()}
{

}

//-----------------------------------------------------------------------------
// Allocation
//-----------------------------------------------------------------------------

inline void* alloy::core::allocator::allocate_bytes(std::size_t bytes,
                                                    std::align_val_t align)
  noexcept
{
  return m_resource->allocate(bytes, align);
}


inline void alloy::core::allocator::deallocate_bytes(void* p,
                                                     std::size_t bytes,
                                                     std::align_val_t align)
  noexcept
{
  return m_resource->deallocate(p, bytes, align);
}


inline bool alloy::core::allocator::resize_allocation(void* p,
                                                      std::size_t bytes)
  noexcept
{
  return m_resource->resize_allocation(p, bytes);
}


inline void* alloy::core::allocator::reallocate_bytes(void* p,
                                                      std::size_t old_size,
                                                      std::size_t new_size,
                                                      std::align_val_t align)
  noexcept
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  // Attempt to just resize first
  if (resize_allocation(p, new_size)) {
    return p;
  }

  auto* const new_p = allocate_bytes(new_size, align);

  // Ensure we remember to delete the old allocation on scope exit
  auto guard = on_scope_exit([&]{
    deallocate_bytes(p, old_size, align);
  });
  (void) guard;

  if (ALLOY_UNLIKELY(new_p == nullptr)) {
    return nullptr;
  }

  const auto size = (old_size < new_size ? old_size : new_size);
  std::memcpy(new_p, p, size);

  return new_p;
}


//-----------------------------------------------------------------------------
// Make / Dispose
//-----------------------------------------------------------------------------

template <typename T, typename...Args, typename>
inline T* alloy::core::allocator::make(Args&&...args)
  noexcept(std::is_nothrow_constructible<T,Args...>::value)
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  return aligned_make<T>(
    std::align_val_t{alignof(T)},
    std::forward<Args>(args)...
  );
}


template <typename T, typename>
inline T* alloy::core::allocator::make_array(std::size_t n)
  noexcept(std::is_nothrow_constructible<T>::value)
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  return aligned_make_array<T>(std::align_val_t{alignof(T)}, n);
}


template <typename T, typename U, typename>
inline T* alloy::core::allocator::make_array(std::size_t n, const U& u)
  noexcept(std::is_nothrow_constructible<T, const U&>::value)
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  return aligned_make_array<T>(n, std::align_val_t{alignof(T)}, u);
}


template <typename T>
inline void alloy::core::allocator::dispose(not_null<T*> p)
  noexcept
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  aligned_dispose<T>(p, std::align_val_t{alignof(T)});
}


template <typename T>
inline void alloy::core::allocator::dispose_array(not_null<T*> p,
                                                  std::size_t n)
  noexcept
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  aligned_dispose_array<T>(p, n, std::align_val_t{alignof(T)});
}

//-----------------------------------------------------------------------------
// Aligned Make / Dispose
//-----------------------------------------------------------------------------

template <typename T, typename...Args, typename>
inline T* alloy::core::allocator::aligned_make(std::align_val_t align, Args&&...args)
  noexcept(std::is_nothrow_constructible<T,Args...>::value)
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  ALLOY_ASSERT(
    align >= std::align_val_t{alignof(T)},
    "Alignment must be greater than the required alignment of T"
  );
  auto* const p = static_cast<T*>(allocate_bytes(sizeof(T), align));

  if (ALLOY_UNLIKELY(p == nullptr)) {
    return p;
  }

#ifdef ALLOY_CORE_EXCEPTIONS_ENABLED
  if constexpr(std::is_nothrow_constructible<T,Args...>::value) {
    return new (p) T(std::forward<Args>(args)...);
  } else {
    try {
      return new (p) T(std::forward<Args>(args)...);
    } catch (...) {
      deallocate_bytes(p, sizeof(T), align);
      throw;
    }
  }
#else
  return new (p) T(std::forward<Args>(args)...);
#endif
}

template <typename T, typename>
inline T* alloy::core::allocator::aligned_make_array(std::size_t n,
                                                     std::align_val_t align)
  noexcept(std::is_nothrow_constructible<T>::value)
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  ALLOY_ASSERT(
    align >= std::align_val_t{alignof(T)},
    "Alignment must be at least the required alignment of T"
  );
  ALLOY_ASSERT(
    n != 0,
    "Array must be non-zero size"
  );

  const auto bytes = sizeof(T) * n;
  auto* const p = static_cast<T*>(allocate_bytes(bytes, align));

  if (ALLOY_UNLIKELY(p == nullptr)) {
    return p;
  }

  auto* const begin = p;
  auto* const end   = begin + n;
  auto* current = begin;
  auto* result = static_cast<T*>(begin);
#ifdef ALLOY_CORE_EXCEPTIONS_ENABLED
  if constexpr(std::is_nothrow_constructible<T>::value) {
    result = new (current++) T();
    for (; current < end; ++current) {
      new (current) T();
    }
  } else {
    try {
      result = new (current++) T();
      for (; current < end; ++current) {
        new (current) T();
      }
    } catch (...) {
      // Call destructor on T's that have already constructed...
      --current;

      for (; current >= begin; --current) {
        current->~T();
      }
      deallocate_bytes(p, bytes, align);
      throw;
    }
  }
#else
  result = new (current++) T();
  for (; current < end; ++current) {
    new (current) T();
  }
#endif
  return result;
}

template <typename T, typename U, typename>
inline T* alloy::core::allocator::aligned_make_array(std::size_t n,
                                                     std::align_val_t align,
                                                     const U& u)
  noexcept(std::is_nothrow_constructible<T,const U&>::value)
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  ALLOY_ASSERT(
    align >= std::align_val_t{alignof(T)},
    "Alignment must be at least the required alignment of T"
  );
  ALLOY_ASSERT(
    n != 0,
    "Array must be non-zero size"
  );

  const auto bytes = sizeof(T) * n;
  auto* const p = static_cast<T*>(allocate_bytes(bytes, align));

  if (ALLOY_UNLIKELY(p == nullptr)) {
    return p;
  }

  auto* const begin = p;
  auto* const end   = begin + n;
  auto* current = begin;
  auto* result = static_cast<T*>(begin);
#ifdef ALLOY_CORE_EXCEPTIONS_ENABLED
  if constexpr(std::is_nothrow_constructible<T>::value) {
    result = new (current++) T(u);
    for (; current < end; ++current) {
      new (current) T(u);
    }
  } else {
    try {
      result = new (current++) T(u);
      for (; current < end; ++current) {
        new (current) T(u);
      }
    } catch (...) {
      // Call destructor on T's that have already constructed...
      --current;

      for (; current >= begin; --current) {
        current->~T();
      }
      deallocate_bytes(p, bytes, align);
      throw;
    }
  }
#else
  result = new (current++) T(u);
  for (; current < end; ++current) {
    new (current) T(u);
  }
#endif
  return result;
}


template <typename T>
inline void alloy::core::allocator::aligned_dispose(not_null<T*> p,
                                                    std::align_val_t align)
  noexcept
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  ALLOY_ASSERT(
    align >= std::align_val_t{alignof(T)},
    "Alignment must be at least the required alignment of T"
  );

  auto* const t = p.get();

  t->~T();
  deallocate_bytes(t, sizeof(T), align);
}


template <typename T>
inline void alloy::core::allocator::aligned_dispose_array(not_null<T*> p,
                                                          std::size_t n,
                                                          std::align_val_t align)
  noexcept
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  ALLOY_ASSERT(
    align >= std::align_val_t{alignof(T)},
    "Alignment must be at least the required alignment of T"
  );
  ALLOY_ASSERT(
    n != 0,
    "Array must be non-zero size"
  );

  const auto bytes = sizeof(T) * n;

  // Destructor is called in reverse order to preserve standard C++
  // destroy semantics
  auto* const t = p.get();
  auto* const rbegin = t + (n - 1);
  auto* const rend   = t;
  auto* current = rbegin;
  for (; current >= rend; --current) {
    current->~T();
  }

  deallocate_bytes(t, bytes, align);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline alloy::core::memory_resource* alloy::core::allocator::resource()
  const noexcept
{
  return m_resource;
}

//-----------------------------------------------------------------------------
// Private Members
//-----------------------------------------------------------------------------

inline alloy::core::allocator::default_resource&
  alloy::core::allocator::get_default_resource()
  noexcept
{
  static auto s_resource = default_resource{};

  return s_resource;
}

//=============================================================================
// inline definitions : class : allocator::default_resource
//=============================================================================

//-----------------------------------------------------------------------------
// Virtual Hooks : Allocation
//-----------------------------------------------------------------------------

inline void*
  alloy::core::allocator::default_resource::do_allocate(std::size_t bytes,
                                                        std::align_val_t align)
  noexcept
{
  return ::operator new(bytes, align);
}


inline void
  alloy::core::allocator::default_resource::do_deallocate(void* p,
                                                          std::size_t bytes,
                                                          std::align_val_t align)
  noexcept
{
  ::operator delete(p, bytes, align);
}

//=============================================================================
// inline definitions : class : stl_allocator_adapter<T>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename T>
inline alloy::core::stl_allocator_adapter<T>
  ::stl_allocator_adapter(allocator alloc)
  noexcept
  : m_allocator{alloc}
{

}


template <typename T>
template <typename U>
inline alloy::core::stl_allocator_adapter<T>
  ::stl_allocator_adapter(const stl_allocator_adapter<U>& other)
  noexcept
  : m_allocator{other.m_allocator}
{

}

//-----------------------------------------------------------------------------
// Allocation
//-----------------------------------------------------------------------------

template <typename T>
inline T* alloy::core::stl_allocator_adapter<T>::allocate(std::size_t n)
  noexcept
{
  auto* const p = m_allocator.allocate_bytes(
    sizeof(T) * n,
    std::align_val_t{alignof(T)}
  );

  ALLOY_ASSERT(p != nullptr);

  return static_cast<T*>(p);
}


template <typename T>
inline void alloy::core::stl_allocator_adapter<T>::deallocate(T* p,
                                                              std::size_t n)
  noexcept
{
  return m_allocator.deallocate_bytes(p, sizeof(T) * n);
}


#endif /* ALLOY_CORE_MEMORY_ALLOCATOR_HPP */