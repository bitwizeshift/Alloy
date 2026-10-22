// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/line_segment.hpp>

#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("line_segment::line_segment(const point3&, const vector3&)", "[ctor]") {
  // Arrange
  const auto origin = point3{1, 2, 3};
  const auto distance = vector3{0, 4, 0};

  // Act
  const auto sut = line_segment{origin, distance};

  // Assert
  SECTION("Starts at the origin") {
    REQUIRE(sut.start_point() == origin);
  }
  SECTION("Ends at the origin moved by the distance") {
    REQUIRE(almost_equal(sut.end_point(), point3{1, 6, 3}));
  }
}

TEST_CASE("line_segment::magnitude()", "[observer]") {
  // Arrange
  const auto sut = line_segment{point3{1, 2, 3}, point3{4, 6, 3}};

  // Act
  const auto result = sut.magnitude();

  // Assert
  SECTION("Is the distance between the points") {
    REQUIRE(almost_equal(result, real{5}));
  }
}

TEST_CASE("line_segment::point_at_percent(real)", "[quantifier]") {
  // Arrange
  const auto sut = line_segment{point3{0, 0, 0}, point3{0, 0, 10}};

  // Act
  const auto result = sut.point_at_percent(real{0.25f});

  // Assert
  SECTION("Is the fraction of the way to the end point") {
    REQUIRE(almost_equal(result, point3{0, 0, 2.5f}));
  }
}

TEST_CASE("line_segment::contains(const point3&)", "[quantifier]") {
  SECTION("Segment is aligned to an axis") {
    // Arrange
    const auto sut = line_segment{point3{1, 0, 0}, point3{5, 0, 0}};

    SECTION("Point is between the end points") {
      // Act
      const auto result = sut.contains(point3{3, 0, 0});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is the start point") {
      // Act
      const auto result = sut.contains(point3{1, 0, 0});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is the end point") {
      // Act
      const auto result = sut.contains(point3{5, 0, 0});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is before the start point") {
      // Act
      const auto result = sut.contains(point3{0, 0, 0});

      // Assert
      REQUIRE_FALSE(result);
    }
    SECTION("Point is past the end point") {
      // Act
      const auto result = sut.contains(point3{6, 0, 0});

      // Assert
      REQUIRE_FALSE(result);
    }
    SECTION("Point is off the segment") {
      // Act
      const auto result = sut.contains(point3{3, 1, 0});

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Segment is not aligned to an axis") {
    // Arrange
    const auto sut = line_segment{point3{0, 0, 0}, point3{2, 4, 6}};

    SECTION("Point is between the end points") {
      // Act
      const auto result = sut.contains(point3{1, 2, 3});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is past the end point") {
      // Act
      const auto result = sut.contains(point3{3, 6, 9});

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Segment has no length") {
    // Arrange
    const auto sut = line_segment{point3{1, 2, 3}, point3{1, 2, 3}};

    SECTION("Point is the end point") {
      // Act
      const auto result = sut.contains(point3{1, 2, 3});

      // Assert
      REQUIRE(result);
    }
    SECTION("Point is elsewhere") {
      // Act
      const auto result = sut.contains(point3{1, 2, 4});

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("line_segment::contains(const point3&, real)", "[quantifier]") {
  // Arrange
  const auto sut = line_segment{point3{1, 0, 0}, point3{5, 0, 0}};
  const auto tolerance = real{0.1f};

  SECTION("Point is within tolerance of the segment") {
    // Act
    const auto result = sut.contains(point3{3, 0.05f, 0}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is within tolerance past the end point") {
    // Act
    const auto result = sut.contains(point3{5.05f, 0, 0}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of the segment") {
    // Act
    const auto result = sut.contains(point3{3, 0.5f, 0}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
