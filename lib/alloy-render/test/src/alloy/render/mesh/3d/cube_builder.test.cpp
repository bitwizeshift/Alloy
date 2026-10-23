// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/3d/cube_builder.hpp>

#include "../mesh_test_utilities.hpp"
#include <alloy/render/mesh/mesh.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::render::test {

TEST_CASE("cube_builder::build()", "[build]") {
  // Arrange
  const auto sut = cube_builder{};

  // Act
  const auto result = sut.build();

  // Assert
  SECTION("Contains 4 vertices per face") {
    REQUIRE(result.vertex_count() == 24u);
  }
  SECTION("Contains 2 triangles per face") {
    REQUIRE(result.index_count() == 36u);
  }
  SECTION("Every triangle winds counter-clockwise around its normal") {
    REQUIRE(count_inward_triangles(result) == 0u);
  }
}

} // namespace alloy::render::test
