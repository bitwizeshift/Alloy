// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/indirect_writer.hpp>

#include "test_doubles.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc

namespace alloy::core::test {
namespace {

//=============================================================================
// class : indirect_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("indirect_writer::indirect_writer(not_null<writer*>)", "[ctor]")
{
  // Arrange
  auto storage = std::array<std::byte,8>{};
  auto destination = fake_writer{storage};

  // Act
  const auto sut = indirect_writer{&destination};

  // Assert
  SECTION("Refers to the destination")
  {
    REQUIRE( &sut.destination() == &destination );
  }
  SECTION("Does not write to the destination yet")
  {
    REQUIRE( destination.write_count() == 0u );
  }
}

TEST_CASE("indirect_writer::indirect_writer(const indirect_writer&)",
          "[ctor]")
{
  // Arrange
  auto storage = std::array<std::byte,8>{};
  auto destination = fake_writer{storage};
  const auto other = indirect_writer{&destination};

  // Act
  const auto sut = other;

  // Assert
  SECTION("Refers to the same destination")
  {
    REQUIRE( &sut.destination() == &other.destination() );
  }
}

//-----------------------------------------------------------------------------
// Writing
//-----------------------------------------------------------------------------

TEST_CASE("indirect_writer::write(std::span<const std::byte>)", "[writing]")
{
  const auto input = sequence_bytes(8u);

  SECTION("Destination accepts bytes")
  {
    // Arrange
    auto storage = std::array<std::byte,8>{};
    auto destination = fake_writer{storage};
    destination.set_chunk_size(3u);
    auto sut = indirect_writer{&destination};

    // Act
    const auto result = sut.write(input);

    // Assert
    SECTION("Writes to the destination once")
    {
      REQUIRE( destination.write_count() == 1u );
    }
    SECTION("Gives every byte to the destination")
    {
      REQUIRE( destination.last_request_size() == input.size() );
    }
    SECTION("Returns the result of the destination")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == 3u );
    }
    SECTION("Writes the bytes to the destination")
    {
      const auto written = destination.written();

      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(),
                   input.begin() + 3)
      );
    }
  }

  SECTION("Destination fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto destination = fake_writer{std::span<std::byte>{}};
    destination.fail_when_full(expected);
    auto sut = indirect_writer{&destination};

    // Act
    const auto result = sut.write(input);

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }
}

} // namespace
} // namespace alloy::core::test
