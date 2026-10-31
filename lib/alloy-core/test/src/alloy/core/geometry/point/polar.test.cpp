// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/point/polar.hpp>

#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/math_constants.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("to_polar(const point2&)", "[utility]") {
  // Arrange
  const auto p = point2{0, 2};

  // Act
  const auto result = to_polar(p);

  // Assert
  SECTION("Has the distance from the origin") {
    REQUIRE(almost_equal(result.r(), real{2}));
  }
  SECTION("Has the angle from the x-axis") {
    REQUIRE(almost_equal(result.theta(), radian{math_constants::half_pi}));
  }
}

TEST_CASE("to_point(const polar&)", "[utility]") {
  SECTION("Angle is a quarter turn") {
    // Arrange
    const auto p = polar{real{2}, radian{math_constants::half_pi}};

    // Act
    const auto result = to_point(p);

    // Assert
    REQUIRE(almost_equal(result, point2{0, 2}));
  }
  SECTION("Converting back from polar") {
    // Arrange
    const auto p = point2{-3, 4};

    // Act
    const auto result = to_point(to_polar(p));

    // Assert
    REQUIRE(almost_equal(result, p));
  }
}

} // namespace alloy::core::test
