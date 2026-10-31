// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/unsafe_mesh_builder.hpp>

#include "mesh_test_utilities.hpp"
#include <alloy/render/mesh/index_format.hpp>
#include <alloy/render/mesh/mesh.hpp>

#include <alloy/core/geometry/point/point2.hpp>
#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/math/vector/vector4.hpp>
#include <alloy/core/model/color.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstdint> // std::int16_t, std::uint8_t, std::uint16_t
#include <cstring> // std::memcpy
#include <span>    // std::span
#include <utility> // std::move
#include <vector>  // std::vector

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

/// \brief Reads the 4 unorm16 skinning weights out of \p m
auto read_weights(const mesh& m) -> std::array<std::uint16_t,4>
{
  auto result = std::array<std::uint16_t,4>{};
  const auto data = m.data().subspan(m.offset_to_weights().count());
  std::memcpy(result.data(), data.data(), sizeof(result));
  return result;
}

/// \brief Reads the first 3 skinning joint indices out of \p m
auto read_joints(const mesh& m) -> std::array<std::uint8_t,3>
{
  auto result = std::array<std::uint8_t,3>{};
  const auto data = m.data().subspan(m.offset_to_joints().count());
  std::memcpy(result.data(), data.data(), sizeof(result));
  return result;
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

TEST_CASE("unsafe_mesh_builder::add_weights(const vector4&)", "[modifier]") {
  // Arrange
  const auto v = core::vector4{1.0f, 0.5f, 0.0f, -1.0f};
  const auto expected = std::array<std::uint16_t,4>{65535u, 32768u, 0u, 0u};
  auto builder = unsafe_mesh_builder{vertex_components{.weights = 4u}};
  builder.add_weights(v);

  // Act
  const auto sut = std::move(builder).release();

  // Assert
  REQUIRE(read_weights(sut) == expected);
}

TEST_CASE("unsafe_mesh_builder::add_joints(const std::array<std::uint8_t,4>&)", "[modifier]") {
  // Arrange
  const auto joints = std::array<std::uint8_t,4>{3u, 1u, 4u, 1u};
  const auto expected = std::array<std::uint8_t,3>{3u, 1u, 4u};
  auto builder = unsafe_mesh_builder{vertex_components{.joints = 3u}};
  builder.add_joints(joints);

  // Act
  const auto sut = std::move(builder).release();

  // Assert
  REQUIRE(read_joints(sut) == expected);
}

TEST_CASE("unsafe_mesh_builder::begin_submesh()", "[modifier]") {
  // Arrange
  const auto add_triangle = [](unsafe_mesh_builder& builder) {
    builder.add_position(core::point2{});
    builder.add_position(core::point2{});
    builder.add_position(core::point2{});
  };

  SECTION("Indexed submeshes are measured in indices") {
    // Arrange
    using range = std::array<std::uint32_t,2>;
    const auto expected = std::vector<range>{range{0u, 3u}, range{3u, 6u}};
    auto builder = unsafe_mesh_builder{vertex_components{.position = 2u}};
    builder.begin_submesh();
    add_triangle(builder);
    builder.add_face(0u, 1u, 2u);
    builder.begin_submesh();
    add_triangle(builder);
    builder.add_face(3u, 4u, 5u);
    builder.add_face(5u, 4u, 3u);

    // Act
    const auto sut = std::move(builder).release();

    // Assert
    REQUIRE(submesh_ranges(sut) == expected);
  }
  SECTION("Non-indexed submeshes are measured in vertices") {
    // Arrange
    using range = std::array<std::uint32_t,2>;
    const auto expected = std::vector<range>{range{0u, 3u}, range{3u, 3u}};
    auto builder = unsafe_mesh_builder{vertex_components{.position = 2u}};
    add_triangle(builder);
    builder.begin_submesh();
    add_triangle(builder);

    // Act
    const auto sut = std::move(builder).release();

    // Assert
    REQUIRE(submesh_ranges(sut) == expected);
  }
  SECTION("A single submesh covers the mesh unless one is started") {
    // Arrange
    using range = std::array<std::uint32_t,2>;
    const auto expected = std::vector<range>{range{0u, 3u}};
    auto builder = unsafe_mesh_builder{vertex_components{.position = 2u}};
    add_triangle(builder);

    // Act
    const auto sut = std::move(builder).release();

    // Assert
    REQUIRE(submesh_ranges(sut) == expected);
  }
}

TEST_CASE("unsafe_mesh_builder::release()", "[build]") {
  SECTION("Indices are 16-bit when every vertex is addressable by them") {
    // Arrange
    auto builder = unsafe_mesh_builder{vertex_components{.position = 1u}};
    for (auto i = 0; i < 65536; ++i) {
      builder.add_position(core::point2{});
    }
    builder.add_face(0u, 1u, 65535u);

    // Act
    const auto sut = std::move(builder).release();

    // Assert
    REQUIRE(sut.index_format() == index_format::u16);
  }
  SECTION("Indices are 32-bit when there are too many vertices for 16-bit") {
    // Arrange
    auto builder = unsafe_mesh_builder{vertex_components{.position = 1u}};
    for (auto i = 0; i < 65537; ++i) {
      builder.add_position(core::point2{});
    }
    builder.add_face(0u, 1u, 65536u);

    // Act
    const auto sut = std::move(builder).release();

    // Assert
    REQUIRE(sut.index_at(2u) == 65536u);
  }
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
