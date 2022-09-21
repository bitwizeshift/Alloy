/*****************************************************************************
 * \file allocator.hpp
 *
 * \brief This header contains the definition for allocator
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019-2022 Matthew Rodusek All rights reserved.

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

#include "alloy/core/memory/lifetime_utilities.hpp"
#include "alloy/core/memory/memory_resource.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/scope_guard.hpp"
#include "alloy/core/intrinsics.hpp"

#include <cstring>     // std::memcpy
#include <cstddef>     // std::size_t, std::max_align_t
#include <type_traits> // std::enable_if, etc
#include <cstdlib>     // std::aligned_alloc, std::free
#include <new>         // ::operator new, ::operator delete
#include <algorithm>   // std::copy

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

    static constexpr auto default_align = alignof(std::max_align_t);

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

    allocator(allocator&&) noexcept = default;
    allocator(const allocator&) noexcept = default;

    //-------------------------------------------------------------------------

    auto operator=(allocator&&) noexcept -> allocator& = default;
    auto operator=(const allocator&) noexcept -> allocator& = default;

    //-------------------------------------------------------------------------
    // Allocation : Bytes
    //-------------------------------------------------------------------------
  public:

    /// \brief Allocates bytes from the underlying memory resource
    ///
    /// \param bytes the number of bytes to allocate
    /// \param align the alignment of the allocation
    /// \return a pointer to the allocated bytes on success, nullptr on failure
    [[nodiscard]]
    auto allocate_bytes(
      std::size_t bytes,
      std::size_t align = default_align
    ) noexcept -> void*;

    /// \brief Deallocates memory pointer to by \p p of size \p bytes
    ///
    /// The \p bytes and \p align parameters must be the same size as was
    /// requested of the original allocate, expand_allocate, or reallocate
    /// functions.
    ///
    /// \param p pointer to the memory to deallocate
    /// \param bytes the number of bytes to deallocate
    /// \param align the alignment of the allocation
    auto deallocate_bytes(
      void* p,
      std::size_t bytes,
      std::size_t align = default_align
    ) noexcept -> void;

    /// \brief Attempts to resizes memory previously allocated
    ///
    /// \param p pointer to memory to attempt to resize
    /// \param bytes the new byte size
    /// \return `true` if the memory was able to be resized
    [[nodiscard]]
    auto resize_bytes(void* p, std::size_t bytes) noexcept -> bool;

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
    [[nodiscard]]
    auto reallocate_bytes(
      void* p,
      std::size_t old_size,
      std::size_t new_size,
      std::size_t align = default_align
    ) noexcept -> void*;

    //-------------------------------------------------------------------------
    // Allocation : Object
    //-------------------------------------------------------------------------
  public:

    /// \brief Allocates n objects from the underlying memory resource
    ///
    /// \param n the number of objects to allocate
    /// \param align the alignment of the allocation
    /// \return a pointer to the allocated objects on success, nullptr on failure
    template<typename T>
    [[nodiscard]]
    auto allocate_objects(
      std::size_t n = 1u,
      std::size_t align = std::size_t{alignof(T)}
    ) noexcept -> T*;

    /// \brief Deallocates memory pointed to by \p p of size \p n
    ///
    /// The \p n and \p align parameters must be the same size as was
    /// requested of the original allocate_objects, resize_objects,
    /// or reallocate reallocate_objects functions.
    ///
    /// \note No destructors are called from this function; only the memory
    ///       for the objects are deallocated.
    ///
    /// \param p pointer to the memory to deallocate
    /// \param n the number of objects to deallocate
    /// \param align the alignment of the allocation
    template <typename T>
    auto deallocate_objects(
      not_null<T*> p,
      std::size_t n = 1u,
      std::size_t align = std::size_t{alignof(T)}
    ) noexcept -> void;

    /// \brief Attempts to resizes memory previously allocated
    ///
    /// \param p pointer to memory to attempt to resize
    /// \param n the new number of objects to resize to
    /// \return `true` if the memory was able to be resized
    template<typename T>
    auto resize_objects(not_null<T*> p, std::size_t n) noexcept -> bool;

    /// \brief Attempts to resize memory previously allocated, and falls back
    ///        to allocating new memory instead
    ///
    /// \note T must be a trivially-copyable type
    ///
    /// \param p pointer to the memory to attempt to resize
    /// \param old_size the size that was previously requested
    /// \param new_size the new size that has been requested
    /// \param align the alignment previously requested when allocating
    /// \return a pointer to the allocated bytes
    template<typename T>
    [[nodiscard]]
    auto reallocate_objects(
      not_null<T*> p,
      std::size_t old_size,
      std::size_t new_size,
      std::size_t align = std::size_t{alignof(T)}
    ) noexcept -> T*;

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
    template<
      typename T,
      typename... Args,
      typename = std::enable_if_t<std::is_constructible<T, Args...>::value>>
    [[nodiscard]]
    auto make(Args&&...args)
      noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T*;

    /// \brief Allocates memory for an array of default-constructed T's
    ///
    /// \tparam T the type to construct
    /// \param n the size of the array to construct
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T array on success,
    ///         nullptr on failure
    template<typename T,
             typename = std::enable_if_t<std::is_constructible<T>::value>>
    [[nodiscard]]
    auto make_array(std::size_t n)
      noexcept(std::is_nothrow_constructible<T>::value) -> T*;

    /// \brief Allocates memory for an array of copy-constructed T's
    ///
    /// \tparam T the type to construct
    /// \param n the size of the array to construct
    /// \param u a type copy-convertable to T
    /// \throws anything T's constructor may throw
    /// \return a pointer to a constructed T array on success, nullptr on failure
    template<
      typename T,
      typename U,
      typename = std::enable_if_t<std::is_constructible<T, const U&>::value>>
    [[nodiscard]]
    auto make_array(std::size_t n, const U& u)
      noexcept(std::is_nothrow_constructible<T, const U&>::value) -> T*;

    /// \brief Deallocates memory previously allocated with a call to 'make'
    ///        for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    template <typename T>
    auto dispose(not_null<T*> p) noexcept -> void;

    /// \brief Deallocates memory previously allocated with a call to
    ///        'make_array' for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    /// \param n the size of the array to deallocate
    template <typename T>
    auto dispose_array(not_null<T*> p, std::size_t n) noexcept -> void;

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
    template<
      typename T,
      typename... Args,
      typename = std::enable_if_t<std::is_constructible<T, Args...>::value>>
    [[nodiscard]]
    auto aligned_make(std::size_t align, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T*;

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
    template<typename T,
             typename = std::enable_if_t<std::is_constructible<T>::value>>
    [[nodiscard]]
    auto aligned_make_array(std::size_t n, std::size_t align)
      noexcept(std::is_nothrow_constructible<T>::value) -> T*;

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
    template<
      typename T,
      typename U,
      typename = std::enable_if_t<std::is_constructible<T, const U&>::value>>
    [[nodiscard]]
    auto aligned_make_array(
      std::size_t n,
      std::size_t align,
      const U& u
    ) noexcept(std::is_nothrow_constructible<T, const U&>::value) -> T*;

    /// \brief Deallocates over-aligned memory previously allocated with a
    ///        call to 'aligned_make' for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    /// \param align the alignment of the over-aligned type
    template <typename T>
    auto aligned_dispose(not_null<T*> p, std::size_t align) noexcept -> void;

    /// \brief Deallocates over-aligned memory previously allocated with a
    ///        call to 'aligned_make_array' for a given type \p T
    ///
    /// \tparam T the type to dispose
    /// \param p the pointer to deallocate memory for
    /// \param n the number of entries in the array
    /// \param align the alignment of the over-aligned type
    template <typename T>
    auto aligned_dispose_array(
      not_null<T*> p,
      std::size_t n,
      std::size_t align
    ) noexcept -> void;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets a pointer to the underlying memory resource
    ///
    /// \return a pointer to the underlying resource
    [[nodiscard]]
    auto resource() const noexcept -> not_null<memory_resource*>;

    //-------------------------------------------------------------------------
    // Private: Make
    //-------------------------------------------------------------------------
  private:

    template <typename T, typename...Args>
    [[nodiscard]]
    auto aligned_make_array_impl(
      std::size_t n,
      std::size_t align,
      const Args&...args
    ) noexcept(std::is_nothrow_constructible<T, const Args&...>::value) -> T*;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    class default_resource;

    /// \brief Gets the default resource
    ///
    /// \return the default resource
    static auto get_default_resource() noexcept -> default_resource&;

    not_null<memory_resource*> m_resource;
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
    auto do_allocate(std::size_t size, std::size_t align) noexcept
      -> void* override;

    auto do_deallocate(
      void* p,
      std::size_t size,
      std::size_t align
    ) noexcept -> void override;
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
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind
    {
      using other = stl_allocator_adapter<U>;
    };

    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_swap = std::true_type;
    using is_always_equal = std::false_type;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------

    stl_allocator_adapter() noexcept = default;
    explicit stl_allocator_adapter(allocator alloc) noexcept;

    stl_allocator_adapter(stl_allocator_adapter&&) noexcept = default;
    stl_allocator_adapter(const stl_allocator_adapter&) noexcept = default;

    template <typename U>
    stl_allocator_adapter(const stl_allocator_adapter<U>& other) noexcept;

    auto operator=(stl_allocator_adapter&&)
      noexcept -> stl_allocator_adapter& = default;
    auto operator=(const stl_allocator_adapter&)
      noexcept -> stl_allocator_adapter& = default;

    //-------------------------------------------------------------------------
    // Allocation
    //-------------------------------------------------------------------------

    auto allocate(std::size_t n) noexcept -> T*;

    auto deallocate(T* p, std::size_t n) noexcept -> void;

    /// \brief Gets a reference to the underlying Alloy allocator
    ///
    /// \return a reference to the underlying allocator
    [[nodiscard]] auto underlying() const noexcept -> const allocator&;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    allocator m_allocator;

    template<typename> friend class stl_allocator_adapter;
  };

  //===========================================================================
  // non-member functions : class : stl_allocator_adapter
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  template<typename T, typename U>
  auto operator==(const stl_allocator_adapter<T>& lhs,
                  const stl_allocator_adapter<U>& rhs) noexcept -> bool;

  template<typename T, typename U>
  auto operator!=(const stl_allocator_adapter<T>& lhs,
                  const stl_allocator_adapter<U>& rhs) noexcept -> bool;

} // namespace alloy::core

//=============================================================================
// inline definitions : class : allocator
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline
alloy::core::allocator::allocator()
  noexcept
  : m_resource{&get_default_resource()}
{

}

inline
alloy::core::allocator::allocator(not_null<memory_resource*> resource)
  noexcept
  : m_resource{resource.get()}
{

}

//-----------------------------------------------------------------------------
// Allocation : Bytes
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::allocator::allocate_bytes(
  std::size_t bytes,
  std::size_t align
) noexcept -> void*
{
  return m_resource->allocate(bytes, align);
}


ALLOY_FORCE_INLINE
auto alloy::core::allocator::deallocate_bytes(
  void* p,
  std::size_t bytes,
  std::size_t align
) noexcept -> void
{
  return m_resource->deallocate(p, bytes, align);
}

ALLOY_FORCE_INLINE
auto alloy::core::allocator::resize_bytes(void* p, std::size_t bytes)
  noexcept -> bool
{
  return m_resource->resize_allocation(p, bytes);
}

inline
auto alloy::core::allocator::reallocate_bytes(
  void* p,
  std::size_t old_size,
  std::size_t new_size,
  std::size_t align
) noexcept -> void*
{
  ALLOY_ASSERT(p != nullptr, "Pointer cannot be null");

  // Attempt to just resize first
  if (resize_bytes(p, new_size)) {
    return p;
  }

  auto* const new_p = allocate_bytes(new_size, align);

  // Ensure we remember to delete the old allocation on scope exit
  auto guard = scope_exit{[&]{
    deallocate_bytes(p, old_size, align);
  }};
  compiler::unused(guard);

  if (ALLOY_UNLIKELY(new_p == nullptr)) {
    return nullptr;
  }

  const auto size = (old_size < new_size ? old_size : new_size);
  std::memcpy(new_p, p, size);

  return new_p;
}


//-----------------------------------------------------------------------------
// Allocation : Objects
//-----------------------------------------------------------------------------

template<typename T>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::allocate_objects(
  std::size_t n,
  std::size_t align
) noexcept -> T*
{
  return static_cast<T*>(allocate_bytes(sizeof(T) * n, align));
}


template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::deallocate_objects(
  not_null<T*> p,
  std::size_t n,
  std::size_t align
) noexcept -> void
{
  deallocate_bytes(p.get(), sizeof(T) * n, align);
}

template<typename T>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::resize_objects(not_null<T*> p, std::size_t n)
  noexcept -> bool
{
  return m_resource->resize_allocation(p.get(), sizeof(T) * n);
}

template<typename T>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::reallocate_objects(
  not_null<T*> p,
  std::size_t old_size,
  std::size_t new_size,
  std::size_t align
) noexcept -> T*
{
  static_assert(
    std::is_trivially_copyable_v<T>,
    "T must be trivially copyable to be reallocated!"
  );
  auto* const result = reallocate_bytes(
    p.get(),
    sizeof(T) * old_size,
    sizeof(T) * new_size,
    align
  );

  return static_cast<T*>(result);
}

//-----------------------------------------------------------------------------
// Make / Dispose
//-----------------------------------------------------------------------------

template<typename T, typename... Args, typename>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::make(Args&&... args)
  noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T*
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);

  return aligned_make<T>(
    alignof(T),
    std::forward<Args>(args)...
  );
}

template<typename T, typename>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::make_array(std::size_t n)
  noexcept(std::is_nothrow_constructible<T>::value) -> T*
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  return aligned_make_array<T>(n, alignof(T));
}

template<typename T, typename U, typename>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::make_array(std::size_t n, const U& u)
  noexcept(std::is_nothrow_constructible<T, const U&>::value) -> T*
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  return aligned_make_array<T>(n, alignof(T), u);
}


template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::dispose(not_null<T*> p)
  noexcept -> void
{
  aligned_dispose<T>(p, std::size_t{alignof(T)});
}


template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::dispose_array(not_null<T*> p, std::size_t n)
  noexcept -> void
{
  aligned_dispose_array<T>(p, n, alignof(T));
}

//-----------------------------------------------------------------------------
// Aligned Make / Dispose
//-----------------------------------------------------------------------------

template<typename T, typename... Args, typename>
inline
auto alloy::core::allocator::aligned_make(
  std::size_t align,
  Args&&... args
) noexcept(std::is_nothrow_constructible<T, Args...>::value) -> T*
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);
  static_assert(!std::is_reference<T>::value);

  ALLOY_ASSERT(
    align >= std::size_t{alignof(T)},
    "Alignment must be greater than the required alignment of T"
  );
  auto* const p = static_cast<T*>(allocate_bytes(sizeof(T), align));

  if (ALLOY_UNLIKELY(p == nullptr)) {
    return p;
  }

#ifdef ALLOY_CORE_EXCEPTIONS_ENABLED
  if constexpr(std::is_nothrow_constructible<T,Args...>::value) {
    return lifetime_utilities::make_at<T>(p, std::forward<Args>(args)...).get();
  } else {
    try {
      return lifetime_utilities::make_at<T>(p, std::forward<Args>(args)...).get();
    } catch (...) {
      deallocate_bytes(p, sizeof(T), align);
      throw;
    }
  }
#else
  return lifetime_utilities::make_at<T>(p, std::forward<Args>(args)...).get();
#endif
}

template<typename T, typename>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::aligned_make_array(
  std::size_t n,
  std::size_t align
) noexcept(std::is_nothrow_constructible<T>::value) -> T*
{
  return aligned_make_array_impl<T>(n, align);
}

template<typename T, typename U, typename>
ALLOY_FORCE_INLINE
auto alloy::core::allocator::aligned_make_array(
  std::size_t n,
  std::size_t align,
  const U& u
) noexcept(std::is_nothrow_constructible<T, const U&>::value) -> T*
{
  return aligned_make_array_impl<T>(n, align, u);
}


template <typename T>
inline
auto alloy::core::allocator::aligned_dispose(
  not_null<T*> p,
  std::size_t align
) noexcept -> void
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);

  ALLOY_ASSERT(
    align >= std::size_t{alignof(T)},
    "Alignment must be at least the required alignment of T"
  );

  lifetime_utilities::destroy(p);
  deallocate_objects(p, align);
}


template <typename T>
inline
auto alloy::core::allocator::aligned_dispose_array(
  not_null<T*> p,
  std::size_t n,
  std::size_t align
) noexcept -> void
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);

  ALLOY_ASSERT(
    align >= alignof(T),
    "Alignment must be at least the required alignment of T"
  );
  ALLOY_ASSERT(
    n != 0,
    "Array must be non-zero size"
  );

  lifetime_utilities::destroy_array(p, n);
  deallocate_objects(p, n, align);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline
auto alloy::core::allocator::resource()
  const noexcept -> not_null<memory_resource*>
{
  return m_resource;
}

//-----------------------------------------------------------------------------
// Private: Make
//-----------------------------------------------------------------------------

template<typename T, typename...Args>
inline
auto alloy::core::allocator::aligned_make_array_impl(
  std::size_t n,
  std::size_t align,
  const Args&...args
) noexcept(std::is_nothrow_constructible<T, const Args&...>::value) -> T*
{
  static_assert(!std::is_abstract<T>::value);
  static_assert(!std::is_void<T>::value);
  static_assert(!std::is_const<T>::value);
  static_assert(!std::is_volatile<T>::value);
  static_assert(!std::is_array<T>::value);

  ALLOY_ASSERT(
    align >= std::size_t{alignof(T)},
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

#ifdef ALLOY_CORE_EXCEPTIONS_ENABLED
  if constexpr(std::is_nothrow_constructible<T>::value) {
    return lifetime_utilities::make_array_at<T>(assume_not_null(p), args...).get();
  } else {
    try {
      return lifetime_utilities::make_array_at<T>(assume_not_null(p), args...).get();
    } catch (...) {
      deallocate_bytes(p, bytes, align);
      throw;
    }
  }
#else
  return lifetime_utilities::make_array_at<T>(p, args...).get();
#endif
}

//-----------------------------------------------------------------------------
// Private Members
//-----------------------------------------------------------------------------

inline
auto alloy::core::allocator::get_default_resource()
  noexcept -> default_resource&
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

ALLOY_FORCE_INLINE
auto alloy::core::allocator::default_resource::do_allocate(
  std::size_t bytes,
  std::size_t align
) noexcept -> void*
{
  return ::operator new(bytes, static_cast<std::align_val_t>(align));
}

ALLOY_FORCE_INLINE
auto alloy::core::allocator::default_resource::do_deallocate(
  void* p,
  std::size_t bytes,
  std::size_t align
) noexcept -> void
{
  compiler::unused(bytes, align);
  ::operator delete(p, static_cast<std::align_val_t>(align));
}

//=============================================================================
// inline definitions : class : stl_allocator_adapter<T>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename T>
ALLOY_FORCE_INLINE
alloy::core::stl_allocator_adapter<T>::stl_allocator_adapter(allocator alloc)
  noexcept
  : m_allocator{alloc}
{

}


template <typename T>
template <typename U>
ALLOY_FORCE_INLINE
alloy::core::stl_allocator_adapter<T>::stl_allocator_adapter(
  const stl_allocator_adapter<U>& other
) noexcept
  : m_allocator{other.m_allocator}
{

}

//-----------------------------------------------------------------------------
// Allocation
//-----------------------------------------------------------------------------

template<typename T>
inline
auto alloy::core::stl_allocator_adapter<T>::allocate(std::size_t n)
  noexcept -> T*
{
  // clang-tidy thinks 'sizeof(T)' is an incorrect use of 'sizeof(T*)', which is
  // incorrect since the `T*` template matching will make `T` be a non-pointer
  // type. This diagnostic  is wrong.
  auto* const p = m_allocator.allocate_bytes(
    sizeof(T) * n, // NOLINT(bugprone-sizeof-expression)
    alignof(T)
  );

  ALLOY_ASSERT(p != nullptr);

  return static_cast<T*>(p);
}


template <typename T>
ALLOY_FORCE_INLINE
auto alloy::core::stl_allocator_adapter<T>::deallocate(T* p, std::size_t n)
  noexcept -> void
{
  // clang-tidy thinks 'sizeof(T)' is an incorrect us of 'sizeof(T*)', which is
  // incorrect since the `T*` template matching will make `T` be a non-pointer
  // type. This diagnostic  is wrong.
  //
  // NOLINTNEXTLINE(bugprone-sizeof-expression)
  return m_allocator.deallocate_bytes(p, sizeof(T) * n);
}

template<typename T>
ALLOY_FORCE_INLINE
auto alloy::core::stl_allocator_adapter<T>::underlying()
  const noexcept -> const allocator&
{
  return m_allocator;
}

//=============================================================================
// non-member functions : class : stl_allocator_adapter
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

template<typename T, typename U>
inline
auto alloy::core::operator==(
  const stl_allocator_adapter<T>& lhs,
  const stl_allocator_adapter<U>& rhs
) noexcept -> bool
{
  return lhs.underlying().resource() == rhs.underlying().resource();
}

template<typename T, typename U>
inline
auto alloy::core::operator!=(
  const stl_allocator_adapter<T>& lhs,
  const stl_allocator_adapter<U>& rhs
) noexcept -> bool
{
  return lhs.underlying().resource() != rhs.underlying().resource();
}

#endif /* ALLOY_CORE_MEMORY_ALLOCATOR_HPP */
