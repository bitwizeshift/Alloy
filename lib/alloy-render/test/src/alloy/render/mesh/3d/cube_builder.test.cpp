// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/3d/cube_builder.hpp>

#include <alloy/render/mesh/mesh.hpp>

#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/vector/vector3.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <cstdint> // std::int16_t
#include <cstring> // std::memcpy

namespace alloy::render::test {
namespace {

auto read_position(const mesh& m, std::size_t vertex) -> core::point3
{
  auto packed = std::array<float,3>{};
  const auto offset = (vertex * m.stride().count())
                    + m.offset_to_positions().count();
  std::memcpy(packed.data(), m.data().data() + offset, sizeof(packed));
  return core::point3{packed[0], packed[1], packed[2]};
}

auto read_normal(const mesh& m, std::size_t vertex) -> core::vector3
{
  auto packed = std::array<std::int16_t,3>{};
  const auto offset = (vertex * m.stride().count())
                    + m.offset_to_normals().count();
  std::memcpy(packed.data(), m.data().data() + offset, sizeof(packed));
  return core::vector3{
    static_cast<float>(packed[0]),
    static_cast<float>(packed[1]),
    static_cast<float>(packed[2]),
  };
}

/// \brief Counts the triangles whose counter-clockwise face normal points
///        away from the stored normal of its first vertex
auto count_inward_triangles(const mesh& m) -> std::size_t
{
  const auto indices = m.indices();
  auto count = std::size_t{0};
  for (auto i = std::size_t{0}; i + 2u < indices.size(); i += 3u) {
    const auto p0 = read_position(m, indices[i]);
    const auto p1 = read_position(m, indices[i + 1u]);
    const auto p2 = read_position(m, indices[i + 2u]);
    const auto face = (p1 - p0).cross(p2 - p0);
    if (face.dot(read_normal(m, indices[i])) <= 0.0f) {
      ++count;
    }
  }
  return count;
}

} // namespace <anonymous>

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
    REQUIRE(result.indices().size() == 36u);
  }
  SECTION("Every triangle winds counter-clockwise around its normal") {
    REQUIRE(count_inward_triangles(result) == 0u);
  }
}

} // namespace alloy::render::test
