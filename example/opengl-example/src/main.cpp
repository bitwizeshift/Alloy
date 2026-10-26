// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "sdl_window_target.hpp"
#include "shaders.hpp"

#include <alloy/core/intrinsics.hpp>
#include <alloy/core/math/angle/radian.hpp>
#include <alloy/core/math/matrix/matrix4.hpp>
#include <alloy/core/math/vector/vector3.hpp>
#include <alloy/core/model/camera.hpp>
#include <alloy/core/model/color.hpp>
#include <alloy/core/model/projection.hpp>
#include <alloy/core/model/transform.hpp>
#include <alloy/core/utilities/optional.hpp>
#include <alloy/core/utilities/signal.hpp>
#include <alloy/extra/sdl2-bindings/sdl2_gl_window.hpp>
#include <alloy/extra/sdl2-bindings/sdl2_pump_source.hpp>
#include <alloy/platform/event.hpp>
#include <alloy/platform/events/window_events.hpp>
#include <alloy/platform/message_pump.hpp>
#include <alloy/platform/window.hpp>
#include <alloy/render/error.hpp>
#include <alloy/render/gl/device.hpp>
#include <alloy/render/light/light.hpp>
#include <alloy/render/light/point_light.hpp>
#include <alloy/render/material/material_spec.hpp>
#include <alloy/render/mesh/3d/cube_builder.hpp>
#include <alloy/render/mesh/3d/sphere_builder.hpp>
#include <alloy/render/mesh/attribute_locations.hpp>
#include <alloy/render/model/model_spec.hpp>
#include <alloy/render/pass/render_batch.hpp>
#include <alloy/render/pass/render_pass.hpp>
#include <alloy/render/shader/program_spec.hpp>
#include <alloy/render/shader/shader_data_type.hpp>
#include <alloy/render/shader/shader_parameter.hpp>
#include <alloy/render/shader/shader_stage.hpp>
#include <alloy/render/texture/sampler_state.hpp>
#include <alloy/render/texture/texture_dimension.hpp>
#include <alloy/render/texture/texture_format.hpp>
#include <alloy/render/texture/texture_spec.hpp>

#include <GL/glew.h>
#include <SDL3/SDL.h>      // ::SDL_Init, ::SDL_Quit
#include <SDL3/SDL_main.h> // main

#include <array>       // std::array
#include <chrono>      // std::chrono::steady_clock
#include <cmath>       // std::cos, std::sin
#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <cstdlib>     // EXIT_FAILURE, EXIT_SUCCESS
#include <iostream>    // std::cerr
#include <numbers>     // std::numbers::pi_v
#include <span>        // std::span, std::as_bytes
#include <string_view> // std::string_view

namespace {

  namespace core = alloy::core;
  namespace render = alloy::render;

  using data_type = render::shader_data_type;

  // Flag to keep the application running
  bool g_keep_running = true;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Stops the application when its window is closed
  //////////////////////////////////////////////////////////////////////////////
  class window_listener final
    : public alloy::platform::message_pump::listener
  {
  public:
    auto handle_message(const alloy::platform::event& e)
      noexcept -> void override;
  };

