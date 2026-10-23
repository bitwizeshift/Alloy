// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/mesh/mesh.hpp"

#include "alloy/render/mesh/index_format.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/string/zstring_view.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef> // std::size_t
#include <utility> // std::move

namespace alloy::render {
namespace {

auto indices_per_topology(primitive_topology topology) noexcept -> std::size_t
{
  switch (topology) {
    case primitive_topology::lines: {
      return 2u;
    }
    case primitive_topology::lines_adjacency: {
      return 4u;
    }
    case primitive_topology::triangles: {
      return 3u;
    }
    case primitive_topology::triangles_adjacency: {
      return 6u;
    }
    case primitive_topology::points:
    case primitive_topology::line_strip:
    case primitive_topology::line_strip_adjacency:
    case primitive_topology::triangle_strip:
    case primitive_topology::triangle_strip_adjacency: {
      return 1u;
    }
  }
  core::compiler::unreachable();
}

inline constexpr auto max_vertex_components = 4u;
inline constexpr auto max_color_components = 4u;
inline constexpr auto max_uv_components = 3u;
inline constexpr auto max_uv2_components = 3u;
inline constexpr auto max_normal_components = 4u;
inline constexpr auto max_tangent_components = 4u;
inline constexpr auto max_weight_components = 4u;
inline constexpr auto max_joint_components = 4u;

} // namespace <anonymous>
} // namespace alloy::render

//==============================================================================
// class : mesh
//==============================================================================

//------------------------------------------------------------------------------
// Static Factories
//------------------------------------------------------------------------------

auto alloy::render::mesh::from_config(mesh_config config)
  -> core::result<mesh,construct_error>
{
  const auto& components = config.components;
  if (ALLOY_UNLIKELY(components.position > max_vertex_components)) {
    return core::fail(construct_error::invalid_position_component);
  }
  if (ALLOY_UNLIKELY(components.color > max_color_components)) {
    return core::fail(construct_error::invalid_color_component);
  }
  if (ALLOY_UNLIKELY(components.uv > max_uv_components)) {
    return core::fail(construct_error::invalid_uv_component);
  }
  if (ALLOY_UNLIKELY(components.uv2 > max_uv2_components)) {
    return core::fail(construct_error::invalid_uv2_component);
  }
  if (ALLOY_UNLIKELY(components.normal > max_normal_components)) {
    return core::fail(construct_error::invalid_normal_component);
  }
  if (ALLOY_UNLIKELY(components.tangent > max_tangent_components)) {
    return core::fail(construct_error::invalid_tangent_component);
  }
  if (ALLOY_UNLIKELY(components.weights > max_weight_components)) {
    return core::fail(construct_error::invalid_weight_component);
  }
  if (ALLOY_UNLIKELY(components.joints > max_joint_components)) {
    return core::fail(construct_error::invalid_joint_component);
  }

  const auto bytes = config.vertex_data.size().count();
  const auto stride = components.stride().count();
  if (ALLOY_UNLIKELY(stride == 0u && bytes != 0u)) {
    return core::fail(construct_error::invalid_vertex_data);
  }
  if (ALLOY_UNLIKELY(stride != 0u && (bytes % stride) != 0u)) {
    return core::fail(construct_error::invalid_vertex_data);
  }

  const auto index_bytes = config.index_data.size().count();
  const auto index_size = bytes_per_index(config.index_format).count();
  if (ALLOY_UNLIKELY((index_bytes % index_size) != 0u)) {
    return core::fail(construct_error::invalid_indices);
  }

  const auto index_divisor = indices_per_topology(config.topology);
  if (ALLOY_UNLIKELY(((index_bytes / index_size) % index_divisor) != 0u)) {
    return core::fail(construct_error::invalid_indices);
  }

  auto result = mesh{std::move(config)};
  const auto vertices = result.vertex_count();
  for (auto i = std::size_t{0}; i < result.index_count(); ++i) {
    if (ALLOY_UNLIKELY(result.index_at(i) >= vertices)) {
      return core::fail(construct_error::invalid_indices);
    }
  }

  return result;
}

