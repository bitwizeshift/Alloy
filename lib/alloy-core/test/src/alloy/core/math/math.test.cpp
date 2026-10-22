// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/math.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Products
//------------------------------------------------------------------------------

TEST_CASE("dot( const T(&)[N], const U(&)[N] )", "[products]")
{
  // Arrange
  const int lhs[3] = {1, 2, 3};
  const int rhs[3] = {4, 5, 6};

  // Act
  const auto result = dot(lhs, rhs);

  // Assert
  SECTION("Sums the products of each pair of entries")
  {
    REQUIRE( result == 32 );
  }
}

TEST_CASE("dot( const T(&)[N], const U(&)[N] ) in a constant expression", "[products]")
{
  // Arrange
  static constexpr int lhs[2] = {2, 3};
  static constexpr int rhs[2] = {4, 5};

  // Act
  constexpr auto result = dot(lhs, rhs);

  // Assert
  SECTION("Is evaluated at compile time")
  {
    STATIC_REQUIRE( result == 23 );
  }
}

} // namespace alloy::core::test
