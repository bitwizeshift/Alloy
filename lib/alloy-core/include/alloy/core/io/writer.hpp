// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file writer.hpp
///
/// \brief This header contains the definition of the writer interface, along
///        with utilities for writing to any writer
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_WRITER_HPP
#define ALLOY_CORE_IO_WRITER_HPP

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
  // class : writer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An interface for a destination of bytes that can be written in
  ///        sequence
  ///
  /// A writer consumes bytes into wherever it is backed by, such as a file,
  /// a socket, or a region of memory. Each write consumes as many bytes as the
  /// writer can readily accept, up to the size of the given buffer; a write
  /// that consumes fewer bytes than given is not an error.
  /////////////////////////////////////////////////////////////////////////////
  class writer
  {
    //-------------------------------------------------------------------------
    // Destructor
    //-------------------------------------------------------------------------
  public:

    virtual ~writer() = default;

    //-------------------------------------------------------------------------
    // Writing
    //-------------------------------------------------------------------------
  public:

    /// \brief Writes up to `buffer.size()` bytes from \p buffer
    ///
    /// Only the first `n` bytes of \p buffer are written, where `n` is the
    /// number of bytes written. A result of `0` for a non-empty \p buffer
    /// indicates that the writer is no longer able to accept bytes.
    ///
    /// \post the number of bytes written is no larger than `buffer.size()`
    ///
    /// \param buffer the bytes to write
    /// \return the number of bytes written on success
    auto write(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code>;

    //-------------------------------------------------------------------------
    // Protected Constructors / Assignment
    //-------------------------------------------------------------------------
  protected:

    writer() noexcept = default;
    writer(writer&&) noexcept = default;
    writer(const writer&) noexcept = default;

    auto operator=(writer&&) noexcept -> writer& = default;
    auto operator=(const writer&) noexcept -> writer& = default;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Writing
    //-------------------------------------------------------------------------
  private:

    virtual auto do_write(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> = 0;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : writer
//=============================================================================

//-----------------------------------------------------------------------------
// Writing
//-----------------------------------------------------------------------------

inline
auto alloy::core::writer::write(std::span<const std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  auto bytes_written = do_write(buffer);

  ALLOY_ASSERT(
    bytes_written.has_error() || *bytes_written <= buffer.size(),
    "A writer cannot write more bytes than the buffer holds"
  );

  return bytes_written;
}

#endif /* ALLOY_CORE_IO_WRITER_HPP */
