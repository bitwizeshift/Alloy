// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/deserializer.hpp>

#include <alloy/core/io/error.hpp>
#include <alloy/core/io/reader.hpp>
#include "test_doubles.hpp"

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <array>        // std::array
#include <cstddef>      // std::byte, std::size_t
#include <cstdint>      // std::int8_t, std::uint8_t, std::uint16_t, ...
#include <cstring>      // std::memcpy
#include <span>         // std::span
#include <system_error> // std::error_code, std::errc
#include <tuple>        // std::tuple
#include <type_traits>  // std::is_same_v
#include <utility>      // std::declval

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

//=============================================================================
// struct : deserializer<T>
//=============================================================================

TEST_CASE("deserializer<T>::deserialize( reader& )", "[serialization]")
{
  SECTION("T is an enum")
  {
    // Arrange
    const auto input = make_bytes(0x01, 0x02);
    auto source = fake_reader{input};

    // Act
    const auto result = deserializer<test_enum>::deserialize(source);

    // Assert
    SECTION("Reads the underlying value in big-endian order")
    {
      REQUIRE( result == test_enum::value );
    }
  }

  SECTION("T is trivially copyable")
  {
    // Arrange
    const auto chunk_size = GENERATE(std::size_t{1}, std::size_t{16});
    const auto expected = trivial_object{0x01020304u, 0x0506u, 0x0708u};
    auto input = std::array<std::byte,sizeof(trivial_object)>{};
    std::memcpy(input.data(), &expected, sizeof(expected));
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);

    // Act
    const auto result = deserializer<trivial_object>::deserialize(source);

    // Assert
    SECTION("Reads the object representation of the value")
    {
      REQUIRE( result.has_value() );
      SECTION("Preserves every field in its original position") {
        const auto fields = std::tuple{result->a, result->b, result->c};
        const auto expected_fields = std::tuple{expected.a, expected.b, expected.c};

        REQUIRE( fields == expected_fields );
      }
    }
  }

  SECTION("Source ends before the value is read")
  {
    // Arrange
    const auto input = make_bytes(0x01, 0x02);
    auto source = fake_reader{input};

    // Act
    const auto result = deserializer<trivial_object>::deserialize(source);

    // Assert
    SECTION("Fails with unexpected eof")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span<const std::byte>{}};
    source.fail_at_end(expected);

    // Act
    const auto result = deserializer<trivial_object>::deserialize(source);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

//=============================================================================
// specializations : struct : deserializer
//=============================================================================

