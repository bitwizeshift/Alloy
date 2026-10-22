// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/quaternion.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/quaternion_constants.hpp>
#include <alloy/core/math/vector/vector3.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("quaternion::angle_axis()", "[observers]")
{
  SECTION("Extracts the angle and axis that the quaternion was made from")
  {
    // Arrange
    const auto angle = radian_constants::quarter_revolution;
    const auto axis = vector3{0, 1, 1}.normalized();
    const auto sut = quaternion::from_angle_axis(angle, axis);

    // Act
    const auto [result_angle, result_axis] = sut.angle_axis();

    // Assert
    REQUIRE( almost_equal(result_angle, angle) );
    REQUIRE( almost_equal(result_axis, axis) );
  }

  SECTION("Identity extracts an angle of 0")
  {
    // Arrange
    const auto sut = quaternion_constants::identity;

    // Act
    const auto [result_angle, result_axis] = sut.angle_axis();

    // Assert
    REQUIRE( result_angle == radian{0} );
  }
}

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
