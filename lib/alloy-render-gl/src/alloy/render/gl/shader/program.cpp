// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/shader/program.hpp"

#include "alloy/core/utilities/result.hpp"
#include "alloy/core/utilities/scope_guard.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/render/gl/error.hpp"
#include <initializer_list>
#include <utility>

//------------------------------------------------------------------------------
// Private Static Factories
//------------------------------------------------------------------------------

// Only allocation failure can throw here, which is treated as fatal.
// NOLINTNEXTLINE(bugprone-exception-escape)
auto alloy::render::gl::program::do_link(
  std::initializer_list<::GLuint> shader_ids
) noexcept -> core::result<program,gl_error_message>
{
  const auto program_id = ::glCreateProgram();

  auto delete_program = core::scope_exit{[&]{
    ::glDeleteProgram(program_id);
  }};
  auto detach_shaders = core::scope_exit{[&]{
    for (const auto shader_id : shader_ids) {
      ::glDetachShader(program_id, shader_id);
    }
  }};

  for (const auto shader_id : shader_ids) {
    ::glAttachShader(program_id, shader_id);
  }
  ::glLinkProgram(program_id);

  auto result = ::GLint{};
	::glGetProgramiv(program_id, GL_LINK_STATUS, &result);

  // If we compiled successfully, return the program
  if (result == GL_TRUE) {
    delete_program.release();
    return program{program_id};
  }

  auto length = ::GLint{};
  ::glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &length);

  auto message = core::string{};
  message.resize(length, '\0');
  ::glGetProgramInfoLog(
    program_id,
    static_cast<::GLsizei>(message.length()),
    nullptr,
    message.data()
  );

  return core::fail(
    gl_error_message::make(std::move(message))
  );
}
