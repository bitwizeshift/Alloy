// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/3d/polygon_3d_builder.hpp>

#include "../mesh_test_utilities.hpp"
#include <alloy/render/mesh/mesh.hpp>
#include <alloy/render/mesh/primitive_topology.hpp>
#include <alloy/render/mesh/winding_order.hpp>

#include <alloy/core/containers/vector.hpp>
#include <alloy/core/geometry/axis_aligned_box.hpp>
#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/vector/vector4.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstdint> // std::uint32_t
#include <vector>  // std::vector

namespace alloy::render::test {

TEST_CASE("polygon_3d_builder::build()", "[build]") {
  SECTION("Positions are 3-component") {
    // Arrange
    auto sut = polygon_3d_builder{};
    sut.add_position(core::point3{1, 2, 3});

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.position_components() == 3u);
  }
  SECTION("Bounding volume encloses positions in every octant") {
    // Arrange
    auto sut = polygon_3d_builder{};
    sut.add_position(core::point3{1, -2, 3});
    sut.add_position(core::point3{-4, 5, -6});
    sut.add_position(core::point3{7, 8, 9});
    sut.add_position(core::point3{-1, -1, -1});
    const auto expected = core::axis_aligned_box{
      core::point3{-4, -2, -6},
      core::point3{7, 8, 9}
    };

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.bounding_volume() == expected);
  }
  SECTION("Bounding volume encloses positions from set_positions") {
    // Arrange
    auto sut = polygon_3d_builder{};
    sut.set_positions(core::vector<core::point3>{
      core::point3{2, 3, 4},
      core::point3{5, 6, 7},
    });
    const auto expected = core::axis_aligned_box{
      core::point3{2, 3, 4},
      core::point3{5, 6, 7}
    };

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.bounding_volume() == expected);
  }
  SECTION("Secondary UVs are 2-component") {
    // Arrange
    auto sut = polygon_3d_builder{};
    sut.add_position(core::point3{1, 2, 3});
    sut.add_uv2(core::point2{0, 1});

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.uv2_components() == 2u);
  }
  SECTION("Submeshes follow the indices added between them") {
    // Arrange
    using range = std::array<std::uint32_t,2>;
    const auto expected = std::vector<range>{range{0u, 3u}, range{3u, 3u}};
    auto sut = polygon_3d_builder{};
    for (auto i = 0; i < 4; ++i) {
      sut.add_position(core::point3{});
    }
    sut.begin_submesh();
    sut.add_face(0u, 1u, 2u);
    sut.begin_submesh();
    sut.add_face(1u, 2u, 3u);

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(submesh_ranges(result) == expected);
  }
  SECTION("Winding order is that of the builder") {
    // Arrange
    auto sut = polygon_3d_builder{
      primitive_topology::triangles,
      winding_order::clockwise
    };
    sut.add_position(core::point3{});

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(result.winding() == winding_order::clockwise);
  }
  SECTION("Tangent handedness is preserved") {
    // Arrange
    auto sut = polygon_3d_builder{};
    sut.add_position(core::point3{1, 2, 3});
    sut.add_tangent(core::vector4{1, 0, 0, -1});

    // Act
    const auto result = sut.build();

    // Assert
    REQUIRE(read_tangent_handedness(result, 0u) == -32767);
  }
}

} // namespace alloy::render::test
