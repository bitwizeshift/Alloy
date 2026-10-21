// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file buffered_writer.hpp
///
/// \brief This header contains the definition of the buffered_writer, a writer
///        that buffers the bytes written to another writer
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_CORE_IO_BUFFERED_WRITER_HPP
#define ALLOY_CORE_IO_BUFFERED_WRITER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/api.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/writer.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code

namespace alloy::core {

  //===========================================================================
  // class : buffered_writer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A writer that writes to another writer through a buffer
  ///
  /// Written bytes are held in the buffer until it cannot hold the next
  /// write, so that many small writes to the buffered_writer become few large
  /// writes to the underlying writer. Writes that are at least as large as
  /// the buffer bypass it once it is empty, since buffering them would only
  /// add a copy.
  ///
  /// Bytes that are still buffered are only written to the underlying writer
  /// by `flush`, or by a write that does not fit in the buffer. They are not
  /// flushed when the buffered_writer is destroyed, since a failure could not
  /// be reported from there; any bytes that were not flushed are discarded.
  ///
  /// \note The buffered_writer does not own the underlying writer. Care must
  ///       be taken to ensure that the underlying writer outlives it.
  ///
  /// \note A moved-from buffered_writer may only be assigned to or destroyed.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_CORE_API buffered_writer final : public writer
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

    /// \brief Makes a buffered_writer of \p destination, with a buffer of
    ///        `default_capacity` bytes allocated from \p alloc
    ///
    /// \pre \p destination must outlive the buffered_writer
    ///
    /// \param destination the writer to write to
    /// \param alloc the allocator to allocate the buffer from
    /// \return the buffered_writer on success, or
    ///         `std::errc::not_enough_memory` if the buffer could not be
    ///         allocated
    static auto from_writer(not_null<writer*> destination, allocator alloc = {})
      noexcept -> result<buffered_writer,std::error_code>;

    /// \brief Makes a buffered_writer of \p destination, with a buffer of
    ///        \p capacity bytes allocated from \p alloc
    ///
    /// \pre \p capacity is not `0`
    /// \pre \p destination must outlive the buffered_writer
    ///
    /// \param capacity the number of bytes the buffer holds
    /// \param destination the writer to write to
    /// \param alloc the allocator to allocate the buffer from
    /// \return the buffered_writer on success, or
    ///         `std::errc::not_enough_memory` if the buffer could not be
    ///         allocated
    static auto with_capacity(
      std::size_t capacity,
      not_null<writer*> destination,
      allocator alloc = {}
    ) noexcept -> result<buffered_writer,std::error_code>;

    /// \brief Constructs this buffered_writer by taking the buffer and the
    ///        underlying writer of \p other
    ///
    /// \param other the other buffered_writer to move
    buffered_writer(buffered_writer&& other) noexcept;
    buffered_writer(const buffered_writer&) = delete;

    //-------------------------------------------------------------------------

    /// \brief Flushes any buffered bytes to the underlying writer, then
    ///        releases the buffer
    ///
    /// A failure to flush cannot be reported from the destructor, and is
    /// ignored. Call flush beforehand to observe it.
    ~buffered_writer() noexcept;

    //-------------------------------------------------------------------------

    /// \brief Assigns the buffer and the underlying writer of \p other to
    ///        this, releasing the buffer of this
    ///
    /// Any bytes of this that were not flushed are discarded.
    ///
    /// \param other the other buffered_writer to move
    /// \return reference to `(*this)`
    auto operator=(buffered_writer&& other) noexcept -> buffered_writer&;
    auto operator=(const buffered_writer&) -> buffered_writer& = delete;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Gets the number of bytes the buffer can hold
    ///
    /// \return the capacity of the buffer
    [[nodiscard]]
    auto capacity() const noexcept -> std::size_t;

    /// \brief Gets the bytes that have been buffered but not yet written to
    ///        the underlying writer
    ///
    /// \return the buffered bytes
    [[nodiscard]]
    auto buffer() const noexcept -> std::span<const std::byte>;

    /// \brief Gets the writer that this buffered_writer writes to
    ///
    /// \note Writing to the underlying writer directly places those bytes
    ///       before any bytes that are still buffered
    ///
    /// \return the underlying writer
    [[nodiscard]]
    auto destination() const noexcept -> writer&;

    //-------------------------------------------------------------------------
    // Buffering
    //-------------------------------------------------------------------------
  public:

    /// \brief Writes every buffered byte to the underlying writer
    ///
    /// On failure, the bytes that were not written remain buffered, so that
    /// flushing may be retried.
    ///
    /// \return `void` on success, `io_error::write_zero` if the underlying
    ///         writer stops accepting bytes, or the error of the underlying
    ///         writer if it fails
    auto flush() noexcept -> result<void,std::error_code>;

    //-------------------------------------------------------------------------
    // Private Constructors
    //-------------------------------------------------------------------------
  private:

    buffered_writer(
      not_null<writer*> destination,
      allocator alloc,
      std::byte* buffer,
      std::size_t capacity
    ) noexcept;

    //-------------------------------------------------------------------------
    // Private Writing
    //-------------------------------------------------------------------------
  private:

    /// \brief Writes \p buffer when it does not fit in the space left in the
    ///        buffer
    ///
    /// \param buffer the bytes to write
    /// \return the number of bytes written on success
    auto write_unbuffered(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code>;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Writing
    //-------------------------------------------------------------------------
  private:

    auto do_write(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    not_null<writer*> m_destination;
    allocator m_allocator;
    std::byte* m_buffer;
    std::size_t m_capacity;
    std::size_t m_size;
  };

} // namespace alloy::core

//=============================================================================
// inline definitions : class : buffered_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer::capacity() const noexcept -> std::size_t
{
  return m_capacity;
}

ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer::buffer()
  const noexcept -> std::span<const std::byte>
{
  return std::span<const std::byte>{m_buffer, m_size};
}

ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer::destination() const noexcept -> writer&
{
  return *m_destination;
}

#endif /* ALLOY_CORE_IO_BUFFERED_WRITER_HPP */
