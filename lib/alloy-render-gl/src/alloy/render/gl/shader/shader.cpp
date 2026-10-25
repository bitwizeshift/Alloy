// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/shader/shader.hpp"

#include "alloy/render/gl/shader/shader_stage.hpp"

#include "alloy/core/string/string.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/core/utilities/scope_guard.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/shader/shader_stage.hpp"

#include <GL/glew.h>

#include <cstddef> // std::byte
#include <span>    // std::span
#include <utility> // std::exchange, std::move

namespace alloy::render::gl {
namespace {

auto compile_failure(::GLuint shader_id) -> render_error
{
  auto length = ::GLint{};
  ::glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

  auto log = core::string{};
  log.resize(static_cast<std::size_t>(length), '\0');
  ::glGetShaderInfoLog(shader_id, length, nullptr, log.data());

  return render_error::make(render_errc::shader_compile_failed, std::move(log));
}

auto is_compiled(::GLuint shader_id) noexcept -> bool
{
  auto status = ::GLint{};
  ::glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
  return status == GL_TRUE;
}

} // namespace <anonymous>
} // namespace alloy::render::gl

//==============================================================================
// class : shader
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::shader::compile_glsl(
  shader_stage stage,
  core::string_view source
) -> core::result<shader, render_error>
{
  const auto shader_id = ::glCreateShader(to_opengl_type(stage));
  auto delete_shader = core::scope_exit{[&]{
    ::glDeleteShader(shader_id);
  }};

  const auto* const data = source.data();
  const auto length = static_cast<::GLint>(source.size());
  ::glShaderSource(shader_id, 1, &data, &length);
  ::glCompileShader(shader_id);

  if (!is_compiled(shader_id)) {
    return core::fail(compile_failure(shader_id));
  }

  delete_shader.release();
  return shader{shader_id, shader_format::glsl};
}

auto alloy::render::gl::shader::load_spirv(
  shader_stage stage,
  std::span<const std::byte> binary,
  core::string_view entry_point
) -> core::result<shader, render_error>
{
  const auto shader_id = ::glCreateShader(to_opengl_type(stage));
  auto delete_shader = core::scope_exit{[&]{
    ::glDeleteShader(shader_id);
  }};

  ::glShaderBinary(
    1,
    &shader_id,
    GL_SHADER_BINARY_FORMAT_SPIR_V,
    binary.data(),
    static_cast<::GLsizei>(binary.size())
  );

  // Contexts that only expose `ARB_gl_spirv` lack the core entry point.
  auto* const specialize = (glSpecializeShader != nullptr)
    ? glSpecializeShader
    : glSpecializeShaderARB;

  const auto entry = core::string{entry_point};
  specialize(shader_id, entry.c_str(), 0, nullptr, nullptr);

  if (!is_compiled(shader_id)) {
    return core::fail(compile_failure(shader_id));
  }

  delete_shader.release();
  return shader{shader_id, shader_format::spirv};
}

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::render::gl::shader::shader(shader&& other) noexcept
  : m_id{std::exchange(other.m_id, 0u)},
    m_format{other.m_format}
{

}

alloy::render::gl::shader::~shader()
{
  if (m_id != 0u) {
    ::glDeleteShader(m_id);
  }
}

auto alloy::render::gl::shader::operator=(shader&& other) noexcept -> shader&
{
  if (this != &other) {
    if (m_id != 0u) {
      ::glDeleteShader(m_id);
    }
    m_id = std::exchange(other.m_id, 0u);
    m_format = other.m_format;
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::gl::shader::id() const noexcept -> ::GLuint
{
  return m_id;
}

auto alloy::render::gl::shader::format() const noexcept -> shader_format
{
  return m_format;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::gl::shader::shader(::GLuint id, shader_format format) noexcept
  : m_id{id},
    m_format{format}
{

}
