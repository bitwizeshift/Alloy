// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/platform/filesystem/file.hpp>

#include <alloy/platform/filesystem/memory_filesystem.hpp>

#include <alloy/platform/buffers/mutable_buffer.hpp>

#include <alloy/core/intrinsics.hpp>
#include <alloy/core/io/error.hpp>
#include <alloy/core/io/reader.hpp>
#include <alloy/core/io/writer.hpp>
#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>        // std::array
#include <cstddef>      // std::byte
#include <cstdint>      // std::int8_t, std::uint8_t, std::uint16_t, ...

namespace alloy::platform::test {

//------------------------------------------------------------------------------
// file
//------------------------------------------------------------------------------

TEST_CASE("file::read( std::span<std::byte> )", "[reading]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,2>{0x01, 0x02};
  auto filesystem = memory_filesystem{};
  auto buffer = std::array<std::byte,4>{};

  const auto expected = std::array<std::byte,4>{std::byte{0x01}, std::byte{0x02}};

  SECTION("File contains bytes")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));
    auto& sut = static_cast<core::reader&>(f);

    // Act
    const auto result = sut.read(buffer);

    // Assert
    SECTION("Returns the number of bytes read")
    {
      REQUIRE( result == storage.size() );
    }
    SECTION("Reads the bytes of the file")
    {
      REQUIRE( buffer == expected );
    }
  }

  SECTION("File is at the end")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));
    core::compiler::unused(f.skip(2));
    auto& sut = static_cast<core::reader&>(f);

    // Act
    const auto result = sut.read(buffer);

    // Assert
    SECTION("Reads no bytes")
    {
      REQUIRE( result == 0u );
    }
  }

  SECTION("File is closed")
  {
    // Arrange
    auto f = file{};
    auto& sut = static_cast<core::reader&>(f);

    // Act
    const auto result = sut.read(buffer);

    // Assert
    SECTION("Fails with closed")
    {
      REQUIRE( result == core::fail(file::error_code::closed) );
    }
  }
}

TEST_CASE("file::write( std::span<const std::byte> )", "[writing]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,2>{};
  auto filesystem = memory_filesystem{};
  const auto buffer = std::array<std::byte,2>{std::byte{0x01}, std::byte{0x02}};

  const auto expected = std::array<std::uint8_t,2>{0x01, 0x02};

  SECTION("File has space for the bytes")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));
    auto& sut = static_cast<core::writer&>(f);

    // Act
    const auto result = sut.write(buffer);

    // Assert
    SECTION("Returns the number of bytes written")
    {
      REQUIRE( result == buffer.size() );
    }
    SECTION("Writes the bytes to the file")
    {
      REQUIRE( storage == expected );
    }
  }

  SECTION("File is at the end")
  {
    // Arrange
    auto f = filesystem.open(mutable_buffer::from_container(storage));
    core::compiler::unused(f.skip(2));
    auto& sut = static_cast<core::writer&>(f);

    // Act
    const auto result = sut.write(buffer);

    // Assert
    SECTION("Writes no bytes")
    {
      REQUIRE( result == 0u );
    }
  }

  SECTION("File is closed")
  {
    // Arrange
    auto f = file{};
    auto& sut = static_cast<core::writer&>(f);

    // Act
    const auto result = sut.write(buffer);

    // Assert
    SECTION("Fails with closed")
    {
      REQUIRE( result == core::fail(file::error_code::closed) );
    }
  }
}

TEST_CASE("file::read_object<T>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,1>{0x01};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));

  // Act
  const auto result = sut.read_object<std::uint16_t>();

  // Assert
  SECTION("Fails with unexpected eof when the file ends before the value")
  {
    REQUIRE( result == core::fail(core::io_error::unexpected_eof) );
  }
}

TEST_CASE("file::write_object<T>( const T& )", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,1>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));

  // Act
  const auto result = sut.write_object(std::uint16_t{0x0102});

  // Assert
  SECTION("Fails with write zero when the file ends before the value")
  {
    REQUIRE( result == core::fail(core::io_error::write_zero) );
  }
}

TEST_CASE("file::read_object<std::int8_t>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,1>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));
  const auto value = std::int8_t{-128};

  core::compiler::unused(sut.write_object(value));
  core::compiler::unused(sut.reset());

  // Act
  const auto result = sut.read_object<std::int8_t>();

  // Assert
  SECTION("Reads back the value that was written")
  {
    REQUIRE( result == value );
  }
}

TEST_CASE("file::read_object<std::uint16_t>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,2>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));
  const auto value = std::uint16_t{0xfedc};

  core::compiler::unused(sut.write_object(value));
  core::compiler::unused(sut.reset());

  // Act
  const auto result = sut.read_object<std::uint16_t>();

  // Assert
  SECTION("Reads back the value that was written")
  {
    REQUIRE( result == value );
  }
}

TEST_CASE("file::read_object<std::uint32_t>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,4>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));
  const auto value = std::uint32_t{0xfedcba98};

  core::compiler::unused(sut.write_object(value));
  core::compiler::unused(sut.reset());

  // Act
  const auto result = sut.read_object<std::uint32_t>();

  // Assert
  SECTION("Reads back the value that was written")
  {
    REQUIRE( result == value );
  }
}

TEST_CASE("file::read_object<std::uint64_t>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,8>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));
  const auto value = std::uint64_t{0xfedcba9876543210};

  core::compiler::unused(sut.write_object(value));
  core::compiler::unused(sut.reset());

  // Act
  const auto result = sut.read_object<std::uint64_t>();

  // Assert
  SECTION("Reads back the value that was written")
  {
    REQUIRE( result == value );
  }
}

TEST_CASE("file::read_object<std::int32_t>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,4>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));
  const auto value = std::int32_t{-19088744};

  core::compiler::unused(sut.write_object(value));
  core::compiler::unused(sut.reset());

  // Act
  const auto result = sut.read_object<std::int32_t>();

  // Assert
  SECTION("Reads back the value that was written")
  {
    REQUIRE( result == value );
  }
}

TEST_CASE("file::read_object<std::int64_t>()", "[serialization]")
{
  // Arrange
  auto storage = std::array<std::uint8_t,8>{};
  auto filesystem = memory_filesystem{};
  auto sut = filesystem.open(mutable_buffer::from_container(storage));
  const auto value = std::int64_t{-81985529216486896};

  core::compiler::unused(sut.write_object(value));
  core::compiler::unused(sut.reset());

  // Act
  const auto result = sut.read_object<std::int64_t>();

  // Assert
  SECTION("Reads back the value that was written")
  {
    REQUIRE( result == value );
  }
}

} // namespace alloy::platform::test
