// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include "test_doubles.hpp"

#include <alloy/core/io/writer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc

namespace alloy::core::test {
namespace {

//=============================================================================
// class : writer
//=============================================================================

//-----------------------------------------------------------------------------
// Writing
//-----------------------------------------------------------------------------

TEST_CASE("writer::write(std::span<const std::byte>)", "[writing]")
{
  const auto input = sequence_bytes(4u);
  auto storage = std::array<std::byte,8>{};

  SECTION("Underlying writer has space")
  {
    // Arrange
    auto fake = fake_writer{storage};
    auto& sut = static_cast<writer&>(fake);

    // Act
    const auto result = sut.write(input);

    // Assert
    SECTION("Passes the whole buffer to the underlying writer")
    {
      REQUIRE( fake.last_request_size() == input.size() );
    }
    SECTION("Returns the number of bytes written")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == input.size() );
    }
    SECTION("Writes the bytes of the buffer")
    {
      const auto written = fake.written();

      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
  }

  SECTION("Underlying writer fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto fake = fake_writer{std::span<std::byte>{}};
    fake.fail_when_full(expected);
    auto& sut = static_cast<writer&>(fake);

    // Act
    const auto result = sut.write(input);

    // Assert
    SECTION("Returns the error of the underlying writer")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }
}

} // namespace
} // namespace alloy::core::test
