// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file texture_format.hpp
///
/// \brief This header defines the conversion of texture formats and
///        dimensions into their OpenGL equivalents
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_TEXTURE_TEXTURE_FORMAT_HPP
#define ALLOY_RENDER_GL_TEXTURE_TEXTURE_FORMAT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/render/texture/texture_dimension.hpp"
#include "alloy/render/texture/texture_format.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The OpenGL description of a texel format
  //////////////////////////////////////////////////////////////////////////////
  struct opengl_texel_format
  {
    /// The format that the texture stores its texels in
    ::GLenum internal_format;

    /// The channels of the texel data that is uploaded
    ::GLenum format;

    /// The type of each channel of the texel data that is uploaded
    ::GLenum type;
  };

  /// \brief Converts \p f into its OpenGL description
  ///
  /// \param f the texture format
  /// \return the OpenGL texel format
  constexpr auto to_opengl_format(texture_format f)
    noexcept -> opengl_texel_format;

  /// \brief Converts \p d into the OpenGL texture target it is bound to
  ///
  /// \param d the texture dimension
  /// \param samples the number of samples per texel
  /// \return the OpenGL texture target
  constexpr auto to_opengl_target(texture_dimension d, sample_count samples)
    noexcept -> ::GLenum;

} // namespace alloy::render::gl

inline constexpr
auto alloy::render::gl::to_opengl_format(texture_format f)
  noexcept -> opengl_texel_format
{
  switch (f) {
    case texture_format::r8_unorm: {
      return {GL_R8, GL_RED, GL_UNSIGNED_BYTE};
    }
    case texture_format::rg8_unorm: {
      return {GL_RG8, GL_RG, GL_UNSIGNED_BYTE};
    }
    case texture_format::rgba8_unorm: {
      return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
    }
    case texture_format::rgba8_srgb: {
      return {GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE};
    }
    case texture_format::bgra8_unorm: {
      return {GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE};
    }
    case texture_format::bgra8_srgb: {
      return {GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE};
    }
    case texture_format::r16_float: {
      return {GL_R16F, GL_RED, GL_HALF_FLOAT};
    }
    case texture_format::rg16_float: {
      return {GL_RG16F, GL_RG, GL_HALF_FLOAT};
    }
    case texture_format::rgba16_float: {
      return {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT};
    }
    case texture_format::r32_float: {
      return {GL_R32F, GL_RED, GL_FLOAT};
    }
    case texture_format::rg32_float: {
      return {GL_RG32F, GL_RG, GL_FLOAT};
    }
    case texture_format::rgba32_float: {
      return {GL_RGBA32F, GL_RGBA, GL_FLOAT};
    }
    case texture_format::r32_uint: {
      return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT};
    }
    case texture_format::depth16_unorm: {
      return {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT};
    }
    case texture_format::depth32_float: {
      return {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT};
    }
    case texture_format::depth32_float_stencil8: {
      return {
        GL_DEPTH32F_STENCIL8,
        GL_DEPTH_STENCIL,
        GL_FLOAT_32_UNSIGNED_INT_24_8_REV
      };
    }
  }
  core::compiler::unreachable();
}

inline constexpr
auto alloy::render::gl::to_opengl_target(
  texture_dimension d,
  sample_count samples
) noexcept -> ::GLenum
{
  const auto multisampled = samples != sample_count::x1;

  switch (d) {
    case texture_dimension::d1: {
      return GL_TEXTURE_1D;
    }
    case texture_dimension::d1_array: {
      return GL_TEXTURE_1D_ARRAY;
    }
    case texture_dimension::d2: {
      return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }
    case texture_dimension::d2_array: {
      return multisampled
        ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY
        : GL_TEXTURE_2D_ARRAY;
    }
    case texture_dimension::d3: {
      return GL_TEXTURE_3D;
    }
    case texture_dimension::cube: {
      return GL_TEXTURE_CUBE_MAP;
    }
    case texture_dimension::cube_array: {
      return GL_TEXTURE_CUBE_MAP_ARRAY;
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_GL_TEXTURE_TEXTURE_FORMAT_HPP */
