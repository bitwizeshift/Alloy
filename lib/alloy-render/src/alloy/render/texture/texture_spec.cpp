// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/texture/texture_spec.hpp"

#include "alloy/render/error.hpp"
#include "alloy/render/texture/texture_dimension.hpp"
#include "alloy/render/texture/texture_format.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/model/image.hpp"
#include "alloy/core/utilities/result.hpp"

#include <algorithm> // std::max
#include <bit>       // std::bit_width
#include <cstddef>   // std::byte, std::size_t
#include <cstdint>   // std::uint32_t
#include <span>      // std::span

namespace alloy::render {
namespace {

auto invalid(const char* details) -> render_error
{
  return render_error::make(render_errc::invalid_texture_spec, details);
}

auto full_chain_length(texture_dimension dimension, texture_extent extent)
  noexcept -> std::uint32_t
{
  auto largest = std::max(extent.width, extent.height);
  if (dimension == texture_dimension::d3) {
    largest = std::max(largest, extent.depth);
  }
  return static_cast<std::uint32_t>(std::bit_width(largest));
}

auto has_zero_extent(texture_extent extent) noexcept -> bool
{
  return extent.width == 0u || extent.height == 0u || extent.depth == 0u;
}

} // namespace <anonymous>
} // namespace alloy::render

//==============================================================================
// class : texture_spec
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::texture_spec::texture_1d(
  texture_format format,
  std::uint32_t width,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  return make_sampled(
    texture_dimension::d1,
    format,
    texture_extent{width, 1u, 1u},
    1u,
    initial_data,
    mips
  );
}

auto alloy::render::texture_spec::texture_1d_array(
  texture_format format,
  std::uint32_t width,
  std::uint32_t layers,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  return make_sampled(
    texture_dimension::d1_array,
    format,
    texture_extent{width, 1u, 1u},
    layers,
    initial_data,
    mips
  );
}

auto alloy::render::texture_spec::texture_2d(
  texture_format format,
  std::uint32_t width,
  std::uint32_t height,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  return make_sampled(
    texture_dimension::d2,
    format,
    texture_extent{width, height, 1u},
    1u,
    initial_data,
    mips
  );
}

auto alloy::render::texture_spec::texture_2d_array(
  texture_format format,
  std::uint32_t width,
  std::uint32_t height,
  std::uint32_t layers,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  return make_sampled(
    texture_dimension::d2_array,
    format,
    texture_extent{width, height, 1u},
    layers,
    initial_data,
    mips
  );
}

auto alloy::render::texture_spec::texture_3d(
  texture_format format,
  std::uint32_t width,
  std::uint32_t height,
  std::uint32_t depth,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  return make_sampled(
    texture_dimension::d3,
    format,
    texture_extent{width, height, depth},
    1u,
    initial_data,
    mips
  );
}

auto alloy::render::texture_spec::cube(
  texture_format format,
  std::uint32_t size,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  return make_sampled(
    texture_dimension::cube,
    format,
    texture_extent{size, size, 1u},
    6u,
    initial_data,
    mips
  );
}

auto alloy::render::texture_spec::cube_array(
  texture_format format,
  std::uint32_t size,
  std::uint32_t cubes,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  return make_sampled(
    texture_dimension::cube_array,
    format,
    texture_extent{size, size, 1u},
    cubes * 6u,
    initial_data,
    mips
  );
}

auto alloy::render::texture_spec::render_target(
  texture_format format,
  std::uint32_t width,
  std::uint32_t height,
  sample_count samples
) -> core::result<texture_spec, render_error>
{
  const auto extent = texture_extent{width, height, 1u};
  if (has_zero_extent(extent)) {
    return core::fail(invalid("texture extents must be nonzero"));
  }

  const auto target = is_depth(format)
    ? texture_usage::depth_stencil
    : texture_usage::render_target;

  return texture_spec{
    texture_dimension::d2,
    format,
    extent,
    1u,
    mip_chain::base_only,
    samples,
    texture_usages{target, texture_usage::sampled},
    {}
  };
}