auto alloy::render::mesh::from_config_unchecked(mesh_config config) -> mesh
{
  return mesh{std::move(config)};
}

//------------------------------------------------------------------------------
// Constructors / Assignment / Copy
//------------------------------------------------------------------------------

auto alloy::render::mesh::copy() const -> mesh
{
  return mesh{*this};
}

auto alloy::render::mesh::copy_with(core::allocator alloc) const -> mesh
{
  return mesh{*this, alloc};
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::mesh::mesh(const mesh& other)
  : m_compressed_data{other.m_compressed_data.copy()},
    m_index_data{other.m_index_data.copy()},
    m_index_format{other.m_index_format},
    m_bounding_volume{other.m_bounding_volume},
    m_topology{other.m_topology},
    m_components{other.m_components}
{

}

alloy::render::mesh::mesh(const mesh& other, core::allocator alloc)
  : m_compressed_data{other.m_compressed_data.copy_with(alloc)},
    m_index_data{other.m_index_data.copy_with(alloc)},
    m_index_format{other.m_index_format},
    m_bounding_volume{other.m_bounding_volume},
    m_topology{other.m_topology},
    m_components{other.m_components}
{

}

alloy::render::mesh::mesh(alloy::render::mesh_config config)
  : m_compressed_data{std::move(config.vertex_data)},
    m_index_data{std::move(config.index_data)},
    m_index_format{config.index_format},
    m_bounding_volume{config.bounding_volume},
    m_topology{config.topology},
    m_components{config.components}
{

}
//==============================================================================
// non-member functions : class : mesh
//==============================================================================

//------------------------------------------------------------------------------
// Utilities
//------------------------------------------------------------------------------

auto alloy::render::error_message(mesh::construct_error e)
  noexcept -> core::zstring_view
{
  switch (e) {
    case mesh::construct_error::invalid_indices: {
      return "Supplied indices are invalid for the topology. This may occur if a "
             "mesh contains triangle topology and the number of indices supplied "
             "are not divisible by 3, or if an index refers to a vertex that "
             "does not exist.";
    }
    case mesh::construct_error::invalid_vertex_data: {
      return "The vertex data provided is invalid. This may occur if the number of "
             "bytes is not divisible by `stride()`, or if vertex data is "
             "supplied without any vertex components.";
    }
    case mesh::construct_error::invalid_position_component: {
      return "The specified number of position components is invalid. This can "
             "occur if `position_components` is not between `0` and `4` "
             "(inclusive)";
    }
    case mesh::construct_error::invalid_color_component: {
      return "The specified number of color components is invalid. This can "
             "occur if `color_components` is not between `0` and `4` "
             "(inclusive)";
    }
    case mesh::construct_error::invalid_uv_component: {
      return "The specified UV components is invalid. This can occur if "
             "`uv_components` is not between `0` and `3` (inclusive)";
    }
    case mesh::construct_error::invalid_uv2_component: {
      return "The specified secondary UV components is invalid. This can "
             "occur if `uv2_components` is not between `0` and `3` "
             "(inclusive)";
    }
    case mesh::construct_error::invalid_normal_component: {
      return "The specified normal components is invalid. This can occur if "
             "`normal_components` is not between `0` and `4` (inclusive)";
    }
    case mesh::construct_error::invalid_tangent_component: {
      return "The specified tangent components is invalid. This can occur if "
             "`tangent_components` is not between `0` and `4` (inclusive)";
    }
    case mesh::construct_error::invalid_weight_component: {
      return "The specified weight components is invalid. This can occur if "
             "`weight_components` is not between `0` and `4` (inclusive)";
    }
    case mesh::construct_error::invalid_joint_component: {
      return "The specified joint components is invalid. This can occur if "
             "`joint_components` is not between `0` and `4` (inclusive)";
    }
  }
  core::compiler::unreachable();
}
