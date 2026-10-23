// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/memory/packed_buffer.hpp>

#include <alloy/core/memory/data_quantity.hpp>

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>    // std::equal
#include <array>        // std::array
#include <cstddef>      // std::byte
#include <cstdint>
#include <system_error>
#include <utility>      // std::move

namespace alloy::core::test {
namespace {

//=============================================================================
// class : packed_buffer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

TEST_CASE("packed_buffer::packed_buffer()", "[ctor]")
{
  // Act
  auto sut = packed_buffer{};

  // Assert
  SECTION("Constructs an empty buffer")
  {
    REQUIRE(sut.size() == bytes{0});
  }
}

TEST_CASE("packed_buffer::packed_buffer(vector<std::byte>)", "[ctor]")
{
  // Arrange
  auto vec = vector<std::byte>{ std::byte{1}, std::byte{2}, std::byte{3} };

  const auto expected_size = vec.size();
  const auto expected_data = vec.data();

  // Act
  auto sut = packed_buffer{std::move(vec)};

  // Assert
  SECTION("Constructs a packed_buffer using the underlying vector")
  {
    SECTION("Contains same size")
    {
      REQUIRE(sut.size() == bytes{expected_size});
    }
    SECTION("Contains same data pointer")
    {
      REQUIRE(sut.data() == expected_data);
    }
  }
}

TEST_CASE("packed_buffer::copy()", "[copy]")
{
  // Arrange
  auto sut = packed_buffer{};
  sut.push_byte(std::byte{1});
  sut.push_byte(std::byte{2});
  sut.push_byte(std::byte{3});

  // Act
  auto copy = sut.copy();

  // Assert
  SECTION("Copies the underlying packed_buffer")
  {
    SECTION("Contains same same data")
    {
      REQUIRE(sut == copy);
    }
  }
}

//-----------------------------------------------------------------------------
// Element Access
//-----------------------------------------------------------------------------

TEST_CASE("packed_buffer::operator[](std::ptrdiff_t)", "[element access]")
{
  // Arrange
  const auto expected = std::byte{2};
  auto vec = vector<std::byte>{ std::byte{1}, expected, std::byte{3} };
  auto sut = packed_buffer{std::move(vec)};

  SECTION("Gets the Nth element")
  {
    // Act
    const auto result = sut[1];

    // Assert
    REQUIRE(result == expected);
  }
}

//-----------------------------------------------------------------------------
// Capacity
//-----------------------------------------------------------------------------

TEST_CASE("packed_buffer::reserve(bytes)", "[capacity]")
{
  // Arrange
  const auto requested = bytes{64};
  auto sut = packed_buffer{};

  // Act
  sut.reserve(requested);

  // Assert
  SECTION("Changes capacity to at least the requested value")
  {
    REQUIRE(sut.capacity() >= requested);
  }
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

TEST_CASE("packed_buffer::push_byte(std::byte)", "[modifiers]")
{
  // Arrange
  auto sut = packed_buffer{};

  sut.push_byte(std::byte{1});
  sut.push_byte(std::byte{2});
  sut.push_byte(std::byte{3});

  const auto expected = std::byte{42};
  const auto old_size = sut.size();

  // Act
  sut.push_byte(expected);

  // Assert
  SECTION("Increases the size of the buffer")
  {
    REQUIRE(old_size < sut.size());
  }

  SECTION("Inserts new element at the end")
  {
    REQUIRE(sut[3] == expected);
  }
}

TEST_CASE("packed_buffer::push_bytes(std::span<const std::byte>)", "[modifiers]")
{
  // Arrange
  const auto array = std::array<std::byte, 4>{
    std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4},
  };
  auto sut = packed_buffer{};

  const auto old_size = sut.size();

  // Act
  sut.push_bytes(array);

  // Assert
  SECTION("Increases the size of the buffer")
  {
    REQUIRE(old_size < sut.size());
  }

  SECTION("Increases N elements")
  {
    REQUIRE(array.size() == sut.size().count());
  }

  SECTION("Inserts elements into the packed_buffer")
  {
    const auto begin = sut.data();
    const auto end   = begin + sut.size();

    REQUIRE(std::equal(begin, end, array.begin(), array.end()));
  }
}


struct uint32_test_packer
{
  auto pack(packed_buffer_writer& buffer, std::uint32_t value) -> void
  {
    const auto b0 = static_cast<std::uint8_t>(0xff & value);
    const auto b1 = static_cast<std::uint8_t>(0xff & (value >> 8));
    const auto b2 = static_cast<std::uint8_t>(0xff & (value >> 16));
    const auto b3 = static_cast<std::uint8_t>(0xff & (value >> 24));

    buffer.pack_object(b0);
    buffer.pack_object(b1);
    buffer.pack_object(b2);
    buffer.pack_object(b3);
  }

