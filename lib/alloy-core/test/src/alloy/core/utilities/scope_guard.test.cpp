// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/utilities/scope_guard.hpp>

#include <catch2/catch_test_macros.hpp>

#include <exception> // std::exception
#include <utility>   // std::move

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

TEST_CASE("scope_exit( scope_exit&& )", "[raii]")
{
  auto count = 0;

  SECTION("Executes the function once when both guards exit scope")
  {
    {
      // Arrange
      auto source = scope_exit{[&count]()
      {
        ++count;
      }};

      // Act
      auto sut = std::move(source);
    }

    // Assert
    REQUIRE( count == 1 );
  }

  SECTION("Transfers execution to the constructed guard")
  {
    // Arrange
    auto source = scope_exit{[&count]()
    {
      ++count;
    }};

    // Act
    auto sut = std::move(source);

    // Assert
    SECTION("Constructed guard will execute")
    {
      REQUIRE( sut.should_execute() );
    }
    SECTION("Moved-from guard will not execute")
    {
      // NOLINTNEXTLINE(bugprone-use-after-move)
      REQUIRE_FALSE( source.should_execute() );
    }
  }

  SECTION("Releasing the constructed guard prevents execution")
  {
    {
      // Arrange
      auto source = scope_exit{[&count]()
      {
        ++count;
      }};
      auto sut = std::move(source);

      // Act
      sut.release();
    }

    // Assert
    REQUIRE( count == 0 );
  }
}

TEST_CASE("scope_success( scope_success&& )", "[raii]")
{
  auto count = 0;

  SECTION("Executes the function once on clean scope exit")
  {
    {
      // Arrange
      auto source = scope_success{[&count]()
      {
        ++count;
      }};

      // Act
      auto sut = std::move(source);
    }

    // Assert
    REQUIRE( count == 1 );
  }
}

TEST_CASE("scope_fail( scope_fail&& )", "[raii]")
{
  auto count = 0;

  SECTION("Executes the function once on exception scope exit")
  {
    // Arrange
    const auto throw_in_scope = [&count]()
    {
      auto source = scope_fail{[&count]()
      {
        ++count;
      }};

      // Act
      auto sut = std::move(source);
      throw std::exception{};
    };

    REQUIRE_THROWS( throw_in_scope() );

    // Assert
    REQUIRE( count == 1 );
  }
}

} // namespace alloy::core::test
