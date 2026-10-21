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

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/utilities/not_null.hpp"
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

  //===========================================================================
  // non-member functions : class : reader
  //===========================================================================

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Reads from \p source into \p buffer until at least \p min bytes
  ///        have been read
  ///
  /// Reading stops as soon as \p min bytes have been read, so more than
  /// \p min bytes may be read when the last read produces more bytes than were
  /// still needed. No reads are made when \p min is `0`.
  ///
  /// On failure, the bytes that were read before the failure have been
  /// written to the start of \p buffer and consumed from \p source.
  ///
  /// \pre \p min is no larger than `buffer.size()`
  ///
  /// \param source the reader to read from
  /// \param buffer the buffer to read into
  /// \param min the minimum number of bytes to read
  /// \return the number of bytes read on success, `io_error::unexpected_eof` if
  ///         the stream ends before \p min bytes are read, or the error of
  ///         \p source if it fails
  ALLOY_CORE_API
  auto read_at_least(
    reader& source,
    std::span<std::byte> buffer,
    std::size_t min
  ) noexcept -> result<std::size_t,std::error_code>;

  /// \brief Reads from \p source until \p buffer is filled
  ///
  /// On failure, the bytes that were read before the failure have been
  /// written to the start of \p buffer and consumed from \p source.
  ///
  /// \param source the reader to read from
  /// \param buffer the buffer to fill
  /// \return `void` on success, `io_error::unexpected_eof` if the stream ends
  ///         before \p buffer is filled, or the error of \p source if it fails
  auto read_exact(reader& source, std::span<std::byte> buffer)
    noexcept -> result<void,std::error_code>;

  /// \brief Reads every remaining byte from \p source, appending them to
  ///        \p out
  ///
  /// The existing contents of \p out are kept. On failure, the bytes that
  /// were read before the failure remain appended to \p out.
  ///
  /// \param source the reader to read from
  /// \param out the vector to append the bytes to
  /// \return the number of bytes appended on success, or the error of
  ///         \p source if it fails
  ALLOY_CORE_API
  auto read_all_into(reader& source, not_null<vector<std::byte>*> out)
    -> result<std::size_t,std::error_code>;

  /// \brief Reads every remaining byte from \p source into a new vector
  ///
  /// \param source the reader to read from
  /// \return the bytes that were read on success, or the error of \p source
  ///         if it fails
  ALLOY_CORE_API
  auto read_all(reader& source) -> result<vector<std::byte>,std::error_code>;

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

//=============================================================================
// inline definitions : non-member functions : class : reader
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

inline
auto alloy::core::read_exact(reader& source, std::span<std::byte> buffer)
  noexcept -> result<void,std::error_code>
{
  const auto bytes_read = read_at_least(source, buffer, buffer.size());
  if (ALLOY_UNLIKELY(bytes_read.has_error())) {
    return fail(bytes_read.error());
  }
  return {};
}

#endif /* ALLOY_CORE_IO_READER_HPP */
