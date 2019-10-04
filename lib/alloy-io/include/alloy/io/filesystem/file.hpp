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
#ifndef ALLOY_IO_FILESYSTEM_FILE_HPP
#define ALLOY_IO_FILESYSTEM_FILE_HPP

#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"
#include "alloy/io/buffers/const_buffer.hpp"
#include "alloy/io/buffers/mutable_buffer.hpp"

#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/expected.hpp"

#include <system_error>
#include <type_traits>

namespace alloy::io {

  //===========================================================================
  // class : file
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A file which may be returned by a filesystem
  ///
  /// Files are owners to any file_streams and use RAII to clean up resources
  /// upon completion.
  /////////////////////////////////////////////////////////////////////////////
  class file
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using size_type = file_stream::size_type;
    using offset_type = file_stream::offset_type;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Error codes that any basic file may return.
    ///
    /// Specific file implementations may expand on this to include more error
    /// codes/reasons
    ///////////////////////////////////////////////////////////////////////////
    enum class error_code
    {
      eof           = 1, ///< End of file reached
      not_readable  = 2, ///< The file is not able to be read
      not_writeable = 3, ///< The file is not able to be written to
      closed        = 4, ///< The file is already closed
    };

    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a closed file object
    file() noexcept;

    /// \brief Constructs a file that owns the specified \p stream that will be
    ///        finalized by \p finalizer
    ///
    /// \param stream the stream to own
    /// \param finalizer the finalizer to finalize the stream
    file(core::not_null<file_stream*> stream,
         core::not_null<file_stream_finalizer*> finalizer) noexcept;

    /// \brief Constructs this file by moving the contents of \p other
    ///
    /// \param other the other file to move
    file(file&& other) noexcept;
    file(const file&) = delete;

    //-------------------------------------------------------------------------

    ~file();

    //-------------------------------------------------------------------------

    /// \brief Assigns the contents of \p other to this
    ///
    /// If this file was open priored to the assignment, it is closed first
    /// before adopting the file from \p other
    ///
    /// \param other the other file to move
    /// \return reference to \c (*this)
    file& operator=(file&& other) noexcept;
    file& operator=(const file&) = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------

    /// \brief Checks whether this file is open
    ///
    /// \return \c true if the file is open
    bool is_open() const noexcept;

    /// \brief Gets the size of this file in bytes
    ///
    /// \return the size of this file in bytes, on success
    core::expected<size_type>
      bytes() const noexcept;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Attempts to close the file
    ///
    /// The only reason this should ever fail is if the file is already closed
    ///
    /// \return void on success
    core::expected<void> close() noexcept;

    /// \brief Skips up to the next N bytes of the file
    ///
    /// \param offset the number of bytes to skip
    /// \return void on success
    core::expected<void> skip(offset_type offset) noexcept;

    /// \brief Reads data into the specified \p buffer, returning a buffer the
    ///        buffer that was read
    ///
    /// \param buffer the buffer to read into
    /// \return the buffer to the data that was read
    core::expected<mutable_buffer> read(mutable_buffer buffer) noexcept;

    /// \brief Writes data from the specified \p buffer, returning a buffer of
    ///        the bytes that were written
    ///
    /// \param buffer the buffer to write from
    /// \return the buffer to the data that was written
    core::expected<const_buffer> write(const_buffer buffer) noexcept;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    file_stream* m_stream;
    file_stream_finalizer* m_finalizer;
  };

  //===========================================================================
  // non-member functions : enum : file::error_code
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Makes an error code from the given file error code
  ///
  /// \param ec the file error code
  /// \return the std::error_code of the file error
  std::error_code make_error_code(file::error_code ec) noexcept;
} // namespace alloy::io

namespace std {
  template <>
  struct is_error_code_enum<alloy::io::file::error_code> : std::true_type{};
} // namespace std

#endif /* ALLOY_IO_FILESYSTEM_FILE_HPP */