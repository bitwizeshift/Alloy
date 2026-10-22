// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include "test_doubles.hpp"

#include <alloy/core/io/indirect_reader.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc

namespace alloy::core::test {
namespace {

//=============================================================================
// class : indirect_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("indirect_reader::indirect_reader(not_null<reader*>)", "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(8u);
  auto source = fake_reader{input};

  // Act
  const auto sut = indirect_reader{&source};

  // Assert
  SECTION("Refers to the source")
  {
    REQUIRE( &sut.source() == &source );
  }
  SECTION("Does not read from the source yet")
  {
    REQUIRE( source.read_count() == 0u );
  }
}

TEST_CASE("indirect_reader::indirect_reader(const indirect_reader&)",
          "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(8u);
  auto source = fake_reader{input};
  const auto other = indirect_reader{&source};

  // Act
  const auto sut = other;

  // Assert
  SECTION("Refers to the same source")
  {
    REQUIRE( &sut.source() == &other.source() );
  }
}

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

TEST_CASE("indirect_reader::read(std::span<std::byte>)", "[reading]")
{
  const auto input = sequence_bytes(8u);

  SECTION("Source has bytes")
  {
    // Arrange
    auto source = fake_reader{input};
    source.set_chunk_size(3u);
    auto sut = indirect_reader{&source};
    auto storage = std::array<std::byte,8>{};

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Reads from the source once")
    {
      REQUIRE( source.read_count() == 1u );
    }
    SECTION("Requests the whole buffer from the source")
    {
      REQUIRE( source.last_request_size() == storage.size() );
    }
    SECTION("Returns the result of the source")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == 3u );
    }
    SECTION("Writes the bytes of the source")
    {
      REQUIRE(
        std::equal(storage.begin(), storage.begin() + 3, input.begin())
      );
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span<const std::byte>{}};
    source.fail_at_end(expected);
    auto sut = indirect_reader{&source};
    auto storage = std::array<std::byte,8>{};

    // Act
    const auto result = sut.read(storage);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }
}

} // namespace
} // namespace alloy::core::test
