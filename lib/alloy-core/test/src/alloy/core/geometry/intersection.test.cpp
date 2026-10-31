// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/core/geometry/intersection.hpp>

#include <alloy/core/geometry/axis_aligned_box.hpp>
#include <alloy/core/geometry/capsule.hpp>
#include <alloy/core/geometry/circle.hpp>
#include <alloy/core/geometry/frustum.hpp>
#include <alloy/core/geometry/line.hpp>
#include <alloy/core/geometry/line_segment.hpp>
#include <alloy/core/geometry/oriented_box.hpp>
#include <alloy/core/geometry/plane.hpp>
#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/geometry/ray.hpp>
#include <alloy/core/geometry/rectangle.hpp>
#include <alloy/core/geometry/sphere.hpp>
#include <alloy/core/geometry/triangle.hpp>
#include <alloy/core/math/basis/basis3.hpp>
#include <alloy/core/math/matrix/matrix4_constants.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/precision/real.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::core::test {

TEST_CASE("intersection::intersection()", "[ctor]") {
  // Act
  const auto sut = intersection<point3, line>{};

  // Assert
  SECTION("Has no intersection") {
    REQUIRE_FALSE(sut.has_intersection());
  }
}

TEST_CASE("intersection::intersection(Type&&)", "[ctor]") {
  // Act
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

    // Act
    const auto actual = result;

    // Assert
    REQUIRE(actual);
  }
  SECTION("Only one has an intersection") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{};

    // Act
    const auto result = (lhs == rhs);

    // Act
    const auto actual = result;

    // Assert
    REQUIRE_FALSE(actual);
  }
  SECTION("Both hold equal values") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{ point3{1, 2, 3} };

    // Act
    const auto result = (lhs == rhs);

    // Act
    const auto actual = result;

    // Assert
    REQUIRE(actual);
  }
  SECTION("Both hold different values") {
    // Arrange
    const auto lhs = intersection<point3, line>{ point3{1, 2, 3} };
    const auto rhs = intersection<point3, line>{ point3{3, 2, 1} };

    // Act
    const auto result = (lhs == rhs);

    // Act
    const auto actual = result;

    // Assert
    REQUIRE_FALSE(actual);
  }
}

TEST_CASE("almost_equal(const intersection<Types...>&, const intersection<Types...>&, real)", "[equality]") {
  // Arrange
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

//------------------------------------------------------------------------------
// intersects
//------------------------------------------------------------------------------

namespace {

  /// A shape that intersects what it is tested against only when 'hit' is set
  struct hit_shape
  {
    bool hit;
  };

  /// Two shapes whose traits are specialized in both orders, with different
  /// results
  struct left_shape {};
  struct right_shape {};

} // namespace
} // namespace alloy::core::test

template <>
struct alloy::core::intersection_traits<
  alloy::core::test::hit_shape,
  alloy::core::sphere
>
{
  static auto intersects(const test::hit_shape& a, const sphere&)
    noexcept -> bool
  {
    return a.hit;
  }
};

template <>
struct alloy::core::intersection_traits<
  alloy::core::test::left_shape,
  alloy::core::test::right_shape
>
{
  static auto intersects(const test::left_shape&, const test::right_shape&)
    noexcept -> bool
  {
    return true;
  }
};

template <>
struct alloy::core::intersection_traits<
  alloy::core::test::right_shape,
  alloy::core::test::left_shape
>
{
  static auto intersects(const test::right_shape&, const test::left_shape&)
    noexcept -> bool
  {
    return false;
  }
};

