// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/mesh/mesh.hpp>

#include <alloy/render/mesh/primitive_topology.hpp>

#include <alloy/core/containers/vector.hpp>
#include <alloy/core/geometry/axis_aligned_box.hpp>
#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/memory/packed_buffer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef> // std::size_t, std::byte
#include <utility> // std::move

namespace alloy::render::test {
namespace {

auto make_vertex_data(std::size_t bytes) -> core::packed_buffer
{
  return core::packed_buffer{core::vector<std::byte>(bytes)};
}

/// \brief Makes a config of 3-component positions with \p vertices vertices
auto make_position_config(std::size_t vertices) -> mesh_config
{
  auto config = mesh_config{};
  config.vertex_data = make_vertex_data(vertices * 3u * sizeof(float));
  config.topology = primitive_topology::triangles;
  config.position_components = 3u;
  return config;
}

} // namespace <anonymous>

TEST_CASE("mesh::from_config(mesh_config)", "[factory][static]") {
  SECTION("Vertex data without any components") {
    // Arrange
    auto config = mesh_config{};
    config.vertex_data = make_vertex_data(12u);
    config.topology = primitive_topology::points;

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.error() == mesh::construct_error::invalid_vertex_data);
  }
  SECTION("Vertex data that is not a multiple of the stride") {
    // Arrange
    auto config = make_position_config(1u);
    config.vertex_data = make_vertex_data(13u);

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.error() == mesh::construct_error::invalid_vertex_data);
  }
  SECTION("Triangle indices that are not a multiple of 3") {
    // Arrange
    auto config = make_position_config(4u);
    config.indices = core::vector<mesh::index_type>{0u, 1u, 2u, 3u};

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.error() == mesh::construct_error::invalid_indices);
  }
  SECTION("Line indices that are not a multiple of 2") {
    // Arrange
    auto config = make_position_config(3u);
    config.topology = primitive_topology::lines;
    config.indices = core::vector<mesh::index_type>{0u, 1u, 2u};

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.error() == mesh::construct_error::invalid_indices);
  }
  SECTION("Line adjacency indices that are not a multiple of 4") {
    // Arrange
    auto config = make_position_config(4u);
    config.topology = primitive_topology::lines_adjacency;
    config.indices = core::vector<mesh::index_type>{0u, 1u};

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.error() == mesh::construct_error::invalid_indices);
  }
  SECTION("Triangle adjacency indices that are not a multiple of 6") {
    // Arrange
    auto config = make_position_config(6u);
    config.topology = primitive_topology::triangles_adjacency;
    config.indices = core::vector<mesh::index_type>{0u, 1u, 2u};

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.error() == mesh::construct_error::invalid_indices);
  }
  SECTION("Index that refers to a vertex that does not exist") {
    // Arrange
    auto config = make_position_config(3u);
    config.indices = core::vector<mesh::index_type>{0u, 1u, 3u};

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.error() == mesh::construct_error::invalid_indices);
  }
  SECTION("Valid configuration") {
    // Arrange
    auto config = make_position_config(3u);
    config.indices = core::vector<mesh::index_type>{0u, 1u, 2u};

    // Act
    const auto sut = mesh::from_config(std::move(config));

    // Assert
    REQUIRE(sut.has_value());
  }
}

TEST_CASE("mesh::vertex_count()", "[observer]") {
  SECTION("Mesh without vertex data") {
    // Arrange
    const auto sut = mesh::from_config_unchecked(mesh_config{});

    // Act
    const auto result = sut.vertex_count();

    // Assert
    REQUIRE(result == 0u);
  }
  SECTION("Mesh with vertex data") {
    // Arrange
    const auto sut = mesh::from_config_unchecked(make_position_config(5u));

    // Act
    const auto result = sut.vertex_count();

    // Assert
    REQUIRE(result == 5u);
  }
}

TEST_CASE("mesh::bounding_volume()", "[observer]") {
  // Arrange
  const auto expected = core::axis_aligned_box{
    core::point3{-1, -2, -3},
    core::point3{4, 5, 6}
  };
  auto config = make_position_config(1u);
  config.bounding_volume = expected;
  const auto sut = mesh::from_config_unchecked(std::move(config));

  // Act
  const auto& result = sut.bounding_volume();

  // Assert
  REQUIRE(result == expected);
}

} // namespace alloy::render::test
