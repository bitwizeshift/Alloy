// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/device.hpp>

#include <alloy/render/error.hpp>

#include <alloy/render/material/material_spec.hpp>
#include <alloy/render/mesh/3d/cube_builder.hpp>
#include <alloy/render/mesh/mesh.hpp>
#include <alloy/render/resource/handle_pool.hpp>
#include <alloy/render/shader/program_layout.hpp>
#include <alloy/render/shader/program_spec.hpp>
#include <alloy/render/shader/shader_stage.hpp>
#include <alloy/render/texture/texture_format.hpp>
#include <alloy/render/texture/texture_spec.hpp>

#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <utility> // std::move

namespace alloy::render::test {
namespace {

/// \brief A device that tracks which resources are alive
class fake_device final : public device
{
public:

  auto live_programs() const noexcept -> std::size_t
  {
    return m_programs.size();
  }

  auto live_textures() const noexcept -> std::size_t
  {
    return m_textures.size();
  }

  auto live_meshes() const noexcept -> std::size_t
  {
    return m_meshes.size();
  }

private:

  handle_pool<program_tag, int> m_programs;
  handle_pool<texture_tag, int> m_textures;
  handle_pool<material_tag, int> m_materials;
  handle_pool<mesh_tag, int> m_meshes;

  auto do_create_program(const program_spec&, program_layout)
    -> core::result<program_handle, render_error> override
  {
    return m_programs.insert(0);
  }

  auto do_create_texture(const texture_spec&)
    -> core::result<texture_handle, render_error> override
  {
    return m_textures.insert(0);
  }

  auto do_create_material(const material_spec&)
    -> core::result<material_handle, render_error> override
  {
    return m_materials.insert(0);
  }

  auto do_create_mesh(const mesh&)
    -> core::result<mesh_handle, render_error> override
  {
    return m_meshes.insert(0);
  }

  auto do_dispose(program_handle handle) noexcept -> void override
  {
    m_programs.erase(handle);
  }

  auto do_dispose(texture_handle handle) noexcept -> void override
  {
    m_textures.erase(handle);
  }

  auto do_dispose(material_handle handle) noexcept -> void override
  {
    m_materials.erase(handle);
  }

  auto do_dispose(mesh_handle handle) noexcept -> void override
  {
    m_meshes.erase(handle);
  }
};

constexpr auto vertex_stage = std::array{
  shader_stage_spec{shader_stage::vertex, {}, "main"},
};

constexpr auto valid_program = program_spec{"test", vertex_stage, {}, {}, {}};

} // namespace <anonymous>

TEST_CASE("device::create_program(const program_spec&)", "[resource creation]") {
  // Arrange
  auto sut = fake_device{};

  SECTION("Valid spec creates the program") {
    // Act
    const auto result = sut.create_program(valid_program);

    // Assert
    REQUIRE(sut.live_programs() == 1u);
  }
  SECTION("Invalid spec fails") {
    // Arrange
    const auto spec = program_spec{"test", {}, {}, {}, {}};

    // Act
    const auto result = sut.create_program(spec);

    // Assert
    REQUIRE(result.error().code() == render_errc::missing_stage);
  }
  SECTION("Invalid spec does not reach the backend") {
    // Arrange
    const auto spec = program_spec{"test", {}, {}, {}, {}};

    // Act
    const auto result = sut.create_program(spec);

    // Assert
    REQUIRE(sut.live_programs() == 0u);
  }
}

TEST_CASE("device::create_texture(const texture_spec&)", "[resource creation]") {
  // Arrange
  auto sut = fake_device{};

  SECTION("Valid spec creates the texture") {
    // Arrange
    const auto spec = texture_spec::texture_2d(
      texture_format::r8_unorm, 4u, 4u
    ).value();

    // Act
    const auto result = sut.create_texture(spec);

    // Assert
    REQUIRE(sut.live_textures() == 1u);
  }
}

TEST_CASE("device::create_mesh(const mesh&)", "[resource creation]") {
  // Arrange
  auto sut = fake_device{};
  const auto mesh = cube_builder{}.build();

  SECTION("Uploads the mesh") {
    // Act
    const auto result = sut.create_mesh(mesh);

    // Assert
    REQUIRE(sut.live_meshes() == 1u);
  }
  SECTION("Owner disposes of the mesh") {
    // Arrange
    auto owner = sut.create_mesh(mesh).value();

    // Act
    owner.reset();

    // Assert
    REQUIRE(sut.live_meshes() == 0u);
  }
}

TEST_CASE("unique_resource::~unique_resource()", "[dtor]") {
  // Arrange
  auto device = fake_device{};

  SECTION("Disposes of the owned resource") {
    // Arrange
    auto sut = device.create_program(valid_program).value();

    // Act
    {
      auto destroyed = std::move(sut);
    }

    // Assert
    REQUIRE(device.live_programs() == 0u);
  }
}

TEST_CASE("unique_resource::unique_resource(unique_resource&&)", "[ctor]") {
  // Arrange
  auto device = fake_device{};
  auto sut = unique_program{};

  SECTION("Keeps the resource alive after the source is destroyed") {
    // Act
    {
      auto source = device.create_program(valid_program).value();
      sut = unique_program{std::move(source)};
    }

    // Assert
    REQUIRE(device.live_programs() == 1u);
  }
}

TEST_CASE("unique_resource::operator=(unique_resource&&)", "[assignment]") {
  // Arrange
  auto device = fake_device{};
  auto sut = device.create_program(valid_program).value();

  SECTION("Disposes of the previously owned resource") {
    // Arrange
    auto other = device.create_program(valid_program).value();

    // Act
    sut = std::move(other);

    // Assert
    REQUIRE(device.live_programs() == 1u);
  }
  SECTION("Takes ownership of the other resource") {
    // Arrange
    auto other = device.create_program(valid_program).value();
    const auto expected = other.get();

    // Act
    sut = std::move(other);

    // Assert
    REQUIRE(sut.get() == expected);
  }
}

TEST_CASE("unique_resource::reset()", "[modifier]") {
  // Arrange
  auto device = fake_device{};
  auto sut = device.create_program(valid_program).value();

  SECTION("Disposes of the owned resource") {
    // Act
    sut.reset();

    // Assert
    REQUIRE(device.live_programs() == 0u);
  }
  SECTION("Leaves a null handle") {
    // Act
    sut.reset();

    // Assert
    REQUIRE(sut.get().is_null());
  }
}

TEST_CASE("unique_resource::release()", "[modifier]") {
  // Arrange
  auto device = fake_device{};

  SECTION("Does not dispose of the resource") {
    // Act
    {
      auto sut = device.create_program(valid_program).value();
      sut.release();
    }

    // Assert
    REQUIRE(device.live_programs() == 1u);
  }
  SECTION("Returns the handle of the resource") {
    // Arrange
    auto sut = device.create_program(valid_program).value();
    const auto expected = sut.get();

    // Act
    const auto result = sut.release();

    // Assert
    REQUIRE(result == expected);
  }
}

} // namespace alloy::render::test
