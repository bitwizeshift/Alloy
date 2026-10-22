// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/triangle.hpp>

#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

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

  // Act
  const auto result = sut.supporting_plane();

  // Assert
  SECTION("Contains every point") {
    REQUIRE(result.contains(sut.a()));
    REQUIRE(result.contains(sut.b()));
    REQUIRE(result.contains(sut.c()));
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
    REQUIRE(almost_equal(result, real{6}));
  }
}

TEST_CASE("triangle::barycentric_of(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 2}, point3{3, 0, 2}, point3{0, 3, 2}};

  // Act
  const auto result = sut.barycentric_of(point3{1, 1, 5});

  // Assert
  SECTION("Is the coordinate of the point projected onto the triangle") {
    REQUIRE(almost_equal(result.u(), real{1} / real{3}));
    REQUIRE(almost_equal(result.v(), real{1} / real{3}));
    REQUIRE(almost_equal(result.w(), real{1} / real{3}));
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

TEST_CASE("triangle::contains(const point3&, real)", "[quantifier]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};
  const auto tolerance = real{0.1f};

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
