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
#include "alloy/core/utilities/result.hpp"

#include <concepts>     // std::derived_from, std::same_as
#include <cstddef>      // std::byte, std::size_t
#include <memory>       // std::allocator_arg_t
#include <span>         // std::span
#include <system_error> // std::error_code
#include <type_traits>  // std::remove_cvref_t, std::is_constructible_v
#include <utility>      // std::in_place_t, std::forward, std::move

namespace alloy::core {

  namespace detail {

    /// \brief True when `T` is a writer object that can be owned and written
    ///        to: a type derived from `writer` that is not cv-qualified
    template <typename T>
    concept writer_object =
      std::derived_from<T, writer> && std::same_as<T, std::remove_cv_t<T>>;

    //=========================================================================
    // class : buffered_writer_core
    //=========================================================================

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The buffer of a buffered_writer, along with every operation on
    ///        it that does not depend on the type of the underlying writer
    ///
    /// The buffer is allocated by the first write that needs it, rather than
    /// on construction, so that a failure to allocate it can be reported
    /// through the result of that write.
    ///////////////////////////////////////////////////////////////////////////
    class ALLOY_CORE_API buffered_writer_core
    {
      //-----------------------------------------------------------------------
      // Constructors / Destructor / Assignment
      //-----------------------------------------------------------------------
    public:

      /// \brief Constructs a buffered_writer_core whose buffer holds
      ///        \p capacity bytes once allocated from \p alloc
      ///
      /// \pre \p capacity is not `0`
      ///
      /// \param capacity the number of bytes the buffer holds
      /// \param alloc the allocator to allocate the buffer from
      buffered_writer_core(std::size_t capacity, allocator alloc) noexcept;

      /// \brief Constructs this buffered_writer_core by taking the buffer of
      ///        \p other
      ///
      /// \param other the other buffered_writer_core to move
      buffered_writer_core(buffered_writer_core&& other) noexcept;
      buffered_writer_core(const buffered_writer_core&) = delete;

      //-----------------------------------------------------------------------

      /// \brief Releases the buffer, discarding any bytes that were not
      ///        flushed
      ~buffered_writer_core() noexcept;

      //-----------------------------------------------------------------------

      /// \brief Assigns the buffer of \p other to this, releasing the buffer
      ///        of this and discarding any bytes that were not flushed
      ///
      /// \param other the other buffered_writer_core to move
      /// \return reference to `(*this)`
      auto operator=(buffered_writer_core&& other)
        noexcept -> buffered_writer_core&;
      auto operator=(const buffered_writer_core&)
        -> buffered_writer_core& = delete;

      //-----------------------------------------------------------------------
      // Observers
      //-----------------------------------------------------------------------
    public:

      /// \brief Gets the number of bytes the buffer can hold
      ///
      /// \return the capacity of the buffer
      [[nodiscard]]
      auto capacity() const noexcept -> std::size_t;

      /// \brief Gets the bytes that have been buffered but not yet flushed
      ///
      /// \return the buffered bytes
      [[nodiscard]]
      auto buffer() const noexcept -> std::span<const std::byte>;

      //-----------------------------------------------------------------------
      // Buffering
      //-----------------------------------------------------------------------
    public:

      /// \brief Writes every buffered byte to \p destination
      ///
      /// On failure, the bytes that were not written remain buffered, so that
      /// flushing may be retried.
      ///
      /// \param destination the writer to flush to
      /// \return `void` on success, `io_error::write_zero` if \p destination
      ///         stops accepting bytes, or the error of \p destination if it
      ///         fails
      auto flush(writer& destination)
        noexcept -> result<void,std::error_code>;

      //-----------------------------------------------------------------------
      // Writing
      //-----------------------------------------------------------------------
    public:

      /// \brief Writes \p buffer into the buffer, flushing to \p destination
      ///        first if it does not fit
      ///
      /// A write that is at least as large as the buffer, once nothing is
      /// buffered, is passed straight to \p destination without allocating
      /// the buffer.
      ///
      /// \param destination the writer that the buffer is flushed to
      /// \param buffer the bytes to write
      /// \return the number of bytes written on success,
      ///         `std::errc::not_enough_memory` if the buffer could not be
      ///         allocated, or the error of \p destination if it fails
      auto write(writer& destination, std::span<const std::byte> buffer)
        noexcept -> result<std::size_t,std::error_code>;

      //-----------------------------------------------------------------------
      // Private Writing
      //-----------------------------------------------------------------------
    private:

      /// \brief Writes \p buffer when it does not fit in the space left in
      ///        the buffer, or when the buffer has not been allocated
      ///
      /// \param destination the writer to write to
      /// \param buffer the bytes to write
      /// \return the number of bytes written on success
      auto write_unbuffered(
        writer& destination,
        std::span<const std::byte> buffer
      ) noexcept -> result<std::size_t,std::error_code>;