//-----------------------------------------------------------------------------
// bool
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<bool>::deserialize( reader& )", "[serialization]")
{
  // Arrange
  using sut_type = deserializer<bool>;

  SECTION("Byte is 0")
  {
    // Arrange
    const auto input = make_bytes(0x00);
    auto source = fake_reader{input};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Reads false")
    {
      REQUIRE( result == false );
    }
  }

  SECTION("Byte is non-zero")
  {
    // Arrange
    const auto byte = GENERATE(0x01, 0x02, 0xff);
    const auto input = make_bytes(byte);
    auto source = fake_reader{input};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Reads true")
    {
      REQUIRE( result == true );
    }
  }

  SECTION("Source is at the end")
  {
    // Arrange
    auto source = fake_reader{std::span<const std::byte>{}};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Fails with unexpected eof")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::uint8_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::uint8_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::uint8_t>;

  SECTION("Source contains a value")
  {
    // Arrange
    const auto input = make_bytes(0xab);
    auto source = fake_reader{input};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Reads the byte")
    {
      REQUIRE( result == std::uint8_t{0xab} );
    }
  }

  SECTION("Source is at the end")
  {
    // Arrange
    auto source = fake_reader{std::span<const std::byte>{}};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Fails with unexpected eof")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int8_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::int8_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::int8_t>;

  SECTION("Returns a result of std::int8_t")
  {
    // Act
    using result_type = decltype(
      sut_type::deserialize(std::declval<reader&>())
    );

    // Act
    constexpr auto property_holds =
      std::is_same_v<result_type,result<std::int8_t,std::error_code>>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }

  SECTION("Source contains a value")
  {
    // Arrange
    const auto input = make_bytes(0xfe);
    auto source = fake_reader{input};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Reads the byte as a two's complement value")
    {
      REQUIRE( result == std::int8_t{-2} );
    }
  }

  SECTION("Source is at the end")
  {
    // Arrange
    auto source = fake_reader{std::span<const std::byte>{}};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Fails with unexpected eof")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::uint16_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::uint16_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::uint16_t>;

  SECTION("Source produces bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(std::size_t{1}, std::size_t{2});
    const auto input = make_bytes(0x01, 0x02);
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Reads the bytes in big-endian order")
    {
      REQUIRE( result == std::uint16_t{0x0102} );
    }
  }

  SECTION("Source ends before the value is read")
  {
    // Arrange
    const auto input = make_bytes(0x01);
    auto source = fake_reader{input};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Fails with unexpected eof")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int16_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::int16_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::int16_t>;

  // Arrange
  const auto input = make_bytes(0xfe, 0xdc);
  auto source = fake_reader{input};

  // Act
  const auto result = sut_type::deserialize(source);

  // Assert
  SECTION("Reads the bytes as a big-endian two's complement value")
  {
    REQUIRE( result == std::int16_t{-292} );
  }
}

//-----------------------------------------------------------------------------
// std::uint32_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::uint32_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::uint32_t>;

  SECTION("Source produces bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{3}, std::size_t{4}
    );
    const auto input = make_bytes(0x01, 0x02, 0x03, 0x04);
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Reads the bytes in big-endian order")
    {
      REQUIRE( result == std::uint32_t{0x01020304} );
    }
  }

  SECTION("Source ends before the value is read")
  {
    // Arrange
    const auto input = make_bytes(0x01, 0x02, 0x03);
    auto source = fake_reader{input};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Fails with unexpected eof")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int32_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::int32_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::int32_t>;

  // Arrange
  const auto input = make_bytes(0xfe, 0xdc, 0xba, 0x98);
  auto source = fake_reader{input};

  // Act
  const auto result = sut_type::deserialize(source);

  // Assert
  SECTION("Reads the bytes as a big-endian two's complement value")
  {
    REQUIRE( result == std::int32_t{-19088744} );
  }
}

//-----------------------------------------------------------------------------
// std::uint64_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::uint64_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::uint64_t>;

  SECTION("Source produces bytes in chunks")
  {
    // Arrange
    const auto chunk_size = GENERATE(
      std::size_t{1}, std::size_t{5}, std::size_t{8}
    );
    const auto input = make_bytes(
      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    );
    auto source = fake_reader{input};
    source.set_chunk_size(chunk_size);

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Reads the bytes in big-endian order")
    {
      REQUIRE( result == std::uint64_t{0x0102030405060708} );
    }
  }

  SECTION("Source ends before the value is read")
  {
    // Arrange
    const auto input = make_bytes(0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07);
    auto source = fake_reader{input};

    // Act
    const auto result = sut_type::deserialize(source);

    // Assert
    SECTION("Fails with unexpected eof")
    {
      REQUIRE( result == fail(io_error::unexpected_eof) );
    }
  }
}

//-----------------------------------------------------------------------------
// std::int64_t
//-----------------------------------------------------------------------------

TEST_CASE("deserializer<std::int64_t>::deserialize( reader& )",
          "[serialization]")
{
  // Arrange
  using sut_type = deserializer<std::int64_t>;

  // Arrange
  const auto input = make_bytes(
    0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
  );
  auto source = fake_reader{input};

  // Act
  const auto result = sut_type::deserialize(source);

  // Assert
  SECTION("Reads the bytes as a big-endian two's complement value")
  {
    REQUIRE( result == std::int64_t{-81985529216486896} );
  }
}

//=============================================================================
// non-member functions : struct : deserializer
//=============================================================================

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

TEST_CASE("deserialize<T>( reader& )", "[serialization]")
{
  SECTION("Source contains a value")
  {
    // Arrange
    const auto input = make_bytes(0x01, 0x02, 0x03, 0x04);
    auto source = fake_reader{input};

    // Act
    const auto result = deserialize<std::uint32_t>(source);

    // Assert
    SECTION("Reads the value with the deserializer of T")
    {
      REQUIRE( result == std::uint32_t{0x01020304} );
    }
  }

  SECTION("Source fails")
  {
    // Arrange
    const auto expected = std::make_error_code(std::errc::io_error);
    auto source = fake_reader{std::span<const std::byte>{}};
    source.fail_at_end(expected);

    // Act
    const auto result = deserialize<std::uint32_t>(source);

    // Assert
    SECTION("Returns the error of the source")
    {
      REQUIRE( result == fail(expected) );
    }
  }
}

} // namespace
} // namespace alloy::core::test
