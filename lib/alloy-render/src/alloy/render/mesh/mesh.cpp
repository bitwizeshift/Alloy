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

#include "alloy/render/mesh/mesh.hpp"

#include <utility>

namespace alloy::render {
namespace {

auto indices_per_topology(primitive_topology topology) noexcept -> std::size_t
{
  switch (topology) {
    case primitive_topology::triangles: {
      return 3u;
    }
    case primitive_topology::lines_adjacency: {
      return 2u;
    }
    // TODO(bitwizeshift): Not currently sure if other topologies have any
    //                     specific expected number of indices
    default: {
      break;
    }
  }
  return 1u;
}

inline constexpr auto max_vertex_components = 4u;
inline constexpr auto max_uv_components = 3u;
inline constexpr auto max_normal_components = 4u;
inline constexpr auto max_tangent_components = 4u;
inline constexpr auto max_bitangent_components = 4u;

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
  if (ALLOY_UNLIKELY(config.position_components > max_vertex_components)) {
    return core::fail(construct_error::invalid_position_component);
  }
  if (ALLOY_UNLIKELY(config.uv_components > max_uv_components)) {
    return core::fail(construct_error::invalid_uv_component);
  }
  if (ALLOY_UNLIKELY(config.normal_components > max_normal_components)) {
    return core::fail(construct_error::invalid_normal_component);
  }
  if (ALLOY_UNLIKELY(config.tangent_components > max_tangent_components)) {
    return core::fail(construct_error::invalid_tangent_component);
  }
  if (ALLOY_UNLIKELY(config.bitangent_components > max_bitangent_components)) {
    return core::fail(construct_error::invalid_bitangent_component);
  }

  const auto vertices = config.vertex_data.size();
  const auto stride = (
    (config.position_components * sizeof(float)) +
    (config.uv_components * sizeof(float)) +
    (round_up_power_two(config.normal_components) * sizeof(std::uint16_t)) +
    (round_up_power_two(config.tangent_components) * sizeof(std::uint16_t)) +
    (round_up_power_two(config.bitangent_components) * sizeof(std::uint16_t))
  );
  if (ALLOY_UNLIKELY((vertices % stride).count() != 0u)) {
    return core::fail(construct_error::invalid_vertex_data);
  }

  const auto indices = config.indices.size();
  const auto index_divisor = indices_per_topology(config.topology);
  if (ALLOY_UNLIKELY((indices % index_divisor) != 0u)) {
    return core::fail(construct_error::invalid_indices);
  }

  return from_config_unchecked(std::move(config));
}

auto alloy::render::mesh::from_config_unchecked(mesh_config config)
  -> mesh
{
  return mesh{std::move(config)};
}

//------------------------------------------------------------------------------
// Constructors / Assignment / Copy
//------------------------------------------------------------------------------

auto alloy::render::mesh::copy()
  const -> mesh
{
  return mesh{*this};
}

auto alloy::render::mesh::copy_with(core::allocator alloc)
  const -> mesh
{
  return mesh{*this, alloc};
}

//------------------------------------------------------------------------------
// Private Constructors
//------------------------------------------------------------------------------

alloy::render::mesh::mesh(const mesh& other)
  : m_compressed_data{other.m_compressed_data.copy()},
    m_indices{other.m_indices.copy()},
    m_bounding_volume{other.m_bounding_volume},
    m_topology{other.m_topology},
    m_position_components{other.m_position_components},
    m_uv_components{other.m_uv_components},
    m_normal_components{other.m_normal_components},
    m_tangent_components{other.m_tangent_components},
    m_bitangent_components{other.m_bitangent_components}
{

}

alloy::render::mesh::mesh(const mesh& other, core::allocator alloc)
  : m_compressed_data{other.m_compressed_data.copy_with(alloc)},
    m_indices{other.m_indices.copy_with(
      core::stl_allocator_adapter<std::uint16_t>{alloc}
    )},
    m_bounding_volume{other.m_bounding_volume},
    m_topology{other.m_topology},
    m_position_components{other.m_position_components},
    m_uv_components{other.m_uv_components},
    m_normal_components{other.m_normal_components},
    m_tangent_components{other.m_tangent_components},
    m_bitangent_components{other.m_bitangent_components}
{

}

alloy::render::mesh::mesh(alloy::render::mesh_config config)
  : m_compressed_data{std::move(config.vertex_data)},
    m_indices{std::move(config.indices)},
    m_bounding_volume{config.bounding_volume},
    m_topology{config.topology},
    m_position_components{config.position_components},
    m_uv_components{config.uv_components},
    m_normal_components{
      static_cast<std::uint8_t>(
        (config.normal_components << 4u) | (round_up_power_two(config.normal_components))
      )
    },
    m_tangent_components{
      static_cast<std::uint8_t>(
        (config.tangent_components << 4u) | (round_up_power_two(config.tangent_components))
      )
    },
    m_bitangent_components{
      static_cast<std::uint8_t>(
        (config.bitangent_components << 4u) | (round_up_power_two(config.bitangent_components))
      )
    }
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
             "are not divisible by 3.";
    }
    case mesh::construct_error::invalid_vertex_data: {
      return "The vertex data provided is invalid. This may occur if the number of "
             "bytes is not divisible by `stride()`";
    }
    case mesh::construct_error::invalid_position_component: {
      return "The specified number of position components is invalid. This can "
             "occur if `position_components` is not between `0` and `4` "
             "(inclusive)";
    }
    case mesh::construct_error::invalid_uv_component: {
      return "The specified UV components is invalid. This can occur if "
             "`uv_components` is not between `0` and `3` (inclusive)";
    }
    case mesh::construct_error::invalid_normal_component: {
      return "The specified normal components is invalid. This can occur if "
             "`normal_components` is not between `0` and `4` (inclusive)";
    }
    case mesh::construct_error::invalid_tangent_component: {
      return "The specified tangent components is invalid. This can occur if "
             "`tangent_components` is not between `0` and `4` (inclusive)";
    }
    case mesh::construct_error::invalid_bitangent_component: {
      return "The specified bitangent components is invalid. This can occur if "
             "`bitangent_components` is not between `0` and `4` (inclusive)";
    }
  }
  core::compiler::unreachable();
}
