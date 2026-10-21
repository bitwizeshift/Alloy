// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include "test_doubles.hpp"

#include <alloy/core/io/reader.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <system_error> // std::error_code, std::errc

namespace alloy::core::test {
namespace {

//=============================================================================
// class : reader
//=============================================================================

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

TEST_CASE("reader::read(std::span<std::byte>)", "[reading]")
{
  const auto input = sequence_bytes(8u);
  auto storage = std::array<std::byte,4>{};

  SECTION("Underlying reader has bytes")
  {
    // Arrange
    auto fake = fake_reader{input};
    auto& sut = static_cast<reader&>(fake);

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Passes the whole buffer to the underlying reader")
    {
      REQUIRE( fake.last_request_size() == storage.size() );
    }
    SECTION("Returns the number of bytes read")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == storage.size() );
    }
    SECTION("Writes the bytes into the buffer")
    {
      REQUIRE( std::equal(storage.begin(), storage.end(), input.begin()) );
    }
  }

  SECTION("Underlying reader fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto fake = fake_reader{std::span<const std::byte>{}};
    fake.fail_at_end(expected);
    auto& sut = static_cast<reader&>(fake);

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Returns the error of the underlying reader")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }
}

} // namespace
} // namespace alloy::core::test
