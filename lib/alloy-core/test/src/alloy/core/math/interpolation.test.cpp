// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/interpolation.hpp>

#include <alloy/core/math/math.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

namespace {
  constexpr auto tolerance = 1e-4f;
} // namespace

//------------------------------------------------------------------------------
// Easing
//------------------------------------------------------------------------------

TEST_CASE("interpolation::linear( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::linear(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::linear(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::linear(v0, v1, 0.25f);

    // Assert
    REQUIRE( almost_equal(result, 2.5f, tolerance) );
  }
}

TEST_CASE("interpolation::quadratic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::quadratic(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::quadratic(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::quadratic(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 2.5f, tolerance) );
  }
}

TEST_CASE("interpolation::cubic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::cubic(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::cubic(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::cubic(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 1.25f, tolerance) );
  }
}

TEST_CASE("interpolation::quartic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::quartic(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::quartic(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::quartic(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 0.625f, tolerance) );
  }
}

TEST_CASE("interpolation::quintic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::quintic(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::quintic(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::quintic(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 0.3125f, tolerance) );
  }
}

TEST_CASE("interpolation::circular( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::circular(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::circular(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::circular(v0, v1, 0.6f);

    // Assert
    REQUIRE( almost_equal(result, 2.0f, tolerance) );
  }
}

TEST_CASE("interpolation::half_cosine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::half_cosine(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::half_cosine(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::half_cosine(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 2.9289322f, tolerance) );
  }
}

TEST_CASE("interpolation::cosine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::cosine(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::cosine(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::cosine(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 5.0f, tolerance) );
  }
}

TEST_CASE("interpolation::half_sine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::half_sine(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::half_sine(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::half_sine(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 7.0710678f, tolerance) );
  }
}

TEST_CASE("interpolation::sine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = 0.0f;
  const auto v1 = 10.0f;

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::sine(v0, v1, 0.0f);

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::sine(v0, v1, 1.0f);

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::sine(v0, v1, 0.5f);

    // Assert
    REQUIRE( almost_equal(result, 5.0f, tolerance) );
  }
  SECTION("Follows the same curve as cosine")
  {
    // Arrange
    const auto t = 0.2f;
    const auto expected = interpolation::cosine(v0, v1, t);

    // Act
    const auto result = interpolation::sine(v0, v1, t);

    // Assert
    REQUIRE( almost_equal(result, expected, tolerance) );
  }
}

//------------------------------------------------------------------------------
// Multi-dimensional
//------------------------------------------------------------------------------

TEST_CASE("interpolation::bilinear( ... )", "[interpolation]")
{
  // Arrange
  const auto v00 = 0.0f;
  const auto v10 = 1.0f;
  const auto v01 = 2.0f;
  const auto v11 = 3.0f;

  // Act
  const auto result = interpolation::bilinear(
    v00, v10, v01, v11,
    0.5f, 0.25f
  );

  // Assert
  SECTION("Interpolates along x, then y")
  {
    // Assert
    REQUIRE( almost_equal(result, 1.0f, tolerance) );
  }
}

TEST_CASE("interpolation::trilinear( ... )", "[interpolation]")
{
  // Act
  const auto result = interpolation::trilinear(
    0.0f, 1.0f, 2.0f, 3.0f,
    4.0f, 5.0f, 6.0f, 7.0f,
    0.5f, 0.5f, 0.25f
  );

  // Assert
  SECTION("Interpolates along x, then y, then z")
  {
    // Assert
    REQUIRE( almost_equal(result, 2.5f, tolerance) );
  }
}

} // namespace alloy::core::test
