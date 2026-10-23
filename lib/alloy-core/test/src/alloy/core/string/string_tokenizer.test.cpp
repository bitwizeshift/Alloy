// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/string/string_tokenizer.hpp>

#include <alloy/core/intrinsics.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {
namespace {

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::string_tokenizer(string_type, string_type)", "[ctor]")
{
  // Arrange
  const auto input = "Hello World";
  const auto delimiters = " ";

  // Act
  auto sut = string_tokenizer(input, delimiters);

  // Assert
  SECTION("Has non-empty buffer") {
    REQUIRE(sut.buffer().size() > 0);
  }

  SECTION("Has specified delimiters") {
    REQUIRE(sut.delimiters() == delimiters);
  }

  SECTION("Has specifeid buffer") {
    REQUIRE(sut.buffer() == input);
  }
}

//----------------------------------------------------------------------------
// Accessors
//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::buffer()")
{
  // Arrange
  const auto input = "Hello World";
  auto sut = string_tokenizer(input, " ");

  SECTION("Returns the buffer") {
    // Act
    const auto result = sut.buffer();

    // Assert
    REQUIRE(result == input);
  }
}

//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::delimiters()")
{
  // Arrange
  const auto delimiters = "\n\r\t";
  auto sut = string_tokenizer("",delimiters);

  SECTION("Returns the delimiter") {
    // Act
    const auto result = sut.delimiters();

    // Assert
    REQUIRE(result == delimiters);
  }
}

//----------------------------------------------------------------------------
// Tokenizing
//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::has_more()")
{
  SECTION("Doesn't have more tokens with empty string") {
    // Arrange
    auto sut = string_tokenizer("", " ");

    // Act
    const auto result = sut.has_more();

    // Assert
    REQUIRE_FALSE(result);
  }

  SECTION("Has more before tokenizing") {
    // Arrange
    auto sut = string_tokenizer("Hello World", " ");

    // Act
    const auto result = sut.has_more();

    // Assert
    REQUIRE(result);
  }

  SECTION("Doesn't have more after tokenizing") {
    // Arrange
    auto sut = string_tokenizer("Hello World", " ");

    alloy::core::compiler::unused(sut.next());
    alloy::core::compiler::unused(sut.next());

    // Act
    const auto result = sut.has_more();

    // Assert
    REQUIRE_FALSE(result);
  }
}

//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::next()")
{
  SECTION("Tokenize empty string") {
    // Arrange
    auto sut = string_tokenizer("", " ");

    // Act
    auto token = sut.next();

    // Assert
    SECTION("Result is an error") {
      REQUIRE(token.has_error());
    }
    SECTION("Result is out_of_tokens error") {
      REQUIRE(token.error() == tokenizer_error::out_of_tokens);
    }
  }

  SECTION("Tokenize delimited string") {
    // Arrange
    auto sut = string_tokenizer("Hello World", " ");

    SECTION("Retrieves first token") {
      // Act
      const auto result = sut.next();

      // Assert
      REQUIRE(result == "Hello");
    }

    SECTION("Retrieves second token") {
      alloy::core::compiler::unused(sut.next());

      // Act
      const auto result = sut.next();

      // Assert
      REQUIRE(result == "World");
    }

    SECTION("Retrieves empty string token") {
      alloy::core::compiler::unused(sut.next());
      alloy::core::compiler::unused(sut.next());

      // Act
      auto token = sut.next();

      // Assert
      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String contains only delimiter") {
    // Arrange
    auto sut = string_tokenizer(";", ";");

    SECTION("Token is empty") {
      // Act
      const auto result = sut.next();

      // Assert
      REQUIRE(result == "");
    }

    SECTION("Retrieves empty string token second") {
      alloy::core::compiler::unused(sut.next());

      // Act
      auto token = sut.next();

      // Assert
      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String ends in delimiter") {
    // Arrange
    auto sut = string_tokenizer("hello;", ";");

    SECTION("Token is first string") {
      // Act
      const auto result = sut.next();

      // Assert
      REQUIRE(result == "hello");
    }

    SECTION("Second token is an error") {
      alloy::core::compiler::unused(sut.next());

      // Act
      auto token = sut.next();

      // Assert
      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }
}

//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::next(const string_type&)")
{
  SECTION("Tokenize empty string") {
    // Arrange
    auto sut = string_tokenizer("", "_");

    // Act
    auto token = sut.next(" ");

    // Assert
    SECTION("Result is an error") {
      REQUIRE(token.has_error());
    }
    SECTION("Result is out_of_tokens error") {
      REQUIRE(token.error() == tokenizer_error::out_of_tokens);
    }
  }

  SECTION("Tokenize delimited string") {
    // Arrange
    auto sut = string_tokenizer("Hello World", "_");

    SECTION("First token is first word") {
      // Act
      const auto result = sut.next(" ");

      // Assert
      REQUIRE(result == "Hello");
    }

    SECTION("Second token is an error") {
      alloy::core::compiler::unused(sut.next());

      // Act
      auto token = sut.next();

      // Assert
      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String contains only delimiter") {
    // Arrange
    auto sut = string_tokenizer(";", "_");

    SECTION("Token is empty") {
      // Act
      const auto result = sut.next(";");

      // Assert
      REQUIRE(result == "");
    }

    SECTION("Second token is empty") {
      alloy::core::compiler::unused(sut.next());

      // Act
      auto token = sut.next(";");

      // Assert
      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String ends in delimiter") {
    // Arrange
    auto sut = string_tokenizer("hello;", "_");

    SECTION("First token is first word") {
      // Act
      const auto result = sut.next(";");

      // Assert
      REQUIRE(result == "hello");
    }

    SECTION("Second token is an error") {
      alloy::core::compiler::unused(sut.next());

      // Act
      auto token = sut.next();

      // Assert
      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }
}

} // namespace <anonymous>
} // namespace alloy::core::test
