// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/shader/shader.hpp"

#include "alloy/render/gl/error.hpp"

#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/core/utilities/scope_guard.hpp"
#include "alloy/platform/buffers/mutable_buffer.hpp"
#include "alloy/platform/filesystem/file.hpp"

#include <utility>

//=============================================================================
// class : shader_base
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

auto alloy::render::gl::shader_base::do_compile_from_source(
  shader_type type,
  core::zstring_view program
) -> core::result<::GLuint, gl_error_message>
{
  const auto shader_id = ::glCreateShader(static_cast<::GLenum>(type));

  auto delete_shader = core::scope_exit{[&]{
    ::glDeleteShader(shader_id);
  }};

  const auto str = program.c_str();
  ::glShaderSource(shader_id, 1, &str, nullptr);
  ::glCompileShader(shader_id);

  auto result = ::GLint{};
  ::glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

  // If we compiled successfully, return
  if (result == GL_TRUE) {
    delete_shader.release();
    return shader_id;
  }

  // If we failed to compile, return the error message
  auto length = ::GLint{};
  ::glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

  auto message = core::string{};
  message.resize(length, '\0');
  ::glGetShaderInfoLog(
    shader_id,
    static_cast<::GLsizei>(message.length()),
    nullptr,
    message.data()
  );

  return core::fail(
    gl_error_message::make(std::move(message))
  );
}

auto alloy::render::gl::shader_base::do_compile_from_file(
  shader_type type,
  platform::file& file,
  core::allocator alloc
) -> core::result<::GLuint, gl_error_message>
{
  const auto size = file.bytes();

  if (size.has_error()) {
    return core::fail(gl_error_message::make("Bad file input"));
  }

  auto source = core::string{core::stl_allocator_adapter<core::string>{alloc}};
  source.resize(*size);

  file.read(platform::mutable_buffer::from_data(source.data(), source.size())).expect("Failed to read shader");

  return do_compile_from_source(type, source);
}
