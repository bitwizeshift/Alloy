// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file texture_spec.hpp
///
/// \brief This header defines the description of a texture that a render
///        device creates
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_TEXTURE_TEXTURE_SPEC_HPP
#define ALLOY_RENDER_TEXTURE_TEXTURE_SPEC_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/texture/texture_dimension.hpp"
#include "alloy/render/texture/texture_format.hpp"

#include "alloy/render/error.hpp"
#include "alloy/render/resource/resource_handle.hpp"

#include "alloy/core/model/image.hpp"
#include "alloy/core/utilities/enum_traits.hpp"
#include "alloy/core/utilities/option_set.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>     // std::byte, std::size_t
#include <cstdint>     // std::uint8_t, std::uint32_t
#include <span>        // std::span
#include <type_traits> // std::true_type, std::integral_constant

namespace alloy::render {

  struct texture_tag;

  /// \brief A non-owning handle to a texture created by a device
  using texture_handle = resource_handle<texture_tag>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The ways in which a texture may be used once it is created
  //////////////////////////////////////////////////////////////////////////////
  enum class texture_usage : std::uint8_t {
    /// The texture may be sampled by a shader
    sampled,

    /// The texture may be rendered into as a color attachment
    render_target,

    /// The texture may be rendered into as a depth/stencil attachment
    depth_stencil,
  };

} // namespace alloy::render

template <>
struct alloy::core::enum_is_contiguous<alloy::render::texture_usage>
  : std::true_type{};

template <>
struct alloy::core::distinct_enumerators<alloy::render::texture_usage>
  : std::integral_constant<std::size_t, 3u>{};

namespace alloy::render {

  /// \brief A set of the ways in which a texture may be used
  using texture_usages = core::option_set<texture_usage>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The number of samples stored per texel of a render target
  //////////////////////////////////////////////////////////////////////////////
  enum class sample_count : std::uint8_t {
    x1 = 1u,
    x2 = 2u,
    x4 = 4u,
    x8 = 8u,
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The mip levels of a sampled texture, and how they are filled
  //////////////////////////////////////////////////////////////////////////////
  enum class mip_chain : std::uint8_t {
    /// The texture has only its base level
    base_only,

    /// The texture has every level down to 1x1, each of which is given by
    /// the initial data
    full,

