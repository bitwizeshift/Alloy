// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/camera.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix4.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/math/vector/vector3_constants.hpp>
#include <alloy/core/math/vector/vector4.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Local Translations
//------------------------------------------------------------------------------

TEST_CASE("camera::translate_local_x( real )", "[translation]")
{
  SECTION("Camera that looks at a subject moves along its x-axis")
  {
    // Arrange
    auto sut = camera{};
    sut.set_origin(5, 0, 0);
    sut.look_at(vector3_constants::zero, vector3_constants::unit_y);

    // Act
    sut.translate_local_x(1);

    // Assert
    REQUIRE( almost_equal(sut.translation(), vector3{5, 0, -1}) );
  }

  SECTION("Yawed camera moves along its rotated x-axis")
  {
    // Arrange
    auto sut = camera{};
    sut.rotate_yaw(radian_constants::quarter_revolution);

    // Act
    sut.translate_local_x(1);

    // Assert
    REQUIRE( almost_equal(sut.translation(), vector3{0, 0, 1}) );
  }
}

TEST_CASE("camera::translate_local_y( real )", "[translation]")
{
  // Arrange
  auto sut = camera{};
  sut.rotate_yaw(radian_constants::quarter_revolution);
  sut.rotate_pitch(radian_constants::quarter_revolution);

  // Act
  sut.translate_local_y(1);

  // Assert
  SECTION("Rotated camera moves along its rotated y-axis")
  {
    REQUIRE( almost_equal(sut.translation(), vector3{0, 0, -1}) );
  }
}

TEST_CASE("camera::translate_local_z( real )", "[translation]")
{
  SECTION("Camera that looks at a subject moves away from it")
  {
    // Arrange
    auto sut = camera{};
    sut.set_origin(5, 0, 0);
    sut.look_at(vector3_constants::zero, vector3_constants::unit_y);

    // Act
    sut.translate_local_z(1);

    // Assert
    REQUIRE( almost_equal(sut.translation(), vector3{6, 0, 0}) );
  }

  SECTION("Rotated camera moves along its rotated z-axis")
  {
    // Arrange
    auto sut = camera{};
    sut.rotate_yaw(radian_constants::quarter_revolution);
    sut.rotate_pitch(radian_constants::quarter_revolution);

    // Act
    sut.translate_local_z(1);

    // Assert
    REQUIRE( almost_equal(sut.translation(), vector3{-1, 0, 0}) );
  }
}

//------------------------------------------------------------------------------
// Rotations
//------------------------------------------------------------------------------

TEST_CASE("camera::look_at( const vector3&, const vector3& )", "[rotation]")
{
  // Arrange
  auto sut = camera{};
  sut.set_origin(5, 0, 0);

  // Act
  sut.look_at(vector3_constants::zero, vector3_constants::unit_y);
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("Subject is in front of the camera")
  {
    REQUIRE( almost_equal(result * vector4{0, 0, 0, 1}, vector4{0, 0, -5, 1}) );
  }
  SECTION("Up direction is the camera's y-axis")
  {
    REQUIRE( almost_equal(result * vector4{5, 1, 0, 1}, vector4{0, 1, 0, 1}) );
  }
}

TEST_CASE("camera::rotate_yaw( radian )", "[rotation]")
{
  // Arrange
  auto sut = camera{};

  // Act
  sut.rotate_yaw(radian_constants::quarter_revolution);
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("View matrix rotates the world by the yaw")
  {
    REQUIRE( almost_equal(result * vector4{0, 0, 1, 1}, vector4{1, 0, 0, 1}) );
  }
}

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

TEST_CASE("camera::to_matrix4()", "[conversion]")
{
  // Arrange
  auto sut = camera{};
  sut.set_origin(0, 0, 5);
  sut.look_at(vector3_constants::zero, vector3_constants::unit_y);
  const auto expected = matrix4{
    1, 0, 0,  0,
    0, 1, 0,  0,
    0, 0, 1, -5,
    0, 0, 0,  1
  };

  // Act
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("Rotates into camera space after translating the camera to the origin")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

} // namespace alloy::core::test
