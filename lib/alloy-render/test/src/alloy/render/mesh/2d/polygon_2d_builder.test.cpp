// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/2d/polygon_2d_builder.hpp>

#include <alloy/render/mesh/mesh.hpp>

#include <alloy/core/containers/vector.hpp>
#include <alloy/core/geometry/axis_aligned_box.hpp>
#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/geometry/point/point3.hpp>

#include <catch2/catch_test_macros.hpp>

namespace alloy::render::test {

TEST_CASE("polygon_2d_builder::build()", "[build]") {
  SECTION("Positions are 2-component") {
    // Arrange
    auto sut = polygon_2d_builder{};
    sut.add_position(core::point2{1, 2});

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.position_components() == 2u);
  }
  SECTION("Bounding volume encloses positions in every quadrant") {
    // Arrange
    auto sut = polygon_2d_builder{};
    sut.add_position(core::point2{1, -2});
    sut.add_position(core::point2{-4, 5});
    sut.add_position(core::point2{7, 8});
    sut.add_position(core::point2{-1, -1});
    const auto expected = core::axis_aligned_box{
      core::point3{-4, -2, 0},
      core::point3{7, 8, 0}
    };

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.bounding_volume() == expected);
  }
  SECTION("Bounding volume encloses positions from set_positions") {
    // Arrange
    auto sut = polygon_2d_builder{};
    sut.set_positions(core::vector<core::point2>{
      core::point2{2, 3},
      core::point2{5, 6},
    });
    const auto expected = core::axis_aligned_box{
      core::point3{2, 3, 0},
      core::point3{5, 6, 0}
    };

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.bounding_volume() == expected);
  }
}

} // namespace alloy::render::test
