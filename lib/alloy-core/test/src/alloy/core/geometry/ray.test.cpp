// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/ray.hpp>

#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("ray::from_points(const point3&, const point3&)", "[factory][static]") {
  // Arrange
  const auto origin = point3{1, 2, 3};
  const auto p = point3{1, 2, 10};

  // Act
  const auto sut = ray::from_points(origin, p);

  // Assert
  SECTION("Starts at the origin") {
    REQUIRE(sut.origin() == origin);
  }
  SECTION("Faces the point") {
    REQUIRE(almost_equal(sut.direction(), vector3{0, 0, 1}));
  }
}

TEST_CASE("ray::inverse()", "[quantifier]") {
  // Arrange
  const auto sut = ray{point3{1, 2, 3}, vector3{0, 0, 1}};

  // Act
  const auto result = sut.inverse();

  // Assert
  SECTION("Keeps the origin") {
    REQUIRE(result.origin() == sut.origin());
  }
  SECTION("Faces the opposite direction") {
    REQUIRE(almost_equal(result.direction(), vector3{0, 0, -1}));
  }
}

TEST_CASE("ray::point_at_distance(real)", "[quantifier]") {
  // Arrange
  const auto sut = ray{point3{1, 2, 3}, vector3{0, 0, 1}};

  // Act
  const auto result = sut.point_at_distance(real{2});

  // Assert
  SECTION("Moves along the direction from the origin") {
    REQUIRE(almost_equal(result, point3{1, 2, 5}));
  }
}

TEST_CASE("ray::contains(const point3&)", "[quantifier]") {
  SECTION("Ray is aligned to an axis") {
    // Arrange
    const auto sut = ray{};

    SECTION("Point is ahead of the origin") {
      // Act
      const auto result = sut.contains(point3{7, 0, 0});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is the origin") {
      // Act
      const auto result = sut.contains(point3{0, 0, 0});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is behind the origin") {
      // Act
      const auto result = sut.contains(point3{-7, 0, 0});

      // Assert
      REQUIRE_FALSE(result);
    }
    SECTION("Point is off the ray") {
      // Act
      const auto result = sut.contains(point3{7, 1, 0});

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Ray is not aligned to an axis") {
    // Arrange
    const auto sut = ray::from_points(point3{1, 1, 1}, point3{2, 2, 2});

    SECTION("Point is ahead of the origin") {
      // Act
      const auto result = sut.contains(point3{4, 4, 4});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is behind the origin") {
      // Act
      const auto result = sut.contains(point3{-2, -2, -2});

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("ray::contains(const point3&, real)", "[quantifier]") {
  // Arrange
  const auto sut = ray{};
  const auto tolerance = real{0.1f};

  SECTION("Point is within tolerance of the ray") {
    // Act
    const auto result = sut.contains(point3{5, 0.05f, 0}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is within tolerance behind the origin") {
    // Act
    const auto result = sut.contains(point3{-0.05f, 0, 0}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of the ray") {
    // Act
    const auto result = sut.contains(point3{5, 0.5f, 0}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
