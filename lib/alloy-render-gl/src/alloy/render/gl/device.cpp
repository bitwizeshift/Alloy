// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/device.hpp"

#include "alloy/render/gl/capabilities.hpp"

#include "alloy/render/gl/mesh/mesh.hpp"
#include "alloy/render/gl/shader/program.hpp"
#include "alloy/render/gl/shader/shader.hpp"
#include "alloy/render/gl/surface/framebuffer.hpp"
#include "alloy/render/gl/surface/presentation_target.hpp"
#include "alloy/render/gl/texture/texture.hpp"
#include "shader/shader_compiler.hpp"
#include "shader/uniform_layout.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/optional.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/render/device.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/material/material_spec.hpp"
#include "alloy/render/mesh/attribute_locations.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/model/model_spec.hpp"
#include "alloy/render/pass/render_batch.hpp"
#include "alloy/render/pass/render_pass.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/resolved_parameters.hpp"
#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/shader/shader_parameter.hpp"
#include "alloy/render/surface/surface_spec.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

#include <algorithm> // std::equal, std::sort
#include <cstddef>   // std::size_t
#include <cstdint>   // std::uint32_t
#include <span>      // std::span
#include <tuple>     // std::tie
#include <utility>   // std::exchange, std::move
#include <variant>   // std::visit

