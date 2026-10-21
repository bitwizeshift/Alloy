// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#ifndef ALLOY_CORE_IO_TEST_DOUBLES_HPP
#define ALLOY_CORE_IO_TEST_DOUBLES_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <alloy/core/io/reader.hpp>
#include <alloy/core/io/writer.hpp>

#include <algorithm>    // std::min, std::copy_n
#include <cstddef>      // std::byte, std::size_t
#include <limits>       // std::numeric_limits
#include <span>         // std::span
#include <system_error> // std::error_code
#include <vector>       // std::vector

namespace alloy::core::test {

  //===========================================================================
  // class : fake_reader
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A reader over a fixed sequence of bytes that records how it is
  ///        read from
  ///
  /// Each read produces at most the number of bytes given to
  /// `set_chunk_size`. Once every byte has been read, reads produce the end of
  /// the stream, or fail with the error given to `fail_at_end` if one was set.
  /////////////////////////////////////////////////////////////////////////////
  class fake_reader final : public reader
  {
  public:

    explicit fake_reader(std::span<const std::byte> bytes) noexcept;

    auto set_chunk_size(std::size_t n) noexcept -> void;
    auto fail_at_end(std::error_code ec) noexcept -> void;

    auto read_count() const noexcept -> std::size_t;
    auto last_request_size() const noexcept -> std::size_t;
    auto remaining() const noexcept -> std::size_t;

  private:

    auto do_read(std::span<std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    std::span<const std::byte> m_bytes;
    std::size_t m_chunk_size = std::numeric_limits<std::size_t>::max();
    std::error_code m_error = {};
    std::size_t m_read_count = 0u;
    std::size_t m_last_request_size = 0u;
  };

  //===========================================================================
  // class : fake_writer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A writer into a fixed region of bytes that records how it is
  ///        written to
  ///
  /// Each write consumes at most the number of bytes given to
  /// `set_chunk_size`. Once the region is full, writes consume no bytes, or
  /// fail with the error given to `fail_when_full` if one was set.
  /////////////////////////////////////////////////////////////////////////////
  class fake_writer final : public writer
  {
  public:

    explicit fake_writer(std::span<std::byte> storage) noexcept;

    auto set_chunk_size(std::size_t n) noexcept -> void;
    auto fail_when_full(std::error_code ec) noexcept -> void;

    auto written() const noexcept -> std::span<const std::byte>;
    auto write_count() const noexcept -> std::size_t;
    auto last_request_size() const noexcept -> std::size_t;

  private:

    auto do_write(std::span<const std::byte> buffer)
      noexcept -> result<std::size_t,std::error_code> override;

    std::span<std::byte> m_storage;
    std::size_t m_size = 0u;
    std::size_t m_chunk_size = std::numeric_limits<std::size_t>::max();
    std::error_code m_error = {};
    std::size_t m_write_count = 0u;
    std::size_t m_last_request_size = 0u;
  };

  //===========================================================================
  // non-member functions
  //===========================================================================

  /// \brief Makes \p n bytes whose values count up from `0`, wrapping at 256
  ///
  /// \param n the number of bytes to make
  /// \return the bytes
  auto sequence_bytes(std::size_t n) -> std::vector<std::byte>;

} // namespace alloy::core::test

//=============================================================================
// inline definitions : class : fake_reader
//=============================================================================

inline
alloy::core::test::fake_reader::fake_reader(
  std::span<const std::byte> bytes
) noexcept
  : m_bytes{bytes}
{

}

inline
auto alloy::core::test::fake_reader::set_chunk_size(std::size_t n)
  noexcept -> void
{
  m_chunk_size = n;
}

inline
auto alloy::core::test::fake_reader::fail_at_end(std::error_code ec)
  noexcept -> void
{
  m_error = ec;
}

inline
auto alloy::core::test::fake_reader::read_count()
  const noexcept -> std::size_t
{
  return m_read_count;
}

inline
auto alloy::core::test::fake_reader::last_request_size()
  const noexcept -> std::size_t
{
  return m_last_request_size;
}

inline
auto alloy::core::test::fake_reader::remaining()
  const noexcept -> std::size_t
{
  return m_bytes.size();
}

inline
auto alloy::core::test::fake_reader::do_read(std::span<std::byte> buffer)
  noexcept -> result<std::size_t,std::error_code>
{
  ++m_read_count;
  m_last_request_size = buffer.size();

  if (m_bytes.empty() && m_error) {
    return fail(m_error);
  }

  const auto count = std::min({buffer.size(), m_bytes.size(), m_chunk_size});
  std::copy_n(m_bytes.data(), count, buffer.data());
  m_bytes = m_bytes.subspan(count);

  return count;
}

//=============================================================================
// inline definitions : class : fake_writer
//=============================================================================

inline
alloy::core::test::fake_writer::fake_writer(std::span<std::byte> storage)
  noexcept
  : m_storage{storage}
{

}

inline
auto alloy::core::test::fake_writer::set_chunk_size(std::size_t n)
  noexcept -> void
{
  m_chunk_size = n;
}

inline
auto alloy::core::test::fake_writer::fail_when_full(std::error_code ec)
  noexcept -> void
{
  m_error = ec;
}

inline
auto alloy::core::test::fake_writer::written()
  const noexcept -> std::span<const std::byte>
{
  return m_storage.first(m_size);
}

inline
auto alloy::core::test::fake_writer::write_count()
  const noexcept -> std::size_t
{
  return m_write_count;
}

inline
auto alloy::core::test::fake_writer::last_request_size()
  const noexcept -> std::size_t
{
  return m_last_request_size;
}

inline
auto alloy::core::test::fake_writer::do_write(
  std::span<const std::byte> buffer
) noexcept -> result<std::size_t,std::error_code>
{
  ++m_write_count;
  m_last_request_size = buffer.size();

  const auto available = m_storage.size() - m_size;
  if (available == 0u && m_error) {
    return fail(m_error);
  }

  const auto count = std::min({buffer.size(), available, m_chunk_size});
  std::copy_n(buffer.data(), count, m_storage.data() + m_size);
  m_size += count;

  return count;
}

//=============================================================================
// inline definitions : non-member functions
//=============================================================================

inline
auto alloy::core::test::sequence_bytes(std::size_t n)
  -> std::vector<std::byte>
{
  auto bytes = std::vector<std::byte>{};
  bytes.reserve(n);
  for (auto i = std::size_t{0}; i < n; ++i) {
    bytes.push_back(static_cast<std::byte>(i));
  }
  return bytes;
}

#endif /* ALLOY_CORE_IO_TEST_DOUBLES_HPP */
