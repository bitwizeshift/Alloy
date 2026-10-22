// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/euler_angles.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>

#include <alloy/core/precision/real.hpp>

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
    REQUIRE( almost_equal(result, real{32}) );
  }
}

} // namespace alloy::core::test