namespace alloy::core::test {

TEST_CASE("intersectable<L, R>", "[intersects]") {
  SECTION("Traits are specialized in the given order") {
    // Act
    constexpr auto property_holds = intersectable<plane, sphere>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Traits are specialized in the reverse order") {
    // Act
    constexpr auto property_holds = intersectable<sphere, plane>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Traits are not specialized in either order") {
    // Act
    constexpr auto property_holds = intersectable<line, ray>;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("intersects(const L&, const R&)", "[intersects]") {
  SECTION("Traits are specialized in the given order") {
    // Arrange
    const auto lhs = hit_shape{true};
    const auto rhs = sphere{};

    // Act
    const auto result = intersects(lhs, rhs);

    // Assert
    REQUIRE(result);
  }
  SECTION("Traits are specialized in the reverse order") {
    // Arrange
    const auto lhs = sphere{};
    const auto rhs = hit_shape{false};

    // Act
    const auto result = intersects(lhs, rhs);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Traits are specialized in both orders") {
    // Arrange
    const auto lhs = right_shape{};
    const auto rhs = left_shape{};

    // Act
    const auto result = intersects(lhs, rhs);

    // Assert
    REQUIRE_FALSE(result);
  }
}

namespace {

  /// The box [-1, 1] on every axis
  auto make_unit_box() -> axis_aligned_box
  {
    return axis_aligned_box{point3{-1, -1, -1}, point3{1, 1, 1}};
  }

  /// A unit cube rotated 45 degrees about the z-axis, centered on \p center
  auto make_rotated_cube(const point3& center = point3{}) -> oriented_box
  {
    const auto orientation = basis3::make(
      vector3{-1, 1, 0},
      vector3{1, 1, 0},
      vector3{0, 0, 1}
    );
    return oriented_box{center, vector3{1, 1, 1}, orientation};
  }

} // namespace

//------------------------------------------------------------------------------
// plane
//------------------------------------------------------------------------------

TEST_CASE("intersects(const plane&, const plane&)", "[intersects]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};

  SECTION("Planes cross") {
    // Arrange
    const auto b = plane{vector3{1, 0, 0}, 0};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Planes are parallel and distinct") {
    // Arrange
    const auto b = plane{vector3{0, 0, 1}, 2};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Planes are the same, facing opposite directions") {
    // Arrange
    const auto b = plane{vector3{0, 0, -1}, -1};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Planes are the same, with different scales") {
    // Arrange
    const auto b = plane{0, 0, 3, -3};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const plane&, const line&)", "[intersects]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};

  SECTION("Line crosses the plane") {
    // Arrange
    const auto b = line{point3{0, 0, 5}, vector3{1, 0, 1}.normalized()};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line is parallel to the plane") {
    // Arrange
    const auto b = line{point3{0, 0, 5}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Line lies on the plane") {
    // Arrange
    const auto b = line{point3{0, 0, 1}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = line{point3{0, 0, 5}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const plane&, const ray&)", "[intersects]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};

  SECTION("Ray faces the plane") {
    // Arrange
    const auto b = ray{point3{0, 0, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Ray faces away from the plane") {
    // Arrange
    const auto b = ray{point3{0, 0, 5}, vector3{0, 0, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Ray faces the plane from below") {
    // Arrange
    const auto b = ray{point3{0, 0, -5}, vector3{0, 0, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Ray starts on the plane") {
    // Arrange
    const auto b = ray{point3{3, 0, 1}, vector3{0, 0, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Ray is parallel to the plane") {
    // Arrange
    const auto b = ray{point3{0, 0, 5}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = ray{point3{0, 0, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const plane&, const line_segment&)", "[intersects]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};

  SECTION("Segment crosses the plane") {
    // Arrange
    const auto b = line_segment{point3{0, 0, 0}, point3{1, 1, 2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment ends on the plane") {
    // Arrange
    const auto b = line_segment{point3{0, 0, 0}, point3{1, 1, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment is on one side of the plane") {
    // Arrange
    const auto b = line_segment{point3{0, 0, 2}, point3{1, 1, 3}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = line_segment{point3{0, 0, 0}, point3{1, 1, 2}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const plane&, const sphere&)", "[intersects]") {
  // Arrange
  const auto a = plane{0, 0, 2, -4}; // z = 2, not normalized

  SECTION("Sphere straddles the plane") {
    // Arrange
    const auto b = sphere{point3{0, 0, 3}, real{2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere touches the plane") {
    // Arrange
    const auto b = sphere{point3{0, 0, 5}, real{3}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere is apart from the plane") {
    // Arrange
    const auto b = sphere{point3{0, 0, 5}, real{2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = sphere{point3{0, 0, 5}, real{2}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const plane&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto b = make_unit_box();

  SECTION("Plane cuts through the box") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, real{0.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane touches a face of the box") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, 1};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane cuts off a corner of the box") {
    // Arrange
    const auto a = plane{vector3{1, 1, 0}.normalized(), real{1.3f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane passes beside a corner of the box") {
    // Arrange
    const auto a = plane{vector3{1, 1, 0}.normalized(), real{1.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, real{0.5f}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

//------------------------------------------------------------------------------
// line
//------------------------------------------------------------------------------

TEST_CASE("intersects(const line&, const sphere&)", "[intersects]") {
  // Arrange
  const auto b = sphere{point3{0, 1, 0}, real{1}};

  SECTION("Line passes through the sphere") {
    // Arrange
    const auto a = line{point3{-5, 1, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line touches the sphere") {
    // Arrange
    const auto a = line{point3{-5, 0, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line misses the sphere") {
    // Arrange
    const auto a = line{point3{-5, -1, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line{point3{-5, 1, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const line&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto b = make_unit_box();

  SECTION("Line passes through the box") {
    // Arrange
    const auto a = line{point3{5, 5, 5}, vector3{1, 1, 1}.normalized()};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line misses the box") {
    // Arrange
    const auto a = line{point3{0, 3, 0}, vector3{1, 1, 0}.normalized()};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Line is parallel to an axis inside the box") {
    // Arrange
    const auto a = line{point3{0, 0.5f, 0.5f}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line is parallel to an axis outside the box") {
    // Arrange
    const auto a = line{point3{0, 1.5f, 0.5f}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line{point3{5, 5, 5}, vector3{1, 1, 1}.normalized()};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

//------------------------------------------------------------------------------
// ray
//------------------------------------------------------------------------------

TEST_CASE("intersects(const ray&, const sphere&)", "[intersects]") {
  // Arrange
  const auto b = sphere{point3{5, 0, 0}, real{1}};

  SECTION("Sphere is ahead of the ray") {
    // Arrange
    const auto a = ray{point3{0, 0, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere is behind the ray") {
    // Arrange
    const auto a = ray{point3{0, 0, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Ray starts inside the sphere") {
    // Arrange
    const auto a = ray{point3{5, 0.5f, 0}, vector3{0, 1, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = ray{point3{0, 0, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const ray&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto b = make_unit_box();

  SECTION("Box is ahead of the ray") {
    // Arrange
    const auto a = ray{point3{-5, 0.5f, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Box is behind the ray") {
    // Arrange
    const auto a = ray{point3{-5, 0.5f, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Ray starts inside the box") {
    // Arrange
    const auto a = ray{point3{0, 0, 0}, vector3{0, 0, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = ray{point3{-5, 0.5f, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

//------------------------------------------------------------------------------
// line_segment
//------------------------------------------------------------------------------

TEST_CASE("intersects(const line_segment&, const sphere&)", "[intersects]") {
  // Arrange
  const auto b = sphere{point3{5, 0, 0}, real{1}};

  SECTION("Segment passes through the sphere") {
    // Arrange
    const auto a = line_segment{point3{0, 0, 0}, point3{10, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment ends short of the sphere") {
    // Arrange
    const auto a = line_segment{point3{0, 0, 0}, point3{3, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Segment ends inside the sphere") {
    // Arrange
    const auto a = line_segment{point3{0, 0, 0}, point3{4.5f, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line_segment{point3{0, 0, 0}, point3{3, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const line_segment&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto b = make_unit_box();

  SECTION("Segment passes through the box") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{5, 0.5f, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment ends short of the box") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{-2, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Segment is inside the box") {
    // Arrange
    const auto a = line_segment{point3{-0.5f, 0, 0}, point3{0.5f, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{-2, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

//------------------------------------------------------------------------------
// sphere
//------------------------------------------------------------------------------

TEST_CASE("intersects(const sphere&, const sphere&)", "[intersects]") {
  // Arrange
  const auto a = sphere{point3{0, 0, 0}, real{1}};

  SECTION("Spheres overlap") {
    // Arrange
    const auto b = sphere{point3{1.5f, 0, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Spheres touch") {
    // Arrange
    const auto b = sphere{point3{0, 3, 0}, real{2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Spheres are apart") {
    // Arrange
    const auto b = sphere{point3{0, 3, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const sphere&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto b = make_unit_box();

  SECTION("Sphere overlaps a face") {
    // Arrange
    const auto a = sphere{point3{1.5f, 0, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere touches a face") {
    // Arrange
    const auto a = sphere{point3{0, 0, 3}, real{2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere is beside a corner, within reach of each face") {
    // Arrange
    const auto a = sphere{point3{1.8f, 1.8f, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Sphere is inside the box") {
    // Arrange
    const auto a = sphere{point3{0, 0, 0}, real{0.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = sphere{point3{1.8f, 1.8f, 0}, real{1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

//------------------------------------------------------------------------------
// axis_aligned_box
//------------------------------------------------------------------------------

TEST_CASE("intersects(const axis_aligned_box&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto a = make_unit_box();

  SECTION("Boxes overlap") {
    // Arrange
    const auto b = axis_aligned_box{point3{0, 0, 0}, point3{2, 2, 2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Boxes are apart") {
    // Arrange
    const auto b = axis_aligned_box{point3{2, 0, 0}, point3{3, 1, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
}

//------------------------------------------------------------------------------
// frustum
//------------------------------------------------------------------------------

TEST_CASE("intersects(const frustum&, const sphere&)", "[intersects]") {
  // Arrange
  const auto a = frustum::from_matrix(matrix4_constants::identity);

  SECTION("Sphere is inside the frustum") {
    // Arrange
    const auto b = sphere{point3{0, 0, 0}, real{0.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere straddles a plane") {
    // Arrange
    const auto b = sphere{point3{1.5f, 0, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere is outside a plane") {
    // Arrange
    const auto b = sphere{point3{0, 0, -3}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = sphere{point3{0, 0, -3}, real{1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const frustum&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto a = frustum::from_matrix(matrix4_constants::identity);

  SECTION("Box is inside the frustum") {
    // Arrange
    const auto b = axis_aligned_box{
      point3{-0.5f, -0.5f, -0.5f},
      point3{0.5f, 0.5f, 0.5f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Box straddles a plane") {
    // Arrange
    const auto b = axis_aligned_box{point3{0.5f, 0, 0}, point3{2, 1, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Box is outside a plane") {
    // Arrange
    const auto b = axis_aligned_box{point3{0, 2, 0}, point3{1, 3, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = axis_aligned_box{point3{0, 2, 0}, point3{1, 3, 1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const plane&, const triangle&)", "[intersects]") {
  // Arrange
  const auto b = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Plane cuts through the triangle") {
    // Arrange
    const auto a = plane{vector3{1, 0, 0}, 1};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane contains the triangle") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, 0};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane is apart from the triangle") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, 1};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, 1};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const plane&, const capsule&)", "[intersects]") {
  // Arrange
  const auto b = capsule{
    line_segment{point3{0, 0, 2}, point3{0, 0, 4}},
    real{1}
  };

  SECTION("Plane crosses the segment of the capsule") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, 3};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane cuts through a cap of the capsule") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, real{1.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane is apart from the capsule") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, 0};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = plane{vector3{0, 0, 1}, 0};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const plane&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto b = make_rotated_cube();

  SECTION("Plane cuts off a corner of the box") {
    // Arrange
    const auto a = plane{vector3{1, 0, 0}, real{1.3f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Plane is beside a corner of the box") {
    // Arrange
    const auto a = plane{vector3{1, 0, 0}, real{1.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = plane{vector3{1, 0, 0}, real{1.5f}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const line&, const triangle&)", "[intersects]") {
  // Arrange
  const auto b = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Line passes through the triangle") {
    // Arrange
    const auto a = line{point3{1, 1, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line passes beside the triangle") {
    // Arrange
    const auto a = line{point3{3, 3, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Line lies on the plane of the triangle, crossing it") {
    // Arrange
    const auto a = line{point3{-1, 1, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line{point3{3, 3, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const line&, const capsule&)", "[intersects]") {
  // Arrange
  const auto b = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Line passes through the capsule") {
    // Arrange
    const auto a = line{point3{0, 2, real{0.5f}}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line lies inside the capsule along its segment") {
    // Arrange
    const auto a = line{point3{real{0.5f}, 0, 0}, vector3{0, 1, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line passes beside the capsule") {
    // Arrange
    const auto a = line{point3{0, 2, 2}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line{point3{0, 2, 2}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const line&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto b = make_rotated_cube();

  SECTION("Line passes through a corner of the box") {
    // Arrange
    const auto a = line{point3{real{1.3f}, 0, 0}, vector3{0, 1, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Line passes beside a corner of the box") {
    // Arrange
    const auto a = line{point3{real{1.5f}, 0, 0}, vector3{0, 1, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line{point3{real{1.5f}, 0, 0}, vector3{0, 1, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const ray&, const triangle&)", "[intersects]") {
  // Arrange
  const auto b = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Triangle is ahead of the ray") {
    // Arrange
    const auto a = ray{point3{1, 1, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Triangle is behind the ray") {
    // Arrange
    const auto a = ray{point3{1, 1, 5}, vector3{0, 0, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = ray{point3{1, 1, 5}, vector3{0, 0, -1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const ray&, const capsule&)", "[intersects]") {
  // Arrange
  const auto b = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Capsule is ahead of the ray") {
    // Arrange
    const auto a = ray{point3{-5, 2, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Capsule is behind the ray") {
    // Arrange
    const auto a = ray{point3{-5, 2, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Ray starts inside the capsule") {
    // Arrange
    const auto a = ray{point3{0, 2, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = ray{point3{-5, 2, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const ray&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto b = make_rotated_cube();

  SECTION("Box is ahead of the ray") {
    // Arrange
    const auto a = ray{point3{-5, 0, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Box is behind the ray") {
    // Arrange
    const auto a = ray{point3{-5, 0, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = ray{point3{-5, 0, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const line_segment&, const triangle&)", "[intersects]") {
  // Arrange
  const auto b = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Segment passes through the triangle") {
    // Arrange
    const auto a = line_segment{point3{1, 1, 1}, point3{1, 1, -1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment ends short of the triangle") {
    // Arrange
    const auto a = line_segment{point3{1, 1, 1}, point3{1, 1, real{0.5f}}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Segment crosses the plane outside of the triangle") {
    // Arrange
    const auto a = line_segment{point3{3, 3, 1}, point3{3, 3, -1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Segment lies on the plane, crossing the triangle") {
    // Arrange
    const auto a = line_segment{point3{-1, 1, 0}, point3{1, 1, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment lies on the plane, outside of the triangle") {
    // Arrange
    const auto a = line_segment{point3{-3, 1, 0}, point3{-2, 1, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line_segment{point3{1, 1, 1}, point3{1, 1, -1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const line_segment&, const capsule&)", "[intersects]") {
  // Arrange
  const auto b = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Segment passes through the capsule") {
    // Arrange
    const auto a = line_segment{
      point3{-3, 2, real{0.5f}},
      point3{3, 2, real{0.5f}}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment passes beside the capsule") {
    // Arrange
    const auto a = line_segment{point3{-3, 2, 2}, point3{3, 2, 2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Segment ends short of the capsule") {
    // Arrange
    const auto a = line_segment{point3{-3, 2, 0}, point3{real{-1.5f}, 2, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line_segment{point3{-3, 2, 2}, point3{3, 2, 2}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const line_segment&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto b = make_rotated_cube();

  SECTION("Segment passes through the box") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{5, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Segment ends short of the box") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{-2, 0, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{-2, 0, 0}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const triangle&, const triangle&)", "[intersects]") {
  // Arrange
  const auto a = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Triangles cross") {
    // Arrange
    const auto b = triangle{
      point3{1, 1, -1},
      point3{1, 1, 1},
      point3{1, -1, 0}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Triangles are apart") {
    // Arrange
    const auto b = triangle{
      point3{-1, 1, -1},
      point3{-1, 1, 1},
      point3{-1, -1, 0}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Triangles are coplanar and overlap") {
    // Arrange
    const auto b = triangle{point3{1, 1, 0}, point3{5, 1, 0}, point3{1, 5, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Triangles are coplanar and apart") {
    // Arrange
    const auto b = triangle{point3{3, 3, 0}, point3{6, 3, 0}, point3{3, 6, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const triangle&, const sphere&)", "[intersects]") {
  // Arrange
  const auto a = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Sphere overlaps the triangle") {
    // Arrange
    const auto b = sphere{point3{1, 1, 1}, real{1.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere is apart from the triangle") {
    // Arrange
    const auto b = sphere{point3{1, 1, 3}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = sphere{point3{1, 1, 1}, real{1.5f}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const triangle&, const capsule&)", "[intersects]") {
  // Arrange
  const auto a = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

  SECTION("Capsule reaches the triangle") {
    // Arrange
    const auto b = capsule{
      line_segment{point3{1, 1, 1}, point3{1, 1, 3}},
      real{1.2f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Capsule is apart from the triangle") {
    // Arrange
    const auto b = capsule{
      line_segment{point3{1, 1, 1}, point3{1, 1, 3}},
      real{0.5f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Segment of the capsule passes through the triangle") {
    // Arrange
    const auto b = capsule{
      line_segment{point3{1, 1, -1}, point3{1, 1, 1}},
      real{0.1f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = capsule{
      line_segment{point3{1, 1, 1}, point3{1, 1, 3}},
      real{0.5f}
    };

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const triangle&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto b = make_unit_box();

  SECTION("Triangle passes through the box") {
    // Arrange
    const auto a = triangle{point3{0, 0, 0}, point3{4, 0, 0}, point3{0, 4, 0}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Triangle is above the box") {
    // Arrange
    const auto a = triangle{point3{0, 0, 2}, point3{4, 0, 2}, point3{0, 4, 2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Triangle is beside an edge of the box") {
    // Arrange
    const auto a = triangle{
      point3{2, real{0.5f}, 0},
      point3{real{0.5f}, 2, 0},
      point3{2, 2, 0}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = triangle{point3{0, 0, 2}, point3{4, 0, 2}, point3{0, 4, 2}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const triangle&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto b = make_rotated_cube();

  SECTION("Triangle is inside a corner of the box") {
    // Arrange
    const auto a = triangle{
      point3{real{1.3f}, 0, real{-0.5f}},
      point3{real{1.3f}, 0, real{0.5f}},
      point3{real{1.3f}, real{0.05f}, 0}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Triangle is beside an edge of the box") {
    // Arrange
    const auto a = triangle{
      point3{real{0.9f}, real{0.9f}, real{-0.5f}},
      point3{real{0.9f}, real{0.9f}, real{0.5f}},
      point3{1, 1, 0}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = triangle{
      point3{real{0.9f}, real{0.9f}, real{-0.5f}},
      point3{real{0.9f}, real{0.9f}, real{0.5f}},
      point3{1, 1, 0}
    };

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const sphere&, const capsule&)", "[intersects]") {
  // Arrange
  const auto b = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Sphere overlaps the capsule") {
    // Arrange
    const auto a = sphere{point3{real{2.5f}, 2, 0}, real{1.6f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere is apart from the capsule") {
    // Arrange
    const auto a = sphere{point3{real{2.5f}, 2, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = sphere{point3{real{2.5f}, 2, 0}, real{1.6f}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const sphere&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto b = make_rotated_cube();

  SECTION("Sphere overlaps a corner of the box") {
    // Arrange
    const auto a = sphere{point3{2, 0, 0}, real{0.7f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Sphere is beside a corner of the box") {
    // Arrange
    const auto a = sphere{point3{2, 0, 0}, real{0.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = sphere{point3{2, 0, 0}, real{0.5f}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const capsule&, const capsule&)", "[intersects]") {
  // Arrange
  const auto a = capsule{
    line_segment{point3{0, 0, 0}, point3{0, 4, 0}},
    real{1}
  };

  SECTION("Capsules overlap") {
    // Arrange
    const auto b = capsule{
      line_segment{point3{-2, 2, real{1.5f}}, point3{2, 2, real{1.5f}}},
      real{1}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Capsules are apart") {
    // Arrange
    const auto b = capsule{
      line_segment{point3{-2, 2, real{1.5f}}, point3{2, 2, real{1.5f}}},
      real{0.4f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const capsule&, const axis_aligned_box&)", "[intersects]") {
  // Arrange
  const auto b = make_unit_box();

  SECTION("Capsule overlaps a face of the box") {
    // Arrange
    const auto a = capsule{
      line_segment{point3{-3, 0, real{1.5f}}, point3{3, 0, real{1.5f}}},
      real{0.6f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Capsule is apart from a face of the box") {
    // Arrange
    const auto a = capsule{
      line_segment{point3{-3, 0, real{1.5f}}, point3{3, 0, real{1.5f}}},
      real{0.4f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Capsule is beside an edge, within reach of each face") {
    // Arrange
    const auto a = capsule{
      line_segment{point3{2, 2, -1}, point3{2, 2, 1}},
      real{1}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = capsule{
      line_segment{point3{-3, 0, real{1.5f}}, point3{3, 0, real{1.5f}}},
      real{0.6f}
    };

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const capsule&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto b = make_rotated_cube();

  SECTION("Capsule overlaps an edge of the box") {
    // Arrange
    const auto a = capsule{
      line_segment{point3{2, 0, -1}, point3{2, 0, 1}},
      real{0.7f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Capsule is beside an edge of the box") {
    // Arrange
    const auto a = capsule{
      line_segment{point3{2, 0, -1}, point3{2, 0, 1}},
      real{0.5f}
    };

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = capsule{
      line_segment{point3{2, 0, -1}, point3{2, 0, 1}},
      real{0.5f}
    };

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const axis_aligned_box&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto a = make_unit_box();

  SECTION("Boxes overlap") {
    // Arrange
    const auto b = make_rotated_cube(point3{real{2.2f}, 0, 0});

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Boxes are apart along an axis of the aligned box") {
    // Arrange
    const auto b = make_rotated_cube(point3{real{2.5f}, 0, 0});

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Boxes are apart along an axis of the oriented box only") {
    // Arrange
    const auto b = make_rotated_cube(point3{real{1.9f}, real{1.9f}, 0});

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = make_rotated_cube(point3{real{2.2f}, 0, 0});

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE(result);
  }
}

TEST_CASE("intersects(const oriented_box&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto a = make_rotated_cube();

  SECTION("Boxes overlap") {
    // Arrange
    const auto b = make_rotated_cube(point3{real{2.5f}, 0, 0});

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Boxes are apart") {
    // Arrange
    const auto b = make_rotated_cube(point3{3, 0, 0});

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const frustum&, const oriented_box&)", "[intersects]") {
  // Arrange
  const auto a = frustum::from_matrix(matrix4_constants::identity);

  SECTION("Box straddles a plane") {
    // Arrange
    const auto b = make_rotated_cube(point3{2, 0, 0});

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Box is outside a plane") {
    // Arrange
    const auto b = make_rotated_cube(point3{3, 0, 0});

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = make_rotated_cube(point3{3, 0, 0});

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const circle&, const circle&)", "[intersects]") {
  // Arrange
  const auto a = circle{point2{0, 0}, real{1}};

  SECTION("Circles overlap") {
    // Arrange
    const auto b = circle{point2{real{1.5f}, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Circles touch") {
    // Arrange
    const auto b = circle{point2{0, 3}, real{2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Circles are apart") {
    // Arrange
    const auto b = circle{point2{0, 3}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const circle&, const rectangle&)", "[intersects]") {
  // Arrange
  const auto b = rectangle{point2{-1, -1}, point2{1, 1}};

  SECTION("Circle overlaps an edge") {
    // Arrange
    const auto a = circle{point2{real{1.5f}, 0}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Circle is beside a corner, within reach of each edge") {
    // Arrange
    const auto a = circle{point2{real{1.8f}, real{1.8f}}, real{1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
  SECTION("Circle is inside the rectangle") {
    // Arrange
    const auto a = circle{point2{0, 0}, real{0.5f}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = circle{point2{real{1.8f}, real{1.8f}}, real{1}};

    // Act
    const auto result = intersects(b, a);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersects(const rectangle&, const rectangle&)", "[intersects]") {
  // Arrange
  const auto a = rectangle{point2{-1, -1}, point2{1, 1}};

  SECTION("Rectangles overlap") {
    // Arrange
    const auto b = rectangle{point2{0, 0}, point2{2, 2}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE(result);
  }
  SECTION("Rectangles are apart") {
    // Arrange
    const auto b = rectangle{point2{2, 0}, point2{3, 1}};

    // Act
    const auto result = intersects(a, b);

    // Assert
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("intersection_computable<L, R>", "[intersection_of]") {
  SECTION("Traits provide the intersection in the given order") {
    // Act
    constexpr auto property_holds = intersection_computable<plane, plane>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Traits provide the intersection in the reverse order") {
    // Act
    constexpr auto property_holds = intersection_computable<sphere, ray>;

    // Assert
    STATIC_REQUIRE(property_holds);
  }
  SECTION("Traits only test for the intersection") {
    // Act
    constexpr auto property_holds = intersection_computable<sphere, sphere>;

    // Assert
    STATIC_REQUIRE_FALSE(property_holds);
  }
}

TEST_CASE("intersection_of(const plane&, const plane&)", "[intersection_of]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};
  const auto tolerance = real{1e-4f};

  SECTION("Planes cross") {
    // Arrange
    const auto b = plane{vector3{1, 0, 0}, 2};

    const auto expected = intersection_of_t<plane, plane>{
      line{point3{2, 0, 1}, vector3{0, 1, 0}}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Planes are the same") {
    // Arrange
    const auto b = plane{0, 0, -2, 2};

    const auto expected = intersection_of_t<plane, plane>{
      a
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Planes are parallel and distinct") {
    // Arrange
    const auto b = plane{vector3{0, 0, 1}, 2};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
}

TEST_CASE("intersection_of(const plane&, const line&)", "[intersection_of]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};
  const auto tolerance = real{1e-4f};

  SECTION("Line crosses the plane") {
    // Arrange
    const auto b = line{point3{3, 2, 5}, vector3{0, 0, -1}};

    const auto expected = intersection_of_t<plane, line>{
      point3{3, 2, 1}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Line lies on the plane") {
    // Arrange
    const auto b = line{point3{3, 2, 1}, vector3{1, 0, 0}};

    const auto expected = intersection_of_t<plane, line>{
      b
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Line is parallel to the plane") {
    // Arrange
    const auto b = line{point3{3, 2, 5}, vector3{1, 0, 0}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto b = line{point3{3, 2, 5}, vector3{0, 0, -1}};

    const auto expected = intersection_of_t<plane, line>{
      point3{3, 2, 1}
    };

    // Act
    const auto result = intersection_of(b, a);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
}

TEST_CASE("intersection_of(const plane&, const ray&)", "[intersection_of]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};
  const auto tolerance = real{1e-4f};

  SECTION("Ray faces the plane") {
    // Arrange
    const auto b = ray{point3{3, 2, 5}, vector3{0, 0, -1}};

    const auto expected = intersection_of_t<plane, ray>{
      point3{3, 2, 1}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Ray faces away from the plane") {
    // Arrange
    const auto b = ray{point3{3, 2, 5}, vector3{0, 0, 1}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
  SECTION("Ray lies on the plane") {
    // Arrange
    const auto b = ray{point3{3, 2, 1}, vector3{1, 0, 0}};

    const auto expected = intersection_of_t<plane, ray>{
      b
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
}

TEST_CASE("intersection_of(const plane&, const line_segment&)", "[intersection_of]") {
  // Arrange
  const auto a = plane{vector3{0, 0, 1}, 1};
  const auto tolerance = real{1e-4f};

  SECTION("Segment crosses the plane") {
    // Arrange
    const auto b = line_segment{point3{0, 0, 0}, point3{4, 2, 2}};

    const auto expected = intersection_of_t<plane, line_segment>{
      point3{2, 1, 1}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Segment lies on the plane") {
    // Arrange
    const auto b = line_segment{point3{0, 0, 1}, point3{4, 2, 1}};

    const auto expected = intersection_of_t<plane, line_segment>{
      b
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Segment is on one side of the plane") {
    // Arrange
    const auto b = line_segment{point3{0, 0, 2}, point3{4, 2, 3}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
}

TEST_CASE("intersection_of(const line&, const sphere&)", "[intersection_of]") {
  // Arrange
  const auto b = sphere{point3{5, 0, 0}, real{1}};
  const auto tolerance = real{1e-4f};

  SECTION("Line passes through the sphere") {
    // Arrange
    const auto a = line{point3{0, 0, 0}, vector3{1, 0, 0}};

    const auto expected = intersection_of_t<line, sphere>{
      line_segment{point3{4, 0, 0}, point3{6, 0, 0}}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Line touches the sphere") {
    // Arrange
    const auto a = line{point3{0, 1, 0}, vector3{1, 0, 0}};

    const auto expected = intersection_of_t<line, sphere>{
      point3{5, 1, 0}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Line misses the sphere") {
    // Arrange
    const auto a = line{point3{0, 2, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line{point3{0, 0, 0}, vector3{1, 0, 0}};

    const auto expected = intersection_of_t<line, sphere>{
      line_segment{point3{4, 0, 0}, point3{6, 0, 0}}
    };

    // Act
    const auto result = intersection_of(b, a);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
}

TEST_CASE("intersection_of(const ray&, const sphere&)", "[intersection_of]") {
  // Arrange
  const auto b = sphere{point3{5, 0, 0}, real{1}};
  const auto tolerance = real{1e-4f};

  SECTION("Ray starts inside the sphere") {
    // Arrange
    const auto a = ray{point3{5, 0, 0}, vector3{1, 0, 0}};

    const auto expected = intersection_of_t<ray, sphere>{
      line_segment{point3{5, 0, 0}, point3{6, 0, 0}}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Sphere is behind the ray") {
    // Arrange
    const auto a = ray{point3{0, 0, 0}, vector3{-1, 0, 0}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
}

TEST_CASE("intersection_of(const line_segment&, const sphere&)", "[intersection_of]") {
  // Arrange
  const auto b = sphere{point3{5, 0, 0}, real{1}};
  const auto tolerance = real{1e-4f};

  SECTION("Segment ends inside the sphere") {
    // Arrange
    const auto a = line_segment{point3{0, 0, 0}, point3{5, 0, 0}};

    const auto expected = intersection_of_t<line_segment, sphere>{
      line_segment{point3{4, 0, 0}, point3{5, 0, 0}}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Segment ends short of the sphere") {
    // Arrange
    const auto a = line_segment{point3{0, 0, 0}, point3{3, 0, 0}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
}

TEST_CASE("intersection_of(const line&, const axis_aligned_box&)", "[intersection_of]") {
  // Arrange
  const auto b = make_unit_box();
  const auto tolerance = real{1e-4f};

  SECTION("Line passes through the box") {
    // Arrange
    const auto a = line{point3{0, 0, 0}, vector3{1, 0, 0}};

    const auto expected = intersection_of_t<line, axis_aligned_box>{
      line_segment{point3{-1, 0, 0}, point3{1, 0, 0}}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Line touches an edge of the box") {
    // Arrange
    const auto a = line{point3{2, 0, 0}, vector3{-1, 1, 0}.normalized()};

    const auto expected = intersection_of_t<line, axis_aligned_box>{
      point3{1, 1, 0}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Line misses the box") {
    // Arrange
    const auto a = line{point3{0, 2, 0}, vector3{1, 0, 0}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
}

TEST_CASE("intersection_of(const ray&, const axis_aligned_box&)", "[intersection_of]") {
  // Arrange
  const auto b = make_unit_box();
  const auto tolerance = real{1e-4f};

  SECTION("Ray starts inside the box") {
    // Arrange
    const auto a = ray{point3{0, 0, 0}, vector3{0, 0, 1}};

    const auto expected = intersection_of_t<ray, axis_aligned_box>{
      line_segment{point3{0, 0, 0}, point3{0, 0, 1}}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Box is behind the ray") {
    // Arrange
    const auto a = ray{point3{0, 0, 5}, vector3{0, 0, 1}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
}

TEST_CASE("intersection_of(const line_segment&, const axis_aligned_box&)", "[intersection_of]") {
  // Arrange
  const auto b = make_unit_box();
  const auto tolerance = real{1e-4f};

  SECTION("Segment passes through the box") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{5, 0, 0}};

    const auto expected = intersection_of_t<line_segment, axis_aligned_box>{
      line_segment{point3{-1, 0, 0}, point3{1, 0, 0}}
    };

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
  SECTION("Segment ends short of the box") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{-2, 0, 0}};

    // Act
    const auto result = intersection_of(a, b);

    // Assert
    REQUIRE_FALSE(result.has_intersection());
  }
  SECTION("Arguments are reversed") {
    // Arrange
    const auto a = line_segment{point3{-5, 0, 0}, point3{0, 0, 0}};

    const auto expected = intersection_of_t<line_segment, axis_aligned_box>{
      line_segment{point3{-1, 0, 0}, point3{0, 0, 0}}
    };

    // Act
    const auto result = intersection_of(b, a);

    // Assert
    REQUIRE(almost_equal(result, expected, tolerance));
  }
}

} // namespace alloy::core::test
