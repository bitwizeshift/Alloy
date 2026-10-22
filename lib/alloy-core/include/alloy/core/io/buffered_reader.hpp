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

#include "alloy/core/io/indirect_reader.hpp"
#include "alloy/core/io/reader.hpp"

#include "alloy/core/api.hpp"
#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <concepts>     // std::derived_from, std::same_as
#include <cstddef>      // std::byte, std::size_t
#include <memory>       // std::allocator_arg_t
#include <span>         // std::span
#include <system_error> // std::error_code
#include <type_traits>  // std::remove_cvref_t, std::is_constructible_v
#include <utility>      // std::in_place_t, std::forward

namespace alloy::core {

  namespace detail {

    /// \brief True when `T` is a reader object that can be owned and read
    ///        from: a type derived from `reader` that is not cv-qualified
    template <typename T>
    concept reader_object =
      std::derived_from<T, reader> && std::same_as<T, std::remove_cv_t<T>>;

    //=========================================================================
    // class : buffered_reader_core
    //=========================================================================

    ///////////////////////////////////////////////////////////////////////////
    /// \brief The buffer of a buffered_reader, along with every operation on
    ///        it that does not depend on the type of the underlying reader
    ///
    /// The buffer is allocated by the first operation that needs it, rather
    /// than on construction, so that a failure to allocate it can be reported
    /// through the result of that operation.
    ///////////////////////////////////////////////////////////////////////////
    class ALLOY_CORE_API buffered_reader_core
    {
      //-----------------------------------------------------------------------
      // Constructors / Destructor / Assignment
      //-----------------------------------------------------------------------
    public:

      /// \brief Constructs a buffered_reader_core whose buffer holds
      ///        \p capacity bytes once allocated from \p alloc
      ///
      /// \pre \p capacity is not `0`
      ///
      /// \param capacity the number of bytes the buffer holds
      /// \param alloc the allocator to allocate the buffer from
      buffered_reader_core(std::size_t capacity, allocator alloc) noexcept;

      /// \brief Constructs this buffered_reader_core by taking the buffer of
      ///        \p other
      ///
      /// \param other the other buffered_reader_core to move
      buffered_reader_core(buffered_reader_core&& other) noexcept;
      buffered_reader_core(const buffered_reader_core&) = delete;

      //-----------------------------------------------------------------------

      /// \brief Releases the buffer, discarding any bytes that were not read
      ~buffered_reader_core() noexcept;

      //-----------------------------------------------------------------------

      /// \brief Assigns the buffer of \p other to this, releasing the buffer
      ///        of this and discarding any bytes that were not read
      ///
      /// \param other the other buffered_reader_core to move
      /// \return reference to `(*this)`
      auto operator=(buffered_reader_core&& other)
        noexcept -> buffered_reader_core&;
      auto operator=(const buffered_reader_core&)
        -> buffered_reader_core& = delete;

      //-----------------------------------------------------------------------
      // Observers
      //-----------------------------------------------------------------------
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

      //-----------------------------------------------------------------------
      // Buffering
      //-----------------------------------------------------------------------
    public:

      /// \brief Gets the buffered bytes, reading from \p source first if
      ///        none are buffered
      ///
      /// The bytes remain buffered until they are consumed with `consume`.
      /// An empty span is returned only once \p source has ended.
      ///
      /// \param source the reader that fills the buffer
      /// \return the buffered bytes on success,
      ///         `std::errc::not_enough_memory` if the buffer could not be
      ///         allocated, or the error of \p source if it fails
      auto fill_buffer(reader& source)
        noexcept -> result<std::span<const std::byte>,std::error_code>;

      /// \brief Marks the first \p n buffered bytes as read
      ///
      /// \pre \p n is no larger than `buffer().size()`
      ///
      /// \param n the number of bytes to consume
      auto consume(std::size_t n) noexcept -> void;

      /// \brief Discards every buffered byte without reading it
      auto discard_buffer() noexcept -> void;

      //-----------------------------------------------------------------------
      // Reading
      //-----------------------------------------------------------------------
    public:

