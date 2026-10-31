// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/camera.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/euler_angles.hpp>
#include <alloy/core/math/interpolation.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix4.hpp>
#include <alloy/core/math/matrix/matrix4_constants.hpp>
#include <alloy/core/math/quaternion.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/math/vector/vector3_constants.hpp>
#include <alloy/core/math/vector/vector4.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// World Translations
//------------------------------------------------------------------------------

TEST_CASE("camera::translate_world( float, float, float )", "[translation]")
{
  // Arrange
  auto sut = camera{};
  sut.set_translation(1, 2, 3);
  sut.rotate_yaw(radian_constants::quarter_revolution);

  // Act
  sut.translate_world(4, 5, 6);
  const auto result = sut.translation();

  // Assert
  SECTION("Moves along the world axes regardless of orientation")
  {
    REQUIRE( almost_equal(result, vector3{5, 7, 9}) );
  }
}

//------------------------------------------------------------------------------
// Local Translations
//------------------------------------------------------------------------------

TEST_CASE("camera::translate_local( const vector3& )", "[translation]")
{
  // Arrange
  auto sut = camera{};
  sut.rotate_yaw(radian_constants::quarter_revolution);

  // Act
  sut.translate_local(vector3{1, 2, 3});
  const auto result = sut.translation();

  // Assert
  SECTION("Moves right, up, and forward along the camera's axes")
  {
    REQUIRE( almost_equal(result, vector3{-3, 2, -1}, 1e-5f) );
  }
}

TEST_CASE("camera::translate_local_x( float )", "[translation]")
{
  SECTION("Camera that looks at a subject moves along its x-axis")
  {
    // Arrange
    auto sut = camera{};
    sut.set_translation(5, 0, 0);
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
    REQUIRE( almost_equal(sut.translation(), vector3{0, 0, -1}) );
  }
}

TEST_CASE("camera::translate_local_y( float )", "[translation]")
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
    REQUIRE( almost_equal(sut.translation(), vector3{1, 0, 0}) );
  }
}

TEST_CASE("camera::translate_local_z( float )", "[translation]")
{
  SECTION("Unrotated camera moves down the negative z-axis")
  {
    // Arrange
    auto sut = camera{};

    // Act
    sut.translate_local_z(1);

    // Assert
    REQUIRE( almost_equal(sut.translation(), vector3{0, 0, -1}) );
  }

  SECTION("Camera that looks at a subject moves towards it")
  {
    // Arrange
    auto sut = camera{};
    sut.set_translation(5, 0, 0);
    sut.look_at(vector3_constants::zero, vector3_constants::unit_y);

    // Act
    sut.translate_local_z(1);

    // Assert
    REQUIRE( almost_equal(sut.translation(), vector3{4, 0, 0}) );
  }

  SECTION("Rotated camera moves in the direction it faces")
  {
    // Arrange
    auto sut = camera{};
    sut.rotate_yaw(radian_constants::quarter_revolution);
    sut.rotate_pitch(radian_constants::quarter_revolution);

    // Act
    sut.translate_local_z(1);

    // Assert
    REQUIRE( almost_equal(sut.translation(), vector3{0, 1, 0}) );
  }
}

//------------------------------------------------------------------------------
// Rotations
//------------------------------------------------------------------------------

TEST_CASE("camera::look_at( float, float, float, const vector3& )", "[rotation]")
{
  // Arrange
  auto sut = camera{};
  sut.set_translation(5, 0, 0);

  // Act
  sut.look_at(5, 0, -5, vector3_constants::unit_x);
  const auto forward = sut.forward();
  const auto up = sut.up();

  // Assert
  SECTION("Faces the subject")
  {
    REQUIRE( almost_equal(forward, vector3{0, 0, -1}, 1e-5f) );
  }
  SECTION("Orients up towards the up direction")
  {
    REQUIRE( almost_equal(up, vector3{1, 0, 0}, 1e-5f) );
  }
}

TEST_CASE("camera::look_at( const vector3&, const vector3& )", "[rotation]")
{
  // Arrange
  auto sut = camera{};
  sut.set_translation(5, 0, 0);

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
    REQUIRE( almost_equal(result * vector4{0, 0, 1, 1}, vector4{-1, 0, 0, 1}) );
  }
}

TEST_CASE("camera::rotate_world( radian, const vector3& )", "[rotation]")
{
  // Arrange
  auto sut = camera{};
  sut.rotate_yaw(radian_constants::quarter_revolution);

  // Act
  sut.rotate_world(
    radian_constants::quarter_revolution,
    vector3_constants::unit_x
  );
  const auto result = sut.forward();

  // Assert
  SECTION("Rotates about the world axis")
  {
    REQUIRE( almost_equal(result, vector3{-1, 0, 0}, 1e-5f) );
  }
}

