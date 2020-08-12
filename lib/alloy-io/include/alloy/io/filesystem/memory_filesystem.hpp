/*****************************************************************************
 * \file memory_filesystem.hpp
 *
 * \brief This header contains the definition of a filesystem that reads and
 *        writes to memory, rather than to an actual file.
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
#ifndef ALLOY_IO_FILESYSTEM_MEMORY_FILESYSTEM_HPP
#define ALLOY_IO_FILESYSTEM_MEMORY_FILESYSTEM_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/io/api.hpp"
#include "alloy/io/buffers/mutable_buffer.hpp"
#include "alloy/io/filesystem/file.hpp"
#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/memory/allocator.hpp"

namespace alloy::io {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A filesystem that reads and writes to memory.
  ///
  /// Files read and write from mutable buffers that are specified from
  /// outside the system.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API memory_filesystem final
    : private file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a memory filesystem using a default allocator
    memory_filesystem() noexcept;

    /// \brief Constructs a memory filesystem that uses \p alloc to allocate
    ///        spaces for the files
    ///
    /// \param alloc the allocator
    explicit memory_filesystem(core::allocator alloc) noexcept;
    memory_filesystem(memory_filesystem&&) = delete;
    memory_filesystem(const memory_filesystem&) = delete;

    //-------------------------------------------------------------------------

    memory_filesystem& operator=(memory_filesystem&&) = delete;
    memory_filesystem& operator=(const memory_filesystem&) = delete;

    //-------------------------------------------------------------------------
    // Filesystem
    //-------------------------------------------------------------------------
  public:

    /// \brief Opens a file that reads and writes from \p buffer
    ///
    /// \return a file that reads and writes from \p buffer
    file open(mutable_buffer buffer) noexcept;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Finalization
    //-------------------------------------------------------------------------
  public:

    /// \brief Finalizes the file stream
    ///
    /// \param stream the stream to finalize
    void finalize(file_stream* stream) noexcept override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  public:

    core::allocator m_allocator;
  };

} // namespace alloy::io

#endif /* ALLOY_IO_FILESYSTEM_MEMORY_FILESYSTEM_HPP */