      /// \brief Releases the buffer, if it has been allocated
      auto release() noexcept -> void;

      //-----------------------------------------------------------------------
      // Private Members
      //-----------------------------------------------------------------------
    private:

      allocator m_allocator;
      std::byte* m_buffer;
      std::size_t m_capacity;
      std::size_t m_size;
    };

  } // namespace detail

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
  /// The buffer is allocated by the first write that needs it, rather than on
  /// construction. A failure to allocate it is reported by that write as
  /// `std::errc::not_enough_memory`.
  ///
  /// Bytes that are still buffered are only written to the underlying writer
  /// by `flush`, by a write that does not fit in the buffer, or by destroying
  /// or assigning to the buffered_writer. A failure to flush from the latter
  /// two cannot be reported, and the bytes that were not flushed are
  /// discarded.
  ///
  /// The buffered_writer owns the underlying writer. A writer that is owned
  /// elsewhere can be written to through an `indirect_writer`.
  ///
  /// \note A moved-from buffered_writer may only be assigned to or destroyed.
  ///
  /// \tparam Writer the type of the underlying writer
  /////////////////////////////////////////////////////////////////////////////
  template <detail::writer_object Writer>
  class buffered_writer final : public writer
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

    /// \brief Constructs a buffered_writer of the writer constructed from
    ///        \p destination, with a buffer of \p capacity bytes allocated
    ///        from \p alloc
    ///
    /// \pre \p capacity is not `0`
    ///
    /// \param destination the writer, or the argument to construct it from
    /// \param capacity the number of bytes the buffer holds
    /// \param alloc the allocator to allocate the buffer from
    template <typename U>
      requires (
        !std::same_as<std::remove_cvref_t<U>, buffered_writer> &&
        !std::same_as<std::remove_cvref_t<U>, std::in_place_t> &&
        !std::same_as<std::remove_cvref_t<U>, std::allocator_arg_t> &&
        std::is_constructible_v<Writer, U>
      )
    explicit buffered_writer(
      U&& destination,
      std::size_t capacity = default_capacity,
      allocator alloc = {}
    ) noexcept(std::is_nothrow_constructible_v<Writer, U>);

    /// \brief Constructs a buffered_writer of the writer constructed in place
    ///        from \p args, with a buffer of `default_capacity` bytes
    ///
    /// \param args the arguments to construct the writer from
    template <typename...Args>
      requires (std::is_constructible_v<Writer, Args...>)
    explicit buffered_writer(std::in_place_t, Args&&...args)
      noexcept(std::is_nothrow_constructible_v<Writer, Args...>);

    /// \brief Constructs a buffered_writer of the writer constructed in place
    ///        from \p args, with a buffer of \p capacity bytes allocated from
    ///        \p alloc
    ///
    /// \pre \p capacity is not `0`
    ///
    /// \param alloc the allocator to allocate the buffer from
    /// \param capacity the number of bytes the buffer holds
    /// \param args the arguments to construct the writer from
    template <typename...Args>
      requires (std::is_constructible_v<Writer, Args...>)
    buffered_writer(
      std::allocator_arg_t,
      allocator alloc,
      std::size_t capacity,
      std::in_place_t,
      Args&&...args
    ) noexcept(std::is_nothrow_constructible_v<Writer, Args...>);

    /// \brief Constructs this buffered_writer by taking the buffer and the
    ///        underlying writer of \p other
    ///
    /// This constructor only participates when `Writer` is move
    /// constructible.
    ///
    /// \param other the other buffered_writer to move
    buffered_writer(buffered_writer&& other) = default;
    buffered_writer(const buffered_writer&) = delete;

    //-------------------------------------------------------------------------

    /// \brief Flushes any buffered bytes to the underlying writer, then
    ///        releases the buffer
    ///
    /// A failure to flush cannot be reported from the destructor, and is
    /// ignored. Call flush beforehand to observe it.
    ~buffered_writer() noexcept;

    //-------------------------------------------------------------------------

    /// \brief Flushes any buffered bytes of this, then assigns the buffer and
    ///        the underlying writer of \p other to this
    ///
    /// A failure to flush cannot be reported from the assignment, and the
    /// bytes of this that were not flushed are discarded. This operator only
    /// participates when `Writer` is move assignable.
    ///
    /// \param other the other buffered_writer to move
    /// \return reference to `(*this)`
    auto operator=(buffered_writer&& other)
      noexcept(std::is_nothrow_move_assignable_v<Writer>) -> buffered_writer&
      requires (std::is_move_assignable_v<Writer>);
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
    auto destination() noexcept -> Writer&;
    [[nodiscard]]
    auto destination() const noexcept -> const Writer&;

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
    // Virtual Hooks : Writing
    //-------------------------------------------------------------------------
  private:

