// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/io/error.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string_view>  // std::string_view
#include <system_error> // std::error_code

namespace alloy::core::test {
namespace {

//=============================================================================
// non-member functions : enum : io_error
//=============================================================================

TEST_CASE("io_error_category()", "[utilities]")
{
  SECTION("Returns the same category on every call")
  {
    // Act
    const auto* first = &io_error_category();
    const auto* second = &io_error_category();

    // Assert
    REQUIRE( first == second );
  }

  SECTION("Names the category")
  {
    // Act
    const auto name = std::string_view{io_error_category().name()};

    // Assert
    REQUIRE( name == "io error" );
  }

  SECTION("Value is not an io error")
  {
    // Act
    const auto message = io_error_category().message(0);

    // Assert
    SECTION("Describes the value as unknown")
    {
      REQUIRE( message == "unknown error code" );
    }
  }
}

TEST_CASE("make_error_code(io_error)", "[utilities]")
{
  // Arrange
  const auto sut = GENERATE(io_error::unexpected_eof, io_error::write_zero);

  // Act
  const auto ec = make_error_code(sut);

  // Assert
  SECTION("Uses the io category")
  {
    REQUIRE( ec.category() == io_error_category() );
  }
  SECTION("Uses the value of the error")
  {
    REQUIRE( ec.value() == static_cast<int>(sut) );
  }
  SECTION("Describes the error")
  {
    REQUIRE( ec.message() != "unknown error code" );
  }
  SECTION("Compares equal to the error")
  {
    REQUIRE( ec == sut );
  }
}

TEST_CASE("std::error_code(io_error)", "[utilities]")
{
  // Arrange
  const auto sut = GENERATE(io_error::unexpected_eof, io_error::write_zero);

  // Act
  const auto ec = std::error_code{sut};

  // Assert
  SECTION("Converts implicitly through make_error_code")
  {
    REQUIRE( ec == make_error_code(sut) );
  }
}

} // namespace
} // namespace alloy::core::test