  auto window_listener::handle_message(const alloy::platform::event& e)
    noexcept -> void
  {
    if (e.is<alloy::platform::window_close_event>()) {
      ::g_keep_running = false;
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The values of a single instance, laid out as the programs'
  ///        instance inputs describe
  //////////////////////////////////////////////////////////////////////////////
  struct instance
  {
    core::matrix4 model;
  };
  static_assert(sizeof(instance) == 16u * sizeof(float));

  constexpr auto pi = std::numbers::pi_v<float>;

  auto as_bytecode(std::string_view source) noexcept
    -> std::span<const std::byte>
  {
    return std::as_bytes(std::span{source.data(), source.size()});
  }

  //----------------------------------------------------------------------------
  // Program descriptions
  //----------------------------------------------------------------------------

  // Offsets follow the std140 rules, which the GLSL blocks are declared with

  constexpr auto view_members = std::array{
    render::uniform_member{"view_projection", data_type::float4x4, 0u},
    render::uniform_member{"camera_position", data_type::float3, 64u},
  };

  constexpr auto environment_members = std::array{
    render::uniform_member{"light_position", data_type::float3, 0u},
    render::uniform_member{"ambient_intensity", data_type::float1, 12u},
    render::uniform_member{"light_color", data_type::float4, 16u},
    render::uniform_member{"attenuation", data_type::float3, 32u},
    render::uniform_member{"diffuse_intensity", data_type::float1, 44u},
  };

  constexpr auto textured_material_members = std::array{
    render::uniform_member{"shininess", data_type::float1, 0u},
  };

  constexpr auto color_material_members = std::array{
    render::uniform_member{"base_color", data_type::float4, 0u},
    render::uniform_member{"shininess", data_type::float1, 16u},
  };

  constexpr auto vertex_inputs = std::array{
    render::vertex_input{
      "position",
      render::attribute_locations::positions,
      render::shader_data_type::float3
    },
    render::vertex_input{
      "uv",
      render::attribute_locations::uvs,
      render::shader_data_type::float2
    },
    render::vertex_input{
      "normal",
      render::attribute_locations::normals,
      render::shader_data_type::float3
    },
  };

  // Every mesh attribute is read from a location below 8
  constexpr auto instance_inputs = std::array{
    render::vertex_input{"model", 8u, render::shader_data_type::float4x4},
  };

  constexpr auto both_stages = render::shader_stages{
    render::shader_stage::vertex,
    render::shader_stage::fragment,
  };

  constexpr auto view_resource = render::program_resource{
    .name = "view",
    .stages = both_stages,
    .kind = render::uniform_buffer_resource{80u, view_members},
    .binding = render::shader_binding{0u, 0u, core::null},
    .scope = render::resource_scope::global,
  };

  constexpr auto environment_resource = render::program_resource{
    .name = "environment",
    .stages = render::shader_stages{render::shader_stage::fragment},
    .kind = render::uniform_buffer_resource{48u, environment_members},
    .binding = render::shader_binding{0u, 1u, core::null},
    .scope = render::resource_scope::global,
  };

  constexpr auto textured_resources = std::array{
    view_resource,
    environment_resource,
    render::program_resource{
      .name = "material",
      .stages = render::shader_stages{render::shader_stage::fragment},
      .kind = render::uniform_buffer_resource{16u, textured_material_members},
      .binding = render::shader_binding{0u, 2u, core::null},
      .scope = render::resource_scope::material,
    },
    render::program_resource{
      .name = "albedo",
      .stages = render::shader_stages{render::shader_stage::fragment},
      .kind = render::sampled_texture_resource{render::texture_dimension::d2},
      .binding = render::shader_binding{0u, 0u, core::null},
      .scope = render::resource_scope::material,
    },
  };

  constexpr auto color_resources = std::array{
    view_resource,
    environment_resource,
    render::program_resource{
      .name = "material",
      .stages = render::shader_stages{render::shader_stage::fragment},
      .kind = render::uniform_buffer_resource{32u, color_material_members},
      .binding = render::shader_binding{0u, 2u, core::null},
      .scope = render::resource_scope::material,
    },
  };

  //----------------------------------------------------------------------------
  // Scene
  //----------------------------------------------------------------------------

  /// \brief One color per cell of the 2 by 3 grid that the cube's faces are
  ///        mapped onto
  constexpr auto face_colors = std::array<std::uint8_t, std::size_t{24u}>{
    230u,  57u,  70u, 255u,   244u, 162u,  97u, 255u,
     42u, 157u, 143u, 255u,    69u, 123u, 157u, 255u,
    233u, 196u, 106u, 255u,   157u,  78u, 221u, 255u,
  };

  constexpr auto face_sampler = render::sampler_state{
    .min_filter = render::texture_filter::nearest,
    .mag_filter = render::texture_filter::nearest,
    .mip_filter = render::mipmap_filter::none,
    .wrap_u = render::texture_wrap::clamp_to_edge,
    .wrap_v = render::texture_wrap::clamp_to_edge,
    .wrap_w = render::texture_wrap::clamp_to_edge,
  };

  constexpr auto light = render::point_light{
    .source = core::point3{2.5f, 3.0f, 3.0f},
    .color = core::color{1.0f, 0.95f, 0.85f, 1.0f},
    .attenuation = render::light::attenuation{1.0f, 0.045f, 0.0075f},
    .intensity = render::light::intensity{0.15f, 1.4f},
  };

  /// \brief The rate that the cube turns, in radians per second
  constexpr auto cube_turn_rate = pi / 9.0f;

  /// \brief The rate that the sphere orbits the cube, in radians per second
  constexpr auto sphere_orbit_rate = pi / 4.0f;

  /// \brief The rate that the sphere spins, in radians per second
  constexpr auto sphere_spin_rate = pi / 2.0f;

  constexpr auto sphere_orbit_radius = 2.75f;

  auto report(const render::render_error& error) -> int
  {
    std::cerr << error.code().message();
    if (!error.details().empty()) {
      std::cerr << ": " << error.details().c_str();
    }
    std::cerr << '\n';
    return EXIT_FAILURE;
  }

  auto cube_instance(float seconds) -> instance
  {
    auto transform = core::transform{};
    transform.set_orientation(
      core::radian{seconds * cube_turn_rate},
      core::vector3{0.3f, 1.0f, 0.2f}.normalized()
    );
    return instance{transform.to_matrix4()};
  }

  auto sphere_instance(float seconds) -> instance
  {
    const auto orbit = seconds * sphere_orbit_rate;

    auto transform = core::transform{};
    transform.set_translation(
      std::cos(orbit) * sphere_orbit_radius,
      0.5f,
      std::sin(orbit) * sphere_orbit_radius
    );
    transform.set_orientation(
      core::radian{seconds * sphere_spin_rate},
      core::vector3{0.0f, 1.0f, 0.0f}
    );
    transform.set_scale(0.45f);
    return instance{transform.to_matrix4()};
  }

  /// \brief Draws the scene into \p window until it is closed
  ///
  /// The device lives only within this function, so that it and everything
  /// it created are destroyed while the window's context still exists.
  auto run(alloy::extra::sdl2_gl_window& window) -> int
  {
    auto device = render::gl::device::from_current_context();
    auto target = alloy::example::sdl_window_target{&window};
    const auto surface = device.create_surface(target);

    // Programs

    const auto vertex_stage = render::shader_stage_spec{
      render::shader_stage::vertex,
      as_bytecode(alloy::example::lit_vertex),
      "main"
    };
    const auto textured_stages = std::array{
      vertex_stage,
      render::shader_stage_spec{
        render::shader_stage::fragment,
        as_bytecode(alloy::example::lit_textured_fragment),
        "main"
      },
    };
    const auto color_stages = std::array{
      vertex_stage,
      render::shader_stage_spec{
        render::shader_stage::fragment,
        as_bytecode(alloy::example::lit_color_fragment),
        "main"
      },
    };

    const auto textured_program = device.create_program(
      render::program_spec{
        .name = "lit_textured",
        .stages = textured_stages,
        .vertex_inputs = vertex_inputs,
        .resources = textured_resources,
        .instance_inputs = instance_inputs,
      }
    );
    if (!textured_program) {
      return report(textured_program.error());
    }
    const auto color_program = device.create_program(
      render::program_spec{
        .name = "lit_color",
        .stages = color_stages,
        .vertex_inputs = vertex_inputs,
        .resources = color_resources,
        .instance_inputs = instance_inputs,
      }
    );
    if (!color_program) {
      return report(color_program.error());
    }

    // Materials

    const auto face_spec = render::texture_spec::texture_2d(
      render::texture_format::rgba8_unorm,
      2u,
      3u,
      std::as_bytes(std::span{face_colors})
    );
    if (!face_spec) {
      return report(face_spec.error());
    }
    const auto face_texture = device.create_texture(*face_spec);
    if (!face_texture) {
      return report(face_texture.error());
    }

    const auto cube_parameters = std::array{
      render::shader_parameter{
        "albedo",
        render::texture_binding{face_texture->get(), face_sampler}
      },
      render::shader_parameter{"shininess", 32.0f},
    };
    const auto cube_material = device.create_material(
      render::material_spec{textured_program->get(), cube_parameters}
    );
    if (!cube_material) {
      return report(cube_material.error());
    }

    const auto sphere_parameters = std::array{
      render::shader_parameter{
        "base_color",
        core::color{0.92f, 0.92f, 0.96f, 1.0f}
      },
      render::shader_parameter{"shininess", 64.0f},
    };
    const auto sphere_material = device.create_material(
      render::material_spec{color_program->get(), sphere_parameters}
    );
    if (!sphere_material) {
      return report(sphere_material.error());
    }

    // Models

    const auto cube_mesh = device.create_mesh(render::cube_builder{}.build());
    if (!cube_mesh) {
      return report(cube_mesh.error());
    }
    const auto sphere_mesh = device.create_mesh(
      render::sphere_builder{48u, 24u}.build()
    );
    if (!sphere_mesh) {
      return report(sphere_mesh.error());
    }

    const auto cube_materials = std::array{cube_material->get()};
    const auto cube_model = device.create_model(
      render::model_spec{cube_mesh->get(), cube_materials}
    );
    if (!cube_model) {
      return report(cube_model.error());
    }
    const auto sphere_materials = std::array{sphere_material->get()};
    const auto sphere_model = device.create_model(
      render::model_spec{sphere_mesh->get(), sphere_materials}
    );
    if (!sphere_model) {
      return report(sphere_model.error());
    }

    // Frame loop

    auto camera = core::camera{};
    camera.translate_world(0.0f, 2.5f, 7.5f);
    camera.look_at(0.0f, 0.0f, 0.0f);

    auto message_pump = alloy::platform::message_pump{};
    auto listener = window_listener{};
    auto& pump_source = alloy::extra::sdl2_pump_source::instance();
    pump_source.attach_window(&window);
    const auto source_connection = core::scoped_connection{
      message_pump.on_poll().connect(&pump_source)
    };
    const auto listener_connection = core::scoped_connection{
      message_pump.on_event().connect(&listener)
    };

    using clock = std::chrono::steady_clock;
    const auto start = clock::now();
    auto batch = render::render_batch{};
    auto exit_code = EXIT_SUCCESS;

    while (::g_keep_running) {
      message_pump.pump();

      const auto seconds = std::chrono::duration<float>{clock::now() - start};
      const auto extent = target.drawable_extent();
      if (extent.width == 0u || extent.height == 0u) {
        continue;
      }

      const auto aspect_ratio = static_cast<float>(extent.width)
        / static_cast<float>(extent.height);
      const auto projection = core::projection::perspective(
        core::radian{pi / 3.0f},
        aspect_ratio,
        core::view_volume::depth_bounds{0.1f, 100.0f}
      );
      const auto view_projection = projection.to_matrix4()
        * camera.to_matrix4();

      const auto cube = std::array{cube_instance(seconds.count())};
      const auto sphere = std::array{sphere_instance(seconds.count())};
      batch.clear();
      batch.add(cube_model->get(), std::span{cube});
      batch.add(sphere_model->get(), std::span{sphere});

      const auto globals = std::array{
        render::shader_parameter{"view_projection", view_projection},
        render::shader_parameter{"camera_position", camera.translation()},
        render::shader_parameter{
          "light_position",
          core::vector3{light.source.x(), light.source.y(), light.source.z()}
        },
        render::shader_parameter{"light_color", light.color},
        render::shader_parameter{
          "attenuation",
          core::vector3{
            light.attenuation.constant,
            light.attenuation.linear,
            light.attenuation.exponential
          }
        },
        render::shader_parameter{"ambient_intensity", light.intensity.ambient},
        render::shader_parameter{"diffuse_intensity", light.intensity.diffuse},
      };

      const auto rendered = device.render(
        surface.get(),
        render::render_pass{
          .batch = &batch,
          .globals = globals,
          .clear_color = core::color{0.05f, 0.06f, 0.08f, 1.0f},
          .clear_depth = 1.0f,
        }
      );
      if (!rendered) {
        exit_code = report(rendered.error());
        break;
      }
      device.present(surface.get());
    }

    pump_source.detach_window(&window);
    return exit_code;
  }

} // namespace <anonymous>

// An exception escaping an example should simply terminate it.
// NOLINTNEXTLINE(bugprone-exception-escape)
auto main(int argc, char** argv) -> int
{
  alloy::core::compiler::unused(argc);
  alloy::core::compiler::unused(argv);

  ::SDL_Init(SDL_INIT_VIDEO);

  auto exit_code = EXIT_FAILURE;
  {
    auto window = alloy::extra::sdl2_gl_window::from_window_data(
      "Alloy OpenGL Example",
      alloy::platform::window::dimensions{1280, 720},
      alloy::extra::sdl_gl_version{4, 1}
    );
    window.set_swap_interval(
      alloy::extra::sdl2_gl_window::swap_interval::synchronized
    );

    ::glewExperimental = GL_TRUE;
    if (::glewInit() == GLEW_OK) {
      exit_code = run(window);
    } else {
      std::cerr << "failed to load OpenGL functions\n";
    }
  }

  ::SDL_Quit();
  return exit_code;
}
