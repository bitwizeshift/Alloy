// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/surface/framebuffer.hpp"

#include "alloy/render/gl/texture/texture.hpp"

#include "alloy/core/utilities/result.hpp"

#include "alloy/render/error.hpp"
#include "alloy/render/texture/texture_format.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

#include <utility> // std::exchange

namespace alloy::render::gl {
namespace {

auto incompatible(const char* details) -> render_error
{
  return render_error::make(render_errc::incompatible_surface, details);
}

auto depth_attachment_of(texture_format format) noexcept -> ::GLenum
{
  return (format == texture_format::depth32_float_stencil8)
    ? GL_DEPTH_STENCIL_ATTACHMENT
    : GL_DEPTH_ATTACHMENT;
}

auto same_extent(texture_extent lhs, texture_extent rhs) noexcept -> bool
{
  return lhs.width == rhs.width
    && lhs.height == rhs.height
    && lhs.depth == rhs.depth;
}

} // namespace <anonymous>
} // namespace alloy::render::gl

//==============================================================================
// class : framebuffer
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::framebuffer::make(
  const texture& color,
  const texture* depth
) -> core::result<framebuffer, render_error>
{
  if (!color.usage().test(texture_usage::render_target)) {
    return core::fail(incompatible("color texture is not a render target"));
  }
  if (depth != nullptr) {
    if (!depth->usage().test(texture_usage::depth_stencil)) {
      return core::fail(incompatible("depth texture is not a depth target"));
    }
    if (!same_extent(color.extent(), depth->extent())) {
      return core::fail(incompatible("textures differ in extent"));
    }
    if (color.samples() != depth->samples()) {
      return core::fail(incompatible("textures differ in sample count"));
    }
  }

  auto id = ::GLuint{};
  ::glGenFramebuffers(1, &id);
  ::glBindFramebuffer(GL_FRAMEBUFFER, id);

  // Constructed immediately, so that the framebuffer is deleted on failure
  auto result = framebuffer{id};

  ::glFramebufferTexture2D(
    GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0,
    color.target(),
    color.id(),
    0
  );
  if (depth != nullptr) {
    ::glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      depth_attachment_of(depth->format()),
      depth->target(),
      depth->id(),
      0
    );
  }

  const auto status = ::glCheckFramebufferStatus(GL_FRAMEBUFFER);
  ::glBindFramebuffer(GL_FRAMEBUFFER, 0u);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    return core::fail(
      incompatible("the context cannot draw into the textures together")
    );
  }
  return result;
}

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::render::gl::framebuffer::framebuffer(framebuffer&& other) noexcept
  : m_id{std::exchange(other.m_id, 0u)}
{

}

//------------------------------------------------------------------------------

alloy::render::gl::framebuffer::~framebuffer()
{
  if (m_id != 0u) {
    ::glDeleteFramebuffers(1, &m_id);
  }
}

//------------------------------------------------------------------------------

auto alloy::render::gl::framebuffer::operator=(framebuffer&& other)
  noexcept -> framebuffer&
{
  if (this != &other) {
    if (m_id != 0u) {
      ::glDeleteFramebuffers(1, &m_id);
    }
    m_id = std::exchange(other.m_id, 0u);
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::gl::framebuffer::id() const noexcept -> ::GLuint
{
  return m_id;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::gl::framebuffer::framebuffer(::GLuint id) noexcept
  : m_id{id}
{

}
