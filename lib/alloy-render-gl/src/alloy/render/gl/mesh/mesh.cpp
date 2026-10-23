// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/mesh/mesh.hpp"

#include "alloy/render/gl/mesh/primitive_topology.hpp"

#include "alloy/render/mesh/attribute_locations.hpp"
#include "alloy/render/mesh/mesh.hpp"

#include <array>
#include <cstddef>
#include <utility>

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

  // secondary UVs

  if (m.has_uv2()) {
    const auto components = m.uv2_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_uv2().count());
    ::glEnableVertexAttribArray(render::attribute_locations::uv2);
    ::glVertexAttribPointer(
      render::attribute_locations::uv2,
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

alloy::render::gl::mesh::mesh(
  ::GLuint vao,
  ::GLuint vbo,
  ::GLuint ebo,
  ::GLenum topology,
  std::size_t indices
)
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
