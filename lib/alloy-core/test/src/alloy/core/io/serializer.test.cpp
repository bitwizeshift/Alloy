// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/serializer.hpp>

#include "test_doubles.hpp"
#include <alloy/core/io/error.hpp>
#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <cstdint>      // std::int8_t, std::uint8_t, std::uint16_t, ...
#include <cstring>      // std::memcmp
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc

namespace alloy::core::test {
namespace {

enum class test_enum : std::uint16_t
{
  value = 0x0102,
};

struct trivial_object
{
  std::uint32_t a;
  std::uint16_t b;
  std::uint16_t c;
};

template <typename...Bytes>
auto make_bytes(Bytes...bytes) -> std::array<std::byte,sizeof...(Bytes)>
{
  return {static_cast<std::byte>(bytes)...};
}

template <std::size_t N>
auto equals(
  std::span<const std::byte> lhs,
  const std::array<std::byte,N>& rhs
) -> bool
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

//=============================================================================
// struct : serializer<T>
//=============================================================================

TEST_CASE("serializer<T>::serialize( writer&, const T& )", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::byte,16>{};

  SECTION("T is an enum")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto result = serializer<test_enum>::serialize(
      destination,
      test_enum::value
    );

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes the underlying value in big-endian order") {
      REQUIRE( equals(destination.written(), make_bytes(0x01, 0x02)) );
    }
  }

  SECTION("T is trivially copyable")
  {
    // Arrange
    const auto chunk_size = GENERATE(std::size_t{1}, std::size_t{16});
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);
    const auto value = trivial_object{0x01020304u, 0x0506u, 0x0708u};

    // Act
    const auto result = serializer<trivial_object>::serialize(
      destination,
      value
    );

