// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/trigonometry.hpp>

#include <alloy/core/math/angle/degree.hpp>
#include <alloy/core/math/angle/gradian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/math_constants.hpp>

#include <alloy/core/precision/real.hpp>

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
    const auto angle = degree{real{60}};

    // Act
    const auto result = trigonometry::cos(angle);

    // Assert
    REQUIRE( almost_equal(result, real{0.5}) );
  }
  SECTION("Converts a gradian to radians")
  {
    // Arrange
    const auto angle = gradian{real{200}};

    // Act
    const auto result = trigonometry::cos(angle);

    // Assert
    REQUIRE( almost_equal(result, real{-1}) );
  }
}

TEST_CASE("trigonometry::sin( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{real{30}};

  // Act
  const auto result = trigonometry::sin(angle);

  // Assert
  SECTION("Converts the angle to radians")
  {
    REQUIRE( almost_equal(result, real{0.5}) );
  }
}

TEST_CASE("trigonometry::tan( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{real{45}};

  // Act
  const auto result = trigonometry::tan(angle);

  // Assert
  SECTION("Converts the angle to radians")
  {
    REQUIRE( almost_equal(result, real{1}) );
  }
}

TEST_CASE("trigonometry::sec( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{real{60}};

  // Act
  const auto result = trigonometry::sec(angle);

  // Assert
  SECTION("Is the reciprocal of the cosine")
  {
    REQUIRE( almost_equal(result, real{2}) );
  }
}

TEST_CASE("trigonometry::csc( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{real{30}};

  // Act
  const auto result = trigonometry::csc(angle);

  // Assert
  SECTION("Is the reciprocal of the sine")
  {
    REQUIRE( almost_equal(result, real{2}) );
  }
}

TEST_CASE("trigonometry::cot( basic_angle<AngleUnit> )", "[trigonometry]")
{
  // Arrange
  const auto angle = degree{real{45}};

  // Act
  const auto result = trigonometry::cot(angle);

  // Assert
  SECTION("Is the reciprocal of the tangent")
  {
    REQUIRE( almost_equal(result, real{1}) );
  }
}

//------------------------------------------------------------------------------
// Inverse Trig
//------------------------------------------------------------------------------

TEST_CASE("trigonometry::arccos( real )", "[trigonometry]")
{
  // Arrange
  const auto value = real{-1};

  // Act
  const auto result = trigonometry::arccos(value);

  // Assert
  SECTION("Returns the angle in radians")
  {
    REQUIRE( almost_equal(result.value(), math_constants::pi) );
  }
}

TEST_CASE("trigonometry::arcsin( real )", "[trigonometry]")
{
  // Arrange
  const auto value = real{1};

  // Act
  const auto result = trigonometry::arcsin(value);

  // Assert
  SECTION("Returns the angle in radians")
  {
    REQUIRE( almost_equal(result.value(), math_constants::half_pi) );
  }
}

TEST_CASE("trigonometry::arctan( real )", "[trigonometry]")
{
  // Arrange
  const auto value = real{1};

  // Act
  const auto result = trigonometry::arctan(value);

  // Assert
  SECTION("Returns the angle in radians")
  {
    REQUIRE( almost_equal(result.value(), math_constants::pi / real{4}) );
  }
}

TEST_CASE("trigonometry::arctan2( real, real )", "[trigonometry]")
{
  SECTION("Point in the second quadrant")
  {
    // Arrange
    const auto y = real{1};
    const auto x = real{-1};

    // Act
    const auto result = trigonometry::arctan2(y, x);

    // Assert
    REQUIRE( almost_equal(result.value(), real{3} * math_constants::pi / real{4}) );
  }
  SECTION("Point in the third quadrant")
  {
    // Arrange
    const auto y = real{-1};
    const auto x = real{-1};

    // Act
    const auto result = trigonometry::arctan2(y, x);

    // Assert
    REQUIRE( almost_equal(result.value(), real{-3} * math_constants::pi / real{4}) );
  }
  SECTION("Point on the positive y-axis")
  {
    // Arrange
    const auto y = real{1};
    const auto x = real{0};

    // Act
    const auto result = trigonometry::arctan2(y, x);

    // Assert
    REQUIRE( almost_equal(result.value(), math_constants::half_pi) );
  }
}

} // namespace alloy::core::test
