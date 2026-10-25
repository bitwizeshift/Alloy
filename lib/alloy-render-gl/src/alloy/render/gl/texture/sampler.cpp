// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/texture/sampler.hpp"

#include "alloy/render/gl/material/depth_compare.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/render/texture/sampler_state.hpp"

#include <GL/glew.h>

#include <algorithm> // std::min
#include <array>     // std::array
#include <utility>   // std::exchange

namespace alloy::render::gl {
namespace {

auto to_opengl_min_filter(texture_filter min, mipmap_filter mip)
  noexcept -> ::GLenum
{
  const auto linear = min == texture_filter::linear;

  switch (mip) {
    case mipmap_filter::none: {
      return linear ? GL_LINEAR : GL_NEAREST;
    }
    case mipmap_filter::nearest: {
      return linear ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
    }
    case mipmap_filter::linear: {
      return linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
    }
  }
  core::compiler::unreachable();
}

auto to_opengl_mag_filter(texture_filter mag) noexcept -> ::GLenum
{
  return (mag == texture_filter::linear) ? GL_LINEAR : GL_NEAREST;
}

auto to_opengl_wrap(texture_wrap wrap) noexcept -> ::GLenum
{
  switch (wrap) {
    case texture_wrap::repeat: {
      return GL_REPEAT;
    }
    case texture_wrap::mirrored_repeat: {
      return GL_MIRRORED_REPEAT;
    }
    case texture_wrap::clamp_to_edge: {
      return GL_CLAMP_TO_EDGE;
    }
    case texture_wrap::clamp_to_border: {
      return GL_CLAMP_TO_BORDER;
    }
  }
  core::compiler::unreachable();
}

auto to_opengl_color(border_color color) noexcept -> std::array<::GLfloat, 4u>
{
  switch (color) {
    case border_color::transparent_black: {
      return {0.0f, 0.0f, 0.0f, 0.0f};
    }
    case border_color::opaque_black: {
      return {0.0f, 0.0f, 0.0f, 1.0f};
    }
    case border_color::opaque_white: {
      return {1.0f, 1.0f, 1.0f, 1.0f};
    }
  }
  core::compiler::unreachable();
}

auto as_parameter(::GLenum value) noexcept -> ::GLint
{
  return static_cast<::GLint>(value);
}

} // namespace <anonymous>
} // namespace alloy::render::gl

//==============================================================================
// class : sampler
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::sampler::make(
  const sampler_state& state,
  float max_anisotropy
) noexcept -> sampler
{
  auto id = ::GLuint{};
  ::glGenSamplers(1, &id);

  ::glSamplerParameteri(
    id,
    GL_TEXTURE_MIN_FILTER,
    as_parameter(to_opengl_min_filter(state.min_filter, state.mip_filter))
  );
  ::glSamplerParameteri(
    id,
    GL_TEXTURE_MAG_FILTER,
    as_parameter(to_opengl_mag_filter(state.mag_filter))
  );
  ::glSamplerParameteri(
    id, GL_TEXTURE_WRAP_S, as_parameter(to_opengl_wrap(state.wrap_u))
  );
  ::glSamplerParameteri(
    id, GL_TEXTURE_WRAP_T, as_parameter(to_opengl_wrap(state.wrap_v))
  );
  ::glSamplerParameteri(
    id, GL_TEXTURE_WRAP_R, as_parameter(to_opengl_wrap(state.wrap_w))
  );

  const auto border = to_opengl_color(state.border);
  ::glSamplerParameterfv(id, GL_TEXTURE_BORDER_COLOR, border.data());

  if (state.compare.has_value()) {
    ::glSamplerParameteri(
      id,
      GL_TEXTURE_COMPARE_MODE,
      as_parameter(GL_COMPARE_REF_TO_TEXTURE)
    );
    ::glSamplerParameteri(
      id,
      GL_TEXTURE_COMPARE_FUNC,
      as_parameter(to_opengl_type(*state.compare))
    );
  }

  ::glSamplerParameterf(id, GL_TEXTURE_MIN_LOD, state.min_lod);
  ::glSamplerParameterf(id, GL_TEXTURE_MAX_LOD, state.max_lod);

  if (max_anisotropy > 1.0f) {
    const auto anisotropy = std::min(
      max_anisotropy,
      static_cast<float>(state.max_anisotropy)
    );
    ::glSamplerParameterf(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
  }

  return sampler{id};
}

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::render::gl::sampler::sampler(sampler&& other) noexcept
  : m_id{std::exchange(other.m_id, 0u)}
{

}

alloy::render::gl::sampler::~sampler()
{
  if (m_id != 0u) {
    ::glDeleteSamplers(1, &m_id);
  }
}

auto alloy::render::gl::sampler::operator=(sampler&& other)
  noexcept -> sampler&
{
  if (this != &other) {
    if (m_id != 0u) {
      ::glDeleteSamplers(1, &m_id);
    }
    m_id = std::exchange(other.m_id, 0u);
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::gl::sampler::id() const noexcept -> ::GLuint
{
  return m_id;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::gl::sampler::sampler(::GLuint id) noexcept
  : m_id{id}
{

}