namespace alloy::render::gl {
namespace {

auto invalid_layout(core::string_view block, const char* what)
  -> render_error
{
  auto details = core::string{"uniform buffer '"};
  details.append(block);
  details.append("' ");
  details.append(what);
  return render_error::make(render_errc::invalid_uniform_layout, std::move(details));
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Validates the members of a single resource, if it is a uniform
///        buffer
////////////////////////////////////////////////////////////////////////////////
struct layout_visitor
{
  core::string_view name;

  auto operator()(const uniform_buffer_resource& buffer)
    const -> core::result<void, render_error>
  {
    struct extent
    {
      std::uint32_t begin;
      std::uint32_t end;
    };

    auto extents = core::vector<extent>{};
    extents.reserve(buffer.members.size());
    for (const auto& member : buffer.members) {
      const auto end = member.offset + std140_size(member.type);
      if (end > buffer.byte_size) {
        return core::fail(invalid_layout(name, "has a member that does not fit"));
      }
      extents.push_back(extent{member.offset, end});
    }

    std::sort(extents.begin(), extents.end(), [](extent lhs, extent rhs) {
      return lhs.begin < rhs.begin;
    });
    for (auto i = std::size_t{1u}; i < extents.size(); ++i) {
      if (extents[i].begin < extents[i - 1u].end) {
        return core::fail(invalid_layout(name, "has overlapping members"));
      }
    }
    return {};
  }

  auto operator()(const sampled_texture_resource&)
    const -> core::result<void, render_error>
  {
    return {};
  }
};

/// \brief Validates that the members of every uniform buffer in \p spec fit
///        within their std140 buffer without overlapping
auto validate_uniform_layout(const program_spec& spec)
  -> core::result<void, render_error>
{
  for (const auto& resource : spec.resources) {
    auto result = std::visit(layout_visitor{resource.name}, resource.kind);
    if (!result) {
      return result;
    }
  }
  return {};
}

auto stale(const char* what) -> render_error
{
  return render_error::make(render_errc::stale_handle, what);
}

auto described(render_errc code, core::string_view subject, const char* what)
  -> render_error
{
  auto details = core::string{"'"};
  details.append(subject);
  details.append("' ");
  details.append(what);
  return render_error::make(code, std::move(details));
}

/// \brief Queries whether \p components provide the attribute that is read
///        from \p location
auto provides(const vertex_components& components, std::uint32_t location)
  noexcept -> bool
{
  switch (location) {
    case attribute_locations::positions: {
      return components.position != 0u;
    }
    case attribute_locations::colors: {
      return components.color != 0u;
    }
    case attribute_locations::uvs: {
      return components.uv != 0u;
    }
    case attribute_locations::normals: {
      return components.normal != 0u;
    }
    case attribute_locations::tangents: {
      return components.tangent != 0u;
    }
    case attribute_locations::uv2: {
      return components.uv2 != 0u;
    }
    case attribute_locations::joints: {
      return components.joints != 0u;
    }
    case attribute_locations::weights: {
      return components.weights != 0u;
    }
    default: {
      return false;
    }
  }
}

/// \brief Gets the number of consecutive attribute locations that an input
///        of type \p type occupies
auto location_count(shader_data_type type) noexcept -> std::uint32_t
{
  switch (type) {
    case shader_data_type::float2x2: {
      return 2u;
    }
    case shader_data_type::float3x3: {
      return 3u;
    }
    case shader_data_type::float4x4: {
      return 4u;
    }
    case shader_data_type::boolean:
    case shader_data_type::int1:
    case shader_data_type::int2:
    case shader_data_type::int3:
    case shader_data_type::int4:
    case shader_data_type::uint1:
    case shader_data_type::uint2:
    case shader_data_type::uint3:
    case shader_data_type::uint4:
    case shader_data_type::float1:
    case shader_data_type::float2:
    case shader_data_type::float3:
    case shader_data_type::float4: {
      return 1u;
    }
  }
  core::compiler::unreachable();
}

/// \brief Checks that the mesh described by \p components provides every
///        vertex input of \p layout
auto check_vertex_inputs(
  const program_layout& layout,
  const vertex_components& components
) -> core::result<void, render_error>
{
  for (const auto& input : layout.vertex_inputs()) {
    if (!provides(components, input.location)) {
      return core::fail(
        described(
          render_errc::missing_vertex_input,
          input.name,
          "is not provided by the mesh"
        )
      );
    }
  }
  return {};
}

/// \brief Checks that \p layout declares the same instance inputs as
///        \p expected
auto check_same_instance_inputs(
  const program_layout& layout,
  const program_layout& expected
) -> core::result<void, render_error>
{
  const auto lhs = layout.instance_inputs();
  const auto rhs = expected.instance_inputs();
  const auto same = std::equal(
    lhs.begin(),
    lhs.end(),
    rhs.begin(),
    rhs.end(),
    [](const instance_input_layout& a, const instance_input_layout& b) {
      return a.location == b.location && a.type == b.type;
    }
  );
  if (!same) {
    return core::fail(
      render_error::make(
        render_errc::instance_layout_mismatch,
        "the programs of a model declare different instance inputs"
      )
    );
  }
  return {};
}

/// \brief Checks that every instance input of \p layout can be read per
///        instance without overlapping an attribute of the mesh described by
///        \p components
auto check_instance_inputs(
  const program_layout& layout,
  const vertex_components& components
) -> core::result<void, render_error>
{
  for (const auto& input : layout.instance_inputs()) {
    if (input.type == shader_data_type::boolean) {
      return core::fail(
        described(
          render_errc::instance_layout_mismatch,
          input.name,
          "is a boolean, which cannot be read per instance"
        )
      );
    }
    const auto first = input.location;
    const auto last = first + location_count(input.type);
    for (auto location = first; location < last; ++location) {
      if (provides(components, location)) {
        return core::fail(
          described(
            render_errc::instance_layout_mismatch,
            input.name,
            "is read from the location of a vertex attribute of the mesh"
          )
        );
      }
    }
  }
  return {};
}

/// \brief Gets the size of a uniform buffer resource, or null for any other
///        kind of resource
struct uniform_buffer_size_visitor
{
  auto operator()(const uniform_buffer_layout& buffer)
    const noexcept -> core::optional<std::uint32_t>
  {
    return buffer.byte_size;
  }

  auto operator()(const sampled_texture_resource&)
    const noexcept -> core::optional<std::uint32_t>
  {
    return core::null;
  }
};

/// \brief How an instance input is read from the instance buffer, one
///        attribute location at a time
struct attribute_format
{
  /// The number of consecutive locations, which is the number of columns of
  /// a matrix
  std::uint32_t columns;

  /// The number of components read from each location
  ::GLint components;

  /// The type of each component
  ::GLenum type;

  /// Whether components are read as integers rather than floats
  bool integral;
};

auto attribute_format_of(shader_data_type type) noexcept -> attribute_format
{
  switch (type) {
    case shader_data_type::int1: {
      return attribute_format{1u, 1, GL_INT, true};
    }
    case shader_data_type::int2: {
      return attribute_format{1u, 2, GL_INT, true};
    }
    case shader_data_type::int3: {
      return attribute_format{1u, 3, GL_INT, true};
    }
    case shader_data_type::int4: {
      return attribute_format{1u, 4, GL_INT, true};
    }
    case shader_data_type::uint1: {
      return attribute_format{1u, 1, GL_UNSIGNED_INT, true};
    }
    case shader_data_type::uint2: {
      return attribute_format{1u, 2, GL_UNSIGNED_INT, true};
    }
    case shader_data_type::uint3: {
      return attribute_format{1u, 3, GL_UNSIGNED_INT, true};
    }
    case shader_data_type::uint4: {
      return attribute_format{1u, 4, GL_UNSIGNED_INT, true};
    }
    case shader_data_type::float1: {
      return attribute_format{1u, 1, GL_FLOAT, false};
    }
    case shader_data_type::float2: {
      return attribute_format{1u, 2, GL_FLOAT, false};
    }
    case shader_data_type::float3: {
      return attribute_format{1u, 3, GL_FLOAT, false};
    }
    case shader_data_type::float4: {
      return attribute_format{1u, 4, GL_FLOAT, false};
    }
    case shader_data_type::float2x2: {
      return attribute_format{2u, 2, GL_FLOAT, false};
    }
    case shader_data_type::float3x3: {
      return attribute_format{3u, 3, GL_FLOAT, false};
    }
    case shader_data_type::float4x4: {
      return attribute_format{4u, 4, GL_FLOAT, false};
    }
    case shader_data_type::boolean: {
      // Models reject boolean instance inputs when they are created
      break;
    }
  }
  core::compiler::unreachable();
}

} // namespace <anonymous>
} // namespace alloy::render::gl

//==============================================================================
// struct : device::surface_binder
//==============================================================================

struct alloy::render::gl::device::surface_binder
{
  auto operator()(const window_surface& surface) const -> bound_surface
  {
    ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
    return bound_surface{
      .extent = surface.target->drawable_extent(),
      .has_depth = true,
    };
  }

