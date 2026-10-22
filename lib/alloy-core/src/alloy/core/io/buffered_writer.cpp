// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/io/buffered_writer.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/error.hpp"
#include "alloy/core/io/writer.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/result.hpp"

#include <algorithm> // std::copy, std::copy_n
#include <cstddef>
#include <span>
#include <system_error>
#include <utility>   // std::exchange

//=============================================================================
// class : detail::buffered_writer_core
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

alloy::core::detail::buffered_writer_core::buffered_writer_core(
  std::size_t capacity,
  allocator alloc
) noexcept
  : m_allocator{alloc},
    m_buffer{nullptr},
    m_capacity{capacity},
    m_size{0u}
{
  ALLOY_ASSERT(capacity != 0u, "A buffered_writer requires a buffer");
}

alloy::core::detail::buffered_writer_core::buffered_writer_core(
  buffered_writer_core&& other
) noexcept
  : m_allocator{other.m_allocator},
    m_buffer{std::exchange(other.m_buffer, nullptr)},
    m_capacity{std::exchange(other.m_capacity, 0u)},
    m_size{std::exchange(other.m_size, 0u)}
{

}

//-----------------------------------------------------------------------------

alloy::core::detail::buffered_writer_core::~buffered_writer_core() noexcept
{
  release();
}

//-----------------------------------------------------------------------------

auto alloy::core::detail::buffered_writer_core::operator=(
  buffered_writer_core&& other
) noexcept -> buffered_writer_core&
{
  release();
  m_allocator = other.m_allocator;
  m_buffer = std::exchange(other.m_buffer, nullptr);
  m_capacity = std::exchange(other.m_capacity, 0u);
  m_size = std::exchange(other.m_size, 0u);
  return (*this);
}

//-----------------------------------------------------------------------------
// Buffering
//-----------------------------------------------------------------------------

auto alloy::core::detail::buffered_writer_core::flush(writer& destination)
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
    const auto bytes_written = destination.write(
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
// Writing
//-----------------------------------------------------------------------------

auto alloy::core::detail::buffered_writer_core::write(
  writer& destination,
  std::span<const std::byte> buffer
) noexcept -> result<std::size_t,std::error_code>
{
  // Only writes that fit in the remaining space of an allocated buffer are
  // buffered here, which keeps the path taken by most writes short.
  const auto fits = buffer.size() < m_capacity - m_size;
  if (ALLOY_LIKELY(fits && m_buffer != nullptr)) {
    std::copy_n(buffer.data(), buffer.size(), m_buffer + m_size);
    m_size += buffer.size();
    return buffer.size();
  }
  return write_unbuffered(destination, buffer);
}

//-----------------------------------------------------------------------------
// Private Writing
//-----------------------------------------------------------------------------

ALLOY_COLD
auto alloy::core::detail::buffered_writer_core::write_unbuffered(
  writer& destination,
  std::span<const std::byte> buffer
) noexcept -> result<std::size_t,std::error_code>
{
  if (m_size != 0u) {
    const auto flushed = flush(destination);
    if (ALLOY_UNLIKELY(flushed.has_error())) {
      return fail(flushed.error());
    }
  }
  if (buffer.size() >= m_capacity) {
    return destination.write(buffer);
  }
  if (ALLOY_UNLIKELY(m_buffer == nullptr)) {
    m_buffer = static_cast<std::byte*>(m_allocator.allocate_bytes(m_capacity));
    if (ALLOY_UNLIKELY(m_buffer == nullptr)) {
      return fail(std::make_error_code(std::errc::not_enough_memory));
    }
  }
  std::copy_n(buffer.data(), buffer.size(), m_buffer);
  m_size = buffer.size();
  return buffer.size();
}

auto alloy::core::detail::buffered_writer_core::release() noexcept -> void
{
  if (m_buffer != nullptr) {
    m_allocator.deallocate_bytes(m_buffer, m_capacity);
  }
}
