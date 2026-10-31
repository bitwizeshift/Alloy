// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/circle.hpp>

#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/math/math.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("circle::nearest_point(const point2&)", "[quantifier]") {
  // Arrange
  const auto sut = circle{point2{1, 1}, 2.0f};

  SECTION("Point is inside the circle") {
    // Act
    const auto result = sut.nearest_point(point2{2, 1});

    // Assert
    REQUIRE(almost_equal(result, point2{2, 1}));
  }
  SECTION("Point is outside the circle") {
    // Act
    const auto result = sut.nearest_point(point2{1, 6});

    // Assert
    REQUIRE(almost_equal(result, point2{1, 3}));
  }
}

TEST_CASE("circle::contains(const point2&)", "[quantifier]") {
  // Arrange
  const auto sut = circle{point2{1, 1}, 2.0f};

  SECTION("Point is inside the circle") {
    // Act
    const auto result = sut.contains(point2{2, 2});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is on the edge") {
    // Act
    const auto result = sut.contains(point2{3, 1});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside the circle") {
    // Act
    const auto result = sut.contains(point2{2.5f, 2.5f});

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("circle::contains(const point2&, float)", "[quantifier]") {
  // Arrange
  const auto sut = circle{point2{1, 1}, 2.0f};
  const auto tolerance = 0.1f;

  SECTION("Point is within tolerance of the edge") {
    // Act
    const auto result = sut.contains(point2{3.05f, 1}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of the edge") {
    // Act
    const auto result = sut.contains(point2{3.5f, 1}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
