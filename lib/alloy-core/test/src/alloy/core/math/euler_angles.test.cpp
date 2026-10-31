// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/euler_angles.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Quantifiers
//------------------------------------------------------------------------------

TEST_CASE("euler_angles::dot( const euler_angles& )", "[quantifiers]")
{
  // Arrange
  const auto lhs = euler_angles{radian{1}, radian{2}, radian{3}};
  const auto rhs = euler_angles{radian{4}, radian{5}, radian{6}};

  // Act
  const auto result = lhs.dot(rhs);

  // Assert
  SECTION("Sums the products of every angle")
  {
    REQUIRE( almost_equal(result, 32.0f) );
  }
}

//------------------------------------------------------------------------------
// Comparisons
//------------------------------------------------------------------------------

TEST_CASE("almost_equal( const euler_angles&, const euler_angles&, float )", "[comparison]")
{
  // Arrange
  const auto lhs = euler_angles{radian{1}, radian{2}, radian{3}};

  SECTION("Angles within tolerance are almost equal")
  {
    // Arrange
    const auto rhs = euler_angles{radian{1}, radian{2.05f}, radian{3}};

    // Act
    const auto result = almost_equal(lhs, rhs, 0.1f);

    // Assert
    REQUIRE( result );
  }

  SECTION("Angles outside tolerance are not almost equal")
  {
    // Arrange
    const auto rhs = euler_angles{radian{1}, radian{2}, radian{3.5f}};

    // Act
    const auto result = almost_equal(lhs, rhs, 0.1f);

    // Assert
    REQUIRE_FALSE( result );
  }
}

} // namespace alloy::core::test
