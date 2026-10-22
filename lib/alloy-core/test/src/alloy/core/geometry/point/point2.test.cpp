// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/point/point2.hpp>

#include <alloy/core/math/vector/vector2.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("point2::operator+=(const vector2&)", "[arithmetic]") {
  // Arrange
  auto sut = point2{1, 2};

  // Act
  sut += vector2{3, -4};

  // Assert
  SECTION("Moves the point by the vector") {
    REQUIRE(sut == point2{4, -2});
  }
}

TEST_CASE("point2::operator-=(const vector2&)", "[arithmetic]") {
  // Arrange
  auto sut = point2{1, 2};

  // Act
  sut -= vector2{3, -4};

  // Assert
  SECTION("Moves the point against the vector") {
    REQUIRE(sut == point2{-2, 6});
  }
}

TEST_CASE("operator-(const point2&, const point2&)", "[arithmetic]") {
  // Arrange
  const auto lhs = point2{4, 6};
  const auto rhs = point2{1, 2};

  // Act
  const auto result = lhs - rhs;

  // Assert
  SECTION("Is the vector from rhs to lhs") {
    REQUIRE(result == vector2{3, 4});
  }
}

} // namespace alloy::core::test
