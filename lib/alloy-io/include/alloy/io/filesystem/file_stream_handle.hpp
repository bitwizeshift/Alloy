/*****************************************************************************
 * \file file_stream_handle.hpp
 *
 * \brief
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
#ifndef ALLOY_IO_FILESYSTEM_FILE_STREAM_HANDLE_HPP
#define ALLOY_IO_FILESYSTEM_FILE_STREAM_HANDLE_HPP

#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/utilities/not_null.hpp"

namespace alloy::io {

  //===========================================================================
  // class : file_stream_handle
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A pointer which holds onto a file_stream, and finalizes it using
  ///        RAII on destruction
  /////////////////////////////////////////////////////////////////////////////
  class file_stream_handle
  {
    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructible
    file_stream_handle() noexcept;
    file_stream_handle(core::not_null<file_stream*> stream,
                       core::not_null<file_stream_finalizer*> finalizer) noexcept;
    file_stream_handle(file_stream_handle&& other) noexcept;
    file_stream_handle(const file_stream_handle&) = delete;

    //-------------------------------------------------------------------------

    ~file_stream_handle();

    //-------------------------------------------------------------------------

    file_stream_handle& operator=(file_stream_handle&& other) noexcept;
    file_stream_handle& operator=(const file_stream_handle&) = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    explicit operator bool() const noexcept;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    void reset() noexcept;

    //-------------------------------------------------------------------------
    // Element Access
    //-------------------------------------------------------------------------
  public:

    file_stream* get() const noexcept;

    file_stream* operator->() const noexcept;
    file_stream& operator*() const noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  public:

    file_stream* m_stream;
    file_stream_finalizer* m_finalizer;
  };

} // namespace alloy::io

//=============================================================================
// inline definition : class : file_stream_handle
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

inline alloy::io::file_stream_handle::file_stream_handle()
  noexcept
  : m_stream{nullptr},
    m_finalizer{nullptr}
{

}

inline alloy::io::file_stream_handle
  ::file_stream_handle(core::not_null<file_stream*> stream,
                       core::not_null<file_stream_finalizer*> finalizer)
  noexcept
  : m_stream{stream.get()},
    m_finalizer{finalizer.get()}
{

}

inline alloy::io::file_stream_handle::file_stream_handle(file_stream_handle&& other)
  noexcept
  : m_stream{other.m_stream},
    m_finalizer{other.m_finalizer}
{
  other.m_stream = nullptr;
}

//-----------------------------------------------------------------------------

inline alloy::io::file_stream_handle::~file_stream_handle()
{
  reset();
}

//-----------------------------------------------------------------------------

inline alloy::io::file_stream_handle&
  alloy::io::file_stream_handle::operator=(file_stream_handle&& other)
  noexcept
{
  reset();

  m_stream = other.m_stream;
  m_finalizer = other.m_finalizer;
  other.m_stream = nullptr;

  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

inline alloy::io::file_stream_handle::operator bool()
  const noexcept
{
  return m_stream != nullptr;
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

inline void alloy::io::file_stream_handle::reset()
  noexcept
{
  if (m_stream != nullptr) {
    m_finalizer->finalize(m_stream);
    m_stream = nullptr;
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

inline alloy::io::file_stream* alloy::io::file_stream_handle::get()
  const noexcept
{
  return m_stream;
}

inline alloy::io::file_stream* alloy::io::file_stream_handle::operator->()
  const noexcept
{
  return m_stream;
}

inline alloy::io::file_stream& alloy::io::file_stream_handle::operator*()
  const noexcept
{
  return (*m_stream);
}

#endif /* ALLOY_IO_FILESYSTEM_FILE_STREAM_HANDLE_HPP */