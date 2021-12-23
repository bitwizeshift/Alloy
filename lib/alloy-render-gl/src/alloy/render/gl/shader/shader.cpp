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

#include "alloy/render/gl/shader/shader.hpp"

//=============================================================================
// class : shader_base
//=============================================================================

//-----------------------------------------------------------------------------
// Static Factories
//-----------------------------------------------------------------------------

auto alloy::render::gl::shader_base::do_compile_from_source(shader_type type,
                                                            core::zstring_view program)
  -> core::result<::GLuint, gl_error_message>
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

auto alloy::render::gl::shader_base::do_compile_from_file(shader_type type,
                                                          io::file& file,
                                                          core::allocator alloc)
  -> core::result<::GLuint, gl_error_message>
{
  const auto size = file.bytes();

  if (size.has_error()) {
    return core::fail(gl_error_message::make("Bad file input"));
  }

  auto source = core::string{core::stl_allocator_adapter<core::string>{alloc}};
  source.resize(*size);

  file.read(io::mutable_buffer::from_data(source.data(), source.size())).expect("Failed to read shader");

  return do_compile_from_source(type, source);
}
