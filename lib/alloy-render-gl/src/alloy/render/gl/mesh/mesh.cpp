// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/mesh/mesh.hpp"

#include "alloy/render/gl/mesh/primitive_topology.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/render/mesh/attribute_locations.hpp"
#include "alloy/render/mesh/index_format.hpp"
#include "alloy/render/mesh/mesh.hpp"

#include <array>
#include <cstddef>
#include <utility>

namespace alloy::render::gl {
namespace {

auto to_opengl_type(index_format f) noexcept -> ::GLenum
{
  switch (f) {
    case index_format::u16: {
      return GL_UNSIGNED_SHORT;
    }
    case index_format::u32: {
      return GL_UNSIGNED_INT;
    }
  }
  core::compiler::unreachable();
}

} // namespace <anonymous>
} // namespace alloy::render::gl

auto alloy::render::gl::mesh::make(const render::mesh& m) -> mesh
{
  auto vao = ::GLuint{};
  auto vbo = ::GLuint{};
  auto ebo = ::GLuint{};

  ::glGenVertexArrays(1, &vao);
  ::glBindVertexArray(vao);

  ::glGenBuffers(1, &vbo);

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

  // weights

  if (m.has_weights()) {
    const auto components = m.weight_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_weights().count());
    ::glEnableVertexAttribArray(render::attribute_locations::weights);
    ::glVertexAttribPointer(
      render::attribute_locations::weights,
      static_cast<::GLint>(components),
      GL_UNSIGNED_SHORT,
      GL_TRUE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  // joints

  if (m.has_joints()) {
    const auto components = m.joint_components();
    const auto offset = reinterpret_cast<void*>(m.offset_to_joints().count());
    ::glEnableVertexAttribArray(render::attribute_locations::joints);
    ::glVertexAttribIPointer(
      render::attribute_locations::joints,
      static_cast<::GLint>(components),
      GL_UNSIGNED_BYTE,
      static_cast<::GLsizei>(stride),
      offset
    );
  }

  const auto topology = to_opengl_type(m.topology());

  if (!m.has_indices()) {
    return mesh{vao, vbo, ebo, topology, GL_NONE, m.vertex_count()};
  }

  const auto indices = m.index_data();
  ::glGenBuffers(1, &ebo);
  ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  ::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    static_cast<::GLsizeiptr>(indices.size_bytes()),
    indices.data(),
    GL_STATIC_DRAW
  );

  const auto index_type = to_opengl_type(m.index_format());

  return mesh{vao, vbo, ebo, topology, index_type, m.index_count()};
}

alloy::render::gl::mesh::mesh(
  ::GLuint vao,
  ::GLuint vbo,
  ::GLuint ebo,
  ::GLenum topology,
  ::GLenum index_type,
  std::size_t count
)
  : m_vao{vao},
    m_vbo{vbo},
    m_ebo{ebo},
    m_topology{topology},
    m_index_type{index_type},
    m_count{count}
{

}

alloy::render::gl::mesh::mesh(alloy::render::gl::mesh&& other)
  : m_vao{std::exchange(other.m_vao, 0)},
    m_vbo{std::exchange(other.m_vbo, 0)},
    m_ebo{std::exchange(other.m_ebo, 0)},
    m_topology{other.m_topology},
    m_index_type{other.m_index_type},
    m_count{other.m_count}
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
  if (m_ebo == 0) {
    ::glDrawArrays(m_topology, 0, static_cast<::GLsizei>(m_count));
    return;
  }
  ::glDrawElements(
    m_topology,
    static_cast<::GLsizei>(m_count),
    m_index_type,
    nullptr
  );
}
