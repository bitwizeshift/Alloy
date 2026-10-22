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
// Quantifiers
//------------------------------------------------------------------------------

TEST_CASE("quaternion::inverse()", "[quantifiers]")
{
  SECTION("Product with a non-unit quaternion is the identity")
  {
    // Arrange
    const auto sut = quaternion{1, 2, 3, 4};

    // Act
    const auto result = sut * sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, quaternion_constants::identity) );
  }

  SECTION("Inverse of a unit quaternion is its conjugate")
  {
    // Arrange
    const auto sut = quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3{1, 0, 0}
    );
    const auto expected = quaternion{sut.w(), -sut.x(), -sut.y(), -sut.z()};

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Zero quaternion inverts to zero")
  {
    // Arrange
    const auto sut = quaternion_constants::zero;

    // Act
    const auto result = sut.inverse();

    // Assert
    REQUIRE( result == quaternion_constants::zero );
  }
}

TEST_CASE("quaternion::conjugated()", "[quantifiers]")
{
  SECTION("Negates the imaginary parts")
  {
    // Arrange
    const auto sut = quaternion{1, 2, 3, 4};

    // Act
    const auto result = sut.conjugated();

    // Assert
    REQUIRE( result == quaternion{1, -2, -3, -4} );
  }

  SECTION("Conjugate of a rotation undoes the rotation")
  {
    // Arrange
    const auto sut = quaternion::from_angles(
      radian{0.3f},
      radian{0.5f},
      radian{0.7f}
    );
    const auto vec = vector3{1, 2, 3};

    // Act
    const auto result = sut.conjugated() * (sut * vec);

    // Assert
    REQUIRE( almost_equal(result, vec) );
  }
}

TEST_CASE("quaternion::roll()", "[quantifiers]")
{
  // Arrange
  const auto sut = quaternion{0, 0, 0, 1};

  SECTION("Half turn about the z-axis is a roll of pi")
  {
    // Act
    const auto result = sut.roll();

    // Assert
    REQUIRE( almost_equal(result, radian_constants::half_revolution) );
  }

  SECTION("Reprojected half turn about the z-axis is a roll of pi")
  {
    // Act
    const auto result = sut.roll(reproject_axis);

    // Assert
    REQUIRE( almost_equal(result, radian_constants::half_revolution) );
  }
}

TEST_CASE("quaternion::pitch()", "[quantifiers]")
{
  // Arrange
  const auto sut = quaternion{0, 1, 0, 0};

  SECTION("Half turn about the x-axis is a pitch of pi")
  {
    // Act
    const auto result = sut.pitch();

    // Assert
    REQUIRE( almost_equal(result, radian_constants::half_revolution) );
  }

  SECTION("Reprojected half turn about the x-axis is a pitch of pi")
  {
    // Act
    const auto result = sut.pitch(reproject_axis);

    // Assert
    REQUIRE( almost_equal(result, radian_constants::half_revolution) );
  }
}

TEST_CASE("quaternion::yaw()", "[quantifiers]")
{
  SECTION("Rotation about the y-axis is a yaw of its angle")
  {
    // Arrange
    const auto angle = radian{0.5f};
    const auto sut = quaternion::from_angle_axis(angle, vector3{0, 1, 0});

    // Act
    const auto result = sut.yaw();

    // Assert
    REQUIRE( almost_equal(result, angle) );
  }

  SECTION("Reprojected half turn about the y-axis is a yaw of pi")
  {
    // Arrange
    const auto sut = quaternion{0, 0, 1, 0};

    // Act
    const auto result = sut.yaw(reproject_axis);

    // Assert
    REQUIRE( almost_equal(result, radian_constants::half_revolution) );
  }
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

TEST_CASE("quaternion::normalize()", "[modifiers]")
{
  SECTION("Non-unit quaternion becomes unit length")
  {
    // Arrange
    auto sut = quaternion{1, 1, 1, 1};

    // Act
    sut.normalize();

    // Assert
    REQUIRE( almost_equal(sut, quaternion{0.5f, 0.5f, 0.5f, 0.5f}) );
  }

  SECTION("Zero quaternion is left unchanged")
  {
    // Arrange
    auto sut = quaternion_constants::zero;

    // Act
    sut.normalize();

    // Assert
    REQUIRE( sut == quaternion_constants::zero );
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
