// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/line.hpp>

#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("line::point_at_distance(real)", "[quantifier]") {
  // Arrange
  const auto sut = line{point3{1, 2, 3}, vector3{0, 1, 0}};

  // Act
  const auto result = sut.point_at_distance(real{-4});

  // Assert
  SECTION("Moves along the direction from the origin") {
    REQUIRE(almost_equal(result, point3{1, -2, 3}));
  }
}

TEST_CASE("line::contains(const point3&)", "[quantifier]") {
  SECTION("Line is aligned to an axis") {
    // Arrange
    const auto sut = line{};

    SECTION("Point is on the line") {
      // Act
      const auto result = sut.contains(point3{-7, 0, 0});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is off the line") {
      // Act
      const auto result = sut.contains(point3{-7, 1, 0});

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Line is not aligned to an axis") {
    // Arrange
    const auto sut = line{point3{1, 1, 1}, vector3{1, 1, 1}.normalized()};

    SECTION("Point is on the line") {
      // Act
      const auto result = sut.contains(point3{-2, -2, -2});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is off the line") {
      // Act
      const auto result = sut.contains(point3{-2, -2, 2});

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("line::contains(const point3&, real)", "[quantifier]") {
  // Arrange
  const auto sut = line{};
  const auto tolerance = real{0.1f};

  SECTION("Point is within tolerance of the line") {
    // Act
    const auto result = sut.contains(point3{5, 0.05f, 0}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of the line") {
    // Act
    const auto result = sut.contains(point3{5, 0.5f, 0}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("operator==(const line&, const line&)", "[equality]") {
  // Arrange
  const auto lhs = line{point3{0, 0, 2}, vector3{0, 1, 0}};

  SECTION("Lines have different origins on the same line") {
    // Arrange
    const auto rhs = line{point3{0, 5, 2}, vector3{0, 1, 0}};

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE(result);
  }
  SECTION("Lines have opposite directions") {
    // Arrange
    const auto rhs = line{point3{0, 5, 2}, vector3{0, -1, 0}};

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE(result);
  }
  SECTION("Lines are parallel and distinct") {
    // Arrange
    const auto rhs = line{point3{0, 0, 3}, vector3{0, 1, 0}};

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Lines cross") {
    // Arrange
    const auto rhs = line{point3{0, 0, 2}, vector3{1, 0, 0}};

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("almost_equal(const line&, const line&, real)", "[equality]") {
  // Arrange
  const auto lhs = line{point3{0, 0, 2}, vector3{0, 1, 0}};
  const auto tolerance = real{1e-3f};

  SECTION("Lines are the same within tolerance") {
    // Arrange
    const auto rhs = line{point3{0, 5, 2.0001f}, vector3{0, -1, 0}};

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Lines are parallel and distinct") {
    // Arrange
    const auto rhs = line{point3{0, 0, 3}, vector3{0, 1, 0}};

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Lines cross") {
    // Arrange
    const auto rhs = line{point3{0, 0, 2}, vector3{1, 0, 0}};

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
