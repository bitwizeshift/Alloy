// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/span_reader.hpp>

#include "test_doubles.hpp"
#include <alloy/core/intrinsics.hpp>
#include <alloy/core/io/reader.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm> // std::equal
#include <array>     // std::array
#include <cstddef>   // std::byte, std::size_t
#include <span>      // std::span

namespace alloy::core::test {
namespace {

//=============================================================================
// class : span_reader
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("span_reader::span_reader(std::span<const std::byte>)", "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(8u);

  // Act
  const auto sut = span_reader{input};

  // Assert
  SECTION("Has every byte remaining")
  {
    SECTION("Views the original span without copying its bytes") {
      REQUIRE( sut.remaining().data() == input.data() );
    }
    SECTION("Exposes the full input extent") {
      REQUIRE( sut.remaining().size() == input.size() );
    }
  }
}

//-----------------------------------------------------------------------------
// Reading
//-----------------------------------------------------------------------------

TEST_CASE("span_reader::read(std::span<std::byte>)", "[read]")
{
  // Arrange
  const auto input = sequence_bytes(8u);

  SECTION("Buffer is smaller than the remaining bytes")
  {
    // Arrange
    const auto request = GENERATE(std::size_t{1}, std::size_t{3});
    auto sut = span_reader{input};
    auto output = std::array<std::byte,8>{};
    const auto buffer = std::span{output}.first(request);

    // Act
    const auto bytes_read = sut.read(buffer);

    // Assert
    SECTION("Reads enough bytes to fill the buffer")
    {
      REQUIRE( bytes_read == request );
    }
    SECTION("Reads the leading bytes")
    {
      REQUIRE( std::equal(buffer.begin(), buffer.end(), input.begin()) );
    }
    SECTION("Leaves the bytes after them remaining")
    {
      SECTION("Advances past the consumed prefix") {
        REQUIRE( sut.remaining().data() == input.data() + request );
      }
      SECTION("Exposes only the unread extent") {
        REQUIRE( sut.remaining().size() == input.size() - request );
      }
    }
  }

  SECTION("Buffer is at least as large as the remaining bytes")
  {
    // Arrange
    auto sut = span_reader{input};
    auto output = std::array<std::byte,12>{};

    // Act
    const auto bytes_read = sut.read(output);

    // Assert
    SECTION("Reads every remaining byte")
    {
      REQUIRE( bytes_read.has_value() );
      SECTION("Reports the full input length") {
        REQUIRE( *bytes_read == input.size() );
      }
      SECTION("Preserves the input byte order") {
        REQUIRE( std::equal(input.begin(), input.end(), output.begin()) );
      }
    }
    SECTION("Leaves no bytes remaining")
    {
      REQUIRE( sut.remaining().empty() );
    }
    SECTION("Does not write past the bytes read")
    {
      const auto tail = std::span{output}.subspan(input.size());
      REQUIRE( std::ranges::all_of(tail, [](std::byte b) {
        return b == std::byte{0};
      }) );
    }
  }

  SECTION("Reading continues from where the last read stopped")
  {
    // Arrange
    auto sut = span_reader{input};
    auto output = std::array<std::byte,8>{};
    const auto first = std::span{output}.first(3u);
    const auto second = std::span{output}.subspan(3u);
    compiler::unused(sut.read(first));

    // Act
    const auto bytes_read = sut.read(second);

    // Assert
    SECTION("Reads the following bytes")
    {
      REQUIRE( bytes_read.has_value() );
      SECTION("Reports the bytes following the previous read") {
        REQUIRE( *bytes_read == second.size() );
      }
      SECTION("Preserves the input byte order") {
        REQUIRE( std::equal(input.begin(), input.end(), output.begin()) );
      }
    }
  }

  SECTION("Every byte has been read")
  {
    // Arrange
    auto sut = span_reader{input};
    auto output = std::array<std::byte,8>{};
    compiler::unused(sut.read(output));

    // Act
    const auto bytes_read = sut.read(output);

    // Assert
    SECTION("Signals the end of the stream")
    {
      REQUIRE( bytes_read == 0u );
    }
  }

  SECTION("Span is empty")
  {
    // Arrange
    auto sut = span_reader{std::span<const std::byte>{}};
    auto output = std::array<std::byte,8>{};

    // Act
    const auto bytes_read = sut.read(output);

    // Assert
    SECTION("Signals the end of the stream")
    {
      REQUIRE( bytes_read == 0u );
    }
  }

  SECTION("Buffer is empty")
  {
    // Arrange
    auto sut = span_reader{input};

    // Act
    const auto bytes_read = sut.read(std::span<std::byte>{});

    // Assert
    SECTION("Reads no bytes")
    {
      REQUIRE( bytes_read == 0u );
    }
    SECTION("Leaves every byte remaining")
    {
      REQUIRE( sut.remaining().size() == input.size() );
    }
  }
}

//=============================================================================
// non-member functions : class : reader
//=============================================================================

TEST_CASE("read_all(reader&) with a span_reader", "[read]")
{
  // Arrange
  const auto input = sequence_bytes(1000u);
  auto sut = span_reader{input};

  // Act
  const auto bytes = read_all(sut);

  // Assert
  SECTION("Reads every byte of the span")
  {
    REQUIRE( bytes.has_value() );
    SECTION("Returns all input bytes in order") {
      REQUIRE( std::ranges::equal(*bytes, input) );
    }
  }
}

} // namespace
} // namespace alloy::core::test