TEST_CASE("camera::rotate_world_yaw( radian )", "[rotation]")
{
  // Arrange
  auto sut = camera{};
  sut.rotate_pitch(radian{0.5f});

  // Act
  sut.rotate_world_yaw(radian_constants::quarter_revolution);
  const auto result = sut.right();

  // Assert
  SECTION("Keeps the right direction level")
  {
    REQUIRE( almost_equal(result, vector3{0, 0, -1}, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Absolute Transformations
//------------------------------------------------------------------------------

TEST_CASE("camera::set_roll( radian )", "[absolute]")
{
  // Arrange
  auto sut = camera{};
  sut.set_orientation(
    euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}}
  );

  // Act
  sut.set_roll(radian{0.2f});
  const auto result = sut.angles();

  // Assert
  SECTION("Replaces the roll, and keeps the yaw and pitch")
  {
    const auto expected = euler_angles{
      radian{0.3f},
      radian{-0.5f},
      radian{0.2f}
    };

    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
}

TEST_CASE("camera::set_pitch( radian )", "[absolute]")
{
  // Arrange
  auto sut = camera{};
  sut.set_orientation(
    euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}}
  );

  // Act
  sut.set_pitch(radian{0.2f});
  const auto result = sut.angles();

  // Assert
  SECTION("Replaces the pitch, and keeps the yaw and roll")
  {
    const auto expected = euler_angles{
      radian{0.3f},
      radian{0.2f},
      radian{2.5f}
    };

    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
}

TEST_CASE("camera::set_yaw( radian )", "[absolute]")
{
  // Arrange
  auto sut = camera{};
  sut.set_orientation(
    euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}}
  );

  // Act
  sut.set_yaw(radian{0.2f});
  const auto result = sut.angles();

  // Assert
  SECTION("Replaces the yaw, and keeps the pitch and roll")
  {
    const auto expected = euler_angles{
      radian{0.2f},
      radian{-0.5f},
      radian{2.5f}
    };

    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("camera::angles()", "[observers]")
{
  // Arrange
  const auto expected = euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}};
  auto sut = camera{};
  sut.set_orientation(expected);

  // Act
  const auto result = sut.angles();

  // Assert
  SECTION("Recovers the angles that the orientation was set from")
  {
    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
}

TEST_CASE("camera::right()", "[observers]")
{
  // Arrange
  auto sut = camera{};
  sut.rotate_yaw(radian_constants::quarter_revolution);

  // Act
  const auto result = sut.right();

  // Assert
  SECTION("Is the rotated x-axis")
  {
    REQUIRE( almost_equal(result, vector3{0, 0, -1}, 1e-5f) );
  }
}

TEST_CASE("camera::up()", "[observers]")
{
  // Arrange
  auto sut = camera{};
  sut.rotate_pitch(radian_constants::quarter_revolution);

  // Act
  const auto result = sut.up();

  // Assert
  SECTION("Is the rotated y-axis")
  {
    REQUIRE( almost_equal(result, vector3{0, 0, 1}, 1e-5f) );
  }
}

TEST_CASE("camera::forward()", "[observers]")
{
  // Arrange
  auto sut = camera{};
  sut.rotate_yaw(radian_constants::quarter_revolution);

  // Act
  const auto result = sut.forward();

  // Assert
  SECTION("Is the rotated negative z-axis")
  {
    REQUIRE( almost_equal(result, vector3{-1, 0, 0}, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

TEST_CASE("camera::to_matrix4()", "[conversion]")
{
  // Arrange
  auto sut = camera{};
  sut.set_translation(0, 0, 5);
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

TEST_CASE("camera::to_inverse_matrix4()", "[conversion]")
{
  // Arrange
  auto sut = camera{};
  sut.set_translation(4, -2, 7);
  sut.set_orientation(euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}});
  const auto matrix = sut.to_matrix4();

  // Act
  const auto result = sut.to_inverse_matrix4() * matrix;

  // Assert
  SECTION("Undoes the view transformation")
  {
    REQUIRE( almost_equal(result, matrix4_constants::identity, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

TEST_CASE("interpolator<camera>", "[interpolation]")
{
  SECTION("Interpolates the translation")
  {
    // Arrange
    const auto start = camera{};
    auto end = camera{};
    end.set_translation(2, 4, 6);

    // Act
    const auto result = interpolation::linear(start, end, 0.5f);

    // Assert
    REQUIRE( almost_equal(result.translation(), vector3{1, 2, 3}) );
  }

  SECTION("Moves along the axes of the interpolated orientation")
  {
    // Arrange
    const auto start = camera{};
    auto end = camera{};
    end.rotate_yaw(radian_constants::quarter_revolution);
    auto sut = interpolation::linear(start, end, 0.5f);
    const auto expected = sut.orientation() * vector3_constants::unit_x;

    // Act
    sut.translate_local_x(1);
    const auto result = sut.translation();

    // Assert
    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
}

} // namespace alloy::core::test
