/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include <alloy/core/string/encoding/encoding_utilities.hpp>

#include <catch2/catch.hpp>

#include <array>

// Note: These tests try to cover general behavior, but may still be missing
//       some edge-cases.
//
// Additionally, these tests have a lot of replication that may potentially be
// cleaned up with fixtures. Part of the issue is that a char32* literals is
// fundamentally a different type from char*, char16*, and char32* -- which
// ultimately forces rewriting literals several times. Ironically the only way
// around this would be to convert it at compile time through utilities such as
// the ones being tested here!

namespace alloy::core::test {
namespace {

//=============================================================================
// class : ansi_encoding
//=============================================================================

TEST_CASE("ansi_encoding::decode(InputIt, InputIt, char32, const std::locale&)") {
  SECTION("Input is ASCII character (non-extended code-page)") {
    // Arrange
    const auto value = GENERATE(range(char{1}, char{127}));

    const auto begin = &value;
    const auto end = begin + 1;

    // Act
    const auto [result, it] = ansi_encoding::decode(begin, end);

    // Asserts
    SECTION("Iterator points to after the consumed character") {
      REQUIRE(it == end);
    }
    // All ASCII values are the same verbatim in UTF-32
    SECTION("Result is widened utf-32 character") {
      REQUIRE(result == static_cast<char32>(value));
    }
  }
  SECTION("Input is extended codepage", "[skip]") {
    // TODO(bitwize): test specific locale codepage

  }
}

TEST_CASE("ansi_encoding::encode(char32, OutputIt, char8)") {
  SECTION("Input is ASCII character (non-extended code-page)") {
    // Arrange
    const auto input = GENERATE(range(char32{1}, char32{127}));

    auto result = char{};

    // Act
    const auto it = ansi_encoding::encode(input, &result);

    // Asserts
    SECTION("Iterator points to after the consumed character") {
      const auto end = (&result + 1);

      REQUIRE(it == end);
    }
    // All ASCII values are the same verbatim in UTF-32
    SECTION("Result is widened utf-32 character") {
      REQUIRE(input == static_cast<char32>(result));
    }
  }
  SECTION("Input is extended codepage", "[skip]") {

  }
  SECTION("Input is out of range") {
    // Arrange
    const auto input = GENERATE(U'🍑', U'🍕', U'🍭', U'🏙');
    const auto expected = U'_';

    auto result = char{};

    // Act
    const auto it = ansi_encoding::encode(input, &result, expected);

    // Asserts
    SECTION("Iterator points to after the consumed character") {
      const auto end = (&result + 1);

      REQUIRE(it == end);
    }
    // All ASCII values are the same verbatim in UTF-32
    SECTION("Decodes into replacement character") {
      REQUIRE(result == expected);
    }
  }
}

TEST_CASE("ansi_encoding::next(ForwardIt, ForwardIt)") {
  // TODO(bitwize): add tests
}

} // namespace <anonymous>
} // namespace alloy::core
