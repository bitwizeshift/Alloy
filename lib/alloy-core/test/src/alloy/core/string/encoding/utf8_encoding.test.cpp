/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include <alloy/core/string/encoding/utf8_encoding.hpp>

#include <catch2/catch.hpp>

#include <algorithm>
#include <array>

namespace alloy::core::test {

TEST_CASE("utf8_encoding::decode(InputIt, InputIt, char32") {
  SECTION("Input is empty") {
    const auto input = std::array<char8,0>{};
    const auto bad_output = char32{};

    const auto [value,iter] = utf8_encoding::decode(
      input.begin(),
      input.end(),
      bad_output
    );

    SECTION("Does not advance iterator") {
      REQUIRE(iter == input.end());
    }
    SECTION("Output is replacement value") {
      REQUIRE(value == bad_output);
    }
  }
  SECTION("Input is one-byte codepoint") {
    SECTION("Input is invalid byte") {
      const auto input = std::array<char8,1u>{{static_cast<char8>(255)}};
      const auto bad_output = char32{};

      const auto [value,iter] = utf8_encoding::decode(
        input.begin(),
        input.end(),
        bad_output
      );

      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
      SECTION("Output is replacement value") {
        REQUIRE(value == bad_output);
      }
    }
    SECTION("Input is valid") {
      const auto input = std::array<char8,1u>{{'a'}};
      constexpr auto expected = char32{'a'};

      const auto [value, iter] = utf8_encoding::decode(
        input.begin(),
        input.end()
      );

      SECTION("Decodes successfully") {
        REQUIRE(value == expected);
      }
      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
    }
  }
  SECTION("Input is two-byte codepoint") {
    SECTION("Input is incomplete") {
      const auto input = std::array<char8,1u>{{
        static_cast<char8>(0xc2),
      }};
      const auto bad_output = U'_';

      const auto [value, iter] = utf8_encoding::decode(
        input.begin(),
        input.end(),
        bad_output
      );

      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
      SECTION("Output is replacement value") {
        REQUIRE(value == bad_output);
      }
    };
    SECTION("Input is invalid") {
      const auto input = std::array<char8,2u>{{
        static_cast<char8>(0xff),
        static_cast<char8>(0x2c),
      }};
      const auto bad_output = U'_';

      const auto [value, iter] = utf8_encoding::decode(
        input.begin(),
        input.end(),
        bad_output
      );

      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
      SECTION("Output is replacement value") {
        REQUIRE(value == bad_output);
      }
    }
    SECTION("Input is valid") {
      const auto input = std::array<char8,2u>{{
        static_cast<char8>(0xc2),
        static_cast<char8>(0xa3)
      }};
      constexpr auto expected = char32{U'£'};

      const auto [value,iter] = utf8_encoding::decode(
        input.begin(),
        input.end()
      );

      SECTION("Decodes successfully") {
        REQUIRE(value == expected);
      }
      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
    }
  }
  SECTION("Input is three-byte codepoint") {
    SECTION("Input is incomplete") {
      const auto input = std::array<char8,1u>{{
        static_cast<char8>(0xe0),
      }};
      const auto bad_output = U'_';

      const auto [value, iter] = utf8_encoding::decode(
        input.begin(),
        input.end(),
        bad_output
      );

      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
      SECTION("Output is replacement value") {
        REQUIRE(value == bad_output);
      }
    };
    SECTION("Input is invalid") {
      const auto input = std::array<char8,3u>{{
        static_cast<char8>(0xff),
        static_cast<char8>(0x2c),
        static_cast<char8>(0xaa),
      }};
      const auto bad_output = U'_';

      const auto [value, iter] = utf8_encoding::decode(
        input.begin(),
        input.end(),
        bad_output
      );

      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
      SECTION("Output is replacement value") {
        REQUIRE(value == bad_output);
      }
    }
    SECTION("Input is valid") {
      const auto input = std::array<char8,3u>{{
        static_cast<char8>(0xe0),
        static_cast<char8>(0xaa),
        static_cast<char8>(0x86),
      }};
      constexpr auto expected = char32{U'આ'};

      const auto [value,iter] = utf8_encoding::decode(
        input.begin(),
        input.end()
      );

      SECTION("Decodes successfully") {
        REQUIRE(value == expected);
      }
      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
    }
  }
  SECTION("Input is four-byte codepoint") {
    SECTION("Input is incomplete") {
      const auto input = std::array<char8,1u>{{
        static_cast<char8>(0xf0),
      }};
      const auto bad_output = U'_';

      const auto [value, iter] = utf8_encoding::decode(
        input.begin(),
        input.end(),
        bad_output
      );

      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
      SECTION("Output is replacement value") {
        REQUIRE(value == bad_output);
      }
    };
    SECTION("Input is invalid") {
      const auto input = std::array<char8,4u>{{
        static_cast<char8>(0xff),
        static_cast<char8>(0x92),
        static_cast<char8>(0x81),
        static_cast<char8>(0x89),
      }};
      const auto bad_output = U'_';

      const auto [value, iter] = utf8_encoding::decode(
        input.begin(),
        input.end(),
        bad_output
      );

      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
      SECTION("Output is replacement value") {
        REQUIRE(value == bad_output);
      }
    }
    SECTION("Input is valid") {
      const auto input = std::array<char8,4u>{{
        static_cast<char8>(0xf0),
        static_cast<char8>(0x92),
        static_cast<char8>(0x81),
        static_cast<char8>(0x89),
      }};
      constexpr auto expected = char32{U'𒁉'};

      const auto [value,iter] = utf8_encoding::decode(
        input.begin(),
        input.end()
      );

      SECTION("Decodes successfully") {
        REQUIRE(value == expected);
      }
      SECTION("Advances iterator") {
        REQUIRE(iter == input.end());
      }
    }
  }
}

TEST_CASE("utf8_encoding::encode(char32, OutputIt, char8)") {
  // TODO(bitwize): add tests
}

TEST_CASE("utf8_encoding::length(ForwardIt, ForwardIt)") {
  // TODO(bitwize): add tests
}

} // namespace alloy::core::test
