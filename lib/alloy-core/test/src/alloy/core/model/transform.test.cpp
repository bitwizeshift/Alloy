// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/model/transform.hpp>

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
// Constructors
//------------------------------------------------------------------------------

TEST_CASE("transform::transform()", "[ctor]")
{
  // Act
  const auto sut = transform{};
  const auto result = sut.to_matrix4();

  // Assert
  SECTION("Is the identity transformation")
  {
    REQUIRE( result == matrix4_constants::identity );
  }
}

//------------------------------------------------------------------------------
// Relative Transformations
//------------------------------------------------------------------------------

TEST_CASE("transform::translate( float, float, float )", "[relative]")
{
  // Arrange
  auto sut = transform{};
  sut.set_translation(1, 2, 3);

  // Act
  sut.translate(4, 5, 6);
  const auto result = sut.translation();

  // Assert
  SECTION("Adds to the current translation")
  {
    REQUIRE( almost_equal(result, vector3{5, 7, 9}) );
  }
}

TEST_CASE("transform::rotate( const quaternion& )", "[relative]")
{
  // Arrange
  auto sut = transform{};
  sut.rotate_yaw(radian_constants::quarter_revolution);

  // Act
  sut.rotate(
    quaternion::from_angle_axis(
      radian_constants::quarter_revolution,
      vector3_constants::unit_x
    )
  );
  const auto result = sut.forward();

  // Assert
  SECTION("Rotates about the transform's own axes")
  {
    REQUIRE( almost_equal(result, vector3{0, 1, 0}, 1e-5f) );
  }
}

TEST_CASE("transform::scale( float )", "[relative]")
{
  // Arrange
  auto sut = transform{};
  sut.set_scale(vector3{1, 2, 3});

  // Act
  sut.scale(2);
  const auto result = sut.scale();

  // Assert
  SECTION("Multiplies every component of the current scale")
  {
    REQUIRE( almost_equal(result, vector3{2, 4, 6}) );
  }
}

TEST_CASE("transform::scale( const vector3& )", "[relative]")
{
  // Arrange
  auto sut = transform{};
  sut.set_scale(vector3{1, 2, 3});

  // Act
  sut.scale(vector3{4, 5, 6});
  const auto result = sut.scale();

  // Assert
  SECTION("Multiplies each component of the current scale")
  {
    REQUIRE( almost_equal(result, vector3{4, 10, 18}) );
  }
}

//------------------------------------------------------------------------------
// Absolute Transformations
//------------------------------------------------------------------------------

TEST_CASE("transform::set_scale( float )", "[absolute]")
{
  // Arrange
  auto sut = transform{};
  sut.set_scale(vector3{1, 2, 3});

  // Act
  sut.set_scale(4);
  const auto result = sut.scale();

  // Assert
  SECTION("Replaces every component of the scale")
  {
    REQUIRE( almost_equal(result, vector3{4, 4, 4}) );
  }
}

