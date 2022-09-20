/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/geometry/sphere.hpp"
#include "alloy/core/geometry/point/point3.hpp"

#include <catch2/catch.hpp>

namespace alloy::core::test {

TEST_CASE("sphere::contains(const point&)", "[containment]") {

  // Arrange
  constexpr auto radius = 3.0;
  constexpr auto center = point3{1,1,1};
  constexpr auto sut = sphere{
    center,
    radius,
  };

  // Act / Assert
  SECTION("Point is inside of sphere") {
    SECTION("Returns true") {
      REQUIRE(sut.contains(center));
    }
  }
  SECTION("Point is on the border") {
    // Arrange
    constexpr auto border = point3{
      center.x() + radius,
      center.y(),
      center.z(),
    };

    // Act / Assert
    SECTION("Returns true") {
      REQUIRE(sut.contains(border));
    }
  }
  SECTION("Point is outside of sphere") {
    // Arrange
    constexpr auto point = point3{
      center.x() + center.x() * 2,
      center.y() + center.y() * 2,
      center.z() + center.z() * 2,
    };

    // Act / Assert
    SECTION("Returns false") {
      REQUIRE_FALSE(sut.contains(point));
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
      REQUIRE(sut.contains(center, tolerance));
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
        REQUIRE(sut.contains(point, tolerance));
      }
    }
    SECTION("Point is outside of tolerance") {
      const auto point = point3{
        center.x() + radius + tolerance * 2,
        center.y(),
        center.z()
      };
      SECTION("Returns false") {
        REQUIRE_FALSE(sut.contains(point, tolerance));
      }
    }
  }
}

} // namespace alloy::core::test
