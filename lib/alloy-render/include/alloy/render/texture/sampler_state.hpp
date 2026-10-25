// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file sampler_state.hpp
///
/// \brief This header defines how a texture is filtered and addressed when a
///        shader samples it
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_TEXTURE_SAMPLER_STATE_HPP
#define ALLOY_RENDER_TEXTURE_SAMPLER_STATE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/material/depth_compare.hpp"

#include "alloy/core/utilities/hash.hpp"
#include "alloy/core/utilities/optional.hpp"

#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint8_t
#include <functional> // std::hash

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief How texels are combined when a texture is magnified or minified
  //////////////////////////////////////////////////////////////////////////////
  enum class texture_filter {
    /// The nearest texel is used
    nearest,

    /// The nearest texels are linearly interpolated
    linear,
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief How mip levels are combined when a texture is minified
  //////////////////////////////////////////////////////////////////////////////
  enum class mipmap_filter {
    /// Only the base level is sampled
    none,

    /// The nearest mip level is sampled
    nearest,

    /// The two nearest mip levels are sampled and linearly interpolated
    linear,
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief How coordinates outside of the [0, 1] range are resolved
  //////////////////////////////////////////////////////////////////////////////
  enum class texture_wrap {
    /// The texture tiles
    repeat,

    /// The texture tiles, mirroring on every other tile
    mirrored_repeat,

    /// The coordinate is clamped to the edge texels
    clamp_to_edge,

    /// Coordinates outside the texture sample the border color
    clamp_to_border,
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The color sampled outside of a texture that uses
  ///        `texture_wrap::clamp_to_border`
  //////////////////////////////////////////////////////////////////////////////
  enum class border_color {
    transparent_black,
    opaque_black,
    opaque_white,
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The complete description of how a texture is sampled
  ///
  /// A sampler state is a value rather than a resource, so equal sampler
  /// states are interchangeable.
  //////////////////////////////////////////////////////////////////////////////
  struct sampler_state
  {
    /// The filter used when the texture is minified
    texture_filter min_filter = texture_filter::linear;

    /// The filter used when the texture is magnified
    texture_filter mag_filter = texture_filter::linear;

    /// The filter used between mip levels
    mipmap_filter mip_filter = mipmap_filter::linear;

    /// The wrapping of the first coordinate
    texture_wrap wrap_u = texture_wrap::repeat;

    /// The wrapping of the second coordinate
    texture_wrap wrap_v = texture_wrap::repeat;

    /// The wrapping of the third coordinate
    texture_wrap wrap_w = texture_wrap::repeat;

    /// The color sampled outside the texture when clamping to the border
    border_color border = border_color::transparent_black;

    /// The comparison performed against a depth texture when sampled, if any
    core::optional<depth_compare> compare = core::null;

    /// The lowest mip level that may be sampled
    float min_lod = 0.0f;

    /// The highest mip level that may be sampled
    float max_lod = 1000.0f;

    /// The maximum anisotropy used when filtering, between 1 and 16
    std::uint8_t max_anisotropy = 1u;
  };

  //============================================================================
  // non-member functions : struct : sampler_state
  //============================================================================

  //----------------------------------------------------------------------------
  // Equality
  //----------------------------------------------------------------------------

  auto operator==(const sampler_state& lhs, const sampler_state& rhs)
    noexcept -> bool;
  auto operator!=(const sampler_state& lhs, const sampler_state& rhs)
    noexcept -> bool;

} // namespace alloy::render

template <>
struct std::hash<alloy::render::sampler_state>
{
  auto operator()(const alloy::render::sampler_state& state)
    const noexcept -> std::size_t
  {
    return alloy::core::hash_values(
      state.min_filter,
      state.mag_filter,
      state.mip_filter,
      state.wrap_u,
      state.wrap_v,
      state.wrap_w,
      state.border,
      state.compare.has_value(),
      state.compare.value_or(alloy::render::depth_compare::never),
      state.min_lod,
      state.max_lod,
      state.max_anisotropy
    );
  }
};

//==============================================================================
// definitions : non-member functions : struct : sampler_state
//==============================================================================

//------------------------------------------------------------------------------
// Equality
//------------------------------------------------------------------------------

inline
auto alloy::render::operator==(
  const sampler_state& lhs,
  const sampler_state& rhs
) noexcept -> bool
{
  return lhs.min_filter == rhs.min_filter
    && lhs.mag_filter == rhs.mag_filter
    && lhs.mip_filter == rhs.mip_filter
    && lhs.wrap_u == rhs.wrap_u
    && lhs.wrap_v == rhs.wrap_v
    && lhs.wrap_w == rhs.wrap_w
    && lhs.border == rhs.border
    && lhs.compare == rhs.compare
    && lhs.min_lod == rhs.min_lod
    && lhs.max_lod == rhs.max_lod
    && lhs.max_anisotropy == rhs.max_anisotropy;
}

inline
auto alloy::render::operator!=(
  const sampler_state& lhs,
  const sampler_state& rhs
) noexcept -> bool
{
  return !(lhs == rhs);
}

#endif /* ALLOY_RENDER_TEXTURE_SAMPLER_STATE_HPP */