    auto do_write(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    Writer m_destination;
    detail::buffered_writer_core m_core;
  };

  template <typename U>
  buffered_writer(U&&) -> buffered_writer<std::remove_cvref_t<U>>;

  template <typename U>
  buffered_writer(U&&, std::size_t) -> buffered_writer<std::remove_cvref_t<U>>;

  template <typename U>
  buffered_writer(U&&, std::size_t, allocator)
    -> buffered_writer<std::remove_cvref_t<U>>;

} // namespace alloy::core

//=============================================================================
// inline definitions : class : detail::buffered_writer_core
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::detail::buffered_writer_core::capacity()
  const noexcept -> std::size_t
{
  return m_capacity;
}

ALLOY_FORCE_INLINE
auto alloy::core::detail::buffered_writer_core::buffer()
  const noexcept -> std::span<const std::byte>
{
  return std::span<const std::byte>{m_buffer, m_size};
}

//=============================================================================
// inline definitions : class : buffered_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

template <alloy::core::detail::writer_object Writer>
template <typename U>
  requires (
    !std::same_as<
      std::remove_cvref_t<U>,
      alloy::core::buffered_writer<Writer>
    > &&
    !std::same_as<std::remove_cvref_t<U>, std::in_place_t> &&
    !std::same_as<std::remove_cvref_t<U>, std::allocator_arg_t> &&
    std::is_constructible_v<Writer, U>
  )
inline
alloy::core::buffered_writer<Writer>::buffered_writer(
  U&& destination,
  std::size_t capacity,
  allocator alloc
) noexcept(std::is_nothrow_constructible_v<Writer, U>)
  : m_destination(std::forward<U>(destination)),
    m_core{capacity, alloc}
{

}

template <alloy::core::detail::writer_object Writer>
template <typename...Args>
  requires (std::is_constructible_v<Writer, Args...>)
inline
alloy::core::buffered_writer<Writer>::buffered_writer(
  std::in_place_t,
  Args&&...args
) noexcept(std::is_nothrow_constructible_v<Writer, Args...>)
  : m_destination(std::forward<Args>(args)...),
    m_core{default_capacity, allocator{}}
{

}

template <alloy::core::detail::writer_object Writer>
template <typename...Args>
  requires (std::is_constructible_v<Writer, Args...>)
inline
alloy::core::buffered_writer<Writer>::buffered_writer(
  std::allocator_arg_t,
  allocator alloc,
  std::size_t capacity,
  std::in_place_t,
  Args&&...args
) noexcept(std::is_nothrow_constructible_v<Writer, Args...>)
  : m_destination(std::forward<Args>(args)...),
    m_core{capacity, alloc}
{

}

//-----------------------------------------------------------------------------

template <alloy::core::detail::writer_object Writer>
inline
alloy::core::buffered_writer<Writer>::~buffered_writer<Writer>() noexcept
{
  const auto flush_result = flush();

  // There is no way to report a failure from the destructor, so we ignore it
  // but attempt to flush any remaining bytes
  compiler::unused(flush_result);
}

//-----------------------------------------------------------------------------

template <alloy::core::detail::writer_object Writer>
inline
auto alloy::core::buffered_writer<Writer>::operator=(buffered_writer&& other)
  noexcept(std::is_nothrow_move_assignable_v<Writer>) -> buffered_writer&
  requires (std::is_move_assignable_v<Writer>)
{
  // The buffered bytes belong to the current destination, so they must be
  // flushed before it is replaced.
  const auto flush_result = flush();
  compiler::unused(flush_result);

  m_destination = std::move(other.m_destination);
  m_core = std::move(other.m_core);
  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <alloy::core::detail::writer_object Writer>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer<Writer>::capacity()
  const noexcept -> std::size_t
{
  return m_core.capacity();
}

template <alloy::core::detail::writer_object Writer>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer<Writer>::buffer()
  const noexcept -> std::span<const std::byte>
{
  return m_core.buffer();
}

template <alloy::core::detail::writer_object Writer>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer<Writer>::destination() noexcept -> Writer&
{
  return m_destination;
}

template <alloy::core::detail::writer_object Writer>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer<Writer>::destination()
  const noexcept -> const Writer&
{
  return m_destination;
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

template <alloy::core::detail::writer_object Writer>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_writer<Writer>::flush()
  noexcept -> result<void,std::error_code>
{
  return m_core.flush(m_destination);
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Writing
//-----------------------------------------------------------------------------

template <alloy::core::detail::writer_object Writer>
inline
auto alloy::core::buffered_writer<Writer>::do_write(
  std::span<const std::byte> buffer
) noexcept -> result<std::size_t,std::error_code>
{
  return m_core.write(m_destination, buffer);
}

#endif /* ALLOY_CORE_IO_BUFFERED_WRITER_HPP */
