// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/mesh/mesh.hpp"

#include "alloy/render/gl/mesh/primitive_topology.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"

#include "alloy/render/mesh/attribute_locations.hpp"
#include "alloy/render/mesh/index_format.hpp"
#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/winding_order.hpp"

#include <array>   // std::array
#include <cstddef> // std::size_t
#include <cstdint> // std::uint16_t, std::uint32_t, std::uintptr_t
#include <span>    // std::span
#include <utility> // std::exchange, std::move

namespace alloy::render::gl {
namespace {

auto to_opengl_type(winding_order w) noexcept -> ::GLenum
{
  switch (w) {
    case winding_order::counter_clockwise: {
      return GL_CCW;
    }
    case winding_order::clockwise: {
      return GL_CW;
    }
  }
  core::compiler::unreachable();
}

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

//==============================================================================
// class : mesh
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

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
    static_cast<::GLsizeiptr>(data.size_bytes()),
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
  const auto front_face = to_opengl_type(m.winding());

  const auto ranges = m.submeshes();
  auto submeshes = core::vector<submesh>{};
  submeshes.assign(ranges.begin(), ranges.end());

  auto index_type = ::GLenum{GL_NONE};
  if (m.has_indices()) {
    const auto indices = m.index_data();
    ::glGenBuffers(1, &ebo);
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
      static_cast<::GLsizeiptr>(indices.size_bytes()),
      indices.data(),
      GL_STATIC_DRAW
    );
    index_type = to_opengl_type(m.index_format());
  }
  ::glBindVertexArray(0u);

  return mesh{
    vao,
    vbo,
    ebo,
    topology,
    front_face,
    index_type,
    m.components(),
    std::move(submeshes)
  };
}

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::render::gl::mesh::mesh(mesh&& other) noexcept
  : m_vao{std::exchange(other.m_vao, 0u)},
    m_vbo{std::exchange(other.m_vbo, 0u)},
    m_ebo{std::exchange(other.m_ebo, 0u)},
    m_topology{other.m_topology},
    m_front_face{other.m_front_face},
    m_index_type{other.m_index_type},
    m_components{other.m_components},
    m_submeshes{std::move(other.m_submeshes)}
{

}

//------------------------------------------------------------------------------

alloy::render::gl::mesh::~mesh()
{
  destroy();
}

//------------------------------------------------------------------------------

auto alloy::render::gl::mesh::operator=(mesh&& other) noexcept -> mesh&
{
  if (this != &other) {
    destroy();
    m_vao = std::exchange(other.m_vao, 0u);
    m_vbo = std::exchange(other.m_vbo, 0u);
    m_ebo = std::exchange(other.m_ebo, 0u);
    m_topology = other.m_topology;
    m_front_face = other.m_front_face;
    m_index_type = other.m_index_type;
    m_components = other.m_components;
    m_submeshes = std::move(other.m_submeshes);
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::gl::mesh::components()
  const noexcept -> const vertex_components&
{
  return m_components;
}

auto alloy::render::gl::mesh::submeshes()
  const noexcept -> std::span<const submesh>
{
  return m_submeshes;
}

//------------------------------------------------------------------------------
// Drawing
//------------------------------------------------------------------------------

auto alloy::render::gl::mesh::bind() const noexcept -> void
{
  ::glBindVertexArray(m_vao);
  ::glFrontFace(m_front_face);
}

auto alloy::render::gl::mesh::draw(std::size_t index, std::uint32_t instances)
  const noexcept -> void
{
  ALLOY_ASSERT(index < m_submeshes.size());

  const auto& range = m_submeshes[index];
  if (m_ebo == 0u) {
    ::glDrawArraysInstanced(
      m_topology,
      static_cast<::GLint>(range.first),
      static_cast<::GLsizei>(range.count),
      static_cast<::GLsizei>(instances)
    );
    return;
  }

  const auto index_size = (m_index_type == GL_UNSIGNED_INT)
    ? sizeof(std::uint32_t)
    : sizeof(std::uint16_t);
  const auto offset = std::uintptr_t{range.first} * index_size;
  ::glDrawElementsInstanced(
    m_topology,
    static_cast<::GLsizei>(range.count),
    m_index_type,
    reinterpret_cast<const void*>(offset),
    static_cast<::GLsizei>(instances)
  );
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::gl::mesh::mesh(
  ::GLuint vao,
  ::GLuint vbo,
  ::GLuint ebo,
  ::GLenum topology,
  ::GLenum front_face,
  ::GLenum index_type,
  vertex_components components,
  core::vector<submesh> submeshes
) noexcept
  : m_vao{vao},
    m_vbo{vbo},
    m_ebo{ebo},
    m_topology{topology},
    m_front_face{front_face},
    m_index_type{index_type},
    m_components{components},
    m_submeshes{std::move(submeshes)}
{

}

//------------------------------------------------------------------------------
// Private Modifiers
//------------------------------------------------------------------------------

auto alloy::render::gl::mesh::destroy() noexcept -> void
{
  if (m_vao == 0u) {
    return;
  }
  const auto buffers = std::array<::GLuint, 2u>{m_vbo, m_ebo};
  ::glDeleteBuffers(2, buffers.data());
  ::glDeleteVertexArrays(1, &m_vao);
  m_vao = 0u;
}
