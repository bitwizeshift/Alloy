// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/shader/program_layout.hpp>

#include <alloy/render/shader/program_spec.hpp>
#include <alloy/render/shader/shader_data_type.hpp>
#include <alloy/render/shader/shader_stage.hpp>

#include <alloy/render/error.hpp>
#include <alloy/render/texture/texture_dimension.hpp>

#include <alloy/core/string/string_view.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstdint> // std::uint32_t
#include <span>    // std::span

namespace alloy::render::test {
namespace {

constexpr auto make_stage(shader_stage stage) noexcept -> shader_stage_spec
{
  return shader_stage_spec{stage, {}, "main"};
}

auto make_program(
  std::span<const shader_stage_spec> stages,
  std::span<const program_resource> resources
) -> program_spec
{
  return program_spec{"test", stages, {}, resources};
}

constexpr auto vertex_and_fragment = std::array{
  make_stage(shader_stage::vertex),
  make_stage(shader_stage::fragment),
};

auto make_uniform_buffer(
  core::string_view name,
  std::uint32_t size,
  std::span<const uniform_member> members
) -> program_resource
{
  return program_resource{
    .name = name,
    .stages = shader_stages{shader_stage::vertex},
    .kind = uniform_buffer_resource{size, members},
    .binding = {},
  };
}

auto make_texture(core::string_view name) -> program_resource
{
  return program_resource{
    .name = name,
    .stages = shader_stages{shader_stage::fragment},
    .kind = sampled_texture_resource{texture_dimension::d2},
    .binding = {},
  };
}

} // namespace <anonymous>

TEST_CASE("program_layout::from_spec(const program_spec&)", "[factory][static]") {
  SECTION("No stages") {
    // Arrange
    const auto spec = make_program({}, {});

    // Act
    const auto sut = program_layout::from_spec(spec);

    // Assert
    REQUIRE(sut.error().code() == render_errc::missing_stage);
  }
  SECTION("Duplicate stages") {
    // Arrange
    const auto stages = std::array{
      make_stage(shader_stage::vertex),
      make_stage(shader_stage::vertex),
    };
    const auto spec = make_program(stages, {});

    // Act
    const auto sut = program_layout::from_spec(spec);

    // Assert
    REQUIRE(sut.error().code() == render_errc::duplicate_stage);
  }
  SECTION("Duplicate resources") {
    // Arrange
    const auto resources = std::array{
      make_texture("albedo"),
      make_texture("albedo"),
    };
    const auto spec = make_program(vertex_and_fragment, resources);

    // Act
    const auto sut = program_layout::from_spec(spec);

    // Assert
    REQUIRE(sut.error().code() == render_errc::duplicate_resource);
  }
  SECTION("Members with the same name") {
    // Arrange
    const auto members = std::array{
      uniform_member{"diffuse", shader_data_type::float4, 0u},
      uniform_member{"diffuse", shader_data_type::float4, 16u},
    };
    const auto resources = std::array{
      make_uniform_buffer("material", 32u, members),
    };
    const auto spec = make_program(vertex_and_fragment, resources);

    // Act
    const auto sut = program_layout::from_spec(spec);

    // Assert
    REQUIRE(sut.error().code() == render_errc::invalid_uniform_layout);
  }
  SECTION("Valid program retains every resource") {
    // Arrange
    const auto members = std::array{
      uniform_member{"view", shader_data_type::float4x4, 0u},
    };
    const auto resources = std::array{
      make_uniform_buffer("camera", 64u, members),
      make_texture("albedo"),
    };
    const auto spec = make_program(vertex_and_fragment, resources);

    // Act
    const auto sut = program_layout::from_spec(spec);

    // Assert
    REQUIRE(sut->resources().size() == 2u);
  }
}

TEST_CASE("program_layout::find_resource(core::string_view)", "[observer]") {
  // Arrange
  const auto resources = std::array{
    make_texture("albedo"),
    make_texture("normal"),
  };
  const auto spec = make_program(vertex_and_fragment, resources);
  const auto sut = program_layout::from_spec(spec).value();

  SECTION("Declared resource") {
    // Act
    const auto result = sut.find_resource("normal");

    // Assert
    REQUIRE(result == 1u);
  }
  SECTION("Undeclared resource") {
    // Act
    const auto result = sut.find_resource("specular");

    // Assert
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("program_layout::find_member(core::string_view)", "[observer]") {
  // Arrange
  const auto camera_members = std::array{
    uniform_member{"view", shader_data_type::float4x4, 0u},
    uniform_member{"position", shader_data_type::float3, 64u},
  };
  const auto light_members = std::array{
    uniform_member{"position", shader_data_type::float3, 0u},
  };
  const auto resources = std::array{
    make_uniform_buffer("camera", 80u, camera_members),
    make_uniform_buffer("light", 16u, light_members),
  };
  const auto spec = make_program(vertex_and_fragment, resources);
  const auto sut = program_layout::from_spec(spec).value();

  SECTION("Qualified name") {
    // Act
    const auto* const result = sut.find_member("light.position");

    // Assert
    REQUIRE(result->resource_index == 1u);
  }
  SECTION("Unqualified name that is unique") {
    // Act
    const auto* const result = sut.find_member("view");

    // Assert
    REQUIRE(result->offset == 0u);
  }
  SECTION("Unqualified name that is ambiguous") {
    // Act
    const auto* const result = sut.find_member("position");

    // Assert
    REQUIRE(result == nullptr);
  }
  SECTION("Undeclared member") {
    // Act
    const auto* const result = sut.find_member("camera.projection");

    // Assert
    REQUIRE(result == nullptr);
  }
}

} // namespace alloy::render::test
