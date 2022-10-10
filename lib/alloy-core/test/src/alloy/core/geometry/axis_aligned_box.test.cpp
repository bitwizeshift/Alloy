/*
 Any copyright for test files is dedicated to the Public Domain.
 https://creativecommons.org/publicdomain/zero/1.0/
*/
#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector3_constants.hpp"

#include <catch2/catch.hpp>

namespace alloy::core::test {

TEST_CASE("axis_aligned_box::from_point_and_size(const point3&, const vector3&)", "[factory][static]") {
  // Arrange
  const auto point = point3{2, -10, 16};
  const auto size  = vector3{-4, 2, 5};

  // Act
  const auto sut = axis_aligned_box::from_point_and_size(point, size);

  // Assert
  SECTION("Bottom-left point is less than top-right") {
    SECTION("Bottom-left X is less than top-right X") {
      REQUIRE(sut.bottom_left_point().x() <= sut.top_right_point().x());
    }
    SECTION("Bottom-left Y is less than top-right Y") {
      REQUIRE(sut.bottom_left_point().y() <= sut.top_right_point().y());
    }
    SECTION("Bottom-left Z is less than top-right Z") {
      REQUIRE(sut.bottom_left_point().z() <= sut.top_right_point().z());
    }
  }
}

TEST_CASE("axis_aligned_box::from_points(const point3&, const point3&)", "[factory][static]") {
  // Arrange
  const auto point1 = point3{2, -10, 16};
  const auto point2 = point3{-4, 2, 5};

  // Act
  const auto sut = axis_aligned_box::from_points(point1, point2);

  // Assert
  SECTION("Bottom-left point is less than top-right") {
    SECTION("Bottom-left X is less than top-right X") {
      REQUIRE(sut.bottom_left_point().x() <= sut.top_right_point().x());
    }
    SECTION("Bottom-left Y is less than top-right Y") {
      REQUIRE(sut.bottom_left_point().y() <= sut.top_right_point().y());
    }
    SECTION("Bottom-left Z is less than top-right Z") {
      REQUIRE(sut.bottom_left_point().z() <= sut.top_right_point().z());
    }
  }
}

TEST_CASE("axis_aligned_box::axis_aligned_box()", "[ctor]") {
  // Arrange / Act
  const auto sut = axis_aligned_box{};

  // Asserts
  SECTION("Bottom left point is zero") {
    REQUIRE(sut.bottom_left_point() == point3{0, 0, 0});
  }
  SECTION("Top right point is zero") {
    REQUIRE(sut.top_right_point() == point3{0, 0, 0});
  }
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

TEST_CASE("axis_aligned_box::contains(const point3&)", "[observers]") {
  // Arrange
  const auto sut = axis_aligned_box::from_point_and_size({-1,-1,-1}, {2,2,2});

  SECTION("Point is outside of box") {
    // Arrange
    const auto point = GENERATE(
      point3{2,0,0},
      point3{-2,0,0},
      point3{0,2,0},
      point3{0,-2,0},
      point3{0,0,2},
      point3{0,0,-2}
    );

    // Act
    const auto result = sut.contains(point);

    // Assert
    SECTION("Returns false") {
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Point is inside the box") {
    // Arrange
    const auto point = point3{0,0,0};

    // Act
    const auto result = sut.contains(point);

    // Assert
    SECTION("Returns true") {
      REQUIRE(result);
    }
  }
}

TEST_CASE("axis_aligned_box::contains(const point3&, real)", "[observers]") {
  // Arrange
  const auto sut = axis_aligned_box::from_point_and_size({-1,-1,-1}, {2,2,2});
  const auto tolerance = 1e-6f;

  SECTION("Point is outside of box") {
    // Arrange
    const auto point = GENERATE(
      point3{2,0,0},
      point3{-2,0,0},
      point3{0,2,0},
      point3{0,-2,0},
      point3{0,0,2},
      point3{0,0,-2}
    );

    // Act
    const auto result = sut.contains(point, tolerance);

    // Assert
    SECTION("Returns false") {
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Point is close to box wall") {
    // Arrange
    const auto x_offset = GENERATE(1e-7f, 1e-9f, 0.f, -1e-9f, -1e-7f);
    const auto y_offset = GENERATE(1e-7f, 1e-9f, 0.f, -1e-9f, -1e-7f);
    const auto z_offset = GENERATE(1e-7f, 1e-9f, 0.f, -1e-9f, -1e-7f);
    const auto offset = vector3{x_offset, y_offset, z_offset};

    const auto wall = GENERATE(
      vector3_constants::neg_unit_x,
      vector3_constants::neg_unit_y,
      vector3_constants::neg_unit_z,
      vector3_constants::unit_x,
      vector3_constants::unit_y,
      vector3_constants::unit_z
    );
    const auto point = point3{} + wall + offset;

    // Act
    const auto result = sut.contains(point, tolerance);

    // Assert
    SECTION("Returns true") {
      REQUIRE(result);
    }
  }
  SECTION("Point is inside the box") {
    // Arrange
    const auto point = point3{0,0,0};

    // Act
    const auto result = sut.contains(point);

    // Assert
    SECTION("Returns true") {
      REQUIRE(result);
    }
  }
}

TEST_CASE("axis_aligned_box::distance_to(const point&)", "[observers]") {

}

TEST_CASE("axis_aligned_box::nearest_point(const point&)", "[observers]") {
  // Arrange
  const auto top_right = point3{1,1,1};
  const auto bottom_left = point3{-1,-1,-1};

  const auto sut = axis_aligned_box::from_points(bottom_left, top_right);

  SECTION("Point is on a box wall") {
    // Arrange
    const auto point = GENERATE(
      point3{1, 1, 0},
      point3{1, 0, 0},
      point3{1, -1, 0},
      point3{-1, 1, 0},
      point3{-1, 0, 0},
      point3{-1, -1, 0},
      point3{0, 1, 1},
      point3{0, 1, 0},
      point3{0, 1, -1},
      point3{0, -1, 1},
      point3{0, -1, 0},
      point3{0, -1, -1},
      point3{1, 0, 1},
      point3{0, 0, 1},
      point3{-1, 0, 1},
      point3{1, 0, -1},
      point3{0, 0, -1},
      point3{-1, 0, -1}
    );

    // Act
    const auto result = sut.nearest_point(point);

    // Assert
    SECTION("Returns that point") {
      REQUIRE(result == point);
    }
  }
  SECTION("Point is adjacent to a box wall") {
    // Arrange
    const auto closest_point = GENERATE(
      point3{1, 0, 0},
      point3{-1, 0, 0},
      point3{0, 1, 0},
      point3{0, -1, 0},
      point3{0, 0, 1},
      point3{0, 0, -1}
    );
    const auto point = point3{
      closest_point.x() * 2,
      closest_point.y() * 2,
      closest_point.z() * 2,
    };

    // Act
    const auto result = sut.nearest_point(point);

    // Assert
    SECTION("Returns closest point") {
      REQUIRE(almost_equal(result, closest_point));
    }
  }
  SECTION("Point is away from a box wall") {
    // Arrange
    const auto closest_point = GENERATE(
      point3{1, 1, 0},
      point3{1, -1, 0},
      point3{-1, 1, 0},
      point3{-1, -1, 0},
      point3{0, 1, 1},
      point3{0, 1, -1},
      point3{0, -1, 1},
      point3{0, -1, -1},
      point3{1, 0, 1},
      point3{-1, 0, 1},
      point3{1, 0, -1},
      point3{-1, 0, -1}
    );
    const auto x_offset = GENERATE(0.f, 1.f);
    const auto y_offset = GENERATE(0.f, 1.f);
    const auto z_offset = GENERATE(0.f, 1.f);

    const auto point = point3{
      closest_point.x() * 2 + (closest_point.x() * x_offset),
      closest_point.y() * 2 + (closest_point.y() * y_offset),
      closest_point.z() * 2 + (closest_point.z() * z_offset),
    };

    // Act
    const auto result = sut.nearest_point(point);

    // Assert
    SECTION("Returns point clamped to corner") {
      REQUIRE(almost_equal(result, closest_point));
    }
  }
}

TEST_CASE("axis_aligned_box::intersects(const axis_aligned_box&)", "[observers]") {
  // Arrange
  const auto top_right = point3{1,1,1};
  const auto bottom_left = point3{-1,-1,-1};

  const auto sut = axis_aligned_box::from_points(bottom_left, top_right);

  SECTION("Box intersects") {
    SECTION("Box intersects one wall") {
      // Arrange
      const auto shift = GENERATE(
        vector3_constants::neg_unit_x,
        vector3_constants::unit_x,
        vector3_constants::neg_unit_y,
        vector3_constants::unit_x,
        vector3_constants::neg_unit_z,
        vector3_constants::unit_x
      );
      const auto other = axis_aligned_box::from_points(
        bottom_left + shift,
        top_right + shift
      );

      // Act
      const auto result = sut.intersects(other);

      // Assert
      REQUIRE(result);
    }
    SECTION("Box is enclosed inside") {
      // Arrange
      const auto enclosed = axis_aligned_box::from_points(
        point3{0.5, 0.5, 0.5},
        point3{-0.5, -0.5, -0.5}
      );

      // Act
      const auto result = sut.intersects(enclosed);

      // Assert
      REQUIRE(result);
    }
    SECTION("Box encloses") {
      // Arrange
      const auto enclosed = axis_aligned_box::from_points(
        point3{2, 2, 2},
        point3{-2, -2, -2}
      );

      // Act
      const auto result = sut.intersects(enclosed);

      // Assert
      REQUIRE(result);
    }
    SECTION("Box is same box") {
      // Act
      const auto result = sut.intersects(sut);

      // Assert
      REQUIRE(result);
    }
  }

  SECTION("Box does not intersect") {
    SECTION("Box is disjoint") {
      // Arrange
      const auto direction = top_right - bottom_left;
      const auto box = axis_aligned_box::from_point_and_size(
        top_right - direction, -direction
      );

      // Act
      const auto result = sut.intersects(box);

      // Assert
      REQUIRE(result);
    }
  }
}

TEST_CASE("axis_aligned_box::encloses(const axis_aligned_box&)", "[observers]") {
  // Arrange
  const auto top_right = point3{1,1,1};
  const auto bottom_left = point3{-1,-1,-1};

  const auto sut = axis_aligned_box::from_points(bottom_left, top_right);

  SECTION("Box is not enclosed") {
    SECTION("Box is disjoint") {
      // Arrange
      const auto direction = top_right - bottom_left;
      const auto box = axis_aligned_box::from_point_and_size(
        top_right - direction, -direction
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE_FALSE(result);
    }
    SECTION("Box intersects but does not enclose") {
      // Arrange
      const auto direction = top_right - bottom_left;
      const auto box = axis_aligned_box::from_point_and_size(
        top_right - direction, direction * 2
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE_FALSE(result);
    }
    SECTION("Box shares wall, but does not intersect") {
      const auto direction = top_right - bottom_left;
      const auto box = axis_aligned_box::from_point_and_size(
        bottom_left, -direction
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE_FALSE(result);
    }
  }
  SECTION("Box is enclosed") {
    SECTION("Box is completely enclosed") {
      // Arrange
      const auto box = axis_aligned_box::from_points(
        {top_right.x() / 2, top_right.y() / 2, top_right.z() / 2},
        {bottom_left.x() / 2, bottom_left.y() / 2, bottom_left.z() / 2}
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
    SECTION("Shares all walls with enclosing box") {
      // Arrange
      const auto box = sut;

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
    SECTION("Shares top wall") {
      // Arrange
      const auto y = sut.top_normal().y();
      const auto box = axis_aligned_box::from_point_and_size(
        {0.5, y, 0.5},
        {-1, -1, -1}
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
    SECTION("Shares bottom wall") {
      // Arrange
      const auto y = sut.bottom_normal().y();
      const auto box = axis_aligned_box::from_point_and_size(
        {-0.5, y, -0.5},
        {1, 1, 1}
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
    SECTION("Shares front wall") {
      // Arrange
      const auto z = sut.front_normal().z();
      const auto box = axis_aligned_box::from_point_and_size(
        {0.5, 0.5, z},
        {-1, -1, -1}
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
    SECTION("Shares back wall") {
      // Arrange
      const auto z = sut.back_normal().z();
      const auto box = axis_aligned_box::from_point_and_size(
        {-0.5, -0.5, z},
        {1, 1, 1}
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
    SECTION("Shares right wall") {
      // Arrange
      const auto x = sut.right_normal().x();
      const auto box = axis_aligned_box::from_point_and_size(
        {x, 0.5, 0.5},
        {-1, -1, -1}
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
    SECTION("Shares left wall") {
      // Arrange
      const auto x = sut.left_normal().x();
      const auto box = axis_aligned_box::from_point_and_size(
        {x, -0.5, -0.5},
        {1, 1, 1}
      );

      // Act
      const auto result = sut.encloses(box);

      // Assert
      REQUIRE(result);
    }
  }
}

} // namespace alloy::core::test
