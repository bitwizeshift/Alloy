// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/intersection.hpp>

#include <alloy/core/geometry/line.hpp>
#include <alloy/core/geometry/point/point3.hpp>

#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("intersection::intersection()", "[ctor]") {
  // Arrange / Act
  const auto sut = intersection<point3, line>{};

  // Assert
  SECTION("Has no intersection") {
    REQUIRE_FALSE(sut.has_intersection());
  }
}

TEST_CASE("intersection::intersection(Type&&)", "[ctor]") {
  // Arrange / Act
  const auto sut = intersection<point3, line>{ line{} };

  // Assert
  SECTION("Has intersection") {
    REQUIRE(sut.has_intersection());
  }
  SECTION("Contains constructed intersection") {
    REQUIRE(sut.contains<line>());
  }
}

TEST_CASE("intersection::has_intersection()", "[observer]") {
  SECTION("Contains an intersection") {
    // Arrange
    const auto sut = intersection<point3, line>{ line{} };

    // Act
    const auto result = sut.has_intersection();

    // Assert
    SECTION("Returns true") {
      REQUIRE(result);
    }
  }
  SECTION("Does not contain an intersection") {
    // Arrange
    const auto sut = intersection<point3, line>{};

    // Act
    const auto result = sut.has_intersection();

    // Assert
    SECTION("Returns false") {
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("intersection::contains<Type>()", "[observer]") {
  SECTION("Contains intersection") {
    // Arrange
    const auto sut = intersection<point3, line>{ line{} };

    SECTION("Request type is not intersection type") {
      // Act
      const auto result = sut.contains<point3>();

      // Assert
      SECTION("Returns false") {
        REQUIRE_FALSE(result);
      }
    }
    SECTION("Requested type is intersection type") {
      // Act
      const auto result = sut.contains<line>();

      // Assert
      SECTION("Returns true") {
        REQUIRE(result);
      }
    }
  }
  SECTION("Does not contain an intersection") {
    // Arrange
    const auto sut = intersection<point3, line>{};

    // Act
    const auto result = sut.contains<point3>();

    // Assert
    SECTION("Returns false") {
      REQUIRE_FALSE(result);
    }
  }
}

TEST_CASE("intersection::try_as<Type>()", "[observer]") {
  SECTION("Contains intersection") {
    // Arrange
    const auto sut = intersection<point3, line>{ line{} };

    SECTION("Request type is not intersection type") {
      // Act
      const auto result = sut.try_as<point3>();

      // Assert
      SECTION("Returns nullptr") {
        REQUIRE(result == nullptr);
      }
    }
    SECTION("Requested type is intersection type") {
      // Act
      const auto result = sut.try_as<line>();

      // Assert
      SECTION("Returns pointer to intersection type") {
        REQUIRE(result != nullptr);
      }
    }
  }
  SECTION("Does not contain an intersection") {
    // Arrange
    const auto sut = intersection<point3, line>{};

    // Act
    const auto result = sut.try_as<point3>();

    // Assert
    SECTION("Returns nullptr") {
      REQUIRE(result == nullptr);
    }
  }
}
TEST_CASE("operator==(const intersection<Types...>&, const intersection<Types...>&)", "[equality]") {
  SECTION("Both have no intersection") {
    // Arrange
    const auto lhs = intersection<point3, line>{};
    const auto rhs = intersection<point3, line>{};

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE(result);
  }
  SECTION("Only one has an intersection") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{};

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Both hold equal values") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{ point3{1, 2, 3} };

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE(result);
  }
  SECTION("Both hold different values") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{ point3{3, 2, 1} };

    // Act
    const auto result = (lhs == rhs);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("almost_equal(const intersection<Types...>&, const intersection<Types...>&, real)", "[equality]") {
  const auto tolerance = real{1e-3f};

  SECTION("Both have no intersection") {
    // Arrange
    const auto lhs = intersection<point3, line>{};
    const auto rhs = intersection<point3, line>{};

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Only one has an intersection") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{};

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Both hold different types") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{} };
    const auto rhs = intersection<point3, line>{ line{} };

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Both hold values within tolerance") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{ point3{1, 2, 3.0001f} };

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE(result);
  }
  SECTION("Both hold values outside of tolerance") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{ point3{1, 2, 4} };

    // Act
    const auto result = almost_equal(lhs, rhs, tolerance);

    // Assert
    REQUIRE_FALSE(result);
  }
}

} // namespace alloy::core::test
