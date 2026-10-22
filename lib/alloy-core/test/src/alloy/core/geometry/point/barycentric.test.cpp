// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/point/barycentric.hpp>

#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/math/math.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("barycentric::barycentric()", "[ctor]") {
  // Arrange / Act
  const auto sut = barycentric{};

  // Assert
  SECTION("Has all components 0") {
    REQUIRE(sut.u() == real{0});
    REQUIRE(sut.v() == real{0});
    REQUIRE(sut.w() == real{0});
  }
}

TEST_CASE("barycentric::from_cartesian(const point3&, const point3&, const point3&, const point3&)", "[factory][static]") {
  // Arrange
  const auto t0 = point3{0, 0, 1};
  const auto t1 = point3{3, 0, 1};
  const auto t2 = point3{0, 3, 1};

  SECTION("Point is a vertex") {
    // Act
    const auto result = barycentric::from_cartesian(t1, t0, t1, t2);

    // Assert
    REQUIRE(almost_equal(result.u(), real{0}));
    REQUIRE(almost_equal(result.v(), real{1}));
    REQUIRE(almost_equal(result.w(), real{0}));
  }
  SECTION("Point is the centroid") {
    // Act
    const auto result = barycentric::from_cartesian(
      point3{1, 1, 1},
      t0,
      t1,
      t2
    );

    // Assert
    REQUIRE(almost_equal(result.u(), real{1} / real{3}));
    REQUIRE(almost_equal(result.v(), real{1} / real{3}));
    REQUIRE(almost_equal(result.w(), real{1} / real{3}));
  }
}

TEST_CASE("barycentric::from_cartesian(const point2&, const point2&, const point2&, const point2&)", "[factory][static]") {
  // Arrange
  const auto t0 = point2{0, 0};
  const auto t1 = point2{3, 0};
  const auto t2 = point2{0, 3};

  SECTION("Point is a vertex") {
    // Act
    const auto result = barycentric::from_cartesian(t2, t0, t1, t2);

    // Assert
    REQUIRE(almost_equal(result.u(), real{0}));
    REQUIRE(almost_equal(result.v(), real{0}));
    REQUIRE(almost_equal(result.w(), real{1}));
  }
  SECTION("Point is the centroid") {
    // Act
    const auto result = barycentric::from_cartesian(point2{1, 1}, t0, t1, t2);

    // Assert
    REQUIRE(almost_equal(result.u(), real{1} / real{3}));
    REQUIRE(almost_equal(result.v(), real{1} / real{3}));
    REQUIRE(almost_equal(result.w(), real{1} / real{3}));
  }
}

} // namespace alloy::core::test
