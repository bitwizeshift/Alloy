// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/3d/sphere_builder.hpp>

#include "../mesh_test_utilities.hpp"
#include <alloy/render/mesh/mesh.hpp>

#include <alloy/core/geometry/axis_aligned_box.hpp>
#include <alloy/core/geometry/point/point3.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::render::test {

TEST_CASE("sphere_builder::build()", "[build]") {
  // Arrange
  const auto sut = sphere_builder{8u, 4u};

  // Act
  const auto result = sut.build();

  // Assert
  SECTION("Contains a vertex per segment boundary") {
    REQUIRE(result.vertex_count() == 45u);
  }
  SECTION("Omits the collapsed triangle of each quad touching a pole") {
    REQUIRE(result.index_count() == 144u);
  }
  SECTION("Every index refers to an existing vertex") {
    REQUIRE(max_index(result) < result.vertex_count());
  }
  SECTION("Bounding volume encloses the unit sphere") {
    const auto expected = core::axis_aligned_box{
      core::point3{-1, -1, -1},
      core::point3{1, 1, 1}
    };

    REQUIRE(result.bounding_volume() == expected);
  }
  SECTION("Every triangle winds counter-clockwise around its normal") {
    REQUIRE(count_inward_triangles(result) == 0u);
  }
}

} // namespace alloy::render::test
