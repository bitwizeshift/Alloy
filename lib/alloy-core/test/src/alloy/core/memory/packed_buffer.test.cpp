#include <alloy/core/memory/data_quantity.hpp>
#include <alloy/core/memory/packed_buffer.hpp>

#include <catch2/catch.hpp>

#include <array>     // std::array
#include <algorithm> // std::equal
#include <utility>   // std::move
#include <cstddef>   // std::byte

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
  auto sut = packed_buffer{};

  SECTION("Constructs an empty buffer")
  {
    REQUIRE(sut.size() == bytes{0});
  }
}

TEST_CASE("packed_buffer::packed_buffer(vector<std::byte>)", "[ctor]")
{
  auto vec = vector<std::byte>{ std::byte{1}, std::byte{2}, std::byte{3} };

  const auto expected_size = vec.size();
  const auto expected_data = vec.data();

  auto sut = packed_buffer{std::move(vec)};

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
  auto sut = packed_buffer{};
  sut.push_byte(std::byte{1});
  sut.push_byte(std::byte{2});
  sut.push_byte(std::byte{3});

  auto copy = sut.copy();

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
  const auto expected = std::byte{2};
  auto vec = vector<std::byte>{ std::byte{1}, expected, std::byte{3} };
  auto sut = packed_buffer{std::move(vec)};

  SECTION("Gets the Nth element")
  {
    REQUIRE(sut[1] == expected);
  }
}

//-----------------------------------------------------------------------------
// Capacity
//-----------------------------------------------------------------------------

TEST_CASE("packed_buffer::reserve(bytes)", "[capacity]")
{
  const auto requested = bytes{64};
  auto sut = packed_buffer{};
  sut.reserve(requested);

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
  auto sut = packed_buffer{};

  sut.push_byte(std::byte{1});
  sut.push_byte(std::byte{2});
  sut.push_byte(std::byte{3});

  const auto expected = std::byte{42};
  const auto old_size = sut.size();

  sut.push_byte(expected);

  SECTION("Increases the size of the buffer")
  {
    REQUIRE(old_size < sut.size());
  }

  SECTION("Inserts new element at the end")
  {
    REQUIRE(sut[3] == expected);
  }
}

TEST_CASE("packed_buffer::push_bytes(span<const std::byte>)", "[modifiers]")
{
  const auto array = std::array<std::byte, 4>{
    std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4},
  };
  auto sut = packed_buffer{};

  const auto old_size = sut.size();

  sut.push_bytes(array);

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

  auto unpack(packed_buffer_reader& buffer) -> expected<std::uint32_t>
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
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_writer{&buffer};

  const auto expected = std::uint32_t{0xdeadbeef};

  sut.pack_object(expected);

  SECTION("Inserts N bytes for the object")
  {
    REQUIRE(buffer.size() == size_of(expected));
  }
}

TEST_CASE("packed_buffer_writer::pack_object(const T&, Packer&&)", "[writer]")
{
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_writer{&buffer};

  const auto input = std::uint32_t{0xdeadbeef};
  const auto expected = std::array<std::byte,4>{
    std::byte{0xef}, std::byte{0xbe}, std::byte{0xad}, std::byte{0xde},
  };

  sut.pack_object(input, uint32_test_packer{});

  SECTION("Inserts N bytes for the object")
  {
    REQUIRE(buffer.size() == size_of(expected));
  }

  SECTION("Inserts elements into the packed_buffer")
  {
    const auto begin = buffer.data();
    const auto end   = begin + buffer.size();

    auto it = begin;
    for (auto x : expected) {
      REQUIRE(x == *it);
      ++it;
    }
    REQUIRE(std::equal(begin, end, expected.begin(), expected.end()));
  }
}


//=============================================================================
// class : packed_buffer_reader
//=============================================================================

TEST_CASE("packed_buffer_reader::unpack_object()", "[reader]")
{
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_reader{&buffer};

  const auto input = std::array<std::byte,4>{
    std::byte{0xef}, std::byte{0xbe}, std::byte{0xad}, std::byte{0xde},
  };
  buffer.push_bytes(input);
  const auto result = sut.unpack_object<std::uint32_t>();

  SECTION("Reads N bytes for the object")
  {
    REQUIRE(buffer.size().count() == sizeof(*result));
  }
}

TEST_CASE("packed_buffer_reader::unpack_object(Packer&&)", "[writer]")
{
  auto buffer = packed_buffer{};
  auto sut = packed_buffer_reader{&buffer};

  const auto input = std::array<std::byte,4>{
    std::byte{0xef}, std::byte{0xbe}, std::byte{0xad}, std::byte{0xde},
  };
  const auto expected = std::uint32_t{0xdeadbeef};
  buffer.push_bytes(input);
  const auto output = sut.unpack_object<std::uint32_t>(uint32_test_packer{});

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
  auto buffer = packed_buffer{};

  auto writer = packed_buffer_writer{&buffer};
  auto reader = packed_buffer_reader{&buffer};

  const auto expected0 = std::uint32_t{0x12345678};
  const auto expected1 = float{3.14f};

  writer.pack_object(expected0);
  writer.pack_object(expected1);

  const auto result0 = reader.unpack_object<std::uint32_t>();
  const auto result1 = reader.unpack_object<float>();

  REQUIRE(expected0 == result0);
  REQUIRE(expected1 == result1);
}

} // namespace <anonymous>
} // namespace alloy::core::test
