/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/geometry/intersection.hpp"
#include "alloy/core/geometry/line.hpp"
#include "alloy/core/geometry/point/point3.hpp"

#include <catch2/catch.hpp>

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
} // namespace alloy::core::test
