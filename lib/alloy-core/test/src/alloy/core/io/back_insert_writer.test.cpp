// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/back_insert_writer.hpp>

#include "test_doubles.hpp"
#include <alloy/core/intrinsics.hpp>
#include <alloy/core/io/span_reader.hpp>
#include <alloy/core/io/writer.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>   // std::ranges::equal
#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint8_t, std::int8_t, std::uint16_t
#include <deque>       // std::deque
#include <span>        // std::span
#include <string>      // std::string
#include <type_traits> // std::is_constructible_v
#include <vector>      // std::vector

namespace alloy::core::test {
namespace {

//=============================================================================
// non-member functions
//=============================================================================

/// \brief Converts each byte of \p bytes to a `T`
///
/// \param bytes the bytes to convert
/// \return the converted bytes
template <typename T>
auto as_values(std::span<const std::byte> bytes) -> std::vector<T>
{
  auto result = std::vector<T>{};
  for (const auto b : bytes) {
    result.push_back(static_cast<T>(b));
  }
  return result;
}

//=============================================================================
// class : back_insert_writer
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

TEST_CASE("back_insert_writer::back_insert_writer(Container&)", "[ctor]")
{
  SECTION("Container holds single bytes")
  {
    SECTION("Accepts a vector of bytes") {
      // Act
      constexpr auto property_holds =
        std::is_constructible_v<back_insert_writer, std::vector<std::byte>&>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Accepts a vector of unsigned byte values") {
      // Act
      constexpr auto property_holds =
        std::is_constructible_v<back_insert_writer, std::vector<std::uint8_t>&>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Accepts a deque of signed byte values") {
      // Act
      constexpr auto property_holds =
        std::is_constructible_v<back_insert_writer, std::deque<std::int8_t>&>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
    SECTION("Accepts a string of characters") {
      // Act
      constexpr auto property_holds =
        std::is_constructible_v<back_insert_writer, std::string&>;

      // Assert
      STATIC_REQUIRE(property_holds);
    }
  }

  SECTION("Container holds values that are not single bytes")
  {
    SECTION("Rejects elements wider than one byte") {
      // Act
      constexpr auto property_holds =
        std::is_constructible_v<back_insert_writer, std::vector<std::uint16_t>&>;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
    SECTION("Rejects packed bits with proxy references") {
      // Act
      constexpr auto property_holds =
        std::is_constructible_v<back_insert_writer, std::vector<bool>&>;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("Container is not a modifiable lvalue")
  {
    SECTION("Rejects a temporary container") {
      // Act
      constexpr auto property_holds =
        std::is_constructible_v<back_insert_writer, std::vector<std::byte>&&>;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
    SECTION("Rejects a const container") {
      // Act
      constexpr auto property_holds = std::is_constructible_v<
          back_insert_writer,
          const std::vector<std::byte>&
        >;

      // Assert
      STATIC_REQUIRE_FALSE(property_holds);
    }
  }

  SECTION("Container is not modified")
  {
    // Arrange
    auto container = std::vector<std::byte>{};

    // Act
    const auto sut = back_insert_writer{container};

    // Assert
    REQUIRE( container.empty() );
  }
}

//-----------------------------------------------------------------------------
// Writing
//-----------------------------------------------------------------------------

TEMPLATE_TEST_CASE("back_insert_writer::write(std::span<const std::byte>)",
                   "[write]",
                   std::vector<std::byte>,
                   std::vector<std::uint8_t>,
                   std::deque<std::int8_t>,
                   std::string)
{
  // Arrange
  using value_type = typename TestType::value_type;

  const auto input = sequence_bytes(256u);

  SECTION("Container is empty")
  {
    // Arrange
    auto container = TestType{};
    auto sut = back_insert_writer{container};

    // Act
    const auto bytes_written = sut.write(input);

    // Assert
    SECTION("Writes every byte")
    {
      REQUIRE( bytes_written == input.size() );
    }
    SECTION("Appends the bytes to the container")
    {
      REQUIRE( std::ranges::equal(container, as_values<value_type>(input)) );
    }
  }

  SECTION("Container has existing contents")
  {
    // Arrange
    const auto existing = sequence_bytes(3u);
    auto container = TestType{};
    for (const auto v : as_values<value_type>(existing)) {
      container.push_back(v);
    }
    auto sut = back_insert_writer{container};
    auto expected = as_values<value_type>(existing);
    for (const auto v : as_values<value_type>(input)) {
      expected.push_back(v);
    }

    // Act
    const auto bytes_written = sut.write(input);

    // Assert
    SECTION("Keeps the existing contents before the appended bytes")
    {
      REQUIRE( bytes_written.has_value() );
      SECTION("Appends bytes after the existing contents") {
        REQUIRE( std::ranges::equal(container, expected) );
      }
    }
  }

  SECTION("Buffer is empty")
  {
    // Arrange
    auto container = TestType{};
    auto sut = back_insert_writer{container};

    // Act
    const auto bytes_written = sut.write(std::span<const std::byte>{});

    // Assert
    SECTION("Writes no bytes")
    {
      REQUIRE( bytes_written == 0u );
    }
    SECTION("Leaves the container unchanged")
    {
      REQUIRE( container.empty() );
    }
  }
}

TEST_CASE("back_insert_writer::write(std::span<const std::byte>) "
          "in sequence", "[write]")
{
  // Arrange
  const auto input = sequence_bytes(8u);
  const auto bytes = std::span<const std::byte>{input};
  auto container = std::vector<std::byte>{};
  auto sut = back_insert_writer{container};
  compiler::unused(sut.write(bytes.first(3u)));

  // Act
  const auto bytes_written = sut.write(bytes.subspan(3u));

  // Assert
  SECTION("Appends each write after the last")
  {
    REQUIRE( bytes_written.has_value() );
    SECTION("Preserves every input byte in order") {
      REQUIRE( std::ranges::equal(container, input) );
    }
  }
}

TEST_CASE("back_insert_writer::back_insert_writer(const back_insert_writer&)",
          "[ctor]")
{
  // Arrange
  const auto input = sequence_bytes(4u);
  auto container = std::vector<std::byte>{};
  const auto original = back_insert_writer{container};

  // Act
  auto sut = original;
  const auto bytes_written = sut.write(input);

  // Assert
  SECTION("Appends to the same container")
  {
    REQUIRE( bytes_written.has_value() );
    SECTION("Preserves every input byte in order") {
      REQUIRE( std::ranges::equal(container, input) );
    }
  }
}

//=============================================================================
// non-member functions : class : writer
//=============================================================================

TEST_CASE("copy(reader&, writer&) into a back_insert_writer", "[write]")
{
  // Arrange
  const auto input = sequence_bytes(1000u);
  auto source = span_reader{input};
  auto container = std::vector<std::byte>{};
  auto destination = back_insert_writer{container};

  // Act
  const auto bytes_copied = copy(source, destination);

  // Assert
  SECTION("Copies every byte into the container")
  {
    REQUIRE( bytes_copied.has_value() );
    SECTION("Reports the full number of copied bytes") {
      REQUIRE( *bytes_copied == input.size() );
    }
    SECTION("Preserves every input byte in order") {
      REQUIRE( std::ranges::equal(container, input) );
    }
  }
}

} // namespace
} // namespace alloy::core::test
