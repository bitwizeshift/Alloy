// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/frustum.hpp>

#include <alloy/core/geometry/plane.hpp>
#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/angle/degree.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/matrix/matrix4_constants.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/model/projection.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("frustum::from_matrix(const matrix4&)", "[factory][static]") {
  SECTION("Matrix is the identity") {
    // Arrange
    const auto matrix = matrix4_constants::identity;

    // Act
    const auto sut = frustum::from_matrix(matrix);

    // Assert
    SECTION("Planes bound the clip-space cube") {
      SECTION("Left plane bounds the negative x face") {
        REQUIRE(almost_equal(sut.left(), plane{vector3{1, 0, 0}, -1}, default_tolerance));
      }
      SECTION("Right plane bounds the positive x face") {
        REQUIRE(almost_equal(sut.right(), plane{vector3{-1, 0, 0}, -1}, default_tolerance));
      }
      SECTION("Top plane bounds the positive y face") {
        REQUIRE(almost_equal(sut.top(), plane{vector3{0, -1, 0}, -1}, default_tolerance));
      }
      SECTION("Bottom plane bounds the negative y face") {
        REQUIRE(almost_equal(sut.bottom(), plane{vector3{0, 1, 0}, -1}, default_tolerance));
      }
      SECTION("Near plane bounds the negative z face") {
        REQUIRE(almost_equal(sut.near(), plane{vector3{0, 0, 1}, -1}, default_tolerance));
      }
      SECTION("Far plane bounds the positive z face") {
        REQUIRE(almost_equal(sut.far(), plane{vector3{0, 0, -1}, -1}, default_tolerance));
      }
    }
  }
  SECTION("Matrix is a perspective projection") {
    // Arrange
    const auto matrix = projection::perspective(
      degree{90},
      real{1},
      clip_space::depth{real{1}, real{10}}
    ).to_matrix4();

    // Act
    const auto sut = frustum::from_matrix(matrix);

    // Assert
    SECTION("Contains a point in front of the camera") {
      REQUIRE(sut.contains(point3{0, 0, -5}));
    }
    SECTION("Contains a point near the edge of the view") {
      REQUIRE(sut.contains(point3{4.5f, 0, -5}));
    }
    SECTION("Excludes a point outside the edge of the view") {
      REQUIRE_FALSE(sut.contains(point3{5.5f, 0, -5}));
    }
    SECTION("Excludes a point behind the camera") {
      REQUIRE_FALSE(sut.contains(point3{0, 0, 5}));
    }
    SECTION("Excludes a point closer than the near plane") {
      REQUIRE_FALSE(sut.contains(point3{0, 0, -0.5f}));
    }
    SECTION("Excludes a point past the far plane") {
      REQUIRE_FALSE(sut.contains(point3{0, 0, -11}));
    }
  }
}

TEST_CASE("frustum::contains(const point3&)", "[contains]") {
  // Arrange
  const auto sut = frustum::make(
    plane{vector3{1, 0, 0}, -1},
    plane{vector3{-1, 0, 0}, -1},
    plane{vector3{0, -1, 0}, -1},
    plane{vector3{0, 1, 0}, -1},
    plane{vector3{0, 0, 1}, -1},
    plane{vector3{0, 0, -1}, -1}
  );

  SECTION("Point is inside every plane") {
    // Act
    const auto result = sut.contains(point3{0.5f, -0.5f, 0.5f});

    // Assert
    REQUIRE(result);
  }
  SECTION("Point is outside one plane") {
    // Act
    const auto result = sut.contains(point3{0.5f, -1.5f, 0.5f});

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
