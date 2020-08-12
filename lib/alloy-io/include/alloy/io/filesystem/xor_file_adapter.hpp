/*****************************************************************************
 * \file xor_file_adapter.hpp
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
#ifndef ALLOY_IO_FILESYSTEM_XOR_FILE_ADAPTER_HPP
#define ALLOY_IO_FILESYSTEM_XOR_FILE_ADAPTER_HPP

#include "alloy/io/api.hpp"
#include "alloy/io/filesystem/file_adapter.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"
#include "alloy/io/filesystem/file.hpp"
#include "alloy/core/memory/allocator.hpp"

#include <cstddef> // std::byte

namespace alloy::io {

  //===========================================================================
  // class : xor_file_adapter
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An interface for adapting a file into a different type of file.
  ///
  /// This is used to allow composable
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API xor_file_adapter final
    : public file_adapter,
      private file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a file adapter that uses default values for the
    ///        XOR pass
    ///
    /// This uses the default allocator
    xor_file_adapter() noexcept;

    /// \brief Constructs a file adapter that uses default values for the
    ///        XOR pass, and is allocated using \p alloc
    ///
    /// \param alloc the allocator to use for creating the file adapters
    explicit xor_file_adapter(core::allocator alloc) noexcept;

    /// \brief Constructs a file adapter that runs an XOR pass of \p b0
    ///        followed by \p b1 on each byte read (and reverses on write)
    ///
    /// \param b0 the first byte to xor on read (second on write)
    /// \param b1 the second byte to xor on read (first on write)
    xor_file_adapter(std::byte b0,
                     std::byte b1) noexcept;

    /// \brief Constructs a file adapter that runs an XOR pass of \p b0
    ///        followed by \p b1 on each byte read (and reverses on write)
    ///
    /// \param b0 the first byte to xor on read (second on write)
    /// \param b1 the second byte to xor on read (first on write)
    /// \param alloc the allocator to use for constructing adapters
    xor_file_adapter(std::byte b0,
                     std::byte b1,
                     core::allocator alloc) noexcept;

    xor_file_adapter(const xor_file_adapter&) = delete;
    xor_file_adapter(xor_file_adapter&&) = delete;

    //-------------------------------------------------------------------------

    xor_file_adapter& operator=(const xor_file_adapter&) = delete;
    xor_file_adapter& operator=(xor_file_adapter&&) = delete;

    //-------------------------------------------------------------------------
    // Virtual Hooks
    //-------------------------------------------------------------------------
  private:

    file do_adapt(file f) noexcept override;

    void finalize(file_stream* stream) noexcept override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    core::allocator m_allocator;
    std::byte m_byte0;
    std::byte m_byte1;
  };

} // namespace alloy::io

#endif /* ALLOY_IO_FILESYSTEM_XOR_FILE_ADAPTER_HPP */
