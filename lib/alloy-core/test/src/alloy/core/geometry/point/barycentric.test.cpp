// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/point/barycentric.hpp>

#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array> // std::array

namespace alloy::core::test {

TEST_CASE("barycentric::barycentric()", "[ctor]") {
  // Arrange
  const auto expected = std::array<real,3>{};

  // Act
  const auto sut = barycentric{};

  const auto weights = std::array{sut.u(), sut.v(), sut.w()};

  // Assert
  REQUIRE(weights == expected);
}

TEST_CASE("barycentric::from_cartesian(const point3&, const point3&, const point3&, const point3&)", "[factory][static]") {
  // Arrange
  const auto t0 = point3{0, 0, 1};
  const auto t1 = point3{3, 0, 1};
  const auto t2 = point3{0, 3, 1};

  SECTION("Point is a vertex") {
    // Arrange
    const auto expected = vector3{real{0}, real{1}, real{0}};

    // Act
    const auto result = barycentric::from_cartesian(t1, t0, t1, t2);

    const auto weights = vector3{result.u(), result.v(), result.w()};

    // Assert
    REQUIRE(almost_equal(weights, expected));
  }
  SECTION("Point is the centroid") {
    // Arrange
    const auto expected = vector3{real{1} / real{3}, real{1} / real{3}, real{1} / real{3}};

    // Act
    const auto result = barycentric::from_cartesian(
      point3{1, 1, 1},
      t0,
      t1,
      t2
    );

    const auto weights = vector3{result.u(), result.v(), result.w()};

    // Assert
    REQUIRE(almost_equal(weights, expected));
  }
}

TEST_CASE("barycentric::from_cartesian(const point2&, const point2&, const point2&, const point2&)", "[factory][static]") {
  // Arrange
  const auto t0 = point2{0, 0};
  const auto t1 = point2{3, 0};
  const auto t2 = point2{0, 3};

  SECTION("Point is a vertex") {
    // Arrange
    const auto expected = vector3{real{0}, real{0}, real{1}};

    // Act
    const auto result = barycentric::from_cartesian(t2, t0, t1, t2);

    const auto weights = vector3{result.u(), result.v(), result.w()};

    // Assert
    REQUIRE(almost_equal(weights, expected));
  }
  SECTION("Point is the centroid") {
    // Arrange
    const auto expected = vector3{real{1} / real{3}, real{1} / real{3}, real{1} / real{3}};

    // Act
    const auto result = barycentric::from_cartesian(point2{1, 1}, t0, t1, t2);

    const auto weights = vector3{result.u(), result.v(), result.w()};

    // Assert
    REQUIRE(almost_equal(weights, expected));
  }
}

} // namespace alloy::core::test
