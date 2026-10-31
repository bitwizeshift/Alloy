// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file span_reader.hpp
///
/// \brief This header contains the definition of the span_reader, a reader
///        over a contiguous region of bytes in memory
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_SPAN_READER_HPP
#define ALLOY_CORE_IO_SPAN_READER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/reader.hpp"
#include "alloy/core/utilities/result.hpp"

#include <algorithm>    // std::min, std::copy_n
#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code

namespace alloy::core {

  //===========================================================================
  // class : span_reader
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A reader that reads the bytes of a span in sequence
  ///
  /// Each read produces as many of the remaining bytes as the buffer can
  /// hold, and the stream ends once every byte of the span has been read.
  /// Reading from a span_reader never fails.
  ///
  /// \note The span_reader does not own the bytes it reads from. Care must be
  ///       taken to ensure that the span_reader does not outlive them.
  /////////////////////////////////////////////////////////////////////////////
  class span_reader final : public reader
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a span_reader that reads from \p bytes
    ///
    /// \pre \p bytes must outlive the span_reader
    ///
    /// \param bytes the bytes to read
    explicit span_reader(std::span<const std::byte> bytes) noexcept;
    span_reader(span_reader&&) noexcept = default;
    span_reader(const span_reader&) noexcept = default;

    //-------------------------------------------------------------------------

    auto operator=(span_reader&&) noexcept -> span_reader& = default;
    auto operator=(const span_reader&) noexcept -> span_reader& = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the bytes that have not yet been read
    ///
    /// \return the remaining bytes
    [[nodiscard]]
    auto remaining() const noexcept -> std::span<const std::byte>;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Reading
    //-------------------------------------------------------------------------
  private:

    auto do_read(std::span<std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    std::span<const std::byte> m_bytes;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : span_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
alloy::core::span_reader::span_reader(std::span<const std::byte> bytes)
  noexcept
  : m_bytes{bytes}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::span_reader::remaining()
  const noexcept -> std::span<const std::byte>
{
  return m_bytes;
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Reading
//-----------------------------------------------------------------------------

inline
auto alloy::core::span_reader::do_read(std::span<std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  const auto count = std::min(buffer.size(), m_bytes.size());

  std::copy_n(m_bytes.data(), count, buffer.data());
  m_bytes = m_bytes.subspan(count);

  return count;
}

#endif /* ALLOY_CORE_IO_SPAN_READER_HPP */