  auto unpack(packed_buffer_reader& buffer)
    -> result<std::uint32_t,std::error_code>
  {
    auto array = std::array<std::byte, sizeof(std::uint32_t)>{};
    buffer.read_bytes({array.data(), array.size()});

    const auto b0 = array[0];
    const auto b1 = array[1];
    const auto b2 = array[2];
    const auto b3 = array[3];

    auto result = std::uint32_t{
      (static_cast<std::uint32_t>(b3) << 24) |
      (static_cast<std::uint32_t>(b2) << 16) |
      (static_cast<std::uint32_t>(b1) << 8) |
      (static_cast<std::uint32_t>(b0) << 0)
    };
    return result;
  }
};

//=============================================================================
// class : packed_buffer_writer
//=============================================================================

TEST_CASE("packed_buffer_writer::pack_object(const T&)", "[writer]")
{
  // Arrange
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_writer{&buffer};

  const auto expected = std::uint32_t{0xdeadbeef};

  // Act
  sut.pack_object(expected);

  // Assert
  SECTION("Inserts N bytes for the object")
  {
    REQUIRE(buffer.size() == size_of(expected));
  }
}

TEST_CASE("packed_buffer_writer::pack_object(const T&, Packer&&)", "[writer]")
{
  // Arrange
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_writer{&buffer};

  const auto input = std::uint32_t{0xdeadbeef};
  const auto expected = std::array<std::byte,4>{
    std::byte{0xef}, std::byte{0xbe}, std::byte{0xad}, std::byte{0xde},
  };

  // Act
  sut.pack_object(input, uint32_test_packer{});

  // Assert
  SECTION("Inserts N bytes for the object")
  {
    REQUIRE(buffer.size() == size_of(expected));
  }

  SECTION("Inserts elements into the packed_buffer")
  {
    const auto begin = buffer.data();
    const auto end   = begin + buffer.size();

    REQUIRE(std::equal(begin, end, expected.begin(), expected.end()));
  }
}


//=============================================================================
// class : packed_buffer_reader
//=============================================================================

TEST_CASE("packed_buffer_reader::unpack_object()", "[reader]")
{
  // Arrange
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_reader{&buffer};

  const auto input = std::array<std::byte,4>{
    std::byte{0xef}, std::byte{0xbe}, std::byte{0xad}, std::byte{0xde},
  };
  buffer.push_bytes(input);

  // Act
  const auto result = sut.unpack_object<std::uint32_t>();

  // Assert
  SECTION("Reads N bytes for the object")
  {
    REQUIRE(buffer.size().count() == sizeof(*result));
  }
}

TEST_CASE("packed_buffer_reader::unpack_object(Packer&&)", "[writer]")
{
  // Arrange
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_reader{&buffer};

  const auto input = std::array<std::byte,4>{
    std::byte{0xef}, std::byte{0xbe}, std::byte{0xad}, std::byte{0xde},
  };
  const auto expected = std::uint32_t{0xdeadbeef};
  buffer.push_bytes(input);

  // Act
  const auto output = sut.unpack_object<std::uint32_t>(uint32_test_packer{});

  // Assert
  SECTION("Reads the stored value from the packed_buffer")
  {
    REQUIRE(output.value() == expected);
  }
}

//=============================================================================
// class : trivial_object_packer<T>
//=============================================================================

TEST_CASE("trivial_object_packer round-trip test", "[acceptance]")
{
  // Arrange
  auto buffer = packed_buffer{};

  auto writer = packed_buffer_writer{&buffer};
  auto reader = packed_buffer_reader{&buffer};

  const auto expected0 = std::uint32_t{0x12345678};
  const auto expected1 = float{3.14f};

  // Act
  writer.pack_object(expected0);
  writer.pack_object(expected1);

  const auto result0 = reader.unpack_object<std::uint32_t>();
  const auto result1 = reader.unpack_object<float>();

  // Assert
  SECTION("Integer survives serialization") {
    // Assert
    REQUIRE(expected0 == result0);
  }
  SECTION("Float survives serialization") {
    // Assert
    REQUIRE(expected1 == result1);
  }
}

} // namespace <anonymous>
} // namespace alloy::core::test
