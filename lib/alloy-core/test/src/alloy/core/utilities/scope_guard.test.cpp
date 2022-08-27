/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include <alloy/core/utilities/scope_guard.hpp>

#include <catch2/catch.hpp>

namespace alloy::core::test {

TEST_CASE("scope_exit( Fn&& fn )", "[raii]")
{
  bool flag = false;

  SECTION("Executes on clean scope exit")
  {
    {
      auto scope = scope_exit{[&]()
      {
        flag = true;
      }};
    }

    REQUIRE( flag == true );
  }

  SECTION("Executes on exception scope exit")
  {
    try {
      auto scope = scope_exit{[&]()
      {
        flag = true;
      }};
      throw std::exception{};
    } catch (...) {
      // do nothing
    }

    REQUIRE( flag == true );
  }
}

TEST_CASE("ALLOY_ON_SCOPE_EXIT(x)", "[raii]")
{
  bool flag = false;

  SECTION("Executes on clean scope exit")
  {
    {
      ALLOY_ON_SCOPE_EXIT([&]()
      {
        flag = true;
      });
    }

    REQUIRE( flag == true );
  }

  SECTION("Executes on exception scope exit")
  {
    try {
      ALLOY_ON_SCOPE_EXIT([&]()
      {
        flag = true;
      });
      throw std::exception{};
    } catch (...) {
      // do nothing
    }

    REQUIRE( flag == true );
  }
}

} // namespace alloy::core::test
