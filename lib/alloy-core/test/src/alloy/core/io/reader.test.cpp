// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/reader.hpp>

#include "test_doubles.hpp"
#include <alloy/core/containers/vector.hpp>
#include <alloy/core/io/error.hpp>
#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <span>
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
  // Arrange
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
      REQUIRE( result == storage.size() );
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
      REQUIRE( result == fail(expected) );
    }
  }
}

//=============================================================================
// non-member functions : class : reader
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

TEST_CASE("read_at_least(reader&, std::span<std::byte>, std::size_t)",
          "[utilities]")
{
  // Arrange
  const auto input = sequence_bytes(16u);
  auto storage = std::array<std::byte,8>{};

  SECTION("Minimum is zero")
  {
    // Arrange
    auto source = fake_reader{input};

    // Act
    const auto result = read_at_least(source, storage, 0u);

    // Assert
    SECTION("Returns zero bytes read")
    {
      REQUIRE( result == 0u );
    }
    SECTION("Does not read from the source")
    {
      REQUIRE( source.read_count() == 0u );
    }
  }

  SECTION("Source produces bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{8}
    );
    const auto min = GENERATE(std::size_t{1}, std::size_t{5}, std::size_t{8});
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);

    // Act
    const auto result = read_at_least(source, storage, min);

    // Assert
    SECTION("Reads at least the minimum number of bytes")
    {
      REQUIRE( result.has_value() );
      SECTION("Reads at least the requested minimum") {
        REQUIRE( *result >= min );
      }
    }
    SECTION("Reads no more than the buffer holds")
    {
      REQUIRE( result.has_value() );
      SECTION("Stops within the output buffer") {
        REQUIRE( *result <= storage.size() );
      }
    }
    SECTION("Writes the bytes into the buffer in order")
    {
      REQUIRE( result.has_value() );
      SECTION("Copies the bytes in source order") {
        REQUIRE(
          std::equal(storage.begin(), storage.begin() + *result, input.begin())
        );
      }
    }
    SECTION("Stops reading once the minimum is reached")
    {
      REQUIRE( source.read_count() == (min + chunk_size - 1u) / chunk_size );
    }
  }

  SECTION("Source ends before the minimum is read")
  {
    // Arrange
    auto source = fake_reader{std::span{input}.first(4u)};

    // Act
    const auto result = read_at_least(source, storage, 5u);

    // Assert
    SECTION("Fails with unexpected end of file")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }

  SECTION("Source fails before the minimum is read")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span{input}.first(4u)};
    source.fail_at_end(expected);

    // Act
    const auto result = read_at_least(source, storage, 5u);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

TEST_CASE("read_exact(reader&, std::span<std::byte>)", "[utilities]")
{
  // Arrange
  const auto input = sequence_bytes(16u);
  auto storage = std::array<std::byte,8>{};

  SECTION("Source produces bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{16}
    );
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);

    // Act
    const auto result = read_exact(source, storage);

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Fills the buffer in order")
    {
      REQUIRE( std::equal(storage.begin(), storage.end(), input.begin()) );
    }
    SECTION("Consumes only the bytes of the buffer")
    {
      REQUIRE( source.remaining() == input.size() - storage.size() );
    }
  }

  SECTION("Buffer is empty")
  {
    // Arrange
    auto source = fake_reader{input};

    // Act
    const auto result = read_exact(source, std::span<std::byte>{});

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Does not read from the source")
    {
      REQUIRE( source.read_count() == 0u );
    }
  }

  SECTION("Source ends before the buffer is filled")
  {
    // Arrange
    auto source = fake_reader{std::span{input}.first(storage.size() - 1u)};

    // Act
    const auto result = read_exact(source, storage);

    // Assert
    SECTION("Fails with unexpected end of file")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }

  SECTION("Source fails before the buffer is filled")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span{input}.first(storage.size() - 1u)};
    source.fail_at_end(expected);

    // Act
    const auto result = read_exact(source, storage);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

TEST_CASE("read_all_into(reader&, not_null<vector<std::byte>*>)",
          "[utilities]")
{
  SECTION("Source ends")
  {
    // Arrange
    const auto size = GENERATE(
      std::size_t{0}, std::size_t{1}, std::size_t{511}, std::size_t{512},
      std::size_t{513}, std::size_t{5000}
    );
    const auto chunk_size = GENERATE(std::size_t{7}, std::size_t{4096});
    const auto input = sequence_bytes(size);
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);
    auto out = vector<std::byte>{};

    // Act
    const auto result = read_all_into(source, &out);

    // Assert
    SECTION("Returns the number of bytes read")
    {
      REQUIRE( result == input.size() );
    }
    SECTION("Appends every byte in order")
    {
      REQUIRE( std::equal(out.begin(), out.end(), input.begin(), input.end()) );
    }
    SECTION("Consumes the source")
    {
      REQUIRE( source.remaining() == 0u );
    }
  }

  SECTION("Output already has contents")
  {
    // Arrange
    const auto existing = sequence_bytes(3u);
    const auto input = sequence_bytes(600u);
    auto source = fake_reader{input};
    auto out = vector<std::byte>{existing.begin(), existing.end()};

    // Act
    const auto result = read_all_into(source, &out);

    // Assert
    SECTION("Returns only the number of bytes read")
    {
      REQUIRE( result == input.size() );
    }
    SECTION("Keeps the existing contents")
    {
      REQUIRE( std::equal(existing.begin(), existing.end(), out.begin()) );
    }
    SECTION("Appends the bytes after the existing contents")
    {
      REQUIRE( std::equal(input.begin(), input.end(), out.begin() + 3) );
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    const auto input = sequence_bytes(600u);
    auto source = fake_reader{input};
    source.fail_at_end(expected);
    auto out = vector<std::byte>{};

    // Act
    const auto result = read_all_into(source, &out);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
    SECTION("Keeps the bytes read before the failure")
    {
      REQUIRE( std::equal(out.begin(), out.end(), input.begin(), input.end()) );
    }
  }
}

TEST_CASE("read_all(reader&)", "[utilities]")
{
  SECTION("Source ends")
  {
    // Arrange
    const auto size = GENERATE(
      std::size_t{0}, std::size_t{1}, std::size_t{513}, std::size_t{5000}
    );
    const auto input = sequence_bytes(size);
    auto source = fake_reader{input};
    source.set_chunk_size(7u);

    // Act
    const auto result = read_all(source);

    // Assert
    SECTION("Returns every byte in order")
    {
      REQUIRE( result.has_value() );
      SECTION("Returns the complete stream contents in order") {
        REQUIRE(
          std::equal(result->begin(), result->end(), input.begin(), input.end())
        );
      }
    }
    SECTION("Consumes the source")
    {
      REQUIRE( source.remaining() == 0u );
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    const auto input = sequence_bytes(600u);
    auto source = fake_reader{input};
    source.fail_at_end(expected);

    // Act
    const auto result = read_all(source);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

} // namespace
} // namespace alloy::core::test
