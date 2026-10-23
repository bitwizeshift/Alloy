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

#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/model/color.hpp"

#include <algorithm> // std::clamp
#include <cmath>     // std::lround
#include <cstddef>
#include <cstdint>
#include <limits>

namespace alloy::render {

  //============================================================================
  // struct : point_packer
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Packs points into a packed buffer
  //////////////////////////////////////////////////////////////////////////////
  struct point_packer
  {
    auto pack(core::packed_buffer_writer& writer, const core::point2& p)
      -> void;
    auto pack(core::packed_buffer_writer& writer, const core::point3& p)
      -> void;
  };

  //============================================================================
  // struct : vector_packer
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Packs vectors into compressed objects
  //////////////////////////////////////////////////////////////////////////////
  struct vector_packer final
  {
    auto pack(core::packed_buffer_writer& writer, const core::vector3& v)
      -> void;

    static auto compress(float x) noexcept -> std::int16_t;
  };

  //============================================================================
  // struct : color_packer
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Packs a color object into a 4-component rgba byte object
  //////////////////////////////////////////////////////////////////////////////
  struct color_packer final
  {
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
  writer.pack_object(p.x());
  writer.pack_object(p.y());
}

inline
auto alloy::render::point_packer::pack(
  core::packed_buffer_writer& writer,
  const core::point3& p
) -> void
{
  writer.pack_object(p.x());
  writer.pack_object(p.y());
  writer.pack_object(p.z());
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
  writer.pack_object(compress(v.x()));
  writer.pack_object(compress(v.y()));
  writer.pack_object(compress(v.z()));
  writer.pack_object(std::uint16_t{});
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
  writer.pack_object(c.r8());
  writer.pack_object(c.g8());
  writer.pack_object(c.b8());
  writer.pack_object(c.a8());
}

#endif /* ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP */
