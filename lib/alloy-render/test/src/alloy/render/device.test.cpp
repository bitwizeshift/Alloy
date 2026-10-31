// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#include <alloy/render/device.hpp>

#include <alloy/render/error.hpp>
#include <alloy/render/material/material_spec.hpp>
#include <alloy/render/mesh/3d/cube_builder.hpp>
#include <alloy/render/mesh/mesh.hpp>
#include <alloy/render/model/model_spec.hpp>
#include <alloy/render/pass/render_batch.hpp>
#include <alloy/render/pass/render_pass.hpp>
#include <alloy/render/resource/handle_pool.hpp>
#include <alloy/render/shader/program_layout.hpp>
#include <alloy/render/shader/program_spec.hpp>
#include <alloy/render/shader/shader_stage.hpp>
#include <alloy/render/surface/surface_spec.hpp>
#include <alloy/render/texture/texture_format.hpp>
#include <alloy/render/texture/texture_spec.hpp>

#include <alloy/core/utilities/optional.hpp>
#include <alloy/core/utilities/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <span>    // std::span
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

  auto live_models() const noexcept -> std::size_t
  {
    return m_models.size();
  }

  auto live_surfaces() const noexcept -> std::size_t
  {
    return m_surfaces.size();
  }

  auto render(surface_handle, const render_pass& pass)
    -> core::result<void, render_error> override
  {
    m_rendered_entries += pass.batch->entries().size();
    return {};
  }

  auto present(surface_handle) -> void override
  {
    ++m_presents;
  }

  auto create_window_surface() -> unique_surface
  {
    return adopt(m_surfaces.insert(0));
  }

  auto rendered_entries() const noexcept -> std::size_t
  {
    return m_rendered_entries;
  }

  auto presents() const noexcept -> std::size_t
  {
    return m_presents;
  }

private:

  handle_pool<program_tag, int> m_programs;
  handle_pool<texture_tag, int> m_textures;
  handle_pool<material_tag, int> m_materials;
  handle_pool<mesh_tag, int> m_meshes;
  handle_pool<model_tag, int> m_models;
  handle_pool<surface_tag, int> m_surfaces;
  std::size_t m_rendered_entries = 0u;
  std::size_t m_presents = 0u;

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

  auto do_create_model(const model_spec&)
    -> core::result<model_handle, render_error> override
  {
    return m_models.insert(0);
  }

  auto do_create_surface(const surface_spec&)
    -> core::result<surface_handle, render_error> override
  {
    return m_surfaces.insert(0);
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

  auto do_dispose(model_handle handle) noexcept -> void override
  {
    m_models.erase(handle);
  }

  auto do_dispose(surface_handle handle) noexcept -> void override
  {
    m_surfaces.erase(handle);
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

TEST_CASE("device::create_model(const model_spec&)", "[resource creation]") {
  // Arrange
  auto sut = fake_device{};
  const auto spec = model_spec{mesh_handle{}, {}};

  SECTION("Creates the model") {
    // Act
    const auto result = sut.create_model(spec);

    // Assert
    REQUIRE(sut.live_models() == 1u);
  }
  SECTION("Owner disposes of the model") {
    // Arrange
    auto owner = sut.create_model(spec).value();

    // Act
    owner.reset();

    // Assert
    REQUIRE(sut.live_models() == 0u);
  }
}

TEST_CASE("device::create_surface(const surface_spec&)", "[resource creation]") {
  // Arrange
  auto sut = fake_device{};
  const auto spec = surface_spec{texture_handle{}, texture_handle{}};

  SECTION("Creates the surface") {
    // Act
    const auto result = sut.create_surface(spec);

    // Assert
    REQUIRE(sut.live_surfaces() == 1u);
  }
  SECTION("Owner disposes of the surface") {
    // Arrange
    auto owner = sut.create_surface(spec).value();

    // Act
    owner.reset();

    // Assert
    REQUIRE(sut.live_surfaces() == 0u);
  }
}

TEST_CASE("device::adopt(resource_handle)", "[resource ownership]") {
  // Arrange
  auto device = fake_device{};

  SECTION("Owner disposes of the adopted resource") {
    // Arrange
    auto sut = device.create_window_surface();

    // Act
    sut.reset();

    // Assert
    REQUIRE(device.live_surfaces() == 0u);
  }
}

TEST_CASE("device::render(surface_handle, const render_pass&)", "[drawing]") {
  // Arrange
  auto sut = fake_device{};
  const auto surface = sut.create_window_surface();
  const auto instances = std::array{0.0f, 1.0f};
  auto batch = render_batch{};
  batch.add(model_handle{}, std::span{instances});
  const auto pass = render_pass{
    .batch = &batch,
    .globals = {},
    .clear_color = core::null,
    .clear_depth = core::null,
  };

  SECTION("Draws the batch of the pass") {
    // Act
    const auto result = sut.render(surface.get(), pass);

    // Assert
    REQUIRE(sut.rendered_entries() == 1u);
  }
}

TEST_CASE("device::present(surface_handle)", "[drawing]") {
  // Arrange
  auto sut = fake_device{};
  const auto surface = sut.create_window_surface();

  SECTION("Presents the surface") {
    // Act
    sut.present(surface.get());

    // Assert
    REQUIRE(sut.presents() == 1u);
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
