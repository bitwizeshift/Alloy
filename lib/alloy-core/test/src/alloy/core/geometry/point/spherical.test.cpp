// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/point/spherical.hpp>

#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/math_constants.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("to_spherical(const point3&)", "[utility]") {
  // Arrange
  const auto p = point3{0, 2, 0};

  // Act
  const auto result = to_spherical(p);

  // Assert
  SECTION("Has the distance from the origin") {
    REQUIRE(almost_equal(result.rho(), real{2}));
  }
  SECTION("Has the angle from the z-axis") {
    REQUIRE(almost_equal(result.phi(), radian{math_constants::half_pi}));
  }
  SECTION("Has the angle from the x-axis") {
    REQUIRE(almost_equal(result.theta(), radian{math_constants::half_pi}));
  }
}

TEST_CASE("to_point(const spherical&)", "[utility]") {
  SECTION("Point is on the y-axis") {
    // Arrange
    const auto p = spherical{
      real{2},
      radian{math_constants::half_pi},
      radian{math_constants::half_pi}
    };

    // Act
    const auto result = to_point(p);

    // Assert
    REQUIRE(almost_equal(result, point3{0, 2, 0}));
  }
  SECTION("Converting back from spherical") {
    // Arrange
    const auto p = point3{-3, 4, 5};

    // Act
    const auto result = to_point(to_spherical(p));

    // Assert
    REQUIRE(almost_equal(result, p));
  }
}

} // namespace alloy::core::test
