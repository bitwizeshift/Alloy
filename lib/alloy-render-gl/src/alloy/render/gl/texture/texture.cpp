// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/gl/texture/texture.hpp"

#include "alloy/render/gl/capabilities.hpp"
#include "alloy/render/gl/texture/texture_format.hpp"

#include "alloy/core/utilities/result.hpp"

#include "alloy/render/error.hpp"
#include "alloy/render/texture/texture_dimension.hpp"
#include "alloy/render/texture/texture_format.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

#include <cstddef> // std::byte
#include <cstdint> // std::uint32_t
#include <utility> // std::exchange

namespace alloy::render::gl {
namespace {

/// \brief The extent of a texture mip level, as OpenGL sizes
struct opengl_extent
{
  ::GLsizei width;
  ::GLsizei height;
  ::GLsizei depth;
};

auto to_opengl_extent(texture_extent extent) noexcept -> opengl_extent
{
  return opengl_extent{
    .width = static_cast<::GLsizei>(extent.width),
    .height = static_cast<::GLsizei>(extent.height),
    .depth = static_cast<::GLsizei>(extent.depth),
  };
}

/// \brief Uploads a single mip level of the bound texture
///
/// \param data the tightly packed data of the level, or `nullptr` to only
///             allocate it
auto upload_level(
  const texture_spec& spec,
  ::GLenum target,
  std::uint32_t level,
  const std::byte* data
) noexcept -> void
{
  const auto texel = to_opengl_format(spec.format());
  const auto internal = static_cast<::GLint>(texel.internal_format);
  const auto mip = static_cast<::GLint>(level);
  const auto [width, height, depth] = to_opengl_extent(spec.mip_extent(level));
  const auto layers = static_cast<::GLsizei>(spec.array_layers());

  switch (spec.dimension()) {
    case texture_dimension::d1: {
      ::glTexImage1D(
        target, mip, internal, width, 0,
        texel.format, texel.type, data
      );
      break;
    }
    case texture_dimension::d1_array: {
      ::glTexImage2D(
        target, mip, internal, width, layers, 0,
        texel.format, texel.type, data
      );
      break;
    }
    case texture_dimension::d2: {
      ::glTexImage2D(
        target, mip, internal, width, height, 0,
        texel.format, texel.type, data
      );
      break;
    }
    case texture_dimension::d2_array:
    case texture_dimension::cube_array: {
      ::glTexImage3D(
        target, mip, internal, width, height, layers, 0,
        texel.format, texel.type, data
      );
      break;
    }
    case texture_dimension::d3: {
      ::glTexImage3D(
        target, mip, internal, width, height, depth, 0,
        texel.format, texel.type, data
      );
      break;
    }
    case texture_dimension::cube: {
      const auto face_bytes = spec.mip_size(level) / 6u;
      for (auto face = 0u; face < 6u; ++face) {
        const auto* const face_data = (data != nullptr)
          ? data + (face * face_bytes)
          : nullptr;
        ::glTexImage2D(
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
          mip, internal, width, height, 0,
          texel.format, texel.type, face_data
        );
      }
      break;
    }
  }
}

auto allocate_multisampled(const texture_spec& spec, ::GLenum target)
  noexcept -> void
{
  const auto internal = to_opengl_format(spec.format()).internal_format;
  const auto samples = static_cast<::GLsizei>(spec.samples());
  const auto [width, height, depth] = to_opengl_extent(spec.extent());

  ::glTexImage2DMultisample(target, samples, internal, width, height, GL_TRUE);
}

auto unsupported(const char* details) -> render_error
{
  return render_error::make(render_errc::unsupported_texture_format, details);
}

} // namespace <anonymous>
} // namespace alloy::render::gl

//==============================================================================
// class : texture
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::gl::texture::make(
  const texture_spec& spec,
  const capabilities& caps
) -> core::result<texture, render_error>
{
  const auto multisampled = spec.samples() != sample_count::x1;
  if (spec.dimension() == texture_dimension::cube_array
      && !caps.texture_cube_map_array) {
    return core::fail(unsupported("cube array textures are unsupported"));
  }
  if (multisampled && !caps.texture_multisample) {
    return core::fail(unsupported("multisampled textures are unsupported"));
  }

  const auto target = to_opengl_target(spec.dimension(), spec.samples());

  auto id = ::GLuint{};
  ::glGenTextures(1, &id);
  ::glBindTexture(target, id);

  // Constructed immediately, so that the texture is deleted on any failure
  auto result = texture{id, target, spec};

  if (multisampled) {
    allocate_multisampled(spec, target);
    return result;
  }

  const auto levels = spec.mip_levels();
  const auto uploaded_levels = spec.generates_mipmaps() ? 1u : levels;
  const auto initial_data = spec.initial_data();

  ::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  ::glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
  ::glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, static_cast<::GLint>(levels - 1u));

  const auto* data = initial_data.empty() ? nullptr : initial_data.data();

  for (auto level = 0u; level < levels; ++level) {
    const auto has_data = data != nullptr && level < uploaded_levels;

    upload_level(spec, target, level, has_data ? data : nullptr);
    if (has_data) {
      data += spec.mip_size(level);
    }
  }

  if (spec.generates_mipmaps()) {
    ::glGenerateMipmap(target);
  }
  return result;
}

//------------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//------------------------------------------------------------------------------

alloy::render::gl::texture::texture(texture&& other) noexcept
  : m_id{std::exchange(other.m_id, 0u)},
    m_target{other.m_target},
    m_dimension{other.m_dimension},
    m_format{other.m_format},
    m_extent{other.m_extent},
    m_samples{other.m_samples},
    m_usage{other.m_usage}
{

}

alloy::render::gl::texture::~texture()
{
  if (m_id != 0u) {
    ::glDeleteTextures(1, &m_id);
  }
}

auto alloy::render::gl::texture::operator=(texture&& other)
  noexcept -> texture&
{
  if (this != &other) {
    if (m_id != 0u) {
      ::glDeleteTextures(1, &m_id);
    }
    m_id = std::exchange(other.m_id, 0u);
    m_target = other.m_target;
    m_dimension = other.m_dimension;
    m_format = other.m_format;
    m_extent = other.m_extent;
    m_samples = other.m_samples;
    m_usage = other.m_usage;
  }
  return (*this);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::gl::texture::id() const noexcept -> ::GLuint
{
  return m_id;
}

auto alloy::render::gl::texture::target() const noexcept -> ::GLenum
{
  return m_target;
}

auto alloy::render::gl::texture::dimension() const noexcept -> texture_dimension
{
  return m_dimension;
}

auto alloy::render::gl::texture::format() const noexcept -> texture_format
{
  return m_format;
}

auto alloy::render::gl::texture::extent() const noexcept -> texture_extent
{
  return m_extent;
}

auto alloy::render::gl::texture::samples() const noexcept -> sample_count
{
  return m_samples;
}

auto alloy::render::gl::texture::usage() const noexcept -> texture_usages
{
  return m_usage;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::gl::texture::texture(
  ::GLuint id,
  ::GLenum target,
  const texture_spec& spec
) noexcept
  : m_id{id},
    m_target{target},
    m_dimension{spec.dimension()},
    m_format{spec.format()},
    m_extent{spec.extent()},
    m_samples{spec.samples()},
    m_usage{spec.usage()}
{

}
