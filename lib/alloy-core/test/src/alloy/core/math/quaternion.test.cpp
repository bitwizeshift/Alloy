// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/quaternion.hpp>

#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/euler_angles.hpp>
#include <alloy/core/math/interpolation.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix3.hpp>
#include <alloy/core/math/quaternion_constants.hpp>
#include <alloy/core/math/vector/vector3.hpp>

#include <alloy/core/precision/real.hpp>

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
    SECTION("Recovers the original rotation angle") {
      REQUIRE( almost_equal(result_angle, angle) );
    }
    SECTION("Recovers the original rotation axis") {
      REQUIRE( almost_equal(result_axis, axis) );
    }
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

TEST_CASE("quaternion::to_euler_angles()", "[observers]")
{
  SECTION("Recovers the angles that the quaternion was made from")
  {
    // Arrange
    const auto expected = euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}};
    const auto sut = quaternion::from_angles(expected);

    // Act
    const auto result = sut.to_euler_angles();

    // Assert
    REQUIRE( almost_equal(result, expected, real{1e-5f}) );
  }

  SECTION("Gimbal lock reproduces the same rotation")
  {
    // Arrange
    const auto sut = quaternion::from_angles(
      radian{0.3f},
      radian_constants::quarter_revolution,
      radian{0.7f}
    );

    // Act
    const auto result = quaternion::from_angles(sut.to_euler_angles());

    // Assert
    REQUIRE( almost_equal(result, sut, real{1e-3f}) );
  }

  SECTION("Gimbal lock sets the roll to 0")
  {
    // Arrange
    const auto sut = quaternion::from_angles(
      radian{0.3f},
      radian_constants::quarter_revolution,
      radian{0.7f}
    );

    // Act
    const auto result = sut.to_euler_angles();

    // Assert
    REQUIRE( result.roll() == radian{0} );
  }
}