      /// \brief Reads up to `buffer.size()` bytes into \p buffer, from the
      ///        buffered bytes or by filling the buffer from \p source
      ///
      /// A read that is at least as large as the buffer, while nothing is
      /// buffered, is passed straight to \p source without allocating the
      /// buffer.
      ///
      /// \param source the reader that fills the buffer
      /// \param buffer the buffer to read into
      /// \return the number of bytes read on success,
      ///         `std::errc::not_enough_memory` if the buffer could not be
      ///         allocated, or the error of \p source if it fails
      auto read(reader& source, std::span<std::byte> buffer)
        noexcept -> result<std::size_t,std::error_code>;

      /// \brief Reads bytes up to and including \p delimiter, appending them
      ///        to \p out
      ///
      /// Reading stops after the first \p delimiter, or once \p source ends.
      /// The existing contents of \p out are kept. On failure, the bytes that
      /// were read before the failure remain appended to \p out.
      ///
      /// \param source the reader that fills the buffer
      /// \param delimiter the byte to read up to
      /// \param out the vector to append the bytes to
      /// \return the number of bytes appended on success,
      ///         `std::errc::not_enough_memory` if the buffer could not be
      ///         allocated, or the error of \p source if it fails
      auto read_until(
        reader& source,
        std::byte delimiter,
        not_null<vector<std::byte>*> out
      ) -> result<std::size_t,std::error_code>;

      /// \brief Skips bytes up to and including \p delimiter
      ///
      /// Skipping stops after the first \p delimiter, or once \p source
      /// ends.
      ///
      /// \param source the reader that fills the buffer
      /// \param delimiter the byte to skip up to
      /// \return the number of bytes skipped on success,
      ///         `std::errc::not_enough_memory` if the buffer could not be
      ///         allocated, or the error of \p source if it fails
      auto skip_until(reader& source, std::byte delimiter)
        noexcept -> result<std::size_t,std::error_code>;

      //-----------------------------------------------------------------------
      // Private Buffering
      //-----------------------------------------------------------------------
    private:

      /// \brief Replaces the contents of the buffer with a single read from
      ///        \p source, allocating the buffer first if it has not been
      ///
      /// \pre every buffered byte has been consumed
      ///
      /// \param source the reader to read from
      /// \return `void` on success, `std::errc::not_enough_memory` if the
      ///         buffer could not be allocated, or the error of \p source if
      ///         it fails
      auto refill(reader& source) noexcept -> result<void,std::error_code>;

      /// \brief Releases the buffer, if it has been allocated
      auto release() noexcept -> void;

      //-----------------------------------------------------------------------
      // Private Members
      //-----------------------------------------------------------------------
    private:

      allocator m_allocator;
      std::byte* m_buffer;
      std::size_t m_capacity;
      std::size_t m_position; // index of the first byte that was not read
      std::size_t m_filled;   // index past the last byte that was buffered
    };

  } // namespace detail

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
  /// The buffer is allocated by the first read that needs it, rather than on
  /// construction. A failure to allocate it is reported by that read as
  /// `std::errc::not_enough_memory`.
  ///
  /// The buffered_reader owns the underlying reader. A reader that is owned
  /// elsewhere can be read through an `indirect_reader`.
  ///
  /// \note A moved-from buffered_reader may only be assigned to or destroyed.
  ///
  /// \tparam Reader the type of the underlying reader
  /////////////////////////////////////////////////////////////////////////////
  template <detail::reader_object Reader>
  class buffered_reader final : public reader
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

    /// \brief Constructs a buffered_reader of the reader constructed from
    ///        \p source, with a buffer of \p capacity bytes allocated from
    ///        \p alloc
    ///
    /// \pre \p capacity is not `0`
    ///
    /// \param source the reader, or the argument to construct it from
    /// \param capacity the number of bytes the buffer holds
    /// \param alloc the allocator to allocate the buffer from
    template <typename U>
      requires (
        !std::same_as<std::remove_cvref_t<U>, buffered_reader> &&
        !std::same_as<std::remove_cvref_t<U>, std::in_place_t> &&
        !std::same_as<std::remove_cvref_t<U>, std::allocator_arg_t> &&
        std::is_constructible_v<Reader, U>
      )
    explicit buffered_reader(
      U&& source,
      std::size_t capacity = default_capacity,
      allocator alloc = {}
    ) noexcept(std::is_nothrow_constructible_v<Reader, U>);

