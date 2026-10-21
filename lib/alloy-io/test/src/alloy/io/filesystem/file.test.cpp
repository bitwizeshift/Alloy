// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/io/filesystem/file.hpp>
#include <alloy/io/filesystem/memory_filesystem.hpp>
#include <alloy/io/buffers/mutable_buffer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>        // std::array
#include <cstdint>      // std::int8_t, std::uint8_t
#include <system_error> // std::error_code
#include <type_traits>  // std::is_same_v
#include <utility>      // std::declval

namespace alloy::io::test {

//------------------------------------------------------------------------------
// file_serializer<std::int8_t>
//------------------------------------------------------------------------------

TEST_CASE("file_serializer<std::int8_t>::serialize( file&, std::int8_t )",
          "[serialization]")
{
  using sut_type = file_serializer<std::int8_t>;

  auto storage = std::array<std::uint8_t,1>{};
  auto filesystem = memory_filesystem{};

  SECTION("File has space for the value")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));
    const auto value = std::int8_t{-2};

    // Act
    const auto result = sut_type::serialize(f, value);

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Writes the two's complement byte")
    {
      REQUIRE( storage[0] == std::uint8_t{0xfe} );
    }
  }

  SECTION("File is at the end")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));
    REQUIRE( f.skip(1).has_value() );

    // Act
    const auto result = sut_type::serialize(f, std::int8_t{-2});

    // Assert
    SECTION("Fails with end of file")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == file::error_code::eof );
    }
  }
}

TEST_CASE("file_serializer<std::int8_t>::deserialize( file& )",
          "[serialization]")
{
  using sut_type = file_serializer<std::int8_t>;

  auto storage = std::array<std::uint8_t,1>{0xfe};
  auto filesystem = memory_filesystem{};

  SECTION("Returns a result of std::int8_t")
  {
    using result_type = decltype(sut_type::deserialize(std::declval<file&>()));

    STATIC_REQUIRE(
      std::is_same_v<result_type,core::result<std::int8_t,std::error_code>>
    );
  }

  SECTION("File contains a value")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));

    // Act
    const auto result = sut_type::deserialize(f);

    // Assert
    SECTION("Succeeds")
    {
      REQUIRE( result.has_value() );
    }
    SECTION("Reads the byte as a two's complement value")
    {
      REQUIRE( *result == std::int8_t{-2} );
    }
  }

  SECTION("File is at the end")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));
    REQUIRE( f.skip(1).has_value() );

    // Act
    const auto result = sut_type::deserialize(f);

    // Assert
    SECTION("Fails with end of file")
    {
      REQUIRE( result.has_error() );
      REQUIRE( result.error() == file::error_code::eof );
    }
  }
}

TEST_CASE("file::read_object<std::int8_t>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,1>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));
  const auto value = std::int8_t{-128};

  REQUIRE( sut.write_object(value).has_value() );
  REQUIRE( sut.reset().has_value() );

  // Act
  const auto result = sut.read_object<std::int8_t>();

  // Assert
  SECTION("Reads back the value that was written")
  {
    REQUIRE( result.has_value() );
    REQUIRE( *result == value );
  }
}

} // namespace alloy::io::test
