/*****************************************************************************
 * \file mutable_buffer.hpp
 *
 * \todo
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
#ifndef ALLOY_IO_BUFFERS_MUTABLE_BUFFER_HPP
#define ALLOY_IO_BUFFERS_MUTABLE_BUFFER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef> // std::byte, std::size_t
#include <memory>  // std::addressof

namespace alloy::io {

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
    static mutable_buffer from_container(Container& container) noexcept;

    /// \brief Creates a mutable buffer from a raw array
    ///
    /// \param array the underlying array
    /// \return a mutable buffer of the array
    template <typename T, std::size_t N>
    static mutable_buffer from_array(T(&array)[N]) noexcept;

    /// \brief Creates a mutable buffer from a pointer and size
    ///
    /// \param data a pointer to the underlying data
    /// \param size the size of the array
    /// \return a mutable buffer of the contiguous sequence
    template <typename T>
    static mutable_buffer from_data(T* data, size_type size) noexcept;

    /// \brief Creates a mutable buffer by reading its object representation
    ///
    /// \param object the object to view
    /// \return a mutable buffer of the object
    template <typename T>
    static mutable_buffer from_object(T& object) noexcept;

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
    /// \return reference to \c (*this)
    mutable_buffer& operator=(const mutable_buffer&) = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets a pointer to the mutable data buffer
    ///
    /// \return a pointer to the underlying data
    pointer data() const noexcept;

    /// \brief Gets the size of this buffer
    ///
    /// \return the size of this buffer
    size_type size() const noexcept;

    /// \brief Gets whether this buffer is empty
    ///
    /// \return \c true if empty
    bool empty() const noexcept;

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

  bool operator==(const mutable_buffer& lhs,
                  const mutable_buffer& rhs) noexcept;
  bool operator!=(const mutable_buffer& lhs,
                  const mutable_buffer& rhs) noexcept;

} // namespace alloy::io

//=============================================================================
// inline definitions : class : mutable_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Static Functions
//-----------------------------------------------------------------------------

template <typename Container>
inline alloy::io::mutable_buffer
  alloy::io::mutable_buffer::from_container(Container& container)
  noexcept
{
  return from_data(container.data(), container.size());
}


template <typename T, std::size_t N>
inline alloy::io::mutable_buffer
  alloy::io::mutable_buffer::from_array(T(&array)[N])
  noexcept
{
  return from_data(static_cast<T*>(array), N);
}


template <typename T>
inline alloy::io::mutable_buffer
  alloy::io::mutable_buffer::from_data(T* p, size_type size)
  noexcept
{
  return mutable_buffer{
    reinterpret_cast<std::byte*>(p),
    p != nullptr ? sizeof(T) * size : 0u
  };
}


template <typename T>
inline alloy::io::mutable_buffer
  alloy::io::mutable_buffer::from_object(T& object)
  noexcept
{
  return from_data(
    std::addressof(object),
    1u
  );
}


//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

inline alloy::io::mutable_buffer::mutable_buffer()
  noexcept
  : m_bytes{nullptr},
    m_size{0u}
{

}


inline alloy::io::mutable_buffer::mutable_buffer(pointer p, size_type size)
  noexcept
  : m_bytes{p},
    m_size{size}
{

}


//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline alloy::io::mutable_buffer::pointer alloy::io::mutable_buffer::data()
  const noexcept
{
  return m_bytes;
}


inline alloy::io::mutable_buffer::size_type alloy::io::mutable_buffer::size()
  const noexcept
{
  return m_size;
}


inline bool alloy::io::mutable_buffer::empty()
  const noexcept
{
  return m_bytes == nullptr || m_size == 0u;
}


//=============================================================================
// inline definitions : inon-member functions : class : mutable_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Equality
//-----------------------------------------------------------------------------

inline bool alloy::io::operator==(const mutable_buffer& lhs,
                                  const mutable_buffer& rhs)
  noexcept
{
  return lhs.data() == rhs.data() && lhs.size() == rhs.size();
}


inline bool alloy::io::operator!=(const mutable_buffer& lhs,
                                  const mutable_buffer& rhs)
  noexcept
{
  return !(lhs == rhs);
}

#endif /* ALLOY_IO_BUFFERS_MUTABLE_BUFFER_HPP */