    /// \brief Constructs a buffered_reader of the reader constructed in place
    ///        from \p args, with a buffer of `default_capacity` bytes
    ///
    /// \param args the arguments to construct the reader from
    template <typename...Args>
      requires (std::is_constructible_v<Reader, Args...>)
    explicit buffered_reader(std::in_place_t, Args&&...args)
      noexcept(std::is_nothrow_constructible_v<Reader, Args...>);

    /// \brief Constructs a buffered_reader of the reader constructed in place
    ///        from \p args, with a buffer of \p capacity bytes allocated from
    ///        \p alloc
    ///
    /// \pre \p capacity is not `0`
    ///
    /// \param alloc the allocator to allocate the buffer from
    /// \param capacity the number of bytes the buffer holds
    /// \param args the arguments to construct the reader from
    template <typename...Args>
      requires (std::is_constructible_v<Reader, Args...>)
    buffered_reader(
      std::allocator_arg_t,
      allocator alloc,
      std::size_t capacity,
      std::in_place_t,
      Args&&...args
    ) noexcept(std::is_nothrow_constructible_v<Reader, Args...>);

    /// \brief Constructs this buffered_reader by taking the buffer and the
    ///        underlying reader of \p other
    ///
    /// This constructor only participates when `Reader` is move
    /// constructible.
    ///
    /// \param other the other buffered_reader to move
    buffered_reader(buffered_reader&& other) = default;
    buffered_reader(const buffered_reader&) = delete;

    //-------------------------------------------------------------------------

    /// \brief Assigns the buffer and the underlying reader of \p other to
    ///        this, releasing the buffer of this
    ///
    /// This operator only participates when `Reader` is move assignable.
    ///
    /// \param other the other buffered_reader to move
    /// \return reference to `(*this)`
    auto operator=(buffered_reader&& other) -> buffered_reader& = default;
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
    auto source() noexcept -> Reader&;
    [[nodiscard]]
    auto source() const noexcept -> const Reader&;

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
    /// \return the buffered bytes on success, `std::errc::not_enough_memory`
    ///         if the buffer could not be allocated, or the error of the
    ///         underlying reader if it fails
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
    /// \return the number of bytes appended on success,
    ///         `std::errc::not_enough_memory` if the buffer could not be
    ///         allocated, or the error of the underlying reader if it fails
    auto read_until(std::byte delimiter, not_null<vector<std::byte>*> out)
      -> result<std::size_t,std::error_code>;

    /// \brief Skips bytes up to and including \p delimiter
    ///
    /// Skipping stops after the first \p delimiter, or once the underlying
    /// reader ends.
    ///
    /// \param delimiter the byte to skip up to
    /// \return the number of bytes skipped on success,
    ///         `std::errc::not_enough_memory` if the buffer could not be
    ///         allocated, or the error of the underlying reader if it fails
    auto skip_until(std::byte delimiter)
      noexcept -> result<std::size_t,std::error_code>;

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

    Reader m_source;
    detail::buffered_reader_core m_core;
  };

  template <typename U>
  buffered_reader(U&&) -> buffered_reader<std::remove_cvref_t<U>>;

  template <typename U>
  buffered_reader(U&&, std::size_t) -> buffered_reader<std::remove_cvref_t<U>>;

  template <typename U>
  buffered_reader(U&&, std::size_t, allocator)
    -> buffered_reader<std::remove_cvref_t<U>>;

  //===========================================================================
  // aliases : class : buffered_reader
  //===========================================================================

  /// \brief A buffered_reader of a reader that it does not own
  ///
  /// \note The underlying reader must outlive the indirect_buffered_reader
  using indirect_buffered_reader = buffered_reader<indirect_reader>;

} // namespace alloy::core

//=============================================================================
// inline definitions : class : detail::buffered_reader_core
//=============================================================================

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

ALLOY_FORCE_INLINE
auto alloy::core::detail::buffered_reader_core::capacity()
  const noexcept -> std::size_t
{
  return m_capacity;
}