    /// The texture has every level down to 1x1, which the device generates
    /// from the base level given by the initial data
    generated,
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The size of a texture mip level, in texels
  //////////////////////////////////////////////////////////////////////////////
  struct texture_extent
  {
    std::uint32_t width;
    std::uint32_t height;
    std::uint32_t depth;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The complete description of a texture to be created by a device
  ///
  /// A texture spec can only be made through its factories, which reject any
  /// combination of properties that does not describe a texture; every spec
  /// that exists may be given to a device.
  ///
  /// The initial data, if any, is tightly packed and ordered by mip level and
  /// then by array layer. The faces of a cube are ordered as +X, -X, +Y, -Y,
  /// +Z, -Z. The spec only views its initial data, which must outlive the
  /// call that creates the texture.
  //////////////////////////////////////////////////////////////////////////////
  class texture_spec
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes a spec for a sampled one-dimensional texture
    ///
    /// \param format the texel format, which may not be a depth format
    /// \param width the width of the texture
    /// \param initial_data the data of every level given by \p mips, or empty
    /// \param mips the mip levels of the texture
    /// \return the spec on success
    static auto texture_1d(
      texture_format format,
      std::uint32_t width,
      std::span<const std::byte> initial_data = {},
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a sampled array of one-dimensional textures
    ///
    /// \param format the texel format, which may not be a depth format
    /// \param width the width of each layer
    /// \param layers the number of layers
    /// \param initial_data the data of every level given by \p mips, or empty
    /// \param mips the mip levels of the texture
    /// \return the spec on success
    static auto texture_1d_array(
      texture_format format,
      std::uint32_t width,
      std::uint32_t layers,
      std::span<const std::byte> initial_data = {},
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a sampled two-dimensional texture
    ///
    /// \param format the texel format, which may not be a depth format
    /// \param width the width of the texture
    /// \param height the height of the texture
    /// \param initial_data the data of every level given by \p mips, or empty
    /// \param mips the mip levels of the texture
    /// \return the spec on success
    static auto texture_2d(
      texture_format format,
      std::uint32_t width,
      std::uint32_t height,
      std::span<const std::byte> initial_data = {},
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a sampled array of two-dimensional textures
    ///
    /// \param format the texel format, which may not be a depth format
    /// \param width the width of each layer
    /// \param height the height of each layer
    /// \param layers the number of layers
    /// \param initial_data the data of every level given by \p mips, or empty
    /// \param mips the mip levels of the texture
    /// \return the spec on success
    static auto texture_2d_array(
      texture_format format,
      std::uint32_t width,
      std::uint32_t height,
      std::uint32_t layers,
      std::span<const std::byte> initial_data = {},
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a sampled three-dimensional texture
    ///
    /// \param format the texel format, which may not be a depth format
    /// \param width the width of the texture
    /// \param height the height of the texture
    /// \param depth the depth of the texture
    /// \param initial_data the data of every level given by \p mips, or empty
    /// \param mips the mip levels of the texture
    /// \return the spec on success
    static auto texture_3d(
      texture_format format,
      std::uint32_t width,
      std::uint32_t height,
      std::uint32_t depth,
      std::span<const std::byte> initial_data = {},
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a sampled cube texture
    ///
    /// \param format the texel format, which may not be a depth format
    /// \param size the width and height of each face
    /// \param initial_data the data of every level given by \p mips, or empty
    /// \param mips the mip levels of the texture
    /// \return the spec on success
    static auto cube(
      texture_format format,
      std::uint32_t size,
      std::span<const std::byte> initial_data = {},
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a sampled array of cube textures
    ///
    /// \param format the texel format, which may not be a depth format
    /// \param size the width and height of each face
    /// \param cubes the number of cubes
    /// \param initial_data the data of every level given by \p mips, or empty
    /// \param mips the mip levels of the texture
    /// \return the spec on success
    static auto cube_array(
      texture_format format,
      std::uint32_t size,
      std::uint32_t cubes,
      std::span<const std::byte> initial_data = {},
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a two-dimensional texture that is rendered
    ///        into, and then sampled
    ///
    /// The texture is usable as a depth/stencil attachment if \p format is a
    /// depth format, and as a color attachment otherwise.
    ///
    /// \param format the texel format
    /// \param width the width of the texture
    /// \param height the height of the texture
    /// \param samples the number of samples per texel
    /// \return the spec on success
    static auto render_target(
      texture_format format,
      std::uint32_t width,
      std::uint32_t height,
      sample_count samples = sample_count::x1
    ) -> core::result<texture_spec, render_error>;

    /// \brief Makes a spec for a sampled two-dimensional texture that is
    ///        initialized with the pixels of \p image
    ///
    /// The resulting spec views the pixels of \p image, which must outlive it.
    /// Only images with four channels are supported; images in an RGB or BGR
    /// format fail with `render_errc::unsupported_texture_format`.
    ///
    /// \param image the image to view
    /// \param mips the mip levels of the texture, which may not be
    ///             `mip_chain::full`
    /// \return the spec on success
    static auto from_image(
      const core::image& image,
      mip_chain mips = mip_chain::base_only
    ) -> core::result<texture_spec, render_error>;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the shape of the texture
    ///
    /// \return the dimension
    auto dimension() const noexcept -> texture_dimension;

    /// \brief Gets the format of each texel
    ///
    /// \return the format
    auto format() const noexcept -> texture_format;

    /// \brief Gets the size of the base mip level
    ///
    /// Only `d3` textures have a depth other than 1.
    ///
    /// \return the extent
    auto extent() const noexcept -> texture_extent;

    /// \brief Gets the number of array layers, where each cube has 6
    ///
    /// \return the number of layers
    auto array_layers() const noexcept -> std::uint32_t;

    /// \brief Gets the number of mip levels
    ///
    /// \return the number of mip levels
    auto mip_levels() const noexcept -> std::uint32_t;

    /// \brief Gets the number of samples per texel
    ///
    /// \return the sample count
    auto samples() const noexcept -> sample_count;

    /// \brief Gets the ways in which the texture may be used
    ///
    /// \return the usages
    auto usage() const noexcept -> texture_usages;

    /// \brief Gets the tightly packed data that initializes the texture
    ///
    /// \return the initial data, which is empty if there is none
    auto initial_data() const noexcept -> std::span<const std::byte>;

    /// \brief Queries whether the device generates every mip level beyond the
    ///        base level, in which case the initial data holds only the base
    ///        level
    ///
    /// \return `true` if the mipmaps are generated
    auto generates_mipmaps() const noexcept -> bool;

    /// \brief Gets the size of the mip level \p level
    ///
    /// \pre \p level is less than `mip_levels()`
    /// \param level the mip level
    /// \return the extent of the level
    auto mip_extent(std::uint32_t level) const noexcept -> texture_extent;

    /// \brief Gets the number of bytes that the mip level \p level occupies
    ///        across every array layer
    ///
    /// \pre \p level is less than `mip_levels()`
    /// \param level the mip level
    /// \return the size of the level, in bytes
    auto mip_size(std::uint32_t level) const noexcept -> std::size_t;

    //--------------------------------------------------------------------------
    // Private Static Factories
    //--------------------------------------------------------------------------
  private:

    static auto make_sampled(
      texture_dimension dimension,
      texture_format format,
      texture_extent extent,
      std::uint32_t layers,
      std::span<const std::byte> initial_data,
      mip_chain mips
    ) -> core::result<texture_spec, render_error>;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    texture_spec(
      texture_dimension dimension,
      texture_format format,
      texture_extent extent,
      std::uint32_t layers,
      mip_chain mips,
      sample_count samples,
      texture_usages usage,
      std::span<const std::byte> initial_data
    ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    texture_dimension m_dimension;
    texture_format m_format;
    texture_extent m_extent;
    std::uint32_t m_array_layers;
    std::uint32_t m_mip_levels;
    sample_count m_samples;
    texture_usages m_usage;
    std::span<const std::byte> m_initial_data;
    bool m_generates_mipmaps;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_TEXTURE_TEXTURE_SPEC_HPP */
