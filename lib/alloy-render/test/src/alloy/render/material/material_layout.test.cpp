// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/material/material_layout.hpp>

#include <alloy/render/error.hpp>
#include <alloy/render/shader/program_layout.hpp>
#include <alloy/render/shader/program_spec.hpp>
#include <alloy/render/shader/shader_data_type.hpp>
#include <alloy/render/shader/shader_parameter.hpp>
#include <alloy/render/shader/shader_stage.hpp>
#include <alloy/render/texture/texture_dimension.hpp>

#include <alloy/core/model/color.hpp>
#include <alloy/core/utilities/optional.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array

namespace alloy::render::test {
namespace {

constexpr auto stages = std::array{
  shader_stage_spec{shader_stage::vertex, {}, "main"},
  shader_stage_spec{shader_stage::fragment, {}, "main"},
};

constexpr auto material_members = std::array{
  uniform_member{"diffuse", shader_data_type::float4, 0u},
  uniform_member{"shininess", shader_data_type::float1, 16u},
  uniform_member{"normal_matrix", shader_data_type::float3x3, 32u},
};

constexpr auto camera_members = std::array{
  uniform_member{"view", shader_data_type::float4x4, 0u},
};

constexpr auto resources = std::array{
  program_resource{
    "camera",
    shader_stages{shader_stage::vertex},
    uniform_buffer_resource{64u, camera_members},
    shader_binding{0u, 0u, core::null},
  },
  program_resource{
    "material",
    shader_stages{shader_stage::fragment},
    uniform_buffer_resource{80u, material_members},
    shader_binding{0u, 1u, core::null},
  },
  program_resource{
    "albedo",
    shader_stages{shader_stage::fragment},
    sampled_texture_resource{texture_dimension::d2},
    shader_binding{0u, 0u, core::null},
  },
};

auto make_layout() -> program_layout
{
  return program_layout::from_spec(
    program_spec{"test", stages, {}, resources}
  ).value();
}

} // namespace <anonymous>

TEST_CASE("resolved_material::from_parameters(...)", "[factory][static]") {
  // Arrange
  const auto layout = make_layout();

  SECTION("Binds a value at the offset of its member") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"shininess", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result->uniform_blocks()[0].values[0].offset == 16u);
  }
  SECTION("Binds a value with the type of its member") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"diffuse", core::color{0.25f, 0.5f, 0.75f, 1.0f}},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result->uniform_blocks()[0].values[0].type == shader_data_type::float4);
  }
  SECTION("Keeps the data of a value") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"shininess", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result->uniform_blocks()[0].values[0].data == uniform_data{0.5f});
  }
  SECTION("Only includes uniform buffers named by a parameter") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"shininess", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result->uniform_blocks().size() == 1u);
  }
  SECTION("Carries the binding of a uniform buffer") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"material.shininess", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result->uniform_blocks()[0].binding.index == 1u);
  }
  SECTION("Carries the size of a uniform buffer") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"shininess", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result->uniform_blocks()[0].byte_size == 80u);
  }
  SECTION("Binds a texture with the dimension its program samples") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"albedo", texture_binding{}},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result->textures()[0].dimension == texture_dimension::d2);
  }
  SECTION("Undeclared name") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"specular", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result.error().code() == render_errc::parameter_not_found);
  }
  SECTION("Value of the wrong type for its member") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"diffuse", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result.error().code() == render_errc::parameter_type_mismatch);
  }
  SECTION("Uniform value for a texture") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"albedo", 0.5f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result.error().code() == render_errc::parameter_type_mismatch);
  }
  SECTION("Texture for a uniform member") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"shininess", texture_binding{}},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result.error().code() == render_errc::parameter_type_mismatch);
  }
  SECTION("Texture for a uniform buffer") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"material", texture_binding{}},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result.error().code() == render_errc::parameter_type_mismatch);
  }
  SECTION("Qualified and unqualified names of the same member") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"shininess", 0.5f},
      shader_parameter{"material.shininess", 0.25f},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result.error().code() == render_errc::duplicate_parameter);
  }
  SECTION("Same texture bound twice") {
    // Arrange
    const auto parameters = std::array{
      shader_parameter{"albedo", texture_binding{}},
      shader_parameter{"albedo", texture_binding{}},
    };

    // Act
    const auto result = resolved_material::from_parameters(layout, parameters);

    // Assert
    REQUIRE(result.error().code() == render_errc::duplicate_parameter);
  }
}

} // namespace alloy::render::test