ALLOY_FORCE_INLINE
auto alloy::core::detail::buffered_reader_core::buffer()
  const noexcept -> std::span<const std::byte>
{
  return std::span<const std::byte>{
    m_buffer + m_position,
    m_filled - m_position
  };
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

inline
auto alloy::core::detail::buffered_reader_core::fill_buffer(reader& source)
  noexcept -> result<std::span<const std::byte>,std::error_code>
{
  if (m_position == m_filled) {
    const auto refilled = refill(source);
    if (ALLOY_UNLIKELY(refilled.has_error())) {
      return fail(refilled.error());
    }
  }
  return buffer();
}

ALLOY_FORCE_INLINE
auto alloy::core::detail::buffered_reader_core::consume(std::size_t n)
  noexcept -> void
{
  ALLOY_ASSERT(n <= m_filled - m_position, "Cannot consume unbuffered bytes");

  m_position += n;
}

ALLOY_FORCE_INLINE
auto alloy::core::detail::buffered_reader_core::discard_buffer()
  noexcept -> void
{
  m_position = 0u;
  m_filled = 0u;
}

//=============================================================================
// inline definitions : class : buffered_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

template <alloy::core::detail::reader_object Reader>
template <typename U>
  requires (
    !std::same_as<
      std::remove_cvref_t<U>,
      alloy::core::buffered_reader<Reader>
    > &&
    !std::same_as<std::remove_cvref_t<U>, std::in_place_t> &&
    !std::same_as<std::remove_cvref_t<U>, std::allocator_arg_t> &&
    std::is_constructible_v<Reader, U>
  )
inline
alloy::core::buffered_reader<Reader>::buffered_reader(
  U&& source,
  std::size_t capacity,
  allocator alloc
) noexcept(std::is_nothrow_constructible_v<Reader, U>)
  : m_source(std::forward<U>(source)),
    m_core{capacity, alloc}
{

}

template <alloy::core::detail::reader_object Reader>
template <typename...Args>
  requires (std::is_constructible_v<Reader, Args...>)
inline
alloy::core::buffered_reader<Reader>::buffered_reader(
  std::in_place_t,
  Args&&...args
) noexcept(std::is_nothrow_constructible_v<Reader, Args...>)
  : m_source(std::forward<Args>(args)...),
    m_core{default_capacity, allocator{}}
{

}

template <alloy::core::detail::reader_object Reader>
template <typename...Args>
  requires (std::is_constructible_v<Reader, Args...>)
inline
alloy::core::buffered_reader<Reader>::buffered_reader(
  std::allocator_arg_t,
  allocator alloc,
  std::size_t capacity,
  std::in_place_t,
  Args&&...args
) noexcept(std::is_nothrow_constructible_v<Reader, Args...>)
  : m_source(std::forward<Args>(args)...),
    m_core{capacity, alloc}
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::capacity()
  const noexcept -> std::size_t
{
  return m_core.capacity();
}

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::buffer()
  const noexcept -> std::span<const std::byte>
{
  return m_core.buffer();
}

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::source() noexcept -> Reader&
{
  return m_source;
}

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::source()
  const noexcept -> const Reader&
{
  return m_source;
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::fill_buffer()
  noexcept -> result<std::span<const std::byte>,std::error_code>
{
  return m_core.fill_buffer(m_source);
}

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::consume(std::size_t n)
  noexcept -> void
{
  m_core.consume(n);
}

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::discard_buffer() noexcept -> void
{
  m_core.discard_buffer();
}

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::read_until(
  std::byte delimiter,
  not_null<vector<std::byte>*> out
) -> result<std::size_t,std::error_code>
{
  return m_core.read_until(m_source, delimiter, out);
}

template <alloy::core::detail::reader_object Reader>
ALLOY_FORCE_INLINE
auto alloy::core::buffered_reader<Reader>::skip_until(std::byte delimiter)
  noexcept -> result<std::size_t,std::error_code>
{
  return m_core.skip_until(m_source, delimiter);
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Reading
//-----------------------------------------------------------------------------

template <alloy::core::detail::reader_object Reader>
inline
auto alloy::core::buffered_reader<Reader>::do_read(std::span<std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  return m_core.read(m_source, buffer);
}

#endif /* ALLOY_CORE_IO_BUFFERED_READER_HPP */
