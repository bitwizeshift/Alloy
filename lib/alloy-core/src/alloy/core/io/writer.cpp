// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/io/writer.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/io/error.hpp"

#include <algorithm> // std::min
#include <array>     // std::array
#include <limits>    // std::numeric_limits

namespace {

  /// \brief Copies bytes from \p source to \p destination until either
  ///        \p source ends or \p limit bytes have been copied
  ///
  /// \param source the reader to copy from
  /// \param destination the writer to copy to
  /// \param limit the maximum number of bytes to copy
  /// \return the number of bytes copied on success
  auto copy_up_to(
    alloy::core::reader& source,
    alloy::core::writer& destination,
    std::size_t limit
  ) noexcept -> alloy::core::result<std::size_t,std::error_code>
  {
    static constexpr auto buffer_size = std::size_t{8192};

    auto buffer = std::array<std::byte,buffer_size>{};
    auto total = std::size_t{0};
    while (total < limit) {
      const auto request = std::min(buffer_size, limit - total);
      const auto bytes_read = source.read(std::span{buffer}.first(request));
      if (ALLOY_UNLIKELY(bytes_read.has_error())) {
        return alloy::core::fail(bytes_read.error());
      }
      if (*bytes_read == 0u) {
        break;
      }
      const auto bytes = std::span<const std::byte>{buffer}.first(*bytes_read);
      const auto written = alloy::core::write_all(destination, bytes);
      if (ALLOY_UNLIKELY(written.has_error())) {
        return alloy::core::fail(written.error());
      }
      total += *bytes_read;
    }
    return total;
  }

} // namespace <anonymous>

//=============================================================================
// non-member functions : class : writer
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

auto alloy::core::write_all(
  writer& destination,
  std::span<const std::byte> buffer
) noexcept -> result<void,std::error_code>
{
  while (!buffer.empty()) {
    const auto bytes_written = destination.write(buffer);
    if (ALLOY_UNLIKELY(bytes_written.has_error())) {
      return fail(bytes_written.error());
    }
    if (ALLOY_UNLIKELY(*bytes_written == 0u)) {
      return fail(io_error::write_zero);
    }
    buffer = buffer.subspan(*bytes_written);
  }
  return {};
}

auto alloy::core::copy(reader& source, writer& destination)
  noexcept -> result<std::size_t,std::error_code>
{
  return ::copy_up_to(
    source,
    destination,
    std::numeric_limits<std::size_t>::max()
  );
}

auto alloy::core::copy_n(reader& source, writer& destination, std::size_t n)
  noexcept -> result<void,std::error_code>
{
  const auto copied = ::copy_up_to(source, destination, n);
  if (ALLOY_UNLIKELY(copied.has_error())) {
    return fail(copied.error());
  }
  if (ALLOY_UNLIKELY(*copied != n)) {
    return fail(io_error::unexpected_eof);
  }
  return {};
}