TEST_CASE("quaternion::extract_axes( ... )", "[observers]")
{
  // Arrange
  const auto sut = quaternion::from_angle_axis(
    radian_constants::quarter_revolution,
    vector3{0, 0, 1}
  );
  auto x_axis = vector3{};
  auto y_axis = vector3{};
  auto z_axis = vector3{};

  const auto expected = matrix3{
    vector3{0, 1, 0},
    vector3{-1, 0, 0},
    vector3{0, 0, 1}
  };

  // Act
  sut.extract_axes(&x_axis, &y_axis, &z_axis);

  const auto axes = matrix3{x_axis, y_axis, z_axis};
  const auto observed_axes = matrix3{sut.x_axis(), sut.y_axis(), sut.z_axis()};

  // Assert
  SECTION("Axes are where the rotation maps the unit vectors")
  {
    REQUIRE( almost_equal(axes, expected) );
  }
  SECTION("Axes agree with x_axis(), y_axis(), and z_axis()")
  {
    REQUIRE( almost_equal(axes, observed_axes) );
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
// Static Factories
//------------------------------------------------------------------------------

TEST_CASE("quaternion::from_angles( radian, radian, radian )", "[factory]")
{
  // Arrange
  const auto yaw = radian{0.3f};
  const auto pitch = radian{0.5f};
  const auto roll = radian{0.7f};
  const auto expected = quaternion::from_angle_axis(yaw, vector3{0, 1, 0})
                      * quaternion::from_angle_axis(pitch, vector3{1, 0, 0})
                      * quaternion::from_angle_axis(roll, vector3{0, 0, 1});

  // Act
  const auto result = quaternion::from_angles(yaw, pitch, roll);

  // Assert
  SECTION("Composes the yaw, then the pitch, then the roll")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("quaternion::from_rotation_axes( ... )", "[factory]")
{
  // Arrange
  const auto expected = quaternion::from_angle_axis(
    radian_constants::quarter_revolution,
    vector3{0, 0, 1}
  );

  // Act
  const auto result = quaternion::from_rotation_axes(
    expected.x_axis(),
    expected.y_axis(),
    expected.z_axis()
  );

  // Assert
  SECTION("Round-trips the axes of a quaternion")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("quaternion::from_to( const vector3&, const vector3& )", "[factory]")
{
  SECTION("Rotates the first direction onto the second")
  {
    // Arrange
    const auto from = vector3{1, 0, 0};
    const auto to = vector3{0, 2, 2};

    // Act
    const auto result = quaternion::from_to(from, to);

    // Assert
    REQUIRE( almost_equal(result * from, to.normalized()) );
  }

  SECTION("Rotates about the axis perpendicular to both directions")
  {
    // Arrange
    const auto from = vector3{1, 0, 0};
    const auto to = vector3{0, 1, 0};
    const auto expected = quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3{0, 0, 1}
    );

    // Act
    const auto result = quaternion::from_to(from, to);

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Same directions produce the identity")
  {
    // Arrange
    const auto from = vector3{1, 2, 3};

    // Act
    const auto result = quaternion::from_to(from, from);

    // Assert
    REQUIRE( almost_equal(result, quaternion_constants::identity) );
  }

  SECTION("Opposite directions produce a half turn")
  {
    // Arrange
    const auto from = vector3{0, 0, 1};
    const auto to = vector3{0, 0, -1};

    // Act
    const auto result = quaternion::from_to(from, to);

    // Assert
    REQUIRE( almost_equal(result * from, to) );
  }
}

TEST_CASE("quaternion::look_rotation( const vector3&, const vector3& )", "[factory]")
{
  SECTION("Facing down the negative z-axis is the identity")
  {
    // Arrange
    const auto forward = vector3{0, 0, -1};
    const auto up = vector3{0, 1, 0};

    // Act
    const auto result = quaternion::look_rotation(forward, up);

    // Assert
    REQUIRE( almost_equal(result, quaternion_constants::identity) );
  }

  SECTION("Maps the negative z-axis to the forward direction")
  {
    // Arrange
    const auto forward = vector3{1, 0, 0};
    const auto up = vector3{0, 1, 0};

    // Act
    const auto result = quaternion::look_rotation(forward, up);

    // Assert
    REQUIRE( almost_equal(result * vector3{0, 0, -1}, forward) );
  }

  SECTION("Orients the y-axis towards the up direction")
  {
    // Arrange
    const auto forward = vector3{1, 0, 0};
    const auto up = vector3{0, 1, 1};

    // Act
    const auto result = quaternion::look_rotation(forward, up);

    // Assert
    REQUIRE( almost_equal(result.y_axis(), up.normalized()) );
  }
}

TEST_CASE("quaternion::from_rotation_matrix( const matrix3& )", "[factory]")
{
  SECTION("Round-trips a rotation with a positive trace")
  {
    // Arrange
    const auto expected = quaternion::from_angle_axis(
      radian{0.5f},
      vector3{1, 2, 3}.normalized()
    );

    // Act
    const auto result = quaternion::from_rotation_matrix(
      expected.rotation_matrix()
    );

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Round-trips a rotation with a negative trace")
  {
    // Arrange
    const auto expected = quaternion::from_angle_axis(
      radian{3},
      vector3{1, 2, 3}.normalized()
    );

    // Act
    const auto result = quaternion::from_rotation_matrix(
      expected.rotation_matrix()
    );

    // Assert
    REQUIRE( almost_equal(result, expected) );
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

TEST_CASE("operator*( const quaternion&, const quaternion& )", "[arithmetic]")
{
  // Arrange
  const auto i = quaternion{0, 1, 0, 0};
  const auto j = quaternion{0, 0, 1, 0};
  const auto k = quaternion{0, 0, 0, 1};
  const auto negative_one = quaternion{-1, 0, 0, 0};

  SECTION("i squared is -1")
  {
    // Act
    const auto result = i * i;

    // Assert
    REQUIRE( result == negative_one );
  }
  SECTION("j squared is -1")
  {
    // Act
    const auto result = j * j;

    // Assert
    REQUIRE( result == negative_one );
  }
  SECTION("k squared is -1")
  {
    // Act
    const auto result = k * k;

    // Assert
    REQUIRE( result == negative_one );
  }
  SECTION("ijk is -1")
  {
    // Act
    const auto result = i * j * k;

    // Assert
    REQUIRE( result == negative_one );
  }
  SECTION("ij is k")
  {
    // Act
    const auto result = i * j;

    // Assert
    REQUIRE( result == k );
  }
}

TEST_CASE("operator*( const quaternion&, const vector3& )", "[arithmetic]")
{
  SECTION("Quarter turn about the z-axis takes the x-axis to the y-axis")
  {
    // Arrange
    const auto sut = quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3{0, 0, 1}
    );

    // Act
    const auto result = sut * vector3{1, 0, 0};

    // Assert
    REQUIRE( almost_equal(result, vector3{0, 1, 0}) );
  }

  SECTION("Agrees with the rotation matrix")
  {
    // Arrange
    const auto sut = quaternion::from_angles(
      radian{0.3f},
      radian{0.5f},
      radian{0.7f}
    );
    const auto vec = vector3{1, 2, 3};

    // Act
    const auto result = sut * vec;

    // Assert
    REQUIRE( almost_equal(result, sut.rotation_matrix() * vec) );
  }

  SECTION("Composition rotates by the right quaternion first")
  {
    // Arrange
    const auto about_z = quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3{0, 0, 1}
    );
    const auto about_x = quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3{1, 0, 0}
    );

    // Act
    const auto result = (about_x * about_z) * vector3{1, 0, 0};

    // Assert
    REQUIRE( almost_equal(result, vector3{0, 0, 1}) );
  }
}

//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

TEST_CASE("slerp( const quaternion&, const quaternion&, real )", "[interpolation]")
{
  // Arrange
  const auto start = quaternion_constants::identity;
  const auto end = quaternion::from_angle_axis(
    radian_constants::quarter_revolution,
    vector3{0, 0, 1}
  );

  SECTION("Interpolates at a constant angular velocity")
  {
    // Arrange
    const auto expected = quaternion::from_angle_axis(
      radian_constants::quarter_revolution * real{0.25},
      vector3{0, 0, 1}
    );

    // Act
    const auto result = slerp(start, end, real{0.25});

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Takes the shortest path when the end is negated")
  {
    // Arrange
    const auto expected = quaternion::from_angle_axis(
      radian_constants::quarter_revolution * real{0.5},
      vector3{0, 0, 1}
    );

    // Act
    const auto result = slerp(start, -end, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, expected) );
  }

  SECTION("Returns the start at 0")
  {
    // Act
    const auto result = slerp(start, end, real{0});

    // Assert
    REQUIRE( almost_equal(result, start) );
  }

  SECTION("Returns the end at 1")
  {
    // Act
    const auto result = slerp(start, end, real{1});

    // Assert
    REQUIRE( almost_equal(result, end) );
  }
}

TEST_CASE("nlerp( const quaternion&, const quaternion&, real )", "[interpolation]")
{
  // Arrange
  const auto start = quaternion_constants::identity;
  const auto end = quaternion::from_angle_axis(
    radian_constants::quarter_revolution,
    vector3{0, 0, 1}
  );
  const auto expected = quaternion::from_angle_axis(
    radian_constants::quarter_revolution * real{0.5},
    vector3{0, 0, 1}
  );

  // Act
  const auto result = nlerp(start, end, real{0.5});

  // Assert
  SECTION("Midpoint is the half rotation")
  {
    REQUIRE( almost_equal(result, expected) );
  }
}

TEST_CASE("interpolator<quaternion>", "[interpolation]")
{
  // Arrange
  const auto start = quaternion_constants::identity;
  const auto end = quaternion::from_angle_axis(
    radian_constants::half_revolution * real{0.9},
    vector3{1, 0, 0}
  );

  // Act
  const auto result = interpolation::linear(start, end, real{0.5});

  // Assert
  SECTION("Interpolated rotation is unit length")
  {
    REQUIRE( almost_equal(result.magnitude(), real{1}) );
  }
}

} // namespace alloy::core::test
