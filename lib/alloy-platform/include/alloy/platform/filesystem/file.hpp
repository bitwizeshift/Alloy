// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file file.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_FILE_HPP
#define ALLOY_PLATFORM_FILESYSTEM_FILE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/api.hpp"
#include "alloy/platform/filesystem/file_stream_handle.hpp"
#include "alloy/platform/buffers/const_buffer.hpp"
#include "alloy/platform/buffers/mutable_buffer.hpp"

#include "alloy/core/io/deserializer.hpp"
#include "alloy/core/io/reader.hpp"
#include "alloy/core/io/serializer.hpp"
#include "alloy/core/io/writer.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <system_error>
#include <type_traits>
#include <cstddef> // std::byte, std::size_t
#include <span>    // std::span

namespace alloy::platform {

  //===========================================================================
  // class : file
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A file which may be returned by a filesystem
  ///
  /// Files are owners to any file_streams and use RAII to clean up resources
  /// upon completion.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API file
    : public core::reader,
      public core::writer
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
      system_error  = 5, ///< An internal error prevented the operation from
                         ///< completing
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
    file(
      core::not_null<file_stream*> stream,
      core::not_null<file_stream_finalizer*> finalizer
    ) noexcept;

    /// \brief Constructs a file that owns the specified file_stream_handle
    ///
    /// \param handle the handle to own
    explicit file(file_stream_handle handle) noexcept;

    /// \brief Constructs this file by moving the contents of \p other
    ///
    /// \param other the other file to move
    file(file&& other) noexcept = default;
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
    /// \return reference to `(*this)`
    auto operator=(file&& other) noexcept -> file&;
    auto operator=(const file&) -> file& = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------

    /// \brief Checks whether this file is open
    ///
    /// \return `true` if the file is open
    auto is_open() const noexcept -> bool;

    /// \brief Gets the size of this file in bytes
    ///
    /// \return the size of this file in bytes, on success
    auto bytes() const noexcept -> core::result<size_type,std::error_code>;

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
  public:

    /// \brief Releases the underlying stream handle
    ///
    /// \note The underlying stream will not have resources managed; they need
    ///       to be handled by the underlying manager
    ///
    /// \return the handle
    auto release() noexcept -> file_stream_handle;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Attempts to close the file
    ///
    /// The only reason this should ever fail is if the file is already closed
    ///
    /// \return void on success
    auto close() noexcept -> core::result<void,std::error_code>;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    auto reset() noexcept -> core::result<void,std::error_code>;

    /// \brief Flushes the contents to the file, if buffered
    ///
    /// \return void on success
    auto flush() noexcept -> core::result<void,std::error_code>;

    /// \brief Skips up to the next N bytes of the file
    ///
    /// \param offset the number of bytes to skip
    /// \return void on success
    auto skip(offset_type offset)
      noexcept -> core::result<void,std::error_code>;

    /// \brief Reads data into the specified \p buffer, returning a buffer the
    ///        buffer that was read
    ///
    /// \param buffer the buffer to read into
    /// \return the buffer to the data that was read
    auto read(mutable_buffer buffer)
      noexcept -> core::result<mutable_buffer,std::error_code>;

    /// \brief Writes data from the specified \p buffer, returning a buffer of
    ///        the bytes that were written
    ///
    /// \param buffer the buffer to write from
    /// \return the buffer to the data that was written
    auto write(const_buffer buffer)
      noexcept -> core::result<const_buffer,std::error_code>;

    using core::reader::read;
    using core::writer::write;

    //-------------------------------------------------------------------------

    /// \brief Reads an object from this file
    ///
    /// This uses the core::deserializer<T> customization point to read
    /// arbitrary types.
    ///
    /// \note Reading objects directly from the file is meant to not only
    ///       provide a means of reading stronger data-types, but also provide
    ///       a portable API for reading from files on systems that might
    ///       otherwise have different endiannesses. Using 'read_object'
    ///       guarantees that integral values read will be written in a
    ///       consistent manner that can be read across systems that might have
    ///       different endiannesses.
    ///
    /// \tparam T the type to read
    /// \return T on success
    template <typename T>
    auto read_object() noexcept -> core::result<T,std::error_code>;

    /// \brief Writes an object to this file
    ///
    /// This uses the core::serializer<T> customization point to write
    /// arbitrary types.
    ///
    /// \note Writing objects directly from the file is meant to not only
    ///       provide a means of reading stronger data-types, but also provide
    ///       a portable API for reading from files on systems that might
    ///       otherwise have different endiannesses. Using 'read_object'
    ///       guarantees that integral values read will be written in a
    ///       consistent manner that can be read across systems that might have
    ///       different endiannesses.
    ///
    /// \param v the value to write
    /// \return void on success
    template <typename T>
    auto write_object(const T& v)
      noexcept -> core::result<void,std::error_code>;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Reading
    //-------------------------------------------------------------------------
  private:

    auto do_read(std::span<std::byte> buffer)
      noexcept -> core::result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Writing
    //-------------------------------------------------------------------------
  private:

    auto do_write(std::span<const std::byte> buffer)
      noexcept -> core::result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    file_stream_handle m_handle;
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
  ALLOY_PLATFORM_API
  auto make_error_code(file::error_code ec) noexcept -> std::error_code;

} // namespace alloy::platform

namespace std {
  template <>
  struct is_error_code_enum<alloy::platform::file::error_code> : std::true_type{};
} // namespace std

//=============================================================================
// inline definitions : class : file
//=============================================================================

//-----------------------------------------------------------------------------
// File API
//-----------------------------------------------------------------------------

template <typename T>
inline
auto alloy::platform::file::read_object() noexcept -> core::result<T,std::error_code>
{
  return core::deserialize<T>(*this);
}

template <typename T>
inline
auto alloy::platform::file::write_object(const T& v)
  noexcept -> core::result<void,std::error_code>
{
  return core::serialize(*this, v);
}

#endif /* ALLOY_PLATFORM_FILESYSTEM_FILE_HPP */

