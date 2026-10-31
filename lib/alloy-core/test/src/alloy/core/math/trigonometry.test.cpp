// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/trigonometry.hpp>

#include <alloy/core/math/angle/degree.hpp>
#include <alloy/core/math/angle/gradian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/math_constants.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

//------------------------------------------------------------------------------
// Trig
//------------------------------------------------------------------------------

TEST_CASE("trigonometry::cos( basic_angle<AngleUnit> )", "[trigonometry]")
{
  SECTION("Converts a degree to radians")
  {
    // Arrange
    const auto angle = degree{60.0f};

    // Act
    const auto result = trigonometry::cos(angle);

    // Assert
    REQUIRE( almost_equal(result, 0.5f) );
  }
  SECTION("Converts a gradian to radians")
  {
    // Arrange
    const auto angle = gradian{200.0f};

    // Act
    const auto result = trigonometry::cos(angle);

    // Assert
    REQUIRE( almost_equal(result, -1.0f) );
  }
}

TEST_CASE("trigonometry::sin( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{30.0f};

  // Act
  const auto result = trigonometry::sin(angle);

  // Assert
  SECTION("Converts the angle to radians")
  {
    REQUIRE( almost_equal(result, 0.5f) );
  }
}

TEST_CASE("trigonometry::tan( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{45.0f};

  // Act
  const auto result = trigonometry::tan(angle);

  // Assert
  SECTION("Converts the angle to radians")
  {
    REQUIRE( almost_equal(result, 1.0f) );
  }
}

TEST_CASE("trigonometry::sec( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{60.0f};

  // Act
  const auto result = trigonometry::sec(angle);

  // Assert
  SECTION("Is the reciprocal of the cosine")
  {
    REQUIRE( almost_equal(result, 2.0f) );
  }
}

TEST_CASE("trigonometry::csc( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{30.0f};

  // Act
  const auto result = trigonometry::csc(angle);

  // Assert
  SECTION("Is the reciprocal of the sine")
  {
    REQUIRE( almost_equal(result, 2.0f) );
  }
}

TEST_CASE("trigonometry::cot( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{45.0f};

  // Act
  const auto result = trigonometry::cot(angle);

  // Assert
  SECTION("Is the reciprocal of the tangent")
  {
    REQUIRE( almost_equal(result, 1.0f) );
  }
}

//------------------------------------------------------------------------------
// Inverse Trig
//------------------------------------------------------------------------------

TEST_CASE("trigonometry::arccos( float )", "[trigonometry]")
{
  // Arrange
  const auto value = -1.0f;

  // Act
  const auto result = trigonometry::arccos(value);

  // Assert
  SECTION("Returns the angle in radians")
  {
    REQUIRE( almost_equal(result.value(), math_constants::pi) );
  }
}

TEST_CASE("trigonometry::arcsin( float )", "[trigonometry]")
{
  // Arrange
  const auto value = 1.0f;

  // Act
  const auto result = trigonometry::arcsin(value);

  // Assert
  SECTION("Returns the angle in radians")
  {
    REQUIRE( almost_equal(result.value(), math_constants::half_pi) );
  }
}

TEST_CASE("trigonometry::arctan( float )", "[trigonometry]")
{
  // Arrange
  const auto value = 1.0f;

  // Act
  const auto result = trigonometry::arctan(value);

  // Assert
  SECTION("Returns the angle in radians")
  {
    REQUIRE( almost_equal(result.value(), math_constants::pi / 4.0f) );
  }
}

TEST_CASE("trigonometry::arctan2( float, float )", "[trigonometry]")
{
  SECTION("Point in the second quadrant")
  {
    // Arrange
    const auto y = 1.0f;
    const auto x = -1.0f;

    // Act
    const auto result = trigonometry::arctan2(y, x);

    // Assert
    REQUIRE( almost_equal(result.value(), 3.0f * math_constants::pi / 4.0f) );
  }
  SECTION("Point in the third quadrant")
  {
    // Arrange
    const auto y = -1.0f;
    const auto x = -1.0f;

    // Act
    const auto result = trigonometry::arctan2(y, x);

    // Assert
    REQUIRE( almost_equal(result.value(), -3.0f * math_constants::pi / 4.0f) );
  }
  SECTION("Point on the positive y-axis")
  {
    // Arrange
    const auto y = 1.0f;
    const auto x = 0.0f;

    // Act
    const auto result = trigonometry::arctan2(y, x);

    // Assert
    REQUIRE( almost_equal(result.value(), math_constants::half_pi) );
  }
}

} // namespace alloy::core::test
