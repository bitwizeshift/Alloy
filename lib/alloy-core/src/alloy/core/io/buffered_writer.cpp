// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/io/buffered_writer.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/io/error.hpp"

#include <algorithm> // std::copy, std::copy_n
#include <utility>   // std::exchange

//=============================================================================
// class : buffered_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

auto alloy::core::buffered_writer::from_writer(
  not_null<writer*> destination,
  allocator alloc
) noexcept -> result<buffered_writer,std::error_code>
{
  return with_capacity(default_capacity, destination, alloc);
}

auto alloy::core::buffered_writer::with_capacity(
  std::size_t capacity,
  not_null<writer*> destination,
  allocator alloc
) noexcept -> result<buffered_writer,std::error_code>
{
  ALLOY_ASSERT(capacity != 0u, "A buffered_writer requires a buffer");

  auto* const buffer = static_cast<std::byte*>(alloc.allocate_bytes(capacity));
  if (ALLOY_UNLIKELY(buffer == nullptr)) {
    return fail(std::make_error_code(std::errc::not_enough_memory));
  }
  return buffered_writer{destination, alloc, buffer, capacity};
}

alloy::core::buffered_writer::buffered_writer(buffered_writer&& other)
  noexcept
  : m_destination{other.m_destination},
    m_allocator{other.m_allocator},
    m_buffer{std::exchange(other.m_buffer, nullptr)},
    m_capacity{std::exchange(other.m_capacity, 0u)},
    m_size{std::exchange(other.m_size, 0u)}
{

}

alloy::core::buffered_writer::buffered_writer(
  not_null<writer*> destination,
  allocator alloc,
  std::byte* buffer,
  std::size_t capacity
) noexcept
  : m_destination{destination},
    m_allocator{alloc},
    m_buffer{buffer},
    m_capacity{capacity},
    m_size{0u}
{

}

//-----------------------------------------------------------------------------

alloy::core::buffered_writer::~buffered_writer() noexcept
{
  const auto flush_result = flush();

  // There is no way to report a failure from the destructor, so we ignore it
  // but attempt to flush any remaining bytes
  compiler::unused(flush_result);

  if (m_buffer != nullptr) {
    m_allocator.deallocate_bytes(m_buffer, m_capacity);
  }
}

//-----------------------------------------------------------------------------

auto alloy::core::buffered_writer::operator=(buffered_writer&& other)
  noexcept -> buffered_writer&
{
  if (m_buffer != nullptr) {
    const auto flush_result = flush();
    compiler::unused(flush_result);
    m_allocator.deallocate_bytes(m_buffer, m_capacity);
  }
  m_destination = other.m_destination;
  m_allocator = other.m_allocator;
  m_buffer = std::exchange(other.m_buffer, nullptr);
  m_capacity = std::exchange(other.m_capacity, 0u);
  m_size = std::exchange(other.m_size, 0u);
  return (*this);
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

auto alloy::core::buffered_writer::flush()
  noexcept -> result<void,std::error_code>
{
  auto written = std::size_t{0};

  // Keeps the bytes that were not written at the front of the buffer, so
  // that a later flush continues from them.
  const auto keep_unwritten = [this, &written]() noexcept {
    std::copy(m_buffer + written, m_buffer + m_size, m_buffer);
    m_size -= written;
  };

  while (written < m_size) {
    const auto bytes_written = m_destination->write(
      std::span<const std::byte>{m_buffer + written, m_size - written}
    );
    if (ALLOY_UNLIKELY(bytes_written.has_error())) {
      keep_unwritten();
      return fail(bytes_written.error());
    }
    if (ALLOY_UNLIKELY(*bytes_written == 0u)) {
      keep_unwritten();
      return fail(io_error::write_zero);
    }
    written += *bytes_written;
  }
  m_size = 0u;
  return {};
}

//-----------------------------------------------------------------------------
// Private Writing
//-----------------------------------------------------------------------------

ALLOY_COLD
auto alloy::core::buffered_writer::write_unbuffered(
  std::span<const std::byte> buffer
) noexcept -> result<std::size_t,std::error_code>
{
  if (m_size != 0u) {
    const auto flushed = flush();
    if (ALLOY_UNLIKELY(flushed.has_error())) {
      return fail(flushed.error());
    }
  }
  if (buffer.size() >= m_capacity) {
    return m_destination->write(buffer);
  }
  std::copy_n(buffer.data(), buffer.size(), m_buffer);
  m_size = buffer.size();
  return buffer.size();
}

//-----------------------------------------------------------------------------
// Virtual Hooks : Writing
//-----------------------------------------------------------------------------

auto alloy::core::buffered_writer::do_write(std::span<const std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  // Only writes that fit in the remaining space are buffered here, which
  // leaves a single comparison on the path taken by most writes.
  if (ALLOY_LIKELY(buffer.size() < m_capacity - m_size)) {
    std::copy_n(buffer.data(), buffer.size(), m_buffer + m_size);
    m_size += buffer.size();
    return buffer.size();
  }
  return write_unbuffered(buffer);
}