auto alloy::render::texture_spec::from_image(
  const core::image& image,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  auto format = texture_format{};
  switch (image.format()) {
    case core::image::pixel_format::rgba: {
      format = texture_format::rgba8_unorm;
      break;
    }
    case core::image::pixel_format::bgra: {
      format = texture_format::bgra8_unorm;
      break;
    }
    case core::image::pixel_format::rgb:
    case core::image::pixel_format::bgr: {
      return core::fail(
        render_error::make(
          render_errc::unsupported_texture_format,
          "three-channel images must be widened to four channels"
        )
      );
    }
  }

  const auto pixels = std::span{
    image.data(),
    image.width() * image.height() * bytes_per_texel(format)
  };
  return texture_2d(
    format,
    static_cast<std::uint32_t>(image.width()),
    static_cast<std::uint32_t>(image.height()),
    pixels,
    mips
  );
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::render::texture_spec::dimension()
  const noexcept -> texture_dimension
{
  return m_dimension;
}

auto alloy::render::texture_spec::format() const noexcept -> texture_format
{
  return m_format;
}

auto alloy::render::texture_spec::extent() const noexcept -> texture_extent
{
  return m_extent;
}

auto alloy::render::texture_spec::array_layers() const noexcept -> std::uint32_t
{
  return m_array_layers;
}

auto alloy::render::texture_spec::mip_levels() const noexcept -> std::uint32_t
{
  return m_mip_levels;
}

auto alloy::render::texture_spec::samples() const noexcept -> sample_count
{
  return m_samples;
}

auto alloy::render::texture_spec::usage() const noexcept -> texture_usages
{
  return m_usage;
}

auto alloy::render::texture_spec::initial_data()
  const noexcept -> std::span<const std::byte>
{
  return m_initial_data;
}

auto alloy::render::texture_spec::generates_mipmaps() const noexcept -> bool
{
  return m_generates_mipmaps;
}

auto alloy::render::texture_spec::mip_extent(std::uint32_t level)
  const noexcept -> texture_extent
{
  ALLOY_ASSERT(level < m_mip_levels, "mip level out of range");

  const auto shrink = [level](std::uint32_t size) {
    return std::max(size >> level, 1u);
  };
  return texture_extent{
    .width = shrink(m_extent.width),
    .height = shrink(m_extent.height),
    .depth = shrink(m_extent.depth),
  };
}

auto alloy::render::texture_spec::mip_size(std::uint32_t level)
  const noexcept -> std::size_t
{
  const auto extent = mip_extent(level);

  return std::size_t{extent.width}
    * extent.height
    * extent.depth
    * m_array_layers
    * bytes_per_texel(m_format);
}

//------------------------------------------------------------------------------
// Private Static Factories
//------------------------------------------------------------------------------

auto alloy::render::texture_spec::make_sampled(
  texture_dimension dimension,
  texture_format format,
  texture_extent extent,
  std::uint32_t layers,
  std::span<const std::byte> initial_data,
  mip_chain mips
) -> core::result<texture_spec, render_error>
{
  if (has_zero_extent(extent)) {
    return core::fail(invalid("texture extents must be nonzero"));
  }
  if (layers == 0u) {
    return core::fail(invalid("texture must have at least one array layer"));
  }
  if (is_depth(format)) {
    return core::fail(invalid("depth textures must be render targets"));
  }
  if (mips == mip_chain::generated) {
    if (initial_data.empty()) {
      return core::fail(invalid("generated mipmaps require a base level"));
    }
    if (format == texture_format::r32_uint) {
      return core::fail(invalid("integer textures cannot generate mipmaps"));
    }
  }

  auto result = texture_spec{
    dimension,
    format,
    extent,
    layers,
    mips,
    sample_count::x1,
    texture_usages{texture_usage::sampled},
    initial_data
  };

  if (!initial_data.empty()) {
    const auto levels = result.m_generates_mipmaps ? 1u : result.m_mip_levels;

    auto expected = std::size_t{0u};
    for (auto level = 0u; level < levels; ++level) {
      expected += result.mip_size(level);
    }
    if (initial_data.size() != expected) {
      return core::fail(
        invalid("initial data does not match the texture's extent")
      );
    }
  }
  return result;
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::texture_spec::texture_spec(
  texture_dimension dimension,
  texture_format format,
  texture_extent extent,
  std::uint32_t layers,
  mip_chain mips,
  sample_count samples,
  texture_usages usage,
  std::span<const std::byte> initial_data
) noexcept
  : m_dimension{dimension},
    m_format{format},
    m_extent{extent},
    m_array_layers{layers},
    m_mip_levels{
      mips == mip_chain::base_only ? 1u : full_chain_length(dimension, extent)
    },
    m_samples{samples},
    m_usage{usage},
    m_initial_data{initial_data},
    m_generates_mipmaps{mips == mip_chain::generated}
{

}
