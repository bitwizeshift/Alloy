// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file attribute_packer.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP
#define ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/assert.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/model/color.hpp"

#include <algorithm> // std::clamp
#include <array>     // std::array
#include <cmath>     // std::lround
#include <cstdint>   // std::uint8_t, std::int16_t
#include <limits>    // std::numeric_limits

namespace alloy::render {

  //============================================================================
  // struct : point_packer
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Packs the leading `components` coordinates of points into a
  ///        packed buffer as 32-bit floats
  //////////////////////////////////////////////////////////////////////////////
  struct point_packer
  {
    std::uint8_t components;

    auto pack(core::packed_buffer_writer& writer, const core::point2& p)
      -> void;
    auto pack(core::packed_buffer_writer& writer, const core::point3& p)
      -> void;
  };

  //============================================================================
  // struct : vector_packer
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Packs the leading `components` components of vectors into a
  ///        packed buffer as snorm16 values
  //////////////////////////////////////////////////////////////////////////////
  struct vector_packer final
  {
    std::uint8_t components;

    auto pack(core::packed_buffer_writer& writer, const core::vector3& v)
      -> void;

    static auto compress(float x) noexcept -> std::int16_t;
  };

  //============================================================================
  // struct : color_packer
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Packs the leading `components` channels of colors into a packed
  ///        buffer as unorm8 values, in RGBA order
  //////////////////////////////////////////////////////////////////////////////
  struct color_packer final
  {
    std::uint8_t components;

    auto pack(core::packed_buffer_writer& writer, const core::color& c) -> void;
  };

} // namespace alloy::render

//==============================================================================
// struct : point_packer
//==============================================================================

inline
auto alloy::render::point_packer::pack(
  core::packed_buffer_writer& writer,
  const core::point2& p
) -> void
{
  ALLOY_ASSERT(components <= 2u);

  const auto values = std::array{p.x(), p.y()};
  writer.pack_objects(values.begin(), values.begin() + components);
}

inline
auto alloy::render::point_packer::pack(
  core::packed_buffer_writer& writer,
  const core::point3& p
) -> void
{
  ALLOY_ASSERT(components <= 3u);

  const auto values = std::array{p.x(), p.y(), p.z()};
  writer.pack_objects(values.begin(), values.begin() + components);
}

//==============================================================================
// struct : vector_packer
//==============================================================================

inline
auto alloy::render::vector_packer::pack(
  core::packed_buffer_writer& writer,
  const core::vector3& v
) -> void
{
  ALLOY_ASSERT(components <= 3u);

  const auto values = std::array{
    compress(v.x()),
    compress(v.y()),
    compress(v.z()),
  };
  writer.pack_objects(values.begin(), values.begin() + components);
}

inline
auto alloy::render::vector_packer::compress(float x) noexcept -> std::int16_t
{
  static constexpr auto max = std::numeric_limits<std::int16_t>::max();

  const auto clamped = std::clamp(x, -1.0f, 1.0f);
  return static_cast<std::int16_t>(std::lround(clamped * max));
}

//==============================================================================
// struct : color_packer
//==============================================================================

inline
auto alloy::render::color_packer::pack(
  core::packed_buffer_writer& writer,
  const core::color& c
) -> void
{
  ALLOY_ASSERT(components <= 4u);

  const auto values = std::array{c.r8(), c.g8(), c.b8(), c.a8()};
  writer.pack_objects(values.begin(), values.begin() + components);
}

#endif /* ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP */
