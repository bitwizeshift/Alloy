// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/io/buffered_reader.hpp"

#include <algorithm> // std::min, std::copy_n
#include <cstring>   // std::memchr
#include <utility>   // std::exchange

namespace {

  /// \brief Consumes bytes of \p reader up to and including \p delimiter,
  ///        passing each consumed run of buffered bytes to \p fn
  ///
  /// \param reader the reader to consume from
  /// \param delimiter the byte to consume up to
  /// \param fn the function to receive each run of consumed bytes
  /// \return the number of bytes consumed on success
  template <typename Fn>
  auto consume_until(
    alloy::core::buffered_reader& reader,
    std::byte delimiter,
    const Fn& fn
  ) -> alloy::core::result<std::size_t,std::error_code>
  {
    auto total = std::size_t{0};
    while (true) {
      const auto available = reader.fill_buffer();
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
      reader.consume(count);
      total += count;

      if (match != nullptr) {
        return total;
      }
    }
  }

} // namespace <anonymous>

//=============================================================================
// class : buffered_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

auto alloy::core::buffered_reader::from_reader(
  not_null<reader*> source,
  allocator alloc
) noexcept -> result<buffered_reader,std::error_code>
{
  return with_capacity(default_capacity, source, alloc);
}

auto alloy::core::buffered_reader::with_capacity(
  std::size_t capacity,
  not_null<reader*> source,
  allocator alloc
) noexcept -> result<buffered_reader,std::error_code>
{
  ALLOY_ASSERT(capacity != 0u, "A buffered_reader requires a buffer");

  auto* const buffer = static_cast<std::byte*>(alloc.allocate_bytes(capacity));
  if (ALLOY_UNLIKELY(buffer == nullptr)) {
    return fail(std::make_error_code(std::errc::not_enough_memory));
  }
  return buffered_reader{source, alloc, buffer, capacity};
}

alloy::core::buffered_reader::buffered_reader(buffered_reader&& other)
  noexcept
  : m_source{other.m_source},
    m_allocator{other.m_allocator},
    m_buffer{std::exchange(other.m_buffer, nullptr)},
    m_capacity{std::exchange(other.m_capacity, 0u)},
    m_position{std::exchange(other.m_position, 0u)},
    m_filled{std::exchange(other.m_filled, 0u)}
{

}

alloy::core::buffered_reader::buffered_reader(
  not_null<reader*> source,
  allocator alloc,
  std::byte* buffer,
  std::size_t capacity
) noexcept
  : m_source{source},
    m_allocator{alloc},
    m_buffer{buffer},
    m_capacity{capacity},
    m_position{0u},
    m_filled{0u}
{

}

//-----------------------------------------------------------------------------

alloy::core::buffered_reader::~buffered_reader() noexcept
{
  if (m_buffer != nullptr) {
    m_allocator.deallocate_bytes(m_buffer, m_capacity);
  }
}

//-----------------------------------------------------------------------------

auto alloy::core::buffered_reader::operator=(buffered_reader&& other)
  noexcept -> buffered_reader&
{
  if (m_buffer != nullptr) {
    m_allocator.deallocate_bytes(m_buffer, m_capacity);
  }
  m_source = other.m_source;
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

auto alloy::core::buffered_reader::read_until(
  std::byte delimiter,
  not_null<vector<std::byte>*> out
) -> result<std::size_t,std::error_code>
{
  const auto append = [out](std::span<const std::byte> bytes) {
    out->insert(out->end(), bytes.begin(), bytes.end());
  };
  return ::consume_until(*this, delimiter, append);
}

auto alloy::core::buffered_reader::skip_until(std::byte delimiter)
  noexcept -> result<std::size_t,std::error_code>
{
  const auto ignore = [](std::span<const std::byte>) noexcept {};
  return ::consume_until(*this, delimiter, ignore);
}

//-----------------------------------------------------------------------------
// Private Buffering
//-----------------------------------------------------------------------------

auto alloy::core::buffered_reader::refill()
  noexcept -> result<void,std::error_code>
{
  ALLOY_ASSERT(m_position == m_filled, "Refilling discards buffered bytes");

  const auto bytes_read = m_source->read(
    std::span<std::byte>{m_buffer, m_capacity}
  );
  if (ALLOY_UNLIKELY(bytes_read.has_error())) {
    return fail(bytes_read.error());
  }
  m_position = 0u;
  m_filled = *bytes_read;
  return {};
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Reading
//-----------------------------------------------------------------------------

auto alloy::core::buffered_reader::do_read(std::span<std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  if (m_position == m_filled) {
    // Nothing is buffered, so a read that would fill the whole buffer gains
    // nothing from it and is passed straight through.
    if (buffer.size() >= m_capacity) {
      return m_source->read(buffer);
    }
    const auto refilled = refill();
    if (ALLOY_UNLIKELY(refilled.has_error())) {
      return fail(refilled.error());
    }
  }
  const auto count = std::min(buffer.size(), m_filled - m_position);
  std::copy_n(m_buffer + m_position, count, buffer.data());
  m_position += count;
  return count;
}
