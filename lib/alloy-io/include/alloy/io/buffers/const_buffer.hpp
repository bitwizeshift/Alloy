// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file const_buffer.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_IO_BUFFERS_CONST_BUFFER_HPP
#define ALLOY_IO_BUFFERS_CONST_BUFFER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <cstddef> // std::byte, std::size_t
#include <memory>  // std::addressof

namespace alloy::io {

  //===========================================================================
  // class : const_buffer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A buffer that points to a const byte sequence
  /////////////////////////////////////////////////////////////////////////////
  class const_buffer
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using pointer   = const std::byte*;
    using size_type = std::size_t;

    //-------------------------------------------------------------------------
    // Factories
    //-------------------------------------------------------------------------
  public:

    /// \brief Creates a const buffer from a contiguous sequence container
    ///
    /// \param container the container to create a buffer from
    /// \return a const buffer of the sequence container
    template <typename Container>
    static auto from_container(const Container& container)
      noexcept -> const_buffer;

    /// \brief Creates a const buffer from a raw array
    ///
    /// \param array the underlying array
    /// \return a const buffer of the array
    template <typename T, std::size_t N>
    static auto from_array(const T(&array)[N]) noexcept -> const_buffer;

    /// \brief Creates a const buffer from a pointer and size
    ///
    /// \param data a pointer to the underlying data
    /// \param size the size of the array
    /// \return a const buffer of the contiguous sequence
    template <typename T>
    static auto from_data(const T* data, size_type size)
      noexcept -> const_buffer;

    /// \brief Creates a const buffer by reading its object representation
    ///
    /// \param object the object to view
    /// \return a const buffer of the object
    template <typename T>
    static auto from_object(const T& object) noexcept -> const_buffer;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a buffer of 0 size and null data
    const_buffer() noexcept;

    /// \brief Constructs a buffer of \p size pointing to the specified \p
    ///        data
    ///
    /// \param data the underlying data pointer, in bytes
    /// \param size the size of the buffer
    const_buffer(pointer data, size_type size) noexcept;

    /// \brief Copies this buffer
    ///
    /// \param other the other buffer to copy
    const_buffer(const const_buffer& other) noexcept = default;

    //-------------------------------------------------------------------------

    /// \brief Copies this buffer
    ///
    /// \param other the other buffer to copy
    /// \return reference to `(*this)`
    auto operator=(const const_buffer&) -> const_buffer& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets a pointer to the const data buffer
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
  // non-member functions : class : const_buffer
  //===========================================================================

  //---------------------------------------------------------------------------
  // Equality
  //---------------------------------------------------------------------------

  auto operator==(const const_buffer& lhs, const const_buffer& rhs)
    noexcept -> bool;
  auto operator!=(const const_buffer& lhs, const const_buffer& rhs)
    noexcept -> bool;

} // namespace alloy::io

//=============================================================================
// inline definitions : class : const_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Static Functions
//-----------------------------------------------------------------------------

template <typename Container>
inline
auto alloy::io::const_buffer::from_container(const Container& container)
  noexcept -> const_buffer
{
  return from_data(container.data(), container.size());
}


template <typename T, std::size_t N>
inline
auto alloy::io::const_buffer::from_array(const T(&array)[N])
  noexcept -> const_buffer
{
  return from_data(static_cast<const T*>(array), N);
}


template <typename T>
inline
auto alloy::io::const_buffer::from_data(const T* p, size_type size)
  noexcept -> const_buffer
{
  return const_buffer{
    reinterpret_cast<const std::byte*>(p),
    p != nullptr ? sizeof(T) * size : 0u
  };
}


template <typename T>
inline
auto alloy::io::const_buffer::from_object(const T& object)
  noexcept -> const_buffer
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
alloy::io::const_buffer::const_buffer() noexcept
  : m_bytes{nullptr},
    m_size{0u}
{

}


inline
alloy::io::const_buffer::const_buffer(pointer p, size_type size) noexcept
  : m_bytes{p},
    m_size{size}
{

}


//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline
auto alloy::io::const_buffer::data() const noexcept -> pointer
{
  return m_bytes;
}


inline
auto alloy::io::const_buffer::size() const noexcept -> size_type
{
  return m_size;
}


inline
auto alloy::io::const_buffer::empty() const noexcept -> bool
{
  return m_bytes == nullptr || m_size == 0u;
}


//=============================================================================
// inline definitions : inon-member functions : class : const_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

inline
auto alloy::io::operator==(const const_buffer& lhs, const const_buffer& rhs)
  noexcept -> bool
{
  return lhs.data() == rhs.data() && lhs.size() == rhs.size();
}


inline
auto alloy::io::operator!=(const const_buffer& lhs, const const_buffer& rhs)
  noexcept -> bool
{
  return !(lhs == rhs);
}

#endif /* ALLOY_IO_BUFFERS_CONST_BUFFER_HPP */

