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
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/material/material_spec.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/resolved_parameters.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

#include <algorithm> // std::sort
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

/// \brief Validates the members of a single resource, if it is a uniform
///        buffer
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
    m_meshes{}
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
