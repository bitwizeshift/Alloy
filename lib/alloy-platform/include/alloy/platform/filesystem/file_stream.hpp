// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file file.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_HPP
#define ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/api.hpp"
#include "alloy/platform/buffers/const_buffer.hpp"
#include "alloy/platform/buffers/mutable_buffer.hpp"

#include "alloy/core/utilities/result.hpp"

#include <cstddef>      // std::size_t
#include <system_error> // std::error_code

namespace alloy::platform {

  //===========================================================================
  // class : file_stream
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An interface for a basic file stream
  ///
  /// File streams are intentionally immovable and uncopyable since their
  /// lifetimes are intended to be bound inside of the file object itself.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API file_stream
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

    auto operator=(const file_stream&) -> file_stream& = delete;
    auto operator=(file_stream&&) -> file_stream& = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the number of bytes in this file stream
    ///
    /// \return the number of bytes in this file stream
    virtual auto bytes()
      const noexcept -> core::result<size_type,std::error_code> = 0;

    //-------------------------------------------------------------------------
    // File Access
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes the file stream for future access
    virtual auto close() noexcept -> void = 0;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    virtual auto reset() noexcept -> core::result<void,std::error_code> = 0;

    /// \brief Flushes the contents of the stream to storage
    ///
    /// \return void on success
    virtual auto flush() noexcept -> core::result<void,std::error_code>;

    /// \brief Skips up to the next N bytes of the file
    ///
    /// \param offset the number of bytes to skip
    /// \return void on success
    virtual auto skip(offset_type offset)
      noexcept -> core::result<void,std::error_code> = 0;

    /// \brief Reads data into the specified \p buffer, returning a buffer the
    ///        buffer that was read
    ///
    /// \param buffer the buffer to read into
    /// \return the buffer that was read on success
    virtual auto read(mutable_buffer buffer)
      noexcept -> core::result<mutable_buffer,std::error_code> = 0;

    /// \brief Writes data from the specified \p buffer, returning a buffer of
    ///        the bytes that were written
    ///
    /// \param buffer the buffer to write from
    /// \return the buffer that was written on success
    virtual auto write(const_buffer buffer)
      noexcept -> core::result<const_buffer,std::error_code> = 0;
  };

} // namespace alloy::platform

//=============================================================================
// inline definitions : class : file_stream
//=============================================================================

//-----------------------------------------------------------------------------
// File Access
//-----------------------------------------------------------------------------

inline
auto alloy::platform::file_stream::flush()
  noexcept -> core::result<void,std::error_code>
{
  return {};
}

#endif /* ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_HPP */
