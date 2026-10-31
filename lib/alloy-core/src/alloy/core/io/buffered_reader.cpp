// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/io/buffered_reader.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/reader.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <algorithm> // std::min, std::copy_n
#include <cstddef>
#include <cstring>   // std::memchr
#include <span>
#include <system_error>
#include <utility>   // std::exchange

namespace {

  /// \brief Consumes bytes of \p source through \p core up to and including
  ///        \p delimiter, passing each consumed run of buffered bytes to \p fn
  ///
  /// \param core the buffer to consume from
  /// \param source the reader that fills the buffer
  /// \param delimiter the byte to consume up to
  /// \param fn the function to receive each run of consumed bytes
  /// \return the number of bytes consumed on success
  template <typename Fn>
  auto consume_until(
    alloy::core::detail::buffered_reader_core& core,
    alloy::core::reader& source,
    std::byte delimiter,
    const Fn& fn
  ) -> alloy::core::result<std::size_t,std::error_code>
  {
    auto total = std::size_t{0};
    while (true) {
      const auto available = core.fill_buffer(source);
      if (ALLOY_UNLIKELY(available.has_error())) {
        return alloy::core::fail(available.error());
      }
      const auto bytes = *available;
      if (bytes.empty()) {
        return total;
      }
      const auto* const match = static_cast<const std::byte*>(
        std::memchr(bytes.data(), std::to_integer<int>(delimiter), bytes.size())
      );
      const auto count = (match == nullptr)
        ? bytes.size()
        : static_cast<std::size_t>(match - bytes.data()) + 1u;

      fn(bytes.first(count));
      core.consume(count);
      total += count;

      if (match != nullptr) {
        return total;
      }
    }
  }

} // namespace <anonymous>

//=============================================================================
// class : detail::buffered_reader_core
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

alloy::core::detail::buffered_reader_core::buffered_reader_core(
  std::size_t capacity,
  allocator alloc
) noexcept
  : m_allocator{alloc},
    m_buffer{nullptr},
    m_capacity{capacity},
    m_position{0u},
    m_filled{0u}
{
  ALLOY_ASSERT(capacity != 0u, "A buffered_reader requires a buffer");
}

alloy::core::detail::buffered_reader_core::buffered_reader_core(
  buffered_reader_core&& other
) noexcept
  : m_allocator{other.m_allocator},
    m_buffer{std::exchange(other.m_buffer, nullptr)},
    m_capacity{std::exchange(other.m_capacity, 0u)},
    m_position{std::exchange(other.m_position, 0u)},
    m_filled{std::exchange(other.m_filled, 0u)}
{

}

//-----------------------------------------------------------------------------

alloy::core::detail::buffered_reader_core::~buffered_reader_core() noexcept
{
  release();
}

//-----------------------------------------------------------------------------

auto alloy::core::detail::buffered_reader_core::operator=(
  buffered_reader_core&& other
) noexcept -> buffered_reader_core&
{
  release();
  m_allocator = other.m_allocator;
  m_buffer = std::exchange(other.m_buffer, nullptr);
  m_capacity = std::exchange(other.m_capacity, 0u);
  m_position = std::exchange(other.m_position, 0u);
  m_filled = std::exchange(other.m_filled, 0u);
  return (*this);
}

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

auto alloy::core::detail::buffered_reader_core::read(
  reader& source,
  std::span<std::byte> buffer
) noexcept -> result<std::size_t,std::error_code>
{
  if (m_position == m_filled) {
    // Nothing is buffered, so a read that would fill the whole buffer gains
    // nothing from it and is passed straight through.
    if (buffer.size() >= m_capacity) {
      return source.read(buffer);
    }
    const auto refilled = refill(source);
    if (ALLOY_UNLIKELY(refilled.has_error())) {
      return fail(refilled.error());
    }
  }
  const auto count = std::min(buffer.size(), m_filled - m_position);
  std::copy_n(m_buffer + m_position, count, buffer.data());
  m_position += count;
  return count;
}

auto alloy::core::detail::buffered_reader_core::read_until(
  reader& source,
  std::byte delimiter,
  not_null<vector<std::byte>*> out
) -> result<std::size_t,std::error_code>
{
  const auto append = [out](std::span<const std::byte> bytes) {
    out->insert(out->end(), bytes.begin(), bytes.end());
  };
  return ::consume_until(*this, source, delimiter, append);
}

auto alloy::core::detail::buffered_reader_core::skip_until(
  reader& source,
  std::byte delimiter
) noexcept -> result<std::size_t,std::error_code>
{
  const auto ignore = [](std::span<const std::byte>) noexcept {};
  return ::consume_until(*this, source, delimiter, ignore);
}

//-----------------------------------------------------------------------------
// Private Buffering
//-----------------------------------------------------------------------------

auto alloy::core::detail::buffered_reader_core::refill(reader& source)
  noexcept -> result<void,std::error_code>
{
  ALLOY_ASSERT(m_position == m_filled, "Refilling discards buffered bytes");

  if (ALLOY_UNLIKELY(m_buffer == nullptr)) {
    m_buffer = static_cast<std::byte*>(m_allocator.allocate_bytes(m_capacity));
    if (ALLOY_UNLIKELY(m_buffer == nullptr)) {
      return fail(std::make_error_code(std::errc::not_enough_memory));
    }
  }
  const auto bytes_read = source.read(
    std::span<std::byte>{m_buffer, m_capacity}
  );
  if (ALLOY_UNLIKELY(bytes_read.has_error())) {
    return fail(bytes_read.error());
  }
  m_position = 0u;
  m_filled = *bytes_read;
  return {};
}

auto alloy::core::detail::buffered_reader_core::release() noexcept -> void
{
  if (m_buffer != nullptr) {
    m_allocator.deallocate_bytes(m_buffer, m_capacity);
  }
}
