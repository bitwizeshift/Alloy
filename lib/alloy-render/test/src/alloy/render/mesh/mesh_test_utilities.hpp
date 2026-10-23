// SPDX-License-Identifier: CC0-1.0
// Any copyright for this file is dedicated to the Public Domain.
// See https://creativecommons.org/publicdomain/zero/1.0/ for the full text.

#ifndef ALLOY_RENDER_MESH_MESH_TEST_UTILITIES_HPP
#define ALLOY_RENDER_MESH_MESH_TEST_UTILITIES_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <alloy/render/mesh/mesh.hpp>

#include <alloy/core/geometry/point/point3.hpp>
#include <alloy/core/math/vector/vector3.hpp>

#include <algorithm> // std::max
#include <array>     // std::array
#include <cstddef>   // std::size_t
#include <cstdint>   // std::int16_t, std::uint32_t
#include <cstring>   // std::memcpy

namespace alloy::render::test {

inline
auto read_position(const mesh& m, std::size_t vertex) -> core::point3
{
  auto packed = std::array<float,3>{};
  const auto offset = (vertex * m.stride().count())
                    + m.offset_to_positions().count();
  std::memcpy(packed.data(), m.data().data() + offset, sizeof(packed));
  return core::point3{packed[0], packed[1], packed[2]};
}

inline
auto read_normal(const mesh& m, std::size_t vertex) -> core::vector3
{
  auto packed = std::array<std::int16_t,3>{};
  const auto offset = (vertex * m.stride().count())
                    + m.offset_to_normals().count();
  std::memcpy(packed.data(), m.data().data() + offset, sizeof(packed));
  return core::vector3{
    static_cast<float>(packed[0]),
    static_cast<float>(packed[1]),
    static_cast<float>(packed[2]),
  };
}

/// \brief Reads the packed snorm16 `w` component of the tangent at \p vertex
inline
auto read_tangent_handedness(const mesh& m, std::size_t vertex) -> std::int16_t
{
  auto result = std::int16_t{};
  const auto offset = (vertex * m.stride().count())
                    + m.offset_to_tangents().count()
                    + (3u * sizeof(std::int16_t));
  std::memcpy(&result, m.data().data() + offset, sizeof(result));
  return result;
}

/// \brief Counts the triangles whose counter-clockwise face normal points
///        away from the stored normal of its first vertex
inline
auto count_inward_triangles(const mesh& m) -> std::size_t
{
  auto count = std::size_t{0};
  for (auto i = std::size_t{0}; i + 2u < m.index_count(); i += 3u) {
    const auto p0 = read_position(m, m.index_at(i));
    const auto p1 = read_position(m, m.index_at(i + 1u));
    const auto p2 = read_position(m, m.index_at(i + 2u));
    const auto face = (p1 - p0).cross(p2 - p0);
    if (face.dot(read_normal(m, m.index_at(i))) <= 0.0f) {
      ++count;
    }
  }
  return count;
}


/// \brief Gets the largest index in \p m
inline
auto max_index(const mesh& m) -> std::uint32_t
{
  auto result = std::uint32_t{0};
  for (auto i = std::size_t{0}; i < m.index_count(); ++i) {
    result = std::max(result, m.index_at(i));
  }
  return result;
}

} // namespace alloy::render::test

#endif /* ALLOY_RENDER_MESH_MESH_TEST_UTILITIES_HPP */
