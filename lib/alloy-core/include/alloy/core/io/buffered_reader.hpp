// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file buffered_reader.hpp
///
/// \brief This header contains the definition of the buffered_reader, a reader
///        that buffers the bytes of another reader
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_BUFFERED_READER_HPP
#define ALLOY_CORE_IO_BUFFERED_READER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/reader.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code

namespace alloy::core {

  //===========================================================================
  // class : buffered_reader
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A reader that reads from another reader through a buffer
  ///
  /// Each read from the underlying reader requests enough bytes to fill the
  /// whole buffer, so that many small reads from the buffered_reader become
  /// few large reads of the underlying reader. Reads that are at least as
  /// large as the buffer bypass it when it is empty, since buffering them
  /// would only add a copy.
  ///
  /// The buffer can also be read from directly with `fill_buffer` and
  /// `consume`, which avoids copying the bytes out of it altogether.
  ///
  /// \note The buffered_reader does not own the underlying reader. Care must
  ///       be taken to ensure that the underlying reader outlives it.
  ///
  /// \note A moved-from buffered_reader may only be assigned to or destroyed.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API buffered_reader final : public reader
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    /// The capacity of the buffer when one is not specified
    static constexpr auto default_capacity = std::size_t{8192};

    //-------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Makes a buffered_reader of \p source, with a buffer of
    ///        `default_capacity` bytes allocated from \p alloc
    ///
    /// \pre \p source must outlive the buffered_reader
    ///
    /// \param source the reader to read from
    /// \param alloc the allocator to allocate the buffer from
    /// \return the buffered_reader on success, or
    ///         `std::errc::not_enough_memory` if the buffer could not be
    ///         allocated
    static auto from_reader(not_null<reader*> source, allocator alloc = {})
      noexcept -> result<buffered_reader,std::error_code>;

    /// \brief Makes a buffered_reader of \p source, with a buffer of
    ///        \p capacity bytes allocated from \p alloc
    ///
    /// \pre \p capacity is not `0`
    /// \pre \p source must outlive the buffered_reader
    ///
    /// \param capacity the number of bytes the buffer holds
    /// \param source the reader to read from
    /// \param alloc the allocator to allocate the buffer from
    /// \return the buffered_reader on success, or
    ///         `std::errc::not_enough_memory` if the buffer could not be
    ///         allocated
    static auto with_capacity(
      std::size_t capacity,
      not_null<reader*> source,
      allocator alloc = {}
    ) noexcept -> result<buffered_reader,std::error_code>;

    /// \brief Constructs this buffered_reader by taking the buffer and the
    ///        underlying reader of \p other
    ///
    /// \param other the other buffered_reader to move
    buffered_reader(buffered_reader&& other) noexcept;
    buffered_reader(const buffered_reader&) = delete;

    //-------------------------------------------------------------------------

    ~buffered_reader() noexcept;

    //-------------------------------------------------------------------------

    /// \brief Assigns the buffer and the underlying reader of \p other to
    ///        this, releasing the buffer of this
    ///
    /// \param other the other buffered_reader to move
    /// \return reference to `(*this)`
    auto operator=(buffered_reader&& other) noexcept -> buffered_reader&;
    auto operator=(const buffered_reader&) -> buffered_reader& = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the number of bytes the buffer can hold
    ///
    /// \return the capacity of the buffer
    [[nodiscard]]
    auto capacity() const noexcept -> std::size_t;

    /// \brief Gets the bytes that have been buffered but not yet read
    ///
    /// \return the buffered bytes
    [[nodiscard]]
    auto buffer() const noexcept -> std::span<const std::byte>;

    /// \brief Gets the reader that this buffered_reader reads from
    ///
    /// \note Reading from the underlying reader directly skips any bytes
    ///       that are still buffered
    ///
    /// \return the underlying reader
    [[nodiscard]]
    auto source() const noexcept -> reader&;

    //-------------------------------------------------------------------------
    // Buffering
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the buffered bytes, reading from the underlying reader
    ///        first if none are buffered
    ///
    /// The bytes remain buffered until they are consumed with `consume`. An
    /// empty span is returned only once the underlying reader has ended.
    ///
    /// \return the buffered bytes on success, or the error of the underlying
    ///         reader if it fails
    auto fill_buffer()
      noexcept -> result<std::span<const std::byte>,std::error_code>;

    /// \brief Marks the first \p n buffered bytes as read
    ///
    /// \pre \p n is no larger than `buffer().size()`
    ///
    /// \param n the number of bytes to consume
    auto consume(std::size_t n) noexcept -> void;

    /// \brief Discards every buffered byte without reading it
    auto discard_buffer() noexcept -> void;

    //-------------------------------------------------------------------------
    // Reading
    //-------------------------------------------------------------------------
  public:

    /// \brief Reads bytes up to and including \p delimiter, appending them to
    ///        \p out
    ///
    /// Reading stops after the first \p delimiter, or once the underlying
    /// reader ends. The existing contents of \p out are kept. On failure, the
    /// bytes that were read before the failure remain appended to \p out.
    ///
    /// \param delimiter the byte to read up to
    /// \param out the vector to append the bytes to
    /// \return the number of bytes appended on success, or the error of the
    ///         underlying reader if it fails
    auto read_until(std::byte delimiter, not_null<vector<std::byte>*> out)
      -> result<std::size_t,std::error_code>;

    /// \brief Skips bytes up to and including \p delimiter
    ///
    /// Skipping stops after the first \p delimiter, or once the underlying
    /// reader ends.
    ///
    /// \param delimiter the byte to skip up to
    /// \return the number of bytes skipped on success, or the error of the
    ///         underlying reader if it fails
    auto skip_until(std::byte delimiter)
      noexcept -> result<std::size_t,std::error_code>;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    buffered_reader(
      not_null<reader*> source,
      allocator alloc,
      std::byte* buffer,
      std::size_t capacity
    ) noexcept;

    //-------------------------------------------------------------------------
    // Private Buffering
    //-------------------------------------------------------------------------
  private:

    /// \brief Replaces the contents of the buffer with a single read from the
    ///        underlying reader
    ///
    /// \pre every buffered byte has been consumed
    ///
    /// \return `void` on success
    auto refill() noexcept -> result<void,std::error_code>;

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

    not_null<reader*> m_source;
    allocator m_allocator;
    std::byte* m_buffer;
    std::size_t m_capacity;
    std::size_t m_position; // index of the first byte that was not read
    std::size_t m_filled;   // index past the last byte that was buffered
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : buffered_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader::capacity() const noexcept -> std::size_t
{
  return m_capacity;
}

ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader::buffer()
  const noexcept -> std::span<const std::byte>
{
  return std::span<const std::byte>{
    m_buffer + m_position,
    m_filled - m_position
  };
}

ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader::source() const noexcept -> reader&
{
  return *m_source;
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

inline
auto alloy::core::buffered_reader::fill_buffer()
  noexcept -> result<std::span<const std::byte>,std::error_code>
{
  if (m_position == m_filled) {
    const auto refilled = refill();
    if (ALLOY_UNLIKELY(refilled.has_error())) {
      return fail(refilled.error());
    }
  }
  return buffer();
}

ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader::consume(std::size_t n) noexcept -> void
{
  ALLOY_ASSERT(n <= m_filled - m_position, "Cannot consume unbuffered bytes");

  m_position += n;
}

ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader::discard_buffer() noexcept -> void
{
  m_position = 0u;
  m_filled = 0u;
}

#endif /* ALLOY_CORE_IO_BUFFERED_READER_HPP */
