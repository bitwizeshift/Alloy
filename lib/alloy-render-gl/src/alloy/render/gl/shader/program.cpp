// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/shader/program.hpp"

#include "alloy/render/gl/shader/shader.hpp"

#include "alloy/core/string/string.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/core/utilities/scope_guard.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"

#include <GL/glew.h>

#include <algorithm> // std::all_of
#include <cstddef>   // std::size_t
#include <span>      // std::span
#include <utility>   // std::exchange, std::move
#include <variant>   // std::visit

namespace alloy::render::gl {
namespace {

auto link_failure(::GLuint program_id) -> render_error
{
  auto length = ::GLint{};
  ::glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

  auto log = core::string{};
  log.resize(static_cast<std::size_t>(length), '\0');
  ::glGetProgramInfoLog(program_id, length, nullptr, log.data());

  return render_error::make(render_errc::program_link_failed, std::move(log));
}

/// \brief Assigns the binding of a single resource to a linked GLSL program
///
/// Resources that the linker optimized away have no location, and are
/// skipped.
struct binding_visitor
{
  ::GLuint program_id;
  const resource_layout* resource;

  auto operator()(const uniform_buffer_layout&) const noexcept -> void
  {
    const auto index = ::glGetUniformBlockIndex(
      program_id,
      resource->name.c_str()
    );
    if (index != GL_INVALID_INDEX) {
      ::glUniformBlockBinding(program_id, index, resource->binding.index);
    }
  }

  auto operator()(const sampled_texture_resource&) const noexcept -> void
  {
    const auto location = ::glGetUniformLocation(
      program_id,
      resource->name.c_str()
    );
    if (location != -1) {
      ::glProgramUniform1i(
        program_id,
        location,
        static_cast<::GLint>(resource->binding.index)
      );
    }
  }
};

} // namespace <anonymous>
} // namespace alloy::render::gl

//==============================================================================
// class : program
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::program::link(
  std::span<const shader> shaders,
  std::span<const vertex_input> vertex_inputs,
  const program_layout& layout
) -> core::result<program, render_error>
{
  const auto is_glsl = [](const shader& s) {
    return s.format() == shader_format::glsl;
  };
  const auto all_glsl = std::all_of(shaders.begin(), shaders.end(), is_glsl);
  const auto any_glsl = std::any_of(shaders.begin(), shaders.end(), is_glsl);
  if (any_glsl && !all_glsl) {
    return core::fail(
      render_error::make(
        render_errc::program_link_failed,
        "GLSL and SPIR-V shaders cannot be linked together"
      )
    );
  }

  const auto program_id = ::glCreateProgram();
  auto delete_program = core::scope_exit{[&]{
    ::glDeleteProgram(program_id);
  }};

  if (all_glsl) {
    for (const auto& input : vertex_inputs) {
      const auto name = core::string{input.name};
      ::glBindAttribLocation(program_id, input.location, name.c_str());
    }
  }

  for (const auto& s : shaders) {
    ::glAttachShader(program_id, s.id());
  }
  ::glLinkProgram(program_id);
  for (const auto& s : shaders) {
    ::glDetachShader(program_id, s.id());
  }

  auto status = ::GLint{};
  ::glGetProgramiv(program_id, GL_LINK_STATUS, &status);
  if (status != GL_TRUE) {
    return core::fail(link_failure(program_id));
  }

  if (all_glsl) {
    for (const auto& resource : layout.resources()) {
      std::visit(binding_visitor{program_id, &resource}, resource.kind);
    }
  }

  delete_program.release();
  return program{program_id};
}

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::render::gl::program::program(program&& other) noexcept
  : m_id{std::exchange(other.m_id, 0u)}
{

}

alloy::render::gl::program::~program()
{
  if (m_id != 0u) {
    ::glDeleteProgram(m_id);
  }
}

auto alloy::render::gl::program::operator=(program&& other)
  noexcept -> program&
{
  if (this != &other) {
    if (m_id != 0u) {
      ::glDeleteProgram(m_id);
    }
    m_id = std::exchange(other.m_id, 0u);
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::gl::program::id() const noexcept -> ::GLuint
{
  return m_id;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::gl::program::program(::GLuint id) noexcept
  : m_id{id}
{

}
