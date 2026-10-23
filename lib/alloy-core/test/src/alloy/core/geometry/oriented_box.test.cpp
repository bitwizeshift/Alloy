// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/oriented_box.hpp>

#include <alloy/core/geometry/axis_aligned_box.hpp>
#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/basis/basis3.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm> // std::ranges::all_of

namespace alloy::core::test {
namespace {

  /// A unit cube rotated 45 degrees about the z-axis, centered on the origin
  auto make_rotated_cube() -> oriented_box
  {
    const auto orientation = basis3::make(
      vector3{-1, 1, 0},
      vector3{1, 1, 0},
      vector3{0, 0, 1}
    );
    return oriented_box{point3{}, vector3{1, 1, 1}, orientation};
  }

} // namespace

TEST_CASE("oriented_box::from_axis_aligned_box(const axis_aligned_box&)", "[factory][static]") {
  // Arrange
  const auto box = axis_aligned_box{point3{1, 2, 3}, point3{3, 6, 9}};

  // Act
  const auto sut = oriented_box::from_axis_aligned_box(box);

  // Assert
  SECTION("Is centered on the box") {
    REQUIRE(almost_equal(sut.center(), point3{2, 4, 6}));
  }
  SECTION("Extends to the faces of the box") {
    REQUIRE(almost_equal(sut.half_extents(), vector3{1, 2, 3}));
  }
  SECTION("Has the same bounding box") {
    REQUIRE(almost_equal(sut.bounding_box(), box, default_tolerance));
  }
}

TEST_CASE("oriented_box::corners()", "[observer]") {
  // Arrange
  const auto sut = make_rotated_cube();

  // Act
  const auto result = sut.corners();

  const auto all_contained = std::ranges::all_of(
    result, [&sut](const point3& corner) { return sut.contains(corner); }
  );
  const auto all_nearest = std::ranges::all_of(
    result,
    [&sut](const point3& corner) {
      return almost_equal(sut.nearest_point(corner), corner);
    }
  );
  const auto all_at_radius = std::ranges::all_of(
    result,
    [&sut](const point3& corner) {
      return almost_equal((corner - sut.center()).magnitude(), sqrt(real{3}));
    }
  );

  // Assert
  SECTION("Every corner is contained in the box") {
    REQUIRE(all_contained);
  }
  SECTION("Every corner is its own nearest surface point") {
    REQUIRE(all_nearest);
  }
  SECTION("Every corner is at the cube's radius") {
    REQUIRE(all_at_radius);
  }
}

TEST_CASE("oriented_box::bounding_box()", "[observer]") {
  // Arrange
  const auto sut = make_rotated_cube();

  // Act
  const auto result = sut.bounding_box();

  // Assert
  SECTION("Encloses the rotated extents") {
    const auto extent = sqrt(real{2});
    const auto expected = axis_aligned_box{
      point3{-extent, -extent, -1},
      point3{extent, extent, 1}
    };
    REQUIRE(almost_equal(result, expected, default_tolerance));
  }
}

TEST_CASE("oriented_box::nearest_point(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = make_rotated_cube();

  SECTION("Point is inside the box") {
    // Act
    const auto result = sut.nearest_point(point3{0.5f, 0, 0});

    // Assert
    REQUIRE(almost_equal(result, point3{0.5f, 0, 0}));
  }
  SECTION("Point is outside a face") {
    // Act
    const auto result = sut.nearest_point(point3{2, 2, 0});

    // Assert
    const auto half_root_two = sqrt(real{2}) * real{0.5};
    REQUIRE(almost_equal(result, point3{half_root_two, half_root_two, 0}));
  }
}

TEST_CASE("oriented_box::contains(const point3&)", "[quantifier]") {
  // Arrange
  const auto sut = make_rotated_cube();

  SECTION("Point is inside the rotated box, outside the unrotated box") {
    // Act
    const auto result = sut.contains(point3{1.3f, 0, 0});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside the rotated box, inside the unrotated box") {
    // Act
    const auto result = sut.contains(point3{0.9f, 0.9f, 0});

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("oriented_box::contains(const point3&, real)", "[quantifier]") {
  // Arrange
  const auto sut = make_rotated_cube();
  const auto tolerance = real{0.1f};

  SECTION("Point is within tolerance of a face") {
    // Act
    const auto result = sut.contains(point3{0, 0, 1.05f}, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside tolerance of a face") {
    // Act
    const auto result = sut.contains(point3{0, 0, 1.5f}, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
