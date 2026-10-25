// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/device.hpp"

#include "alloy/render/gl/capabilities.hpp"

#include "alloy/render/gl/mesh/mesh.hpp"
#include "alloy/render/gl/shader/program.hpp"
#include "alloy/render/gl/shader/shader.hpp"
#include "alloy/render/gl/texture/texture.hpp"
#include "shader/shader_compiler.hpp"
#include "shader/uniform_layout.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/material/material_spec.hpp"
#include "alloy/render/mesh/attribute_locations.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/model/model_spec.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/resolved_parameters.hpp"
#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

#include <algorithm> // std::equal, std::sort
#include <cstddef>   // std::size_t
#include <cstdint>   // std::uint32_t
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

} // namespace <anonymous>
} // namespace alloy::render::gl

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
// Drawing
//------------------------------------------------------------------------------

auto alloy::render::gl::device::bind_material(material_handle material)
  noexcept -> void
{
  const auto* const entry = m_materials.find(material);
  ALLOY_ASSERT(entry != nullptr, "material has been disposed");

  const auto* const program = m_programs.find(entry->program);
  ALLOY_ASSERT(program != nullptr, "material outlived its program");

  ::glUseProgram(program->program.id());

  for (const auto& buffer : entry->uniform_buffers) {
    ::glBindBufferBase(GL_UNIFORM_BUFFER, buffer.binding(), buffer.id());
  }
  for (const auto& bound : entry->textures) {
    const auto* const texture = m_textures.find(bound.texture);
    ALLOY_ASSERT(texture != nullptr, "material outlived its texture");

    ::glActiveTexture(GL_TEXTURE0 + bound.unit);
    ::glBindTexture(texture->target(), texture->id());
    ::glBindSampler(bound.unit, bound.sampler);
  }
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
    m_models{}
{

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

  auto linked = gl::program::link(shaders, spec.vertex_inputs, layout);
  if (!linked) {
    return core::fail(std::move(linked).error());
  }

  return m_programs.insert(
    program_entry{std::move(linked).value(), std::move(layout)}
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
