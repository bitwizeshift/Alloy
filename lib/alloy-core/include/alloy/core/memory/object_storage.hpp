/*****************************************************************************
 * \file object_storage.hpp
 *
 * \brief This header defines storage for objects that have not yet been
 *        allocated
 *****************************************************************************/

/*
 The MIT License (MIT)

 Copyright (c) 2022 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_CORE_MEMORY_OBJECT_STORAGE_HPP
#define ALLOY_CORE_MEMORY_OBJECT_STORAGE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include "alloy/core/memory/data_quantity.hpp"
#include "alloy/core/intrinsics.hpp"

#include <array>            // std::array
#include <cstddef>          // std::byte
#include <initializer_list> // std::initialize_list
#include <new>              // std::launder, placement-new
#include <utility>          // std::forward

namespace alloy::core {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Storage space capable of holding any object under a given size and
  ///        alignment requirement.
  ///
  /// \note
  /// This class is not copyable or moveable to prevent breaking non-trivially
  /// copyable types.
  ///
  /// \warning This class can be very dangerous if misused.
  ///
  /// \tparam Size the size of the storage
  /// \tparam Align the alignment of the storage
  //////////////////////////////////////////////////////////////////////////////
  template <std::size_t Size, std::size_t Align = alignof(std::max_align_t)>
  class alignas(Align) any_object_storage
  {
    static_assert((Align & (Align - 1)) == 0u, "Align must be a power of two");

    //--------------------------------------------------------------------------
    // Static Constants
    //--------------------------------------------------------------------------
  public:

    inline static constexpr auto size = bytes{Size};
    inline static constexpr auto align = bytes{Align};

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    any_object_storage() = default;
    any_object_storage(const any_object_storage&) = delete;
    any_object_storage(any_object_storage&&) = delete;

    auto operator=(const any_object_storage&) -> any_object_storage& = delete;
    auto operator=(any_object_storage&&) -> any_object_storage& = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Engages an object in this storage
    ///
    /// \param args the arguments to forward to `T`'s constructor
    /// \return a reference to the constructed `T`
    template <typename T, typename...Args,
              typename = std::enable_if_t<std::is_constructible_v<T,Args...>>>
    auto emplace(Args&&...args)
      noexcept(std::is_nothrow_constructible_v<T,Args...>) -> T&;

    /// \brief Engages an object in this storage
    ///
    /// \param ilist an initializer list of arguments
    /// \param args the arguments to forward to `T`'s constructor
    /// \return a reference to the constructed `T`
    template <typename T, typename U, typename...Args,
              typename = std::enable_if_t<std::is_constructible_v<T,std::initializer_list<U>,Args...>>>
    auto emplace(std::initializer_list<U> ilist, Args&&...args)
      noexcept(std::is_nothrow_constructible_v<T,std::initializer_list<U>,Args...>) -> T&;

    /// \brief Destroys the object at this storage
    template <typename T>
    auto destroy() noexcept -> void;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets a reference to the underlying storage
    ///
    /// \return a reference to the storage
    template <typename T>
    auto get() noexcept -> T&;
    template <typename T>
    auto get() const noexcept -> const T&;
    /// \}

    /// \{
    /// \brief Gets a pointer to the raw data
    ///
    /// \return a pointer to the raw data
    [[nodiscard]]
    constexpr auto data() noexcept -> std::byte*;
    [[nodiscard]]
    constexpr auto data() const noexcept -> const std::byte*;
    /// \}

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    alignas(Align) std::byte m_storage[Size];
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Object storage for a single `T` type
  ///
  /// This is simply an aligned piece of storage suitably sized for a single `T`
  /// entry. Whether an object lives here or not is up to the user to check.
  ///
  /// This class mostly exists as a convenience for sequences of unconstructed
  /// values, to make reserving large blocks of unconstructed data easier than
  /// requiring manual calculations.
  ///
  /// \warning This class can be very dangerous if misused.
  //////////////////////////////////////////////////////////////////////////////
  template <typename T>
  class alignas(T) object_storage
  {
    static_assert(!std::is_const_v<T>);
    static_assert(!std::is_void_v<T>);
    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_array_v<T>, "Use object_storage_array instead");

    //--------------------------------------------------------------------------
    // Static Constants
    //--------------------------------------------------------------------------
  public:

    static inline constexpr auto size = bytes{sizeof(T)};
    static inline constexpr auto align = bytes{alignof(T)};

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    object_storage() = default;
    object_storage(const object_storage&) = delete;
    object_storage(object_storage&&) = delete;

    auto operator=(const object_storage&) -> object_storage& = delete;
    auto operator=(object_storage&&) -> object_storage& = delete;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Engages an object in this storage
    ///
    /// \param args the arguments to forward to `T`'s constructor
    /// \return a reference to the constructed `T`
    template <typename...Args,
              typename = std::enable_if_t<std::is_constructible_v<T,Args...>>>
    auto emplace(Args&&...args)
      noexcept(std::is_nothrow_constructible_v<T,Args...>) -> T&;

    /// \brief Engages an object in this storage
    ///
    /// \param ilist an initializer list of arguments
    /// \param args the arguments to forward to `T`'s constructor
    /// \return a reference to the constructed `T`
    template <typename U, typename...Args,
              typename = std::enable_if_t<std::is_constructible_v<T,std::initializer_list<U>,Args...>>>
    auto emplace(std::initializer_list<U> ilist, Args&&...args)
      noexcept(std::is_nothrow_constructible_v<T,std::initializer_list<U>,Args...>) -> T&;

    /// \brief Destroys the object at this storage
    auto destroy() noexcept -> void;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets a reference to the underlying storage
    ///
    /// \return a reference to the storage
    auto get() noexcept -> T&;
    auto get() const noexcept -> const T&;
    /// \}

    /// \{
    /// \brief Gets a pointer to the raw data
    ///
    /// \return a pointer to the raw data
    [[nodiscard]]
    constexpr auto data() noexcept -> std::byte*;
    [[nodiscard]]
    constexpr auto data() const noexcept -> const std::byte*;
    /// \}

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    using storage_type = any_object_storage<sizeof(T), alignof(T)>;

    storage_type m_storage;
  };

  template <typename T, std::size_t N>
  using object_storage_array = std::array<object_storage<T>, N>;

} // namespace alloy::core

//==============================================================================
// any_object_storage
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <std::size_t Size, std::size_t Align>
template <typename T, typename...Args, typename>
inline
auto alloy::core::any_object_storage<Size,Align>::emplace(Args&&...args)
  noexcept(std::is_nothrow_constructible_v<T,Args...>) -> T&
{
  static_assert(sizeof(T) <= Size);
  static_assert(alignof(T) <= Align);

  auto* const where = static_cast<void*>(m_storage);
  auto* const p = new(where) T(std::forward<Args>(args)...);

  return *p;
}

template <std::size_t Size, std::size_t Align>
template <typename T, typename U, typename...Args, typename>
inline
auto alloy::core::any_object_storage<Size,Align>::emplace(
  std::initializer_list<U> ilist,
  Args&&...args
) noexcept(std::is_nothrow_constructible_v<T,std::initializer_list<U>,Args...>) -> T&
{
  return emplace<T>(ilist, std::forward<Args>(args)...);
}

template <std::size_t Size, std::size_t Align>
template <typename T>
inline
auto alloy::core::any_object_storage<Size,Align>::destroy()
  noexcept -> void
{
  auto& instance = get<T>();
  instance.~T();
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <std::size_t Size, std::size_t Align>
template <typename T>
inline
auto alloy::core::any_object_storage<Size,Align>::get()
  noexcept -> T&
{
  return *std::launder(reinterpret_cast<T*>(m_storage));
}

template <std::size_t Size, std::size_t Align>
template <typename T>
inline
auto alloy::core::any_object_storage<Size,Align>::get()
  const noexcept -> const T&
{
  return *std::launder(reinterpret_cast<const T*>(m_storage));
}

template <std::size_t Size, std::size_t Align>
inline constexpr
auto alloy::core::any_object_storage<Size,Align>::data()
  noexcept -> std::byte*
{
  return m_storage;
}

template <std::size_t Size, std::size_t Align>
inline constexpr
auto alloy::core::any_object_storage<Size,Align>::data()
  const noexcept -> const std::byte*
{
  return m_storage;
}

//==============================================================================
// object_storage
//==============================================================================

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

template <typename T>
template <typename...Args, typename>
inline
auto alloy::core::object_storage<T>::emplace(Args&&...args)
  noexcept(std::is_nothrow_constructible_v<T,Args...>) -> T&
{
  return m_storage.template emplace<T>(std::forward<Args>(args)...);
}

template <typename T>
template <typename U, typename...Args, typename>
inline
auto alloy::core::object_storage<T>::emplace(
  std::initializer_list<U> ilist,
  Args&&...args
) noexcept(std::is_nothrow_constructible_v<T,std::initializer_list<U>,Args...>) -> T&
{
  return m_storage.template emplace<T>(ilist, std::forward<Args>(args)...);
}

template <typename T>
inline
auto alloy::core::object_storage<T>::destroy()
  noexcept -> void
{
  get()->~T();
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

template <typename T>
inline
auto alloy::core::object_storage<T>::get()
  noexcept -> T&
{
  return m_storage.template get<T>();
}

template <typename T>
inline
auto alloy::core::object_storage<T>::get()
  const noexcept -> const T&
{
  return m_storage.template get<T>();
}

template <typename T>
inline constexpr
auto alloy::core::object_storage<T>::data()
  noexcept -> std::byte*
{
  return m_storage.data();
}

template <typename T>
inline constexpr
auto alloy::core::object_storage<T>::data()
  const noexcept -> const std::byte*
{
  return m_storage.data();
}

#endif /* ALLOY_CORE_MEMORY_OBJECT_STORAGE_HPP */
