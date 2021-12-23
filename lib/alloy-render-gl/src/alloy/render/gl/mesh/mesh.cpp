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

#include "alloy/render/gl/mesh/mesh.hpp"
#include "alloy/render/gl/mesh/primitive_topology.hpp"

#include "alloy/render/mesh/attribute_locations.hpp"

#include <array>

auto alloy::render::gl::mesh::make(const render::mesh& m) -> mesh
{
  auto buffers = std::array<::GLuint,2>{};

  auto vao = ::GLuint{};
  const auto& vbo = buffers[0];
  const auto& ebo = buffers[1];

  ::glGenVertexArrays(1, &vao);
  ::glBindVertexArray(vao);

  ::glGenBuffers(2, buffers.data());

  // Vertices
  const auto data = m.data();
  const auto stride = m.stride().count();
  ::glBindBuffer(GL_ARRAY_BUFFER, vbo);
  ::glBufferData(GL_ARRAY_BUFFER,
    static_cast<::GLsizei>(data.size_bytes()),
    data.data(),
    GL_STATIC_DRAW
  );

  // Positions

  if (m.has_positions()) {
    const auto components = m.position_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_positions().count());
    ::glEnableVertexAttribArray(render::attribute_locations::positions);
    ::glVertexAttribPointer(
      render::attribute_locations::positions,
      static_cast<::GLint>(components),
      GL_FLOAT,
      GL_FALSE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  // Colors

  if (m.has_color()) {
    const auto components = m.color_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_colors().count());
    ::glEnableVertexAttribArray(render::attribute_locations::colors);
    ::glVertexAttribPointer(
      render::attribute_locations::colors,
      static_cast<::GLint>(components),
      GL_UNSIGNED_BYTE,
      GL_TRUE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  // UVS

  if (m.has_uvs()) {
    const auto components = m.uv_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_uvs().count());
    ::glEnableVertexAttribArray(render::attribute_locations::uvs);
    ::glVertexAttribPointer(
      render::attribute_locations::uvs,
      static_cast<::GLint>(components),
      GL_FLOAT,
      GL_FALSE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  // normals

  if (m.has_normals()) {
    const auto components = m.normal_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_normals().count());
    ::glEnableVertexAttribArray(render::attribute_locations::normals);
    ::glVertexAttribPointer(
      render::attribute_locations::normals,
      static_cast<::GLint>(components),
      GL_SHORT,
      GL_TRUE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  // tangents

  if (m.has_tangents()) {
    const auto components = m.tangent_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_tangents().count());
    ::glEnableVertexAttribArray(render::attribute_locations::tangents);
    ::glVertexAttribPointer(
      render::attribute_locations::tangents,
      static_cast<::GLint>(components),
      GL_SHORT,
      GL_TRUE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  // bitangents

  if (m.has_bitangents()) {
    const auto components = m.bitangent_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_bitangents().count());
    ::glEnableVertexAttribArray(render::attribute_locations::bitangents);
    ::glVertexAttribPointer(
      render::attribute_locations::bitangents,
      static_cast<::GLint>(components),
      GL_SHORT,
      GL_TRUE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  const auto indices = m.indices();
  const auto index_count = m.indices().size();
  ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  ::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    static_cast<::GLsizeiptr>(indices.size_bytes()),
    indices.data(),
    GL_STATIC_DRAW
  );

  const auto topology = to_opengl_type(m.topology());

  return mesh{vao, vbo, ebo, topology, index_count};
}

alloy::render::gl::mesh::mesh(::GLuint vao,
                              ::GLuint vbo,
                              ::GLuint ebo,
                              ::GLenum topology,
                              std::size_t indices)
  : m_vao{vao},
    m_vbo{vbo},
    m_ebo{ebo},
    m_topology{topology},
    m_indices{indices}
{

}

alloy::render::gl::mesh::mesh(alloy::render::gl::mesh&& other)
  : m_vao{std::exchange(other.m_vao, 0)},
    m_vbo{std::exchange(other.m_vbo, 0)},
    m_ebo{std::exchange(other.m_ebo, 0)},
    m_topology{other.m_topology},
    m_indices{other.m_indices}
{

}

alloy::render::gl::mesh::~mesh()
{
  if (m_vao != 0) {
    auto buffers = std::array<::GLuint,2u>{m_vbo, m_ebo};
    ::glDeleteBuffers(2u, buffers.data());
    ::glDeleteVertexArrays(1u, &m_vao);
  }
}


auto alloy::render::gl::mesh::draw() -> void
{
  ::glBindVertexArray(m_vao);
  ::glDrawElements(
    m_topology,
    static_cast<::GLsizei>(m_indices),
    GL_UNSIGNED_SHORT,
    nullptr
  );
}
