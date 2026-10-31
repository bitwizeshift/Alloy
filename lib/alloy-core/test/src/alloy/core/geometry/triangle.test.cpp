// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/triangle.hpp>

#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::ranges::all_of
#include <array>     // std::array

namespace alloy::core::test {

TEST_CASE("triangle::normal()", "[observer]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 2}, point3{2, 0, 2}, point3{0, 2, 2}};

  // Act
  const auto result = sut.normal();

  // Assert
  SECTION("Faces the side the points wind counter-clockwise from") {
    REQUIRE(almost_equal(result, vector3{0, 0, 1}));
  }
}

TEST_CASE("triangle::supporting_plane()", "[observer]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 2}, point3{2, 0, 2}, point3{0, 2, 2}};

  const auto vertices = std::array{sut.a(), sut.b(), sut.c()};

  // Act
  const auto result = sut.supporting_plane();

  const auto contains_vertices = std::ranges::all_of(
    vertices, [&result](const point3& point) { return result.contains(point); }
  );

  // Assert
  SECTION("Contains every point") {
    REQUIRE(contains_vertices);
  }
  SECTION("Faces the same way as the triangle") {
    REQUIRE(almost_equal(result.normal(), sut.normal()));
  }
}

TEST_CASE("triangle::area()", "[observer]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 2}, point3{4, 0, 2}, point3{0, 3, 2}};

  // Act
  const auto result = sut.area();

  // Assert
  SECTION("Is half of the base times the height") {
    REQUIRE(almost_equal(result, 6.0f));
  }
}

TEST_CASE("triangle::barycentric_of(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 2}, point3{3, 0, 2}, point3{0, 3, 2}};

  const auto expected = vector3{
    1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f
  };

  // Act
  const auto result = sut.barycentric_of(point3{1, 1, 5});

  const auto weights = vector3{result.u(), result.v(), result.w()};

  // Assert
  SECTION("Is the coordinate of the point projected onto the triangle") {
    REQUIRE(almost_equal(weights, expected));
  }
}

TEST_CASE("triangle::nearest_point(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Point projects onto the face") {
    // Act
    const auto result = sut.nearest_point(point3{1, 1, 3});

    // Assert
    REQUIRE(almost_equal(result, point3{1, 1, 0}));
  }
  SECTION("Point is nearest to vertex a") {
    // Act
    const auto result = sut.nearest_point(point3{-1, -1, 1});

    // Assert
    REQUIRE(almost_equal(result, point3{0, 0, 0}));
  }
  SECTION("Point is nearest to vertex b") {
    // Act
    const auto result = sut.nearest_point(point3{6, -1, 0});

    // Assert
    REQUIRE(almost_equal(result, point3{4, 0, 0}));
  }
  SECTION("Point is nearest to vertex c") {
    // Act
    const auto result = sut.nearest_point(point3{-1, 6, 0});

    // Assert
    REQUIRE(almost_equal(result, point3{0, 4, 0}));
  }
  SECTION("Point is nearest to edge ab") {
    // Act
    const auto result = sut.nearest_point(point3{2, -3, 1});

    // Assert
    REQUIRE(almost_equal(result, point3{2, 0, 0}));
  }
  SECTION("Point is nearest to edge ac") {
    // Act
    const auto result = sut.nearest_point(point3{-3, 2, 1});

    // Assert
    REQUIRE(almost_equal(result, point3{0, 2, 0}));
  }
  SECTION("Point is nearest to edge bc") {
    // Act
    const auto result = sut.nearest_point(point3{3, 3, 0});

    // Assert
    REQUIRE(almost_equal(result, point3{2, 2, 0}));
  }
}

TEST_CASE("triangle::contains(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Point is inside the triangle") {
    // Act
    const auto result = sut.contains(point3{1, 1, 0});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is on an edge") {
    // Act
    const auto result = sut.contains(point3{2, 2, 0});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is off the plane of the triangle") {
    // Act
    const auto result = sut.contains(point3{1, 1, 1});

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Point is on the plane outside of the triangle") {
    // Act
    const auto result = sut.contains(point3{3, 3, 0});

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("triangle::contains(const point3&, float)", "[quantifier]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};
  const auto tolerance = 0.1f;

  SECTION("Point is within tolerance of the triangle") {
    // Act
    const auto result = sut.contains(point3{1, 1, 0.05f}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of the triangle") {
    // Act
    const auto result = sut.contains(point3{1, 1, 0.5f}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
