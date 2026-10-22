// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/core/io/reader.hpp"

#include "alloy/core/io/error.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/result.hpp"

#include <algorithm> // std::max
#include <cstddef>
#include <span>
#include <system_error>

//=============================================================================
// non-member functions : class : reader
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

auto alloy::core::read_at_least(
  reader& source,
  std::span<std::byte> buffer,
  std::size_t min
) noexcept -> result<std::size_t,std::error_code>
{
  ALLOY_ASSERT(min <= buffer.size(), "Cannot read more than buffer holds");

  auto total = std::size_t{0};
  while (total < min) {
    const auto bytes_read = source.read(buffer.subspan(total));
    if (ALLOY_UNLIKELY(bytes_read.has_error())) {
      return fail(bytes_read.error());
    }
    if (ALLOY_UNLIKELY(*bytes_read == 0u)) {
      return fail(io_error::unexpected_eof);
    }
    total += *bytes_read;
  }
  return total;
}

auto alloy::core::read_all_into(
  reader& source,
  not_null<vector<std::byte>*> out
) -> result<std::size_t,std::error_code>
{
  static constexpr auto min_read_size = std::size_t{512};

  const auto start = out->size();
  while (true) {
    const auto filled = out->size();

    // Reads go directly into the vector's storage, using all of the spare
    // capacity before the vector is grown again.
    out->resize(std::max(out->capacity(), filled + min_read_size));

    const auto bytes_read = source.read(
      std::span<std::byte>{out->data() + filled, out->size() - filled}
    );
    if (ALLOY_UNLIKELY(bytes_read.has_error())) {
      out->resize(filled);
      return fail(bytes_read.error());
    }
    out->resize(filled + *bytes_read);
    if (*bytes_read == 0u) {
      return out->size() - start;
    }
  }
}

auto alloy::core::read_all(reader& source)
  -> result<vector<std::byte>,std::error_code>
{
  auto out = vector<std::byte>{};
  const auto bytes_read = read_all_into(source, &out);
  if (ALLOY_UNLIKELY(bytes_read.has_error())) {
    return fail(bytes_read.error());
  }
  return out;
}
