// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/math/interpolation.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

namespace {
  constexpr auto tolerance = real{1e-4f};
} // namespace

//------------------------------------------------------------------------------
// Easing
//------------------------------------------------------------------------------

TEST_CASE("interpolation::linear( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::linear(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::linear(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::linear(v0, v1, real{0.25});

    // Assert
    REQUIRE( almost_equal(result, real{2.5}, tolerance) );
  }
}

TEST_CASE("interpolation::quadratic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::quadratic(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::quadratic(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::quadratic(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{2.5}, tolerance) );
  }
}

TEST_CASE("interpolation::cubic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::cubic(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::cubic(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::cubic(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{1.25}, tolerance) );
  }
}

TEST_CASE("interpolation::quartic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::quartic(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::quartic(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::quartic(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{0.625}, tolerance) );
  }
}

TEST_CASE("interpolation::quintic( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::quintic(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::quintic(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::quintic(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{0.3125}, tolerance) );
  }
}

TEST_CASE("interpolation::circular( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::circular(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::circular(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::circular(v0, v1, real{0.6});

    // Assert
    REQUIRE( almost_equal(result, real{2}, tolerance) );
  }
}

TEST_CASE("interpolation::half_cosine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::half_cosine(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::half_cosine(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::half_cosine(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{2.9289322}, tolerance) );
  }
}

TEST_CASE("interpolation::cosine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::cosine(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::cosine(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::cosine(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{5}, tolerance) );
  }
}

TEST_CASE("interpolation::half_sine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::half_sine(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::half_sine(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::half_sine(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{7.0710678}, tolerance) );
  }
}

TEST_CASE("interpolation::sine( const V&, const V&, const T& )", "[interpolation]")
{
  // Arrange
  const auto v0 = real{0};
  const auto v1 = real{10};

  SECTION("Starts at the first value")
  {
    // Act
    const auto result = interpolation::sine(v0, v1, real{0});

    // Assert
    REQUIRE( almost_equal(result, v0, tolerance) );
  }
  SECTION("Ends at the second value")
  {
    // Act
    const auto result = interpolation::sine(v0, v1, real{1});

    // Assert
    REQUIRE( almost_equal(result, v1, tolerance) );
  }
  SECTION("Follows its curve between the values")
  {
    // Act
    const auto result = interpolation::sine(v0, v1, real{0.5});

    // Assert
    REQUIRE( almost_equal(result, real{5}, tolerance) );
  }
  SECTION("Follows the same curve as cosine")
  {
    // Arrange
    const auto t = real{0.2f};
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
  const auto v00 = real{0};
  const auto v10 = real{1};
  const auto v01 = real{2};
  const auto v11 = real{3};

  // Act
  const auto result = interpolation::bilinear(
    v00, v10, v01, v11,
    real{0.5f}, real{0.25f}
  );

  // Assert
  SECTION("Interpolates along x, then y")
  {
    // Assert
    REQUIRE( almost_equal(result, real{1}, tolerance) );
  }
}

TEST_CASE("interpolation::trilinear( ... )", "[interpolation]")
{
  // Act
  const auto result = interpolation::trilinear(
    real{0}, real{1}, real{2}, real{3},
    real{4}, real{5}, real{6}, real{7},
    real{0.5f}, real{0.5f}, real{0.25f}
  );

  // Assert
  SECTION("Interpolates along x, then y, then z")
  {
    // Assert
    REQUIRE( almost_equal(result, real{2.5f}, tolerance) );
  }
}

} // namespace alloy::core::test
