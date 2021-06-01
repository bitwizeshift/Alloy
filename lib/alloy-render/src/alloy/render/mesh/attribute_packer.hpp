////////////////////////////////////////////////////////////////////////////////
/// \file attribute_packer.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP
#define ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP

#include "alloy/core/memory/packed_buffer.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/geometry/point/point2.hpp"
#include "alloy/core/geometry/point/point3.hpp"
#include "alloy/core/model/color.hpp"

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
    auto pack(core::packed_buffer_writer& writer, const core::point2& p) -> void;
    auto pack(core::packed_buffer_writer& writer, const core::point3& p) -> void;
  };

  //============================================================================
  // struct : vector_packer
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Packs vectors into compressed objects
  //////////////////////////////////////////////////////////////////////////////
  struct vector_packer final
  {
    auto pack(core::packed_buffer_writer& writer, const core::vector3& v) -> void;

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
auto alloy::render::point_packer::pack(core::packed_buffer_writer& writer,
                                       const core::point2& p)
  -> void
{
  writer.pack_object(p.x());
  writer.pack_object(p.y());
}

inline
auto alloy::render::point_packer::pack(core::packed_buffer_writer& writer,
                                       const core::point3& p)
  -> void
{
  writer.pack_object(p.x());
  writer.pack_object(p.y());
  writer.pack_object(p.z());
}

//==============================================================================
// struct : vector_packer
//==============================================================================

inline
auto alloy::render::vector_packer::pack(core::packed_buffer_writer& writer,
                                        const core::vector3& v)
  -> void
{
  writer.pack_object(compress(v.x()));
  writer.pack_object(compress(v.y()));
  writer.pack_object(compress(v.z()));
  writer.pack_object(std::uint16_t{});
}

inline
auto alloy::render::vector_packer::compress(float x)
  noexcept -> std::int16_t
{
  static constexpr auto max = std::numeric_limits<std::int16_t>::max();

  return static_cast<std::int16_t>(x * max);
}

//==============================================================================
// struct : color_packer
//==============================================================================

inline
auto alloy::render::color_packer::pack(core::packed_buffer_writer& writer,
                                       const core::color& c)
  -> void
{
  auto rgba = static_cast<std::uint32_t>(c.to_rgba32());
  const auto r = static_cast<std::uint8_t>((rgba & 0xff000000) >> 24);
  const auto g = static_cast<std::uint8_t>((rgba & 0x00ff0000) >> 16);
  const auto b = static_cast<std::uint8_t>((rgba & 0x0000ff00) >> 8);
  const auto a = static_cast<std::uint8_t>((rgba & 0x000000ff) >> 0);

  writer.pack_object(r);
  writer.pack_object(g);
  writer.pack_object(b);
  writer.pack_object(a);
}

#endif /* ALLOY_RENDER_MESH_ATTRIBUTE_PACKER_HPP */
