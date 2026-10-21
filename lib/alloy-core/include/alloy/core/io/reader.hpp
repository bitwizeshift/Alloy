// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file reader.hpp
///
/// \brief This header contains the definition of the reader interface, along
///        with utilities for reading from any reader
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_READER_HPP
#define ALLOY_CORE_IO_READER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/assert.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code

namespace alloy::core {

  //===========================================================================
  // class : reader
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An interface for a source of bytes that can be read in sequence
  ///
  /// A reader produces bytes from wherever it is backed by, such as a file,
  /// a socket, or a region of memory. Each read produces as many bytes as the
  /// reader has readily available, up to the size of the requested buffer;
  /// a read that produces fewer bytes than requested is not an error.
  ///
  /// The end of the stream is signaled by successfully reading `0` bytes into
  /// a non-empty buffer.
  /////////////////////////////////////////////////////////////////////////////
  class reader
  {
    //-------------------------------------------------------------------------
    // Destructor
    //-------------------------------------------------------------------------
  public:

    virtual ~reader() = default;

    //-------------------------------------------------------------------------
    // Reading
    //-------------------------------------------------------------------------
  public:

    /// \brief Reads up to `buffer.size()` bytes into \p buffer
    ///
    /// Only the first `n` bytes of \p buffer are written to, where `n` is the
    /// number of bytes read. A result of `0` indicates the end of the stream
    /// when \p buffer is not empty.
    ///
    /// \post the number of bytes read is no larger than `buffer.size()`
    ///
    /// \param buffer the buffer to read into
    /// \return the number of bytes read on success
    auto read(std::span<std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code>;

    //-------------------------------------------------------------------------
    // Protected Constructors / Assignment
    //-------------------------------------------------------------------------
  protected:

    reader() noexcept = default;
    reader(reader&&) noexcept = default;
    reader(const reader&) noexcept = default;

    auto operator=(reader&&) noexcept -> reader& = default;
    auto operator=(const reader&) noexcept -> reader& = default;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Reading
    //-------------------------------------------------------------------------
  private:

    virtual auto do_read(std::span<std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> = 0;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : reader
//=============================================================================

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

inline
auto alloy::core::reader::read(std::span<std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  auto bytes_read = do_read(buffer);

  ALLOY_ASSERT(
    bytes_read.has_error() || *bytes_read <= buffer.size(),
    "A reader cannot read more bytes than the buffer can hold"
  );

  return bytes_read;
}

#endif /* ALLOY_CORE_IO_READER_HPP */