    // Assert
    SECTION("Writes the object representation of the value")
    {
      const auto written = destination.written();

      REQUIRE( result.has_value() );
      SECTION("Writes the complete object representation") {
        const auto expected = std::as_bytes(std::span{&value, 1u});

        REQUIRE( std::ranges::equal(written, expected) );
      }
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    auto destination = fake_writer{std::span{storage}.first(2u)};

    // Act
    const auto result = serializer<trivial_object>::serialize(
      destination,
      trivial_object{}
    );

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
    }
  }

  SECTION("Destination fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto destination = fake_writer{std::span<std::byte>{}};
    destination.fail_when_full(expected);

    // Act
    const auto result = serializer<trivial_object>::serialize(
      destination,
      trivial_object{}
    );

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

//=============================================================================
// specializations : struct : serializer
//=============================================================================

//-----------------------------------------------------------------------------
// bool
//-----------------------------------------------------------------------------

TEST_CASE("serializer<bool>::serialize( writer&, bool )", "[serialization]")
{
  // Arrange
  using sut_type = serializer<bool>;

  auto storage = std::array<std::byte,1>{};

  SECTION("Value is true")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto result = sut_type::serialize(destination, true);

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes a byte of 1") {
      REQUIRE( equals(destination.written(), make_bytes(0x01)) );
    }
  }

  SECTION("Value is false")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto result = sut_type::serialize(destination, false);

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes a byte of 0") {
      REQUIRE( equals(destination.written(), make_bytes(0x00)) );
    }
  }

  SECTION("Destination is full")
  {
    // Arrange
    auto destination = fake_writer{std::span<std::byte>{}};

    // Act
    const auto result = sut_type::serialize(destination, true);

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::uint8_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::uint8_t>::serialize( writer&, std::uint8_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::uint8_t>;

  auto storage = std::array<std::byte,1>{};

  SECTION("Destination has space for the value")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto result = sut_type::serialize(destination, std::uint8_t{0xab});

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes the byte") {
      REQUIRE( equals(destination.written(), make_bytes(0xab)) );
    }
  }

  SECTION("Destination is full")
  {
    // Arrange
    auto destination = fake_writer{std::span<std::byte>{}};

    // Act
    const auto result = sut_type::serialize(destination, std::uint8_t{0xab});

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int8_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::int8_t>::serialize( writer&, std::int8_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::int8_t>;

  auto storage = std::array<std::byte,1>{};

  SECTION("Destination has space for the value")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto result = sut_type::serialize(destination, std::int8_t{-2});

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes the two's complement byte") {
      REQUIRE( equals(destination.written(), make_bytes(0xfe)) );
    }
  }

  SECTION("Destination is full")
  {
    // Arrange
    auto destination = fake_writer{std::span<std::byte>{}};

    // Act
    const auto result = sut_type::serialize(destination, std::int8_t{-2});

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::uint16_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::uint16_t>::serialize( writer&, std::uint16_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::uint16_t>;

  auto storage = std::array<std::byte,2>{};

  SECTION("Destination accepts bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(std::size_t{1}, std::size_t{2});
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);

    // Act
    const auto result = sut_type::serialize(destination, std::uint16_t{0x0102});

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes every byte of the value in big-endian order") {
      REQUIRE( equals(destination.written(), make_bytes(0x01, 0x02)) );
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    auto destination = fake_writer{std::span{storage}.first(1u)};

    // Act
    const auto result = sut_type::serialize(destination, std::uint16_t{0x0102});

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int16_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::int16_t>::serialize( writer&, std::int16_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::int16_t>;

  // Arrange
  auto storage = std::array<std::byte,2>{};
  auto destination = fake_writer{storage};

  // Act
  const auto result = sut_type::serialize(destination, std::int16_t{-292});

  // Assert
  SECTION("Serialization succeeds") {
    REQUIRE( result.has_value() );
  }
  SECTION("Writes every byte of the value in big-endian order") {
    REQUIRE( equals(destination.written(), make_bytes(0xfe, 0xdc)) );
  }
}

//-----------------------------------------------------------------------------
// std::uint32_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::uint32_t>::serialize( writer&, std::uint32_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::uint32_t>;

  auto storage = std::array<std::byte,4>{};

  SECTION("Destination accepts bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{4}
    );
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);

    // Act
    const auto result = sut_type::serialize(
      destination,
      std::uint32_t{0x01020304}
    );

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes every byte of the value in big-endian order") {
      REQUIRE(
          equals(destination.written(), make_bytes(0x01, 0x02, 0x03, 0x04))
        );
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    auto destination = fake_writer{std::span{storage}.first(3u)};

    // Act
    const auto result = sut_type::serialize(
      destination,
      std::uint32_t{0x01020304}
    );

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int32_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::int32_t>::serialize( writer&, std::int32_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::int32_t>;

  // Arrange
  auto storage = std::array<std::byte,4>{};
  auto destination = fake_writer{storage};

  // Act
  const auto result = sut_type::serialize(
    destination,
    std::int32_t{-19088744}
  );

  // Assert
  SECTION("Serialization succeeds") {
    REQUIRE( result.has_value() );
  }
  SECTION("Writes every byte of the value in big-endian order") {
    REQUIRE(
        equals(destination.written(), make_bytes(0xfe, 0xdc, 0xba, 0x98))
      );
  }
}

//-----------------------------------------------------------------------------
// std::uint64_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::uint64_t>::serialize( writer&, std::uint64_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::uint64_t>;

  auto storage = std::array<std::byte,8>{};

  SECTION("Destination accepts bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{5}, std::size_t{8}
    );
    auto destination = fake_writer{storage};
    destination.set_chunk_size(chunk_size);
    const auto expected = make_bytes(
      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    );

    // Act
    const auto result = sut_type::serialize(
      destination,
      std::uint64_t{0x0102030405060708}
    );

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes every byte of the value in big-endian order") {
      REQUIRE( equals(destination.written(), expected) );
    }
  }

  SECTION("Destination stops accepting bytes")
  {
    // Arrange
    auto destination = fake_writer{std::span{storage}.first(7u)};

    // Act
    const auto result = sut_type::serialize(
      destination,
      std::uint64_t{0x0102030405060708}
    );

    // Assert
    SECTION("Fails with write zero")
    {
      REQUIRE( result == fail(io_error::write_zero) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int64_t
//-----------------------------------------------------------------------------

TEST_CASE("serializer<std::int64_t>::serialize( writer&, std::int64_t )",
          "[serialization]")
{
  // Arrange
  using sut_type = serializer<std::int64_t>;

  // Arrange
  auto storage = std::array<std::byte,8>{};
  auto destination = fake_writer{storage};
  const auto expected = make_bytes(
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
  );

  // Act
  const auto result = sut_type::serialize(
    destination,
    std::int64_t{-81985529216486896}
  );

  // Assert
  SECTION("Serialization succeeds") {
    REQUIRE( result.has_value() );
  }
  SECTION("Writes every byte of the value in big-endian order") {
    REQUIRE( equals(destination.written(), expected) );
  }
}

//=============================================================================
// non-member functions : struct : serializer
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

TEST_CASE("serialize( writer&, const T& )", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::byte,4>{};

  SECTION("Destination has space for the value")
  {
    // Arrange
    auto destination = fake_writer{storage};

    // Act
    const auto result = serialize(destination, std::uint32_t{0x01020304});

    // Assert
    SECTION("Serialization succeeds") {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes the value with the serializer of T") {
      REQUIRE(
          equals(destination.written(), make_bytes(0x01, 0x02, 0x03, 0x04))
        );
    }
  }

  SECTION("Destination fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto destination = fake_writer{std::span<std::byte>{}};
    destination.fail_when_full(expected);

    // Act
    const auto result = serialize(destination, std::uint32_t{0x01020304});

    // Assert
    SECTION("Returns the error of the destination")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

} // namespace
} // namespace alloy::core::test
