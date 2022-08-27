/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include <alloy/core/string/string_tokenizer.hpp>

#include <catch2/catch.hpp>

namespace alloy::core::test {
namespace {

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::string_tokenizer(string_type, string_type)", "[ctor]")
{
  const auto input = "Hello World";
  const auto delimiters = " ";
  auto tokenizer = string_tokenizer(input, delimiters);

  SECTION("Has non-empty buffer") {
    REQUIRE(tokenizer.buffer().size() > 0);
  }

  SECTION("Has specified delimiters") {
    REQUIRE(tokenizer.delimiters() == delimiters);
  }

  SECTION("Has specifeid buffer") {
    REQUIRE(tokenizer.buffer() == input);
  }
}

//----------------------------------------------------------------------------
// Accessors
//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::buffer()")
{
  const auto input = "Hello World";
  auto tokenizer = string_tokenizer(input, " ");

  SECTION("Returns the buffer") {
    REQUIRE(tokenizer.buffer() == input);
  }
}

//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::delimiters()")
{
  const auto delimiters = "\n\r\t";
  auto tokenizer = string_tokenizer("",delimiters);

  SECTION("Returns the delimiter") {
    REQUIRE(tokenizer.delimiters() == delimiters);
  }
}

//----------------------------------------------------------------------------
// Tokenizing
//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::has_more()")
{
  SECTION("Doesn't have more tokens with empty string") {
    auto tokenizer = string_tokenizer("", " ");

    REQUIRE_FALSE(tokenizer.has_more());
  }

  SECTION("Has more before tokenizing") {
    auto tokenizer = string_tokenizer("Hello World", " ");

    REQUIRE(tokenizer.has_more());
  }

  SECTION("Doesn't have more after tokenizing") {
    auto tokenizer = string_tokenizer("Hello World", " ");

    alloy::core::compiler::unused(tokenizer.next());
    alloy::core::compiler::unused(tokenizer.next());

    REQUIRE_FALSE(tokenizer.has_more());
  }
}

//----------------------------------------------------------------------------

TEST_CASE("string_tokenizer::next()")
{
  SECTION("Tokenize empty string") {
    auto tokenizer = string_tokenizer("", " ");

    auto token = tokenizer.next();

    SECTION("Result is an error") {
      REQUIRE(token.has_error());
    }
    SECTION("Result is out_of_tokens error") {
      REQUIRE(token.error() == tokenizer_error::out_of_tokens);
    }
  }

  SECTION("Tokenize delimited string") {
    auto tokenizer = string_tokenizer("Hello World", " ");

    SECTION("Retrieves first token") {
      REQUIRE(tokenizer.next() == "Hello");
    }

    SECTION("Retrieves second token") {
      alloy::core::compiler::unused(tokenizer.next());

      REQUIRE(tokenizer.next() == "World");
    }

    SECTION("Retrieves empty string token") {
      alloy::core::compiler::unused(tokenizer.next());
      alloy::core::compiler::unused(tokenizer.next());

      auto token = tokenizer.next();

      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String contains only delimiter") {
    auto tokenizer = string_tokenizer(";", ";");

    SECTION("Token is empty") {
      REQUIRE(tokenizer.next() == "");
    }

    SECTION("Retrieves empty string token second") {
      alloy::core::compiler::unused(tokenizer.next());

      auto token = tokenizer.next();

      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String ends in delimiter") {
    auto tokenizer = string_tokenizer("hello;", ";");

    SECTION("Token is first string") {
      REQUIRE(tokenizer.next() == "hello");
    }

    SECTION("Second token is an error") {
      alloy::core::compiler::unused(tokenizer.next());
      auto token = tokenizer.next();

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
    auto tokenizer = string_tokenizer("", "_");

    auto token = tokenizer.next(" ");

    SECTION("Result is an error") {
      REQUIRE(token.has_error());
    }
    SECTION("Result is out_of_tokens error") {
      REQUIRE(token.error() == tokenizer_error::out_of_tokens);
    }
  }

  SECTION("Tokenize delimited string") {
    auto tokenizer = string_tokenizer("Hello World", "_");

    SECTION("First token is first word") {
      REQUIRE(tokenizer.next(" ") == "Hello");
    }

    SECTION("Second token is an error") {
      alloy::core::compiler::unused(tokenizer.next());
      auto token = tokenizer.next();

      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String contains only delimiter") {
    auto tokenizer = string_tokenizer(";", "_");

    SECTION("Token is empty") {
      REQUIRE(tokenizer.next(";") == "");
    }

    SECTION("Second token is empty") {
      alloy::core::compiler::unused(tokenizer.next());

      auto token = tokenizer.next(";");

      SECTION("Result is an error") {
        REQUIRE(token.has_error());
      }
      SECTION("Result is out_of_tokens error") {
        REQUIRE(token.error() == tokenizer_error::out_of_tokens);
      }
    }
  }

  SECTION("String ends in delimiter") {
    auto tokenizer = string_tokenizer("hello;", "_");

    SECTION("First token is first word") {
      REQUIRE(tokenizer.next(";") == "hello");
    }

    SECTION("Second token is an error") {
      alloy::core::compiler::unused(tokenizer.next());
      auto token = tokenizer.next();

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
