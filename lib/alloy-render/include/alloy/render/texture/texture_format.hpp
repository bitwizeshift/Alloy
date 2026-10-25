// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file texture_format.hpp
///
/// \brief This header defines the formats that texels may be stored in
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_TEXTURE_TEXTURE_FORMAT_HPP
#define ALLOY_RENDER_TEXTURE_TEXTURE_FORMAT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"

#include <cstddef> // std::size_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The format of each texel in a texture
  //////////////////////////////////////////////////////////////////////////////
  enum class texture_format {
    // Color

    /// One 8-bit normalized channel
    r8_unorm,

    /// Two 8-bit normalized channels
    rg8_unorm,

    /// Four 8-bit normalized channels, in RGBA order
    rgba8_unorm,

    /// Four 8-bit normalized channels, in RGBA order, with sRGB-encoded color
    rgba8_srgb,

    /// Four 8-bit normalized channels, in BGRA order
    bgra8_unorm,

    /// Four 8-bit normalized channels, in BGRA order, with sRGB-encoded color
    bgra8_srgb,

    // Floating point / Integral

    /// One 16-bit floating point channel
    r16_float,

    /// Two 16-bit floating point channels
    rg16_float,

    /// Four 16-bit floating point channels
    rgba16_float,

    /// One 32-bit floating point channel
    r32_float,

    /// Two 32-bit floating point channels
    rg32_float,

    /// Four 32-bit floating point channels
    rgba32_float,

    /// One 32-bit unsigned integer channel
    r32_uint,

    // Depth / Stencil

    /// A 16-bit normalized depth
    depth16_unorm,

    /// A 32-bit floating point depth
    depth32_float,

    /// A 32-bit floating point depth with an 8-bit stencil
    depth32_float_stencil8,
  };

  /// \brief Gets the number of bytes that a single texel of format \p f
  ///        occupies in tightly packed texture data
  ///
  /// \param f the texture format
  /// \return the size of a texel
  constexpr auto bytes_per_texel(texture_format f) noexcept -> std::size_t;

  /// \brief Queries whether \p f stores a depth, rather than a color
  ///
  /// \param f the texture format
  /// \return `true` if \p f is a depth format
  constexpr auto is_depth(texture_format f) noexcept -> bool;

  /// \brief Queries whether \p f stores sRGB-encoded color
  ///
  /// \param f the texture format
  /// \return `true` if \p f is an sRGB format
  constexpr auto is_srgb(texture_format f) noexcept -> bool;

} // namespace alloy::render

//==============================================================================
// definitions : enum : texture_format
//==============================================================================

inline constexpr
auto alloy::render::bytes_per_texel(texture_format f) noexcept -> std::size_t
{
  switch (f) {
    case texture_format::r8_unorm: {
      return 1u;
    }
    case texture_format::rg8_unorm:
    case texture_format::r16_float:
    case texture_format::depth16_unorm: {
      return 2u;
    }
    case texture_format::rgba8_unorm:
    case texture_format::rgba8_srgb:
    case texture_format::bgra8_unorm:
    case texture_format::bgra8_srgb:
    case texture_format::rg16_float:
    case texture_format::r32_float:
    case texture_format::r32_uint:
    case texture_format::depth32_float: {
      return 4u;
    }
    case texture_format::rgba16_float:
    case texture_format::rg32_float:
    case texture_format::depth32_float_stencil8: {
      return 8u;
    }
    case texture_format::rgba32_float: {
      return 16u;
    }
  }
  core::compiler::unreachable();
}

inline constexpr
auto alloy::render::is_depth(texture_format f) noexcept -> bool
{
  switch (f) {
    case texture_format::depth16_unorm:
    case texture_format::depth32_float:
    case texture_format::depth32_float_stencil8: {
      return true;
    }
    default: {
      return false;
    }
  }
}

inline constexpr
auto alloy::render::is_srgb(texture_format f) noexcept -> bool
{
  switch (f) {
    case texture_format::rgba8_srgb:
    case texture_format::bgra8_srgb: {
      return true;
    }
    default: {
      return false;
    }
  }
}

#endif /* ALLOY_RENDER_TEXTURE_TEXTURE_FORMAT_HPP */
