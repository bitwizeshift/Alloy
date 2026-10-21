// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file mutable_buffer.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_BUFFERS_MUTABLE_BUFFER_HPP
#define ALLOY_PLATFORM_BUFFERS_MUTABLE_BUFFER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstddef> // std::byte, std::size_t
#include <memory>  // std::addressof

namespace alloy::platform {

  //===========================================================================
  // class : mutable_buffer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A buffer that points to a mutable byte sequence
  /////////////////////////////////////////////////////////////////////////////
  class mutable_buffer
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using pointer = std::byte*;
    using size_type = std::size_t;

    //-------------------------------------------------------------------------
    // Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Creates a mutable buffer from a contiguous sequence container
    ///
    /// \param container the container to create a buffer from
    /// \return a mutable buffer of the sequence container
    template <typename Container>
    static auto from_container(Container& container) noexcept -> mutable_buffer;

    /// \brief Creates a mutable buffer from a raw array
    ///
    /// \param array the underlying array
    /// \return a mutable buffer of the array
    template <typename T, std::size_t N>
    static auto from_array(T(&array)[N]) noexcept -> mutable_buffer;

    /// \brief Creates a mutable buffer from a pointer and size
    ///
    /// \param data a pointer to the underlying data
    /// \param size the size of the array
    /// \return a mutable buffer of the contiguous sequence
    template <typename T>
    static auto from_data(T* data, size_type size) noexcept -> mutable_buffer;

    /// \brief Creates a mutable buffer by reading its object representation
    ///
    /// \param object the object to view
    /// \return a mutable buffer of the object
    template <typename T>
    static auto from_object(T& object) noexcept -> mutable_buffer;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a buffer of 0 size and null data
    mutable_buffer() noexcept;

    /// \brief Constructs a buffer of \p size pointing to the specified \p
    ///        data
    ///
    /// \param data the underlying data pointer, in bytes
    /// \param size the size of the buffer
    mutable_buffer(pointer data, size_type size) noexcept;

    /// \brief Copies this buffer
    ///
    /// \param other the other buffer to copy
    mutable_buffer(const mutable_buffer& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copies this buffer
    ///
    /// \param other the other buffer to copy
    /// \return reference to `(*this)`
    auto operator=(const mutable_buffer&) -> mutable_buffer& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets a pointer to the mutable data buffer
    ///
    /// \return a pointer to the underlying data
    auto data() const noexcept -> pointer;

    /// \brief Gets the size of this buffer
    ///
    /// \return the size of this buffer
    auto size() const noexcept -> size_type;

    /// \brief Gets whether this buffer is empty
    ///
    /// \return `true` if empty
    auto empty() const noexcept -> bool;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    pointer m_bytes;
    size_type m_size;
  };

  //===========================================================================
  // non-member functions : class : mutable_buffer
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  auto operator==(const mutable_buffer& lhs, const mutable_buffer& rhs)
    noexcept -> bool;
  auto operator!=(const mutable_buffer& lhs, const mutable_buffer& rhs)
    noexcept -> bool;

} // namespace alloy::platform

//=============================================================================
// inline definitions : class : mutable_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Static Functions
//-----------------------------------------------------------------------------

template <typename Container>
inline
auto alloy::platform::mutable_buffer::from_container(Container& container)
  noexcept -> mutable_buffer
{
  return from_data(container.data(), container.size());
}


template <typename T, std::size_t N>
inline
auto alloy::platform::mutable_buffer::from_array(T(&array)[N])
  noexcept -> mutable_buffer
{
  return from_data(static_cast<T*>(array), N);
}


template <typename T>
inline
auto alloy::platform::mutable_buffer::from_data(T* p, size_type size)
  noexcept -> mutable_buffer
{
  return mutable_buffer{
    reinterpret_cast<std::byte*>(p),
    p != nullptr ? sizeof(T) * size : 0u
  };
}


template <typename T>
inline
auto alloy::platform::mutable_buffer::from_object(T& object)
  noexcept -> mutable_buffer
{
  return from_data(
    std::addressof(object),
    1u
  );
}


//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline
alloy::platform::mutable_buffer::mutable_buffer() noexcept
  : m_bytes{nullptr},
    m_size{0u}
{

}


inline
alloy::platform::mutable_buffer::mutable_buffer(pointer p, size_type size) noexcept
  : m_bytes{p},
    m_size{size}
{

}


//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline
auto alloy::platform::mutable_buffer::data() const noexcept -> pointer
{
  return m_bytes;
}


inline
auto alloy::platform::mutable_buffer::size() const noexcept -> size_type
{
  return m_size;
}


inline
auto alloy::platform::mutable_buffer::empty() const noexcept -> bool
{
  return m_bytes == nullptr || m_size == 0u;
}


//=============================================================================
// inline definitions : inon-member functions : class : mutable_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

inline
auto alloy::platform::operator==(const mutable_buffer& lhs, const mutable_buffer& rhs)
  noexcept -> bool
{
  return lhs.data() == rhs.data() && lhs.size() == rhs.size();
}


inline
auto alloy::platform::operator!=(const mutable_buffer& lhs, const mutable_buffer& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

#endif /* ALLOY_PLATFORM_BUFFERS_MUTABLE_BUFFER_HPP */

