/*****************************************************************************
 * \file file.hpp
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
#ifndef ALLOY_IO_FILESYSTEM_FILE_STREAM_HPP
#define ALLOY_IO_FILESYSTEM_FILE_STREAM_HPP

#include "alloy/io/buffers/const_buffer.hpp"
#include "alloy/io/buffers/mutable_buffer.hpp"
#include "alloy/core/utilities/expected.hpp"

#include <cstddef> // std::size_t

namespace alloy::io {

  //===========================================================================
  // class : file_stream
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An interface for a basic file stream
  ///
  /// File streams are intentionally immovable and uncopyable since their
  /// lifetimes are intended to be bound inside of the file object itself.
  /////////////////////////////////////////////////////////////////////////////
  class file_stream
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using size_type = std::size_t;
    using offset_type = std::size_t;

    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default constructible
    file_stream() noexcept = default;
    file_stream(const file_stream&) = delete;
    file_stream(file_stream&&) = delete;

    //-------------------------------------------------------------------------

    virtual ~file_stream() = default;

    //-------------------------------------------------------------------------

    file_stream& operator=(const file_stream&) = delete;
    file_stream& operator=(file_stream&&) = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the number of bytes in this file stream
    ///
    /// \return the number of bytes in this file stream
    virtual core::expected<size_type>
      bytes() const noexcept = 0;

    //-------------------------------------------------------------------------
    // File Access
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes the file stream for future access
    virtual void close() noexcept = 0;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    virtual core::expected<void> reset() noexcept = 0;

    /// \brief Flushes the contents of the stream to storage
    ///
    /// \return void on success
    virtual core::expected<void> flush() noexcept;

    /// \brief Skips up to the next N bytes of the file
    ///
    /// \param offset the number of bytes to skip
    /// \return void on success
    virtual core::expected<void>
      skip(offset_type offset) noexcept = 0;

    /// \brief Reads data into the specified \p buffer, returning a buffer the
    ///        buffer that was read
    ///
    /// \param buffer the buffer to read into
    /// \return the buffer that was read on success
    virtual core::expected<mutable_buffer>
      read(mutable_buffer buffer) noexcept = 0;

    /// \brief Writes data from the specified \p buffer, returning a buffer of
    ///        the bytes that were written
    ///
    /// \param buffer the buffer to write from
    /// \return the buffer that was written on success
    virtual core::expected<const_buffer>
      write(const_buffer buffer) noexcept = 0;
  };

} // namespace alloy::io

//=============================================================================
// inline definitions : class : file_stream
//=============================================================================

//-----------------------------------------------------------------------------
// File Access
//-----------------------------------------------------------------------------

inline alloy::core::expected<void> alloy::io::file_stream::flush()
  noexcept
{
  return {};
}

#endif /* ALLOY_IO_FILESYSTEM_FILE_STREAM_HPP */
