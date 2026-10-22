// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/quaternion.hpp>

#include <alloy/core/math/quaternion_constants.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Arithmetic Operators
//------------------------------------------------------------------------------

TEST_CASE("operator+( const quaternion&, const quaternion& )", "[arithmetic]")
{
  // Arrange
  const auto lhs = quaternion{1, 2, 3, 4};

  // Act
  const auto result = lhs + quaternion_constants::zero;

  // Assert
  SECTION("Adding the zero quaternion leaves the quaternion unchanged")
  {
    REQUIRE( result == lhs );
  }
}

} // namespace alloy::core::test
