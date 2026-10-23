// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/rectangle.hpp>

#include <alloy/core/geometry/point/point2.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector2.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("rectangle::from_point_and_size(const point2&, const vector2&)", "[factory][static]") {
  // Arrange
  const auto point = point2{2, -10};
  const auto size = vector2{-4, 2};

  const auto expected = rectangle{point2{-2, -10}, point2{2, -8}};

  // Act
  const auto sut = rectangle::from_point_and_size(point, size);

  // Assert
  SECTION("Corners are ordered") {
    REQUIRE(sut == expected);
  }
}

TEST_CASE("rectangle::from_points(const point2&, const point2&)", "[factory][static]") {
  // Arrange
  const auto p0 = point2{2, -10};
  const auto p1 = point2{-4, 2};

  const auto expected = rectangle{point2{-4, -10}, point2{2, 2}};

  // Act
  const auto sut = rectangle::from_points(p0, p1);

  // Assert
  SECTION("Corners are ordered") {
    REQUIRE(sut == expected);
  }
}

TEST_CASE("rectangle::size()", "[observer]") {
  // Arrange
  const auto sut = rectangle{point2{1, 2}, point2{4, 8}};

  // Act
  const auto result = sut.size();

  // Assert
  SECTION("Is the width and height") {
    REQUIRE(result == vector2{3, 6});
  }
}

TEST_CASE("rectangle::distance_to(const point2&)", "[quantifier]") {
  // Arrange
  const auto sut = rectangle{point2{-1, -1}, point2{1, 1}};

  SECTION("Point is inside the rectangle") {
    // Act
    const auto result = sut.distance_to(point2{0.5f, 0});

    // Assert
    REQUIRE(almost_equal(result, real{0}));
  }
  SECTION("Point is outside an edge") {
    // Act
    const auto result = sut.distance_to(point2{0, 4});

    // Assert
    REQUIRE(almost_equal(result, real{3}));
  }
  SECTION("Point is outside a corner") {
    // Act
    const auto result = sut.distance_to(point2{4, 5});

    // Assert
    REQUIRE(almost_equal(result, real{5}));
  }
}

TEST_CASE("rectangle::nearest_point(const point2&)", "[quantifier]") {
  // Arrange
  const auto sut = rectangle{point2{-1, -1}, point2{1, 1}};

  SECTION("Point is inside the rectangle") {
    // Act
    const auto result = sut.nearest_point(point2{0.5f, 0});

    // Assert
    REQUIRE(almost_equal(result, point2{0.5f, 0}));
  }
  SECTION("Point is outside a corner") {
    // Act
    const auto result = sut.nearest_point(point2{4, -5});

    // Assert
    REQUIRE(almost_equal(result, point2{1, -1}));
  }
}

TEST_CASE("rectangle::contains(const point2&)", "[quantifier]") {
  // Arrange
  const auto sut = rectangle{point2{-1, -1}, point2{1, 1}};

  SECTION("Point is inside the rectangle") {
    // Act
    const auto result = sut.contains(point2{0.5f, -0.5f});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is on an edge") {
    // Act
    const auto result = sut.contains(point2{1, 0});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside the rectangle") {
    // Act
    const auto result = sut.contains(point2{1.5f, 0});

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("rectangle::contains(const point2&, real)", "[quantifier]") {
  // Arrange
  const auto sut = rectangle{point2{-1, -1}, point2{1, 1}};
  const auto tolerance = real{0.1f};

  SECTION("Point is within tolerance of an edge") {
    // Act
    const auto result = sut.contains(point2{1.05f, 0}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of an edge") {
    // Act
    const auto result = sut.contains(point2{1.5f, 0}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("rectangle::intersects(const rectangle&)", "[quantifier]") {
  // Arrange
  const auto sut = rectangle{point2{-1, -1}, point2{1, 1}};

  SECTION("Rectangles overlap") {
    // Act
    const auto result = sut.intersects(rectangle{point2{0, 0}, point2{2, 2}});

    // Assert
    REQUIRE(result);
  }
  SECTION("Rectangles touch at an edge") {
    // Act
    const auto result = sut.intersects(rectangle{point2{1, -1}, point2{2, 1}});

    // Assert
    REQUIRE(result);
  }
  SECTION("Rectangles are apart") {
    // Act
    const auto result = sut.intersects(rectangle{point2{2, 2}, point2{3, 3}});

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("rectangle::encloses(const rectangle&)", "[quantifier]") {
  // Arrange
  const auto sut = rectangle{point2{-1, -1}, point2{1, 1}};

  SECTION("Other rectangle is inside") {
    // Act
    const auto result = sut.encloses(rectangle{point2{-0.5f, 0}, point2{1, 1}});

    // Assert
    REQUIRE(result);
  }
  SECTION("Other rectangle overlaps the edge") {
    // Act
    const auto result = sut.encloses(rectangle{point2{0, 0}, point2{2, 2}});

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
