/*
  The MIT License (MIT)

  Copyright (c) 2022 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <alloy/core/string/encoding/encoding_utilities.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
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
// class : encoding_utilities::ansi
//=============================================================================

// ANSI is easy to test for, since it's not a multi-encoding. These tests are
// just simple checks for the alphabet encoding.

TEST_CASE("encoding_utilities::ansi::to_ansi(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::ansi;
  using other_type = encoding_utilities::ansi;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  constexpr auto expected = input;
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_ansi(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces identical output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

// ansi is a subset of latin1, so this test is identical to to_ansi. It's not
// possible to have ansi characters that are not part of the latin character
// set.
TEST_CASE("encoding_utilities::ansi::to_latin1(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::ansi;
  using other_type = encoding_utilities::latin1;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  constexpr auto expected = input;
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_latin1(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces identical output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::ansi::to_wide(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::ansi;
  using other_type = encoding_utilities::wide;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  constexpr auto expected = encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_wide(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::ansi::to_utf8(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::ansi;
  using other_type = encoding_utilities::utf8;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  constexpr auto expected = encoded_string{
    u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf8(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::ansi::to_utf16(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::ansi;
  using other_type = encoding_utilities::utf16;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  constexpr auto expected = encoded_string{
    u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf16(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::ansi::to_utf32(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::ansi;
  using other_type = encoding_utilities::utf32;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf8(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::ansi::length(ForwardIt, ForwardIt)") {
  using sut_type = encoding_utilities::ansi;

  SECTION("Input range is empty") {
    constexpr auto data = std::array<char,0u>{};

    SECTION("Returns 0") {
      const auto result = sut_type::length(data.begin(), data.end());

      REQUIRE(result == uquantity<char32>{0u});
    }
  }
  SECTION("Input range is non-empty") {
    constexpr auto data = std::array<char,12u>{"Hello world"};

    SECTION("Returns length") {
      const auto result = sut_type::length(data.begin(), data.end());

      REQUIRE(result == uquantity<char32>{data.size()});
    }
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::ansi::range_from(ForwardIt, ForwardIt)") {
  using sut_type = encoding_utilities::ansi;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string = std::basic_string_view<char32>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
  };

  SECTION("Range converts to UTF32 codepoint range") {
    constexpr auto sut = sut_type::range_from(std::begin(input), std::end(input));

    REQUIRE(std::equal(sut.begin(), sut.end(), expected.begin(), expected.end()));
  }
}

//=============================================================================
// class : encoding_utilities::latin1
//=============================================================================

// Can't use latin1 literals here, since the compiler interprets the literals
// in UTF-8 due to the formatting of this file.

TEST_CASE("encoding_utilities::latin1::to_ansi(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::latin1;
  using other_type = encoding_utilities::ansi;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "_____________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_ansi(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces output with untranslatable characters") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::latin1::to_latin1(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::latin1;
  using other_type = encoding_utilities::latin1;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  constexpr auto expected = input;
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_latin1(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces identical output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::latin1::to_wide(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::latin1;
  using other_type = encoding_utilities::wide;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  constexpr auto expected = encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};
  const auto output_string = encoded_string{output.data(), output.size()};

  // Act
  auto it = sut_type::to_wide(input.begin(), input.end(), output.begin());

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::latin1::to_utf8(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::latin1;
  using other_type = encoding_utilities::utf8;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  constexpr auto expected = encoded_string{
    u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    u8"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf8(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::latin1::to_utf16(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::latin1;
  using other_type = encoding_utilities::utf16;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  constexpr auto expected = encoded_string{
    u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    u"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf16(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::latin1::to_utf32(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::latin1;
  using other_type = encoding_utilities::utf32;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    U"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf32(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::latin1::range_from(ForwardIt, ForwardIt)") {
  using sut_type = encoding_utilities::latin1;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string = std::basic_string_view<char32>;

  constexpr auto input = pre_encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    U"®¯°±²³´µ¶·¸¹º"
  };

  SECTION("Range converts to UTF32 codepoint range") {
    constexpr auto sut = sut_type::range_from(std::begin(input), std::end(input));

    REQUIRE(std::equal(sut.begin(), sut.end(), expected.begin(), expected.end()));
  }
}

//=============================================================================
// class : encoding_utilities::wide
//=============================================================================

TEST_CASE("encoding_utilities::wide::to_ansi(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::wide;
  using other_type = encoding_utilities::ansi;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "_____________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_ansi(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::wide::to_latin1(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::wide;
  using other_type = encoding_utilities::latin1;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_latin1(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::wide::to_wide(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::wide;
  using other_type = encoding_utilities::wide;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  constexpr auto expected = encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_wide(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::wide::to_utf8(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::wide;
  using other_type = encoding_utilities::utf8;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  constexpr auto expected = encoded_string{
    u8"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    u8"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf8(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::wide::to_utf16(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::wide;
  using other_type = encoding_utilities::utf16;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  constexpr auto expected = encoded_string{
    u"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    u"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf16(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::wide::to_utf32(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::wide;
  using other_type = encoding_utilities::utf32;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    U"®¯°±²³´µ¶·¸¹º"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf32(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}


//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::wide::range_from(ForwardIt, ForwardIt)") {
  using sut_type = encoding_utilities::wide;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string = std::basic_string_view<char32>;

  constexpr auto input = pre_encoded_string{
    L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    L"®¯°±²³´µ¶·¸¹º"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    U"®¯°±²³´µ¶·¸¹º"
  };

  SECTION("Range converts to UTF32 codepoint range") {
    constexpr auto sut = sut_type::range_from(std::begin(input), std::end(input));

    REQUIRE(std::equal(sut.begin(), sut.end(), expected.begin(), expected.end()));
  }
}

//=============================================================================
// class : encoding_utilities::utf8
//=============================================================================

TEST_CASE("encoding_utilities::utf8::to_ansi(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf8;
  using other_type = encoding_utilities::ansi;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijk"
    "_____________"
    "_____________"
    "_______"
    "________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_ansi(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf8::to_latin1(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf8;
  using other_type = encoding_utilities::latin1;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijk"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
    "\xb5\xa2_\xc6\xc7\xd8_______"
    "_______"
    "________"
  };

  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_latin1(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

#if 0
TEST_CASE("encoding_utilities::utf8::to_wide(ForwardIt, ForwardIt, char32)","[skip]") {
  // Arrange
  using sut_type = encoding_utilities::utf8;
  using char_type = wchar;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    L"abcdefghijk"
    L"®¯°±²³´µ¶·¸¹º"
    L"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    L"______"
    L"________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_wide(input.begin(), input.end(), output.begin(), L'_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}
#endif

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf8::to_utf8(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf8;
  using other_type = sut_type;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = input;
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf8(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf8::to_utf16(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf8;
  using other_type = encoding_utilities::utf16;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf16(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    auto input_length = sut_type::length(input.begin(), input.end());
    auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length.count() == output_length.count());
  }
  SECTION("Produces expected output") {
    REQUIRE(expected == output_string);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf8::to_utf32(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf8;
  using other_type = encoding_utilities::utf32;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf32(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(expected == output_string);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf8::range_from(ForwardIt, ForwardIt)") {
  using sut_type = encoding_utilities::utf8;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string = std::basic_string_view<char32>;

  constexpr auto input = pre_encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };

  SECTION("Range converts to UTF32 codepoint range") {
    constexpr auto sut = sut_type::range_from(std::begin(input), std::end(input));

    REQUIRE(std::equal(sut.begin(), sut.end(), expected.begin(), expected.end()));
  }
}

//=============================================================================
// class : encoding_utilities::utf16
//=============================================================================

TEST_CASE("encoding_utilities::utf16::to_ansi(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf16;
  using other_type = encoding_utilities::ansi;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijk"
    "_____________"
    "_____________"
    "_______"
    "________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_ansi(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length.count() == output_length.count());
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf16::to_latin1(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf16;
  using other_type = encoding_utilities::latin1;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijk"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
    "\xb5\xa2_\xc6\xc7\xd8_______"
    "_______"
    "________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_latin1(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf16::to_wide(ForwardIt, ForwardIt, char32)", "[hide]") {
  // TODO(bitwize): Test wide-encoding. This is hard to do cross-platform.
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf16::to_utf8(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf16;
  using other_type = encoding_utilities::utf8;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf8(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(expected == output_string);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf16::to_utf16(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf16;
  using other_type = encoding_utilities::utf16;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = input;
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf16(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf16::to_utf32(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf16;
  using other_type = encoding_utilities::utf32;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf32(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(expected == output_string);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf16::range_from(ForwardIt, ForwardIt)") {
  using sut_type = encoding_utilities::utf16;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string = std::basic_string_view<char32>;

  constexpr auto input = pre_encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };

  SECTION("Range converts to UTF32 codepoint range") {
    constexpr auto sut = sut_type::range_from(std::begin(input), std::end(input));

    REQUIRE(std::equal(sut.begin(), sut.end(), expected.begin(), expected.end()));
  }
}

//=============================================================================
// class : encoding_utilities::utf32
//=============================================================================

TEST_CASE("encoding_utilities::utf32::to_ansi(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf32;
  using other_type = encoding_utilities::ansi;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijk"
    "_____________"
    "_____________"
    "_______"
    "________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_ansi(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length.count() == output_length.count());
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf32::to_latin1(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf32;
  using other_type = encoding_utilities::latin1;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    "abcdefghijk"
    "\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba"
    "\xb5\xa2_\xc6\xc7\xd8_______"
    "_______"
    "________"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_latin1(input.begin(), input.end(), output.begin(), '_');
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf32::to_wide(ForwardIt, ForwardIt, char32)", "[hide]") {
  // TODO(bitwize): Test wide-encoding. This is hard to do cross-platform.
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf32::to_utf8(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf32;
  using other_type = encoding_utilities::utf8;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    u8"abcdefghijk"
    u8"®¯°±²³´µ¶·¸¹º"
    u8"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u8"ऄख़ઊણ௵ඔ"
    u8"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf8(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(expected == output_string);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf32::to_utf16(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf32;
  using other_type = encoding_utilities::utf16;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = encoded_string{
    u"abcdefghijk"
    u"®¯°±²³´µ¶·¸¹º"
    u"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    u"ऄख़ઊણ௵ඔ"
    u"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf16(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(expected == output_string);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf32::to_utf32(ForwardIt, ForwardIt, char32)") {
  // Arrange
  using sut_type   = encoding_utilities::utf32;
  using other_type = encoding_utilities::utf32;
  using char_type  = typename other_type::char_type;
  using pre_encoded_string = std::basic_string_view<typename sut_type::char_type>;
  using encoded_string     = std::basic_string_view<char_type>;

  constexpr auto input = pre_encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = input;
  auto output = std::array<char_type,expected.size()>{};

  // Act
  auto it = sut_type::to_utf32(input.begin(), input.end(), output.begin());
  const auto output_string = encoded_string{output.data(), output.size()};

  // Asserts
  SECTION("Contains the same codepoints as input") {
    const auto input_length = sut_type::length(input.begin(), input.end());
    const auto output_length = other_type::length(expected.begin(), expected.end());

    REQUIRE(input_length == output_length);
  }
  SECTION("Produces expected output") {
    REQUIRE(output_string == expected);
  }
  SECTION("Iterator points to end of output sequence") {
    REQUIRE(it == output.end());
  }
}

//-----------------------------------------------------------------------------

TEST_CASE("encoding_utilities::utf32::range_from(ForwardIt, ForwardIt)") {
  using sut_type = encoding_utilities::utf32;
  using encoded_string = std::basic_string_view<char32>;

  constexpr auto input = encoded_string{
    U"abcdefghijk"
    U"®¯°±²³´µ¶·¸¹º"
    U"µ¢€ÆÇØĜɆΘΩΨΦΣ"
    U"ऄख़ઊણ௵ඔ"
    U"𒎆𒍽𓁇𓃄𓃏🔨🔑📫"
  };
  constexpr auto expected = input;

  SECTION("Range converts to UTF32 codepoint range") {
    constexpr auto sut = sut_type::range_from(std::begin(input), std::end(input));

    REQUIRE(std::equal(sut.begin(), sut.end(), expected.begin(), expected.end()));
  }
}

} // namespace <anonymous>
} // namespace alloy::core::test