  auto operator()(const texture_surface& surface) const -> bound_surface
  {
    ::glBindFramebuffer(GL_FRAMEBUFFER, surface.framebuffer.id());
    return bound_surface{
      .extent = surface.extent,
      .has_depth = surface.has_depth,
    };
  }
};

//==============================================================================
// struct : device::surface_presenter
//==============================================================================

struct alloy::render::gl::device::surface_presenter
{
  auto operator()(const window_surface& surface) const -> void
  {
    surface.target->present();
  }

  auto operator()(const texture_surface&) const -> void
  {
    // Drawing into textures leaves nothing to present
  }
};

//==============================================================================
// class : device
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::device::from_current_context() noexcept -> device
{
  return device{capabilities::from_current_context()};
}

//------------------------------------------------------------------------------
// Resource Creation
//------------------------------------------------------------------------------

auto alloy::render::gl::device::create_surface(presentation_target& target)
  -> unique_surface
{
  return adopt(
    m_surfaces.insert(surface_entry{window_surface{&target}})
  );
}

//------------------------------------------------------------------------------
// Drawing
//------------------------------------------------------------------------------

auto alloy::render::gl::device::render(
  surface_handle surface,
  const render_pass& pass
) -> core::result<void, render_error>
{
  const auto* const target = m_surfaces.find(surface);
  ALLOY_ASSERT(target != nullptr, "surface has been disposed");

  const auto bound = std::visit(surface_binder{}, *target);
  ::glViewport(
    0,
    0,
    static_cast<::GLsizei>(bound.extent.width),
    static_cast<::GLsizei>(bound.extent.height)
  );

  auto clear_mask = ::GLbitfield{0u};
  if (pass.clear_color.has_value()) {
    const auto& color = *pass.clear_color;
    ::glClearColor(color.r(), color.g(), color.b(), color.a());
    clear_mask |= GL_COLOR_BUFFER_BIT;
  }
  if (pass.clear_depth.has_value() && bound.has_depth) {
    ::glDepthMask(GL_TRUE);
    ::glClearDepth(static_cast<::GLdouble>(*pass.clear_depth));
    clear_mask |= GL_DEPTH_BUFFER_BIT;
  }
  if (clear_mask != 0u) {
    ::glClear(clear_mask);
  }

  sort_draws(*pass.batch);
  if (m_draws.empty()) {
    return {};
  }
  if (auto uploaded = upload_globals(pass.globals); !uploaded) {
    return uploaded;
  }
  m_instances.upload(pass.batch->instance_data());

  if (bound.has_depth) {
    ::glEnable(GL_DEPTH_TEST);
    ::glDepthFunc(GL_LESS);
    ::glDepthMask(GL_TRUE);
  } else {
    ::glDisable(GL_DEPTH_TEST);
  }
  ::glEnable(GL_CULL_FACE);
  ::glCullFace(GL_BACK);

  const auto* program = static_cast<const program_entry*>(nullptr);
  const auto* material = static_cast<const material_entry*>(nullptr);
  const auto* mesh = static_cast<const gl::mesh*>(nullptr);
  auto current = draw_item{m_draws.front()};
  for (const auto& draw : m_draws) {
    if (program == nullptr || draw.program != current.program) {
      program = m_programs.find(draw.program);
      ALLOY_ASSERT(program != nullptr, "model outlived its program");
      bind_program(*program);
      material = nullptr;
    }
    if (material == nullptr || draw.material != current.material) {
      material = m_materials.find(draw.material);
      ALLOY_ASSERT(material != nullptr, "model outlived its material");
      bind_material(*material);
    }
    if (mesh == nullptr || draw.mesh != current.mesh) {
      mesh = m_meshes.find(draw.mesh);
      ALLOY_ASSERT(mesh != nullptr, "model outlived its mesh");
      mesh->bind();
    }
    current = draw;

    bind_instances(*draw.model, *draw.entry);
    mesh->draw(draw.submesh, draw.entry->instance_count);
  }
  ::glBindVertexArray(0u);
  return {};
}

auto alloy::render::gl::device::present(surface_handle surface) -> void
{
  const auto* const target = m_surfaces.find(surface);
  ALLOY_ASSERT(target != nullptr, "surface has been disposed");

  std::visit(surface_presenter{}, *target);
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::gl::device::device(const capabilities& caps) noexcept
  : m_capabilities{caps},
    m_samplers{caps.max_anisotropy},
    m_programs{},
    m_textures{},
    m_materials{},
    m_meshes{},
    m_models{},
    m_surfaces{},
    m_instances{},
    m_draws{}
{

}

//------------------------------------------------------------------------------
// Private Drawing
//------------------------------------------------------------------------------

auto alloy::render::gl::device::sort_draws(const render_batch& batch) -> void
{
  m_draws.clear();
  for (const auto& entry : batch.entries()) {
    const auto* const model = m_models.find(entry.model);
    ALLOY_ASSERT(model != nullptr, "model has been disposed");

    for (const auto& part : model->parts) {
      m_draws.push_back(
        draw_item{
          .program = part.program,
          .material = part.material,
          .mesh = model->mesh,
          .submesh = part.submesh,
          .model = model,
          .entry = &entry,
        }
      );
    }
  }

  const auto by_state = [](const draw_item& lhs, const draw_item& rhs) {
    return std::tie(lhs.program, lhs.material, lhs.mesh)
      < std::tie(rhs.program, rhs.material, rhs.mesh);
  };
  std::sort(m_draws.begin(), m_draws.end(), by_state);
}

auto alloy::render::gl::device::upload_globals(
  std::span<const shader_parameter> globals
) -> core::result<void, render_error>
{
  auto previous = program_handle{};
  for (const auto& draw : m_draws) {
    if (draw.program == previous) {
      continue;
    }
    previous = draw.program;

    const auto* const program = m_programs.find(draw.program);
    ALLOY_ASSERT(program != nullptr, "model outlived its program");

    auto resolved = resolved_parameters::from_parameters(
      program->layout,
      resource_scope::global,
      globals
    );
    if (!resolved) {
      return core::fail(std::move(resolved).error());
    }

    for (const auto& block : resolved->uniform_blocks()) {
      const auto buffer = std::find_if(
        program->global_buffers.begin(),
        program->global_buffers.end(),
        [&](const uniform_buffer& buffer) {
          return buffer.binding() == block.binding.index;
        }
      );
      ALLOY_ASSERT(buffer != program->global_buffers.end());

      const auto bytes = pack_uniform_block(block);
      ::glBindBuffer(GL_UNIFORM_BUFFER, buffer->id());
      ::glBufferData(
        GL_UNIFORM_BUFFER,
        static_cast<::GLsizeiptr>(bytes.size()),
        bytes.data(),
        GL_DYNAMIC_DRAW
      );
    }
  }
  ::glBindBuffer(GL_UNIFORM_BUFFER, 0u);
  return {};
}

auto alloy::render::gl::device::bind_program(const program_entry& program)
  const noexcept -> void
{
  ::glUseProgram(program.program.id());
  for (const auto& buffer : program.global_buffers) {
    ::glBindBufferBase(GL_UNIFORM_BUFFER, buffer.binding(), buffer.id());
  }
}

auto alloy::render::gl::device::bind_material(const material_entry& material)
  const noexcept -> void
{
  for (const auto& buffer : material.uniform_buffers) {
    ::glBindBufferBase(GL_UNIFORM_BUFFER, buffer.binding(), buffer.id());
  }
  for (const auto& bound : material.textures) {
    const auto* const texture = m_textures.find(bound.texture);
    ALLOY_ASSERT(texture != nullptr, "material outlived its texture");

    ::glActiveTexture(GL_TEXTURE0 + bound.unit);
    ::glBindTexture(texture->target(), texture->id());
    ::glBindSampler(bound.unit, bound.sampler);
  }
}

auto alloy::render::gl::device::bind_instances(
  const model_entry& model,
  const batch_entry& entry
) const noexcept -> void
{
  if (model.instance_attributes.empty()) {
    return;
  }

  ::glBindBuffer(GL_ARRAY_BUFFER, m_instances.id());
  const auto stride = static_cast<::GLsizei>(model.instance_stride);
  for (const auto& attribute : model.instance_attributes) {
    const auto format = attribute_format_of(attribute.type);
    const auto column_size = static_cast<std::size_t>(format.components) * 4u;

    for (auto column = 0u; column < format.columns; ++column) {
      const auto location = attribute.location + column;
      const auto offset = entry.offset
        + attribute.offset
        + (column * column_size);
      const auto* const pointer = reinterpret_cast<const void*>(offset);

      ::glEnableVertexAttribArray(location);
      if (format.integral) {
        ::glVertexAttribIPointer(
          location,
          format.components,
          format.type,
          stride,
          pointer
        );
      } else {
        ::glVertexAttribPointer(
          location,
          format.components,
          format.type,
          GL_FALSE,
          stride,
          pointer
        );
      }
      ::glVertexAttribDivisor(location, 1u);
    }
  }
}

//------------------------------------------------------------------------------
// Virtual Hooks
//------------------------------------------------------------------------------

auto alloy::render::gl::device::do_create_program(
  const program_spec& spec,
  program_layout layout
) -> core::result<program_handle, render_error>
{
  if (auto result = validate_uniform_layout(spec); !result) {
    return core::fail(std::move(result).error());
  }

  auto shaders = core::vector<shader>{};
  shaders.reserve(spec.stages.size());

  for (const auto& stage : spec.stages) {
    auto compiled = compile_stage(stage, m_capabilities);
    if (!compiled) {
      return core::fail(std::move(compiled).error());
    }
    shaders.push_back(std::move(compiled).value());
  }

  auto linked = gl::program::link(shaders, layout);
  if (!linked) {
    return core::fail(std::move(linked).error());
  }

  auto global_buffers = core::vector<uniform_buffer>{};
  for (const auto& resource : layout.resources()) {
    if (resource.scope != resource_scope::global) {
      continue;
    }
    const auto size = std::visit(
      uniform_buffer_size_visitor{},
      resource.kind
    );
    if (!size.has_value()) {
      continue;
    }

    auto id = ::GLuint{};
    ::glGenBuffers(1, &id);
    global_buffers.emplace_back(resource.binding.index, id);

    ::glBindBuffer(GL_UNIFORM_BUFFER, id);
    ::glBufferData(
      GL_UNIFORM_BUFFER,
      static_cast<::GLsizeiptr>(*size),
      nullptr,
      GL_DYNAMIC_DRAW
    );
  }
  ::glBindBuffer(GL_UNIFORM_BUFFER, 0u);

  return m_programs.insert(
    program_entry{
      .program = std::move(linked).value(),
      .layout = std::move(layout),
      .global_buffers = std::move(global_buffers),
    }
  );
}

auto alloy::render::gl::device::do_create_texture(const texture_spec& spec)
  -> core::result<texture_handle, render_error>
{
  auto created = texture::make(spec, m_capabilities);
  if (!created) {
    return core::fail(std::move(created).error());
  }
  return m_textures.insert(std::move(created).value());
}

auto alloy::render::gl::device::do_create_material(const material_spec& spec)
  -> core::result<material_handle, render_error>
{
  const auto* const program = m_programs.find(spec.program);
  if (program == nullptr) {
    return core::fail(
      render_error::make(render_errc::stale_handle, "program has been disposed")
    );
  }

  auto resolved = resolved_parameters::from_parameters(
    program->layout,
    resource_scope::material,
    spec.parameters
  );
  if (!resolved) {
    return core::fail(std::move(resolved).error());
  }

  auto entry = material_entry{
    .program = spec.program,
    .uniform_buffers = {},
    .textures = {},
  };

  entry.textures.reserve(resolved->textures().size());
  for (const auto& slot : resolved->textures()) {
    const auto* const texture = m_textures.find(slot.texture);
    if (texture == nullptr) {
      return core::fail(
        render_error::make(render_errc::stale_handle, "texture has been disposed")
      );
    }
    if (texture->dimension() != slot.dimension) {
      return core::fail(
        render_error::make(render_errc::dimension_mismatch)
      );
    }
    entry.textures.push_back(
      bound_texture{
        .unit = slot.binding.index,
        .texture = slot.texture,
        .sampler = m_samplers.find_or_create(slot.sampler),
      }
    );
  }

  entry.uniform_buffers.reserve(resolved->uniform_blocks().size());
  for (const auto& block : resolved->uniform_blocks()) {
    const auto bytes = pack_uniform_block(block);

    auto id = ::GLuint{};
    ::glGenBuffers(1, &id);
    entry.uniform_buffers.emplace_back(block.binding.index, id);

    ::glBindBuffer(GL_UNIFORM_BUFFER, id);
    ::glBufferData(
      GL_UNIFORM_BUFFER,
      static_cast<::GLsizeiptr>(bytes.size()),
      bytes.data(),
      GL_STATIC_DRAW
    );
  }
  ::glBindBuffer(GL_UNIFORM_BUFFER, 0u);

  return m_materials.insert(std::move(entry));
}

auto alloy::render::gl::device::do_create_mesh(const render::mesh& mesh)
  -> core::result<mesh_handle, render_error>
{
  return m_meshes.insert(gl::mesh::make(mesh));
}

auto alloy::render::gl::device::do_create_model(const model_spec& spec)
  -> core::result<model_handle, render_error>
{
  const auto* const mesh = m_meshes.find(spec.mesh);
  if (mesh == nullptr) {
    return core::fail(stale("mesh has been disposed"));
  }
  if (spec.materials.size() != mesh->submeshes().size()) {
    return core::fail(render_error::make(render_errc::submesh_count_mismatch));
  }

  auto entry = model_entry{
    .mesh = spec.mesh,
    .parts = {},
    .instance_attributes = {},
    .instance_stride = 0u,
  };
  entry.parts.reserve(spec.materials.size());

  const auto& components = mesh->components();
  // Every part draws from the model's single instance buffer, so all programs
  // must declare identical instance inputs. The first layout is the reference
  // the others are checked against, which lets it alone describe the model's
  // instance attributes once the loop completes.
  auto first_layout = static_cast<const program_layout*>(nullptr);
  for (auto i = std::size_t{0u}; i < spec.materials.size(); ++i) {
    const auto* const material = m_materials.find(spec.materials[i]);
    if (material == nullptr) {
      return core::fail(stale("material has been disposed"));
    }
    const auto* const program = m_programs.find(material->program);
    if (program == nullptr) {
      return core::fail(stale("program has been disposed"));
    }

    const auto& layout = program->layout;
    if (first_layout == nullptr) {
      first_layout = &layout;
    }
    if (auto checked = check_vertex_inputs(layout, components); !checked) {
      return core::fail(std::move(checked).error());
    }
    auto checked = check_same_instance_inputs(layout, *first_layout);
    if (!checked) {
      return core::fail(std::move(checked).error());
    }

    entry.parts.push_back(
      model_part{
        .program = material->program,
        .material = spec.materials[i],
        .submesh = static_cast<std::uint32_t>(i),
      }
    );
  }

  if (first_layout != nullptr) {
    const auto& layout = *first_layout;
    if (auto checked = check_instance_inputs(layout, components); !checked) {
      return core::fail(std::move(checked).error());
    }

    entry.instance_attributes.reserve(layout.instance_inputs().size());
    for (const auto& input : layout.instance_inputs()) {
      entry.instance_attributes.push_back(
        instance_attribute{
          .location = input.location,
          .type = input.type,
          .offset = static_cast<std::uint32_t>(input.offset.count()),
        }
      );
    }
    entry.instance_stride = static_cast<std::uint32_t>(
      layout.instance_stride().count()
    );
  }

  return m_models.insert(std::move(entry));
}

auto alloy::render::gl::device::do_create_surface(const surface_spec& spec)
  -> core::result<surface_handle, render_error>
{
  const auto* const color = m_textures.find(spec.color);
  if (color == nullptr) {
    return core::fail(stale("color texture has been disposed"));
  }
  const auto* depth = static_cast<const texture*>(nullptr);
  if (!spec.depth.is_null()) {
    depth = m_textures.find(spec.depth);
    if (depth == nullptr) {
      return core::fail(stale("depth texture has been disposed"));
    }
  }

  auto created = gl::framebuffer::make(*color, depth);
  if (!created) {
    return core::fail(std::move(created).error());
  }

  const auto extent = color->extent();
  return m_surfaces.insert(
    surface_entry{
      texture_surface{
        .framebuffer = std::move(created).value(),
        .extent = surface_extent{extent.width, extent.height},
        .has_depth = depth != nullptr,
      }
    }
  );
}

auto alloy::render::gl::device::do_dispose(program_handle handle)
  noexcept -> void
{
  m_programs.erase(handle);
}

auto alloy::render::gl::device::do_dispose(texture_handle handle)
  noexcept -> void
{
  m_textures.erase(handle);
}

auto alloy::render::gl::device::do_dispose(material_handle handle)
  noexcept -> void
{
  m_materials.erase(handle);
}

auto alloy::render::gl::device::do_dispose(mesh_handle handle)
  noexcept -> void
{
  m_meshes.erase(handle);
}

auto alloy::render::gl::device::do_dispose(model_handle handle)
  noexcept -> void
{
  m_models.erase(handle);
}

auto alloy::render::gl::device::do_dispose(surface_handle handle)
  noexcept -> void
{
  m_surfaces.erase(handle);
}

//==============================================================================
// class : device::uniform_buffer
//==============================================================================

alloy::render::gl::device::uniform_buffer::uniform_buffer(
  std::uint32_t binding,
  ::GLuint id
) noexcept
  : m_binding{binding},
    m_id{id}
{

}

alloy::render::gl::device::uniform_buffer::uniform_buffer(
  uniform_buffer&& other
) noexcept
  : m_binding{other.m_binding},
    m_id{std::exchange(other.m_id, 0u)}
{

}

alloy::render::gl::device::uniform_buffer::~uniform_buffer()
{
  if (m_id != 0u) {
    ::glDeleteBuffers(1, &m_id);
  }
}

auto alloy::render::gl::device::uniform_buffer::binding()
  const noexcept -> std::uint32_t
{
  return m_binding;
}

auto alloy::render::gl::device::uniform_buffer::id()
  const noexcept -> ::GLuint
{
  return m_id;
}

//==============================================================================
// class : device::instance_buffer
//==============================================================================

alloy::render::gl::device::instance_buffer::instance_buffer() noexcept
  : m_id{0u}
{
  ::glGenBuffers(1, &m_id);
}

alloy::render::gl::device::instance_buffer::~instance_buffer()
{
  ::glDeleteBuffers(1, &m_id);
}

auto alloy::render::gl::device::instance_buffer::upload(
  std::span<const std::byte> data
) noexcept -> void
{
  // Respecifying the whole store lets the driver hand out fresh memory
  // rather than wait for draws that still read the previous batch
  ::glBindBuffer(GL_ARRAY_BUFFER, m_id);
  ::glBufferData(
    GL_ARRAY_BUFFER,
    static_cast<::GLsizeiptr>(data.size()),
    data.data(),
    GL_STREAM_DRAW
  );
}

auto alloy::render::gl::device::instance_buffer::id()
  const noexcept -> ::GLuint
{
  return m_id;
}
