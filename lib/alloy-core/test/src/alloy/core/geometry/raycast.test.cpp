// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/raycast.hpp>

#include <alloy/core/geometry/axis_aligned_box.hpp>
#include <alloy/core/geometry/capsule.hpp>
#include <alloy/core/geometry/line.hpp>
#include <alloy/core/geometry/line_segment.hpp>
#include <alloy/core/geometry/oriented_box.hpp>
#include <alloy/core/geometry/plane.hpp>
#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/geometry/ray.hpp>
#include <alloy/core/geometry/sphere.hpp>
#include <alloy/core/geometry/triangle.hpp>

#include <alloy/core/math/basis/basis3.hpp>
#include <alloy/core/math/math.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {
namespace {

  /// A unit cube rotated 45 degrees about the z-axis, centered on the origin
  auto make_rotated_cube() -> oriented_box
  {
    const auto orientation = basis3::make(
      vector3{-1, 1, 0},
      vector3{1, 1, 0},
      vector3{0, 0, 1}
    );
    return oriented_box{point3{}, vector3{1, 1, 1}, orientation};
  }

} // namespace

TEST_CASE("raycastable<Shape>", "[raycast]") {
  SECTION("Traits are specialized for the shape") {
    // Act
    constexpr auto property_holds = raycastable<sphere>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Traits are not specialized for the shape") {
    // Act
    constexpr auto property_holds = raycastable<line>;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("raycast(const ray&, const plane&)", "[raycast]") {
  // Arrange
  const auto sut = plane{0, 0, 2, -2}; // z = 1, not normalized

  SECTION("Ray faces the plane") {
    // Arrange
    const auto r = ray{point3{0, 0, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the plane crossing") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{4}));
    }
  }
  SECTION("Ray faces the plane from the side opposite its normal") {
    // Arrange
    const auto r = ray{point3{0, 0, -1}, vector3{0, 0, 1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the plane crossing") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{2}));
    }
  }
  SECTION("Ray starts on the plane") {
    // Arrange
    const auto r = ray{point3{3, 3, 1}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Reports an immediate hit at the ray origin") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{0}));
    }
  }
  SECTION("Ray faces away from the plane") {
    // Arrange
    const auto r = ray{point3{0, 0, 5}, vector3{0, 0, 1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
  SECTION("Ray is parallel to the plane") {
    // Arrange
    const auto r = ray{point3{0, 0, 5}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("raycast(const ray&, const triangle&)", "[raycast]") {
  // Arrange
  const auto sut = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Ray faces the front of the triangle") {
    // Arrange
    const auto r = ray{point3{1, 1, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the triangle face") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{5}));
    }
  }
  SECTION("Ray faces the back of the triangle") {
    // Arrange
    const auto r = ray{point3{1, 1, -2}, vector3{0, 0, 1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the triangle face") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{2}));
    }
  }
  SECTION("Ray passes beside the triangle") {
    // Arrange
    const auto r = ray{point3{3, 3, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
  SECTION("Triangle is behind the ray") {
    // Arrange
    const auto r = ray{point3{1, 1, 5}, vector3{0, 0, 1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
  SECTION("Ray lies in the plane of the triangle") {
    // Arrange
    const auto r = ray{point3{-1, 1, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("raycast(const ray&, const sphere&)", "[raycast]") {
  // Arrange
  const auto sut = sphere{point3{5, 0, 0}, real{1}};

  SECTION("Ray faces the sphere") {
    // Arrange
    const auto r = ray{point3{0, 0, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the nearest sphere surface") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{4}));
    }
  }
  SECTION("Ray grazes the sphere") {
    // Arrange
    const auto r = ray{point3{0, 1, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the tangent point") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{5}));
    }
  }
  SECTION("Ray starts inside the sphere") {
    // Arrange
    const auto r = ray{point3{5, real{0.5f}, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Reports an immediate hit at the ray origin") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{0}));
    }
  }
  SECTION("Ray passes beside the sphere") {
    // Arrange
    const auto r = ray{point3{0, 2, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
  SECTION("Sphere is behind the ray") {
    // Arrange
    const auto r = ray{point3{0, 0, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("raycast(const ray&, const capsule&)", "[raycast]") {
  // Arrange
  const auto sut = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Ray faces the side of the capsule") {
    // Arrange
    const auto r = ray{point3{-5, 2, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the cylindrical side") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{4}));
    }
  }
  SECTION("Ray faces a cap along the axis") {
    // Arrange
    const auto r = ray{point3{0, 10, 0}, vector3{0, -1, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the nearest capsule cap") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{5}));
    }
  }
  SECTION("Ray faces a cap from below") {
    // Arrange
    const auto r = ray{point3{0, -5, 0}, vector3{0, 1, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the nearest capsule cap") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{4}));
    }
  }
  SECTION("Ray starts inside the capsule") {
    // Arrange
    const auto r = ray{point3{0, 2, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Reports an immediate hit at the ray origin") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{0}));
    }
  }
  SECTION("Ray passes beside the capsule") {
    // Arrange
    const auto r = ray{point3{-5, 2, 2}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
  SECTION("Capsule is behind the ray") {
    // Arrange
    const auto r = ray{point3{-5, 2, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("raycast(const ray&, const axis_aligned_box&)", "[raycast]") {
  // Arrange
  const auto sut = axis_aligned_box{point3{-1, -1, -1}, point3{1, 1, 1}};

  SECTION("Ray faces the box") {
    // Arrange
    const auto r = ray{point3{-5, real{0.5f}, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the box entry") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{4}));
    }
  }
  SECTION("Ray starts inside the box") {
    // Arrange
    const auto r = ray{point3{0, 0, 0}, vector3{0, 0, 1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Reports an immediate hit at the ray origin") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{0}));
    }
  }
  SECTION("Ray passes beside the box") {
    // Arrange
    const auto r = ray{point3{-5, 2, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
  SECTION("Box is behind the ray") {
    // Arrange
    const auto r = ray{point3{-5, real{0.5f}, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("raycast(const ray&, const oriented_box&)", "[raycast]") {
  // Arrange
  const auto sut = make_rotated_cube();

  SECTION("Ray faces a corner of the box") {
    // Arrange
    const auto r = ray{point3{-5, 0, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Measures the distance to the rotated corner") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, 5 - sqrt(real{2})));
    }
  }
  SECTION("Ray starts inside the box") {
    // Arrange
    const auto r = ray{point3{real{1.3f}, 0, 0}, vector3{0, 0, 1}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE(result.has_value());
    SECTION("Reports an immediate hit at the ray origin") {
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      REQUIRE(almost_equal(*result, real{0}));
    }
  }
  SECTION("Ray passes beside the box") {
    // Arrange
    const auto r = ray{point3{-5, 0, 2}, vector3{1, 0, 0}};

    // Act
    const auto result = raycast(r, sut);

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

} // namespace alloy::core::test
