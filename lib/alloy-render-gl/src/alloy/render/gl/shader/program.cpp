/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "alloy/render/gl/shader/program.hpp"

#include "alloy/core/utilities/scope_guard.hpp"
#include "alloy/core/string/string.hpp"

//------------------------------------------------------------------------------
// Private Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::program::do_link(std::initializer_list<::GLuint> shader_ids)
  noexcept -> core::result<program,gl_error_message>
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
  ::glGetProgramInfoLog(program_id, message.length(), nullptr, message.data());

  return core::fail(
    gl_error_message::make(std::move(message))
  );
}
