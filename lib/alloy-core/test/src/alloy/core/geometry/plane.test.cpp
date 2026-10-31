// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/plane.hpp>

#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::ranges::all_of
#include <array>     // std::array

namespace alloy::core::test {

TEST_CASE("plane::from_point_and_normal(const point3&, const vector3&)", "[factory][static]") {
  // Arrange
  const auto point = point3{1, 2, 3};
  const auto normal = vector3{0, 1, 0};

  // Act
  const auto sut = plane::from_point_and_normal(point, normal);

  // Assert
  SECTION("Contains the point") {
    REQUIRE(sut.contains(point));
  }
  SECTION("Has the normal") {
    REQUIRE(almost_equal(sut.normal(), normal));
  }
}

TEST_CASE("plane::from_points_counter_clockwise(const point3&, const point3&, const point3&)", "[factory][static]") {
  // Arrange
  const auto p0 = point3{0, 0, 5};
  const auto p1 = point3{1, 0, 5};
  const auto p2 = point3{0, 1, 5};

  const auto points = std::array{p0, p1, p2};

  // Act
  const auto sut = plane::from_points_counter_clockwise(p0, p1, p2);

  const auto contains_inputs = std::ranges::all_of(
    points, [&sut](const point3& point) { return sut.contains(point); }
  );

  // Assert
  SECTION("Contains all points") {
    REQUIRE(contains_inputs);
  }
  SECTION("Normal faces positive z") {
    REQUIRE(almost_equal(sut.normal(), vector3{0, 0, 1}));
  }
}

TEST_CASE("plane::from_points_clockwise(const point3&, const point3&, const point3&)", "[factory][static]") {
  // Arrange
  const auto p0 = point3{0, 0, 5};
  const auto p1 = point3{1, 0, 5};
  const auto p2 = point3{0, 1, 5};

  const auto points = std::array{p0, p1, p2};

  // Act
  const auto sut = plane::from_points_clockwise(p0, p1, p2);

  const auto contains_inputs = std::ranges::all_of(
    points, [&sut](const point3& point) { return sut.contains(point); }
  );

  // Assert
  SECTION("Contains all points") {
    REQUIRE(contains_inputs);
  }
  SECTION("Normal faces negative z") {
    REQUIRE(almost_equal(sut.normal(), vector3{0, 0, -1}));
  }
}

TEST_CASE("plane::plane(const vector3&, float)", "[ctor]") {
  // Arrange
  const auto normal = vector3{0, 0, 1};
  const auto distance = 4.0f;

  // Act
  const auto sut = plane{normal, distance};

  // Assert
  SECTION("Contains the point at distance along the normal") {
    REQUIRE(sut.contains(point3{0, 0, 4}));
  }
}

TEST_CASE("plane::normalize()", "[modifier]") {
  // Arrange
  auto sut = plane{0, 0, 2, -8};

  // Act
  sut.normalize();

  // Assert
  SECTION("Scales every component by the normal magnitude") {
    REQUIRE(almost_equal(sut, plane{0, 0, 1, -4}, default_tolerance));
  }
}

TEST_CASE("plane::invert()", "[modifier]") {
  // Arrange
  auto sut = plane{vector3{0, 0, 1}, 4};

  // Act
  sut.invert();

  // Assert
  SECTION("Negates the normal") {
    REQUIRE(almost_equal(sut.normal(), vector3{0, 0, -1}));
  }
  SECTION("Contains the same points") {
    REQUIRE(sut.contains(point3{1, 1, 4}));
  }
}

TEST_CASE("plane::distance_to(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = plane{vector3{0, 0, 1}, 4};

  SECTION("Point is on the side the normal faces") {
    // Act
    const auto result = sut.distance_to(point3{0, 0, 6});

    // Assert
    REQUIRE(almost_equal(result, 2.0f));
  }
  SECTION("Point is on the side opposite the normal") {
    // Act
    const auto result = sut.distance_to(point3{0, 0, 1});

    // Assert
    REQUIRE(almost_equal(result, -3.0f));
  }
}

TEST_CASE("plane::projection(const vector3&)", "[quantifier]") {
  // Arrange
  const auto sut = plane{vector3{0, 0, 1}, 4};
  const auto vec = vector3{1, 2, 3};

  // Act
  const auto result = sut.projection(vec);

  // Assert
  SECTION("Removes the component along the normal") {
    REQUIRE(almost_equal(result, vector3{1, 2, 0}));
  }
}

TEST_CASE("plane::rejection(const vector3&)", "[quantifier]") {
  // Arrange
  const auto sut = plane{vector3{0, 0, 1}, 4};
  const auto vec = vector3{1, 2, 3};

  // Act
  const auto result = sut.rejection(vec);

  // Assert
  SECTION("Keeps only the component along the normal") {
    REQUIRE(almost_equal(result, vector3{0, 0, 3}));
  }
}

TEST_CASE("plane::contains(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = plane{vector3{0, 0, 1}, 4};

  SECTION("Point is on the plane") {
    // Act
    const auto result = sut.contains(point3{3, -2, 4});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is off the plane") {
    // Act
    const auto result = sut.contains(point3{3, -2, -4});

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("plane::contains(const point3&, float)", "[quantifier]") {
  // Arrange
  const auto sut = plane{vector3{0, 0, 1}, 4};
  const auto tolerance = 0.1f;

  SECTION("Point is within tolerance of the plane") {
    // Act
    const auto result = sut.contains(point3{0, 0, 4.05f}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of the plane") {
    // Act
    const auto result = sut.contains(point3{0, 0, 4.5f}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("is_point_over_plane(const point3&, const plane&)", "[utility]") {
  // Arrange
  const auto sut = plane{vector3{0, 0, 1}, 4};

  SECTION("Point is on the side the normal faces") {
    // Act
    const auto result = is_point_over_plane(point3{0, 0, 5}, sut);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is between the origin and the plane") {
    // Act
    const auto result = is_point_over_plane(point3{0, 0, 2}, sut);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("is_point_under_plane(const point3&, const plane&)", "[utility]") {
  // Arrange
  const auto sut = plane{vector3{0, 0, 1}, 4};

  SECTION("Point is between the origin and the plane") {
    // Act
    const auto result = is_point_under_plane(point3{0, 0, 2}, sut);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is on the side the normal faces") {
    // Act
    const auto result = is_point_under_plane(point3{0, 0, 5}, sut);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
