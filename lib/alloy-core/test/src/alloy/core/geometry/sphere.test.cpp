// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/sphere.hpp>

#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("sphere::contains(const point&)", "[containment]") {
  // Arrange
  constexpr auto radius = 3.0;
  constexpr auto center = point3{1,1,1};
  constexpr auto sut = sphere{
    center,
    radius,
  };

  SECTION("Point is inside of sphere") {
    SECTION("Returns true") {
      // Act
      const auto result = sut.contains(center);

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Point is on the border") {
    // Arrange
    constexpr auto border = point3{
      center.x() + radius,
      center.y(),
      center.z(),
    };

    SECTION("Returns true") {
      // Act
      const auto result = sut.contains(border);

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Point is outside of sphere") {
    // Arrange
    constexpr auto point = point3{
      center.x() + center.x() * 2,
      center.y() + center.y() * 2,
      center.z() + center.z() * 2,
    };

    SECTION("Returns false") {
      // Act
      const auto result = sut.contains(point);

      // Assert
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("sphere::contains(const point&, real)", "[containment]") {
  // Arrange
  constexpr auto tolerance = real{0.1};
  constexpr auto radius = 3.0;
  constexpr auto center = point3{1,1,1};
  constexpr auto sut = sphere{
    center,
    radius,
  };

  SECTION("Point is inside of sphere") {
    SECTION("Returns true") {
      // Act
      const auto result = sut.contains(center, tolerance);

      // Assert
      REQUIRE(result);
    }
  }
  SECTION("Point is outside of sphere") {
    SECTION("Point is within tolerance") {
      const auto point = point3{
        center.x() + tolerance / 2,
        center.y(),
        center.z()
      };
      SECTION("Returns true") {
        // Act
        const auto result = sut.contains(point, tolerance);

        // Assert
        REQUIRE(result);
      }
    }
    SECTION("Point is outside of tolerance") {
      const auto point = point3{
        center.x() + radius + tolerance * 2,
        center.y(),
        center.z()
      };
      SECTION("Returns false") {
        // Act
        const auto result = sut.contains(point, tolerance);

        // Assert
        REQUIRE_FALSE(result);
      }
    }
  }
}

} // namespace alloy::core::test