TEST_CASE("transform::set_roll( radian )", "[absolute]")
{
  // Arrange
  auto sut = transform{};
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

TEST_CASE("transform::set_pitch( radian )", "[absolute]")
{
  // Arrange
  auto sut = transform{};
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

TEST_CASE("transform::set_yaw( radian )", "[absolute]")
{
  // Arrange
  auto sut = transform{};
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

TEST_CASE("transform::look_at( const vector3&, const vector3& )", "[absolute]")
{
  // Arrange
  auto sut = transform{};
  sut.set_translation(5, 0, 0);

  // Act
  sut.look_at(vector3{5, 0, -5}, vector3_constants::unit_x);
  const auto forward = sut.forward();
  const auto up = sut.up();

  // Assert
  SECTION("Faces the target")
  {
    REQUIRE( almost_equal(forward, vector3{0, 0, -1}, 1e-5f) );
  }
  SECTION("Orients up towards the up direction")
  {
    REQUIRE( almost_equal(up, vector3{1, 0, 0}, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

TEST_CASE("transform::angles()", "[observers]")
{
  // Arrange
  const auto expected = euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}};
  auto sut = transform{};
  sut.set_orientation(expected);

  // Act
  const auto result = sut.angles();

  // Assert
  SECTION("Recovers the angles that the orientation was set from")
  {
    REQUIRE( almost_equal(result, expected, 1e-5f) );
  }
}

TEST_CASE("transform::right()", "[observers]")
{
  // Arrange
  auto sut = transform{};
  sut.rotate_yaw(radian_constants::quarter_revolution);

  // Act
  const auto result = sut.right();

  // Assert
  SECTION("Is the rotated x-axis")
  {
    REQUIRE( almost_equal(result, vector3{0, 0, -1}, 1e-5f) );
  }
}

TEST_CASE("transform::up()", "[observers]")
{
  // Arrange
  auto sut = transform{};
  sut.rotate_pitch(radian_constants::quarter_revolution);

  // Act
  const auto result = sut.up();

  // Assert
  SECTION("Is the rotated y-axis")
  {
    REQUIRE( almost_equal(result, vector3{0, 0, 1}, 1e-5f) );
  }
}

TEST_CASE("transform::forward()", "[observers]")
{
  // Arrange
  auto sut = transform{};
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
// Transformation
//------------------------------------------------------------------------------

TEST_CASE("transform::transform_point( const vector3& )", "[transformation]")
{
  // Arrange
  auto sut = transform{};
  sut.scale(vector3{2, 3, 1});
  sut.rotate(radian_constants::quarter_revolution, vector3_constants::unit_z);
  sut.translate(10, 0, 0);

  // Act
  const auto result = sut.transform_point(vector3{0, 1, 0});

  // Assert
  SECTION("Scales, then rotates, then translates")
  {
    REQUIRE( almost_equal(result, vector3{7, 0, 0}, 1e-5f) );
  }
}

TEST_CASE("transform::transform_vector( const vector3& )", "[transformation]")
{
  // Arrange
  auto sut = transform{};
  sut.scale(vector3{2, 3, 1});
  sut.rotate(radian_constants::quarter_revolution, vector3_constants::unit_z);
  sut.translate(10, 0, 0);

  // Act
  const auto result = sut.transform_vector(vector3{0, 1, 0});

  // Assert
  SECTION("Scales, then rotates, without translating")
  {
    REQUIRE( almost_equal(result, vector3{-3, 0, 0}, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Conversion
//------------------------------------------------------------------------------

TEST_CASE("transform::to_matrix4()", "[conversion]")
{
  // Arrange
  auto sut = transform{};
  sut.scale(2);
  sut.rotate(
    radian_constants::quarter_revolution,
    vector3_constants::unit_z
  );
  sut.translate(10, 0, 0);
  const auto point = vector4{1, 0, 0, 1};

  // Act
  const auto result = sut.to_matrix4() * point;

  // Assert
  SECTION("Scales, then rotates, then translates")
  {
    REQUIRE( almost_equal(result, vector4{10, 2, 0, 1}, 1e-5f) );
  }
}

TEST_CASE("transform::extract_matrix( not_null<matrix4*> )", "[conversion]")
{
  // Arrange
  auto sut = transform{};
  sut.scale(vector3{2, 3, 1});
  sut.rotate(
    radian_constants::quarter_revolution,
    vector3_constants::unit_z
  );
  sut.translate(10, 0, 0);
  const auto point = vector4{0, 1, 0, 1};
  auto matrix = matrix4{};

  // Act
  sut.extract_matrix(&matrix);
  const auto result = matrix * point;

  // Assert
  SECTION("Scales each axis, then rotates, then translates")
  {
    REQUIRE( almost_equal(result, vector4{7, 0, 0, 1}, 1e-5f) );
  }
}

TEST_CASE("transform::to_inverse_matrix4()", "[conversion]")
{
  // Arrange
  auto sut = transform{};
  sut.scale(vector3{2, 3, 4});
  sut.rotate(euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}});
  sut.translate(10, -2, 7);
  const auto matrix = sut.to_matrix4();

  // Act
  const auto result = sut.to_inverse_matrix4() * matrix;

  // Assert
  SECTION("Undoes the transformation")
  {
    REQUIRE( almost_equal(result, matrix4_constants::identity, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Interpolation
//------------------------------------------------------------------------------

TEST_CASE("interpolator<transform>", "[interpolation]")
{
  // Arrange
  const auto start = transform{};
  auto end = transform{};
  end.set_translation(2, 4, 6);
  end.set_scale(vector3{3, 5, 7});
  end.set_yaw(radian_constants::quarter_revolution);

  // Act
  const auto result = interpolation::linear(start, end, 0.5f);

  // Assert
  SECTION("Interpolates the translation")
  {
    REQUIRE( almost_equal(result.translation(), vector3{1, 2, 3}) );
  }
  SECTION("Interpolates the scale")
  {
    REQUIRE( almost_equal(result.scale(), vector3{2, 3, 4}) );
  }
  SECTION("Interpolates the orientation along the shortest rotation")
  {
    const auto expected = euler_angles{
      radian_constants::quarter_revolution * 0.5f,
      radian{0},
      radian{0}
    };

    REQUIRE( almost_equal(result.angles(), expected, 1e-5f) );
  }
}

//------------------------------------------------------------------------------
// Non-member Functions
//------------------------------------------------------------------------------

TEST_CASE("almost_equal( const transform&, const transform&, float )",
          "[comparison]")
{
  // Arrange
  auto lhs = transform{};
  lhs.scale(vector3{2, 3, 4});
  lhs.rotate(euler_angles{radian{0.3f}, radian{-0.5f}, radian{2.5f}});
  lhs.translate(10, -2, 7);

  SECTION("Same transform is almost equal")
  {
    // Arrange
    const auto rhs = lhs;

    // Act
    const auto result = almost_equal(lhs, rhs, 1e-5f);

    // Assert
    REQUIRE( result );
  }
  SECTION("Negated orientation is almost equal")
  {
    // Arrange
    auto rhs = lhs;
    rhs.set_orientation(-lhs.orientation());

    // Act
    const auto result = almost_equal(lhs, rhs, 1e-5f);

    // Assert
    REQUIRE( result );
  }
  SECTION("Different translation is not almost equal")
  {
    // Arrange
    auto rhs = lhs;
    rhs.translate_x(1);

    // Act
    const auto result = almost_equal(lhs, rhs, 1e-5f);

    // Assert
    REQUIRE_FALSE( result );
  }
  SECTION("Different scale is not almost equal")
  {
    // Arrange
    auto rhs = lhs;
    rhs.scale_x(2);

    // Act
    const auto result = almost_equal(lhs, rhs, 1e-5f);

    // Assert
    REQUIRE_FALSE( result );
  }
  SECTION("Different orientation is not almost equal")
  {
    // Arrange
    auto rhs = lhs;
    rhs.rotate_yaw(radian{0.5f});

    // Act
    const auto result = almost_equal(lhs, rhs, 1e-5f);

    // Assert
    REQUIRE_FALSE( result );
  }
}

} // namespace alloy::core::test
