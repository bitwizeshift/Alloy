/*****************************************************************************
 * \file crc16_file_adapter.hpp
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
#ifndef ALLOY_IO_FILESYSTEM_CRC16_FILE_ADAPTER_HPP
#define ALLOY_IO_FILESYSTEM_CRC16_FILE_ADAPTER_HPP

#include "alloy/io/filesystem/file_adapter.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"
#include "alloy/io/filesystem/file.hpp"
#include "alloy/core/memory/allocator.hpp"

#include <system_error> // std::is_error_code_enum, std::error_code
#include <type_traits>  // std::true_type

namespace alloy::io {

  //===========================================================================
  // class : crc16_file_adapter
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Adapts a file to contain a CRC16 checksum
  /////////////////////////////////////////////////////////////////////////////
  class crc16_file_adapter final
    : public file_adapter,
      private file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Error codes for the crc16 checksum files
    ///////////////////////////////////////////////////////////////////////////
    enum class error_code
    {
      bad_checksum = 1, ///< The checksum did not match expected value
      unable_to_compute_checksum = 2, ///< Unable to perform computation
      unable_to_read_checksum = 3,
      unable_to_write_checksum = 4,
    };

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a file adapter that uses default values for the
    ///        XOR pass
    ///
    /// This uses the default allocator
    crc16_file_adapter() noexcept;

    /// \brief Constructs a file adapter that uses default values for the
    ///        XOR pass, and is allocated using \p alloc
    ///
    /// \param alloc the allocator to use for creating the file adapters
    explicit crc16_file_adapter(core::allocator alloc) noexcept;

    crc16_file_adapter(const crc16_file_adapter&) = delete;
    crc16_file_adapter(crc16_file_adapter&&) = delete;

    //-------------------------------------------------------------------------

    crc16_file_adapter& operator=(const crc16_file_adapter&) = delete;
    crc16_file_adapter& operator=(crc16_file_adapter&&) = delete;

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
  };

  //===========================================================================
  // non-member functions : class : crc16_file_adapter
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Creates a std::error_code from the crc16 error code
  ///
  /// \param ec the error code to make into a std::error_code
  /// \return a std::error_code
  std::error_code make_error_code(crc16_file_adapter::error_code ec) noexcept;

} // namespace alloy::io

namespace std {
  template <>
  struct is_error_code_enum<alloy::io::crc16_file_adapter::error_code>
    : std::true_type{};
} // namespace std

#endif /* ALLOY_IO_FILESYSTEM_CRC16_FILE_ADAPTER_HPP */