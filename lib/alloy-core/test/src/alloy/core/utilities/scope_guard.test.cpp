// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/utilities/scope_guard.hpp>

#include <catch2/catch_test_macros.hpp>

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
    const auto throw_in_scope = [&]()
    {
      auto scope = scope_exit{[&]()
      {
        flag = true;
      }};
      throw std::exception{};
    };

    REQUIRE_THROWS( throw_in_scope() );

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
    const auto throw_in_scope = [&]()
    {
      ALLOY_ON_SCOPE_EXIT([&]()
      {
        flag = true;
      });
      throw std::exception{};
    };

    REQUIRE_THROWS( throw_in_scope() );

    REQUIRE( flag == true );
  }
}

} // namespace alloy::core::test
