// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include "test_doubles.hpp"

#include <alloy/core/io/error.hpp>
#include <alloy/core/io/reader.hpp>
#include <alloy/core/io/writer.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc
#include <vector>       // std::vector

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

//=============================================================================
// non-member functions : class : writer
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

TEST_CASE("write_all(writer&, std::span<const std::byte>)", "[utilities]")
{
  const auto input = sequence_bytes(16u);
  auto storage = std::array<std::byte,16>{};

  SECTION("Destination accepts bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{16}
    );
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);

    // Act
    const auto result = write_all(destination, input);

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes every byte in order")
    {
      const auto written = destination.written();

      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
  }

  SECTION("Buffer is empty")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto result = write_all(destination, std::span<const std::byte>{});

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Does not write to the destination")
    {
      REQUIRE( destination.write_count() == 0u );
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    auto destination = fake_writer{std::span{storage}.first(4u)};

    // Act
    const auto result = write_all(destination, input);

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == io_error::write_zero );
    }
  }

  SECTION("Destination fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto destination = fake_writer{std::span{storage}.first(4u)};
    destination.fail_when_full(expected);

    // Act
    const auto result = write_all(destination, input);

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }
}

TEST_CASE("copy(reader&, writer&)", "[utilities]")
{
  SECTION("Source ends")
  {
    // Arrange
    const auto size = GENERATE(
      std::size_t{0}, std::size_t{1}, std::size_t{8192}, std::size_t{8193},
      std::size_t{20000}
    );
    const auto chunk_size = GENERATE(std::size_t{7}, std::size_t{10000});
    const auto input = sequence_bytes(size);
    auto storage = std::vector<std::byte>(size);
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);

    // Act
    const auto result = copy(source, destination);

    // Assert
    SECTION("Returns the number of bytes copied")
    {
      REQUIRE( result.has_value() );
      REQUIRE( *result == input.size() );
    }
    SECTION("Writes every byte in order")
    {
      const auto written = destination.written();

      REQUIRE(
        std::equal(written.begin(), written.end(), input.begin(), input.end())
      );
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    const auto input = sequence_bytes(16u);
    auto storage = std::array<std::byte,16>{};
    auto source = fake_reader{input};
    source.fail_at_end(expected);
    auto destination = fake_writer{storage};

    // Act
    const auto result = copy(source, destination);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    const auto input = sequence_bytes(16u);
    auto storage = std::array<std::byte,4>{};
    auto source = fake_reader{input};
    auto destination = fake_writer{storage};

    // Act
    const auto result = copy(source, destination);

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == io_error::write_zero );
    }
  }

  SECTION("Destination fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    const auto input = sequence_bytes(16u);
    auto storage = std::array<std::byte,4>{};
    auto source = fake_reader{input};
    auto destination = fake_writer{storage};
    destination.fail_when_full(expected);

    // Act
    const auto result = copy(source, destination);

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == expected );
    }
  }
}

TEST_CASE("copy_n(reader&, writer&, std::size_t)", "[utilities]")
{
  const auto input = sequence_bytes(20000u);
  auto storage = std::vector<std::byte>(input.size());

  SECTION("Source has more than n bytes")
  {
    // Arrange
    const auto n = GENERATE(
      std::size_t{1}, std::size_t{8192}, std::size_t{8193}
    );
    const auto chunk_size = GENERATE(std::size_t{7}, std::size_t{10000});
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);
    auto destination = fake_writer{storage};

    // Act
    const auto result = copy_n(source, destination, n);

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes the first n bytes in order")
    {
      const auto written = destination.written();

      REQUIRE( written.size() == n );
      REQUIRE( std::equal(written.begin(), written.end(), input.begin()) );
    }
    SECTION("Leaves the bytes after n in the source")
    {
      REQUIRE( source.remaining() == input.size() - n );
    }
  }

  SECTION("n is zero")
  {
    // Arrange
    auto source = fake_reader{input};
    auto destination = fake_writer{storage};

    // Act
    const auto result = copy_n(source, destination, 0u);

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

  SECTION("Source ends before n bytes are copied")
  {
    // Arrange
    auto source = fake_reader{std::span{input}.first(100u)};
    auto destination = fake_writer{storage};

    // Act
    const auto result = copy_n(source, destination, 101u);

    // Assert
    SECTION("Fails with unexpected end of file")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == io_error::unexpected_eof );
    }
  }

  SECTION("Source fails before n bytes are copied")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span{input}.first(100u)};
    source.fail_at_end(expected);
    auto destination = fake_writer{storage};

    // Act
    const auto result = copy_n(source, destination, 101u);

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
