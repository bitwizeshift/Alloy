// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/unsafe_mesh_builder.hpp>

#include <alloy/render/mesh/mesh.hpp>

#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/model/color.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstdint> // std::int16_t
#include <cstring> // std::memcpy
#include <span>    // std::span
#include <utility> // std::move

namespace alloy::render::test {
namespace {

/// \brief Reads the first 3 snorm16 normal components out of \p m
auto read_normal(const mesh& m) -> std::array<std::int16_t,3>
{
  auto result = std::array<std::int16_t,3>{};
  const auto data = m.data().subspan(m.offset_to_normals().count());
  std::memcpy(result.data(), data.data(), sizeof(result));
  return result;
}

/// \brief Builds a mesh containing a single normal \p v
auto build_normal_mesh(const core::vector3& v) -> mesh
{
  auto builder = unsafe_mesh_builder{vertex_components{.normal = 3u}};
  builder.add_normal(v);
  return std::move(builder).release();
}

} // namespace <anonymous>

TEST_CASE("unsafe_mesh_builder::add_normal(const vector3&)", "[modifier]") {
  SECTION("Unit components map to the snorm16 extremes") {
    // Arrange
    const auto v = core::vector3{1.0f, -1.0f, 0.0f};
    const auto expected = std::array<std::int16_t,3>{32767, -32767, 0};

    // Act
    const auto sut = build_normal_mesh(v);

    // Assert
    REQUIRE(read_normal(sut) == expected);
  }
  SECTION("Components are rounded to the nearest value") {
    // Arrange
    const auto v = core::vector3{0.5f, -0.5f, 0.0f};
    const auto expected = std::array<std::int16_t,3>{16384, -16384, 0};

    // Act
    const auto sut = build_normal_mesh(v);

    // Assert
    REQUIRE(read_normal(sut) == expected);
  }
  SECTION("Components outside of [-1, 1] are clamped") {
    // Arrange
    const auto v = core::vector3{2.0f, -3.0f, 0.0f};
    const auto expected = std::array<std::int16_t,3>{32767, -32767, 0};

    // Act
    const auto sut = build_normal_mesh(v);

    // Assert
    REQUIRE(read_normal(sut) == expected);
  }
}

TEST_CASE("unsafe_mesh_builder::release()", "[build]") {
  SECTION("Each vertex is padded to the stride") {
    // Arrange
    const auto components = vertex_components{
      .position = 3u,
      .normal   = 3u,
      .color    = 4u,
    };
    auto builder = unsafe_mesh_builder{components};
    for (auto i = 0; i < 2; ++i) {
      builder.add_position(core::point3{1, 2, 3});
      builder.add_normal(core::vector3{0, 1, 0});
      builder.add_color(core::color{});
    }

    // Act
    const auto sut = std::move(builder).release();

    // Assert
    REQUIRE(sut.data().size() == 48u);
  }
  SECTION("Only the configured number of components are packed") {
    // Arrange
    auto builder = unsafe_mesh_builder{vertex_components{.color = 3u}};
    builder.add_color(core::color{});
    builder.add_color(core::color{});

    // Act
    const auto sut = std::move(builder).release();

    // Assert
    REQUIRE(sut.data().size() == 6u);
  }
}

} // namespace alloy::render::test
