// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/capsule.hpp>

#include <alloy/core/geometry/line_segment.hpp>
#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("capsule::nearest_point(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Point is inside the capsule") {
    // Act
    const auto result = sut.nearest_point(point3{0.5f, 2, 0});

    // Assert
    REQUIRE(almost_equal(result, point3{0.5f, 2, 0}));
  }
  SECTION("Point is beside the cylinder") {
    // Act
    const auto result = sut.nearest_point(point3{3, 2, 0});

    // Assert
    REQUIRE(almost_equal(result, point3{1, 2, 0}));
  }
  SECTION("Point is past a hemisphere cap") {
    // Act
    const auto result = sut.nearest_point(point3{0, 7, 0});

    // Assert
    REQUIRE(almost_equal(result, point3{0, 5, 0}));
  }
}

TEST_CASE("capsule::contains(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Point is inside the cylinder") {
    // Act
    const auto result = sut.contains(point3{0.5f, 2, 0.5f});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is inside a hemisphere cap") {
    // Act
    const auto result = sut.contains(point3{0, -0.9f, 0});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is past a hemisphere cap") {
    // Act
    const auto result = sut.contains(point3{0, 5.5f, 0});

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Point is in the corner a cylinder would cover") {
    // Act
    const auto result = sut.contains(point3{0.9f, 4.9f, 0});

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("capsule::contains(const point3&, real)", "[quantifier]") {
  // Arrange
  const auto sut = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };
  const auto tolerance = real{0.1f};

  SECTION("Point is within tolerance of the surface") {
    // Act
    const auto result = sut.contains(point3{1.05f, 2, 0}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of the surface") {
    // Act
    const auto result = sut.contains(point3{1.5f, 2, 0}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
