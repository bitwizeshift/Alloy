/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

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

#include <alloy/core/utilities/string_tokenizer.hpp>

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
