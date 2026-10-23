// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/render/mesh/3d/cube_builder.hpp"

#include "alloy/render/mesh/mesh.hpp"
#include "alloy/render/mesh/primitive_topology.hpp"
#include "alloy/render/mesh/unsafe_mesh_builder.hpp"

#include "alloy/core/geometry/axis_aligned_box.hpp"
#include "alloy/core/memory/allocator.hpp"
#include "alloy/core/utilities/quantity.hpp"

#include <utility>

auto alloy::render::cube_builder::build_with(core::allocator alloc)
  const -> mesh
{
  static constexpr auto one_third = static_cast<float>(1) / static_cast<float>(3);
  static constexpr auto two_thirds = static_cast<float>(2) / static_cast<float>(3);

  auto builder = unsafe_mesh_builder{primitive_topology::triangles, alloc};

  // We are working in 3D-space, with 2D textures
  builder.set_components_per_position(3u);
  builder.set_components_per_uv(2u);
  builder.set_components_per_normal(3u);

  // 24 vertex points on a cube (4 per face), comprised of 36 indices
  builder.reserve_vertices(24u);
  builder.reserve_indices(core::quantity<unsafe_mesh_builder::index_type>{36});

  { // Bottom-face
    builder.add_position({-1.0, -1.0, -1.0});
    builder.add_uv({0, 0});
    builder.add_normal({0, -1, 0});

    builder.add_position({1.0, -1.0, -1.0});
    builder.add_uv({0.5, 0});
    builder.add_normal({0, -1, 0});

    builder.add_position({1.0, -1.0, 1.0});
    builder.add_uv({0.5, one_third});
    builder.add_normal({0, -1, 0});

    builder.add_position({-1.0, -1.0, 1.0});
    builder.add_uv({0, one_third});
    builder.add_normal({0, -1, 0});

    builder.add_face(0u, 1u, 2u, 3u);
  }

  { // Back-face
    builder.add_position({1.0,-1.0,-1.0});
    builder.add_uv({0.5f,0.0f});
    builder.add_normal({0,0,-1});

    builder.add_position({-1.0,-1.0,-1.0});
    builder.add_uv({1.0f,0.0f});
    builder.add_normal({0,0,-1});

    builder.add_position({-1.0,1.0,-1.0});
    builder.add_uv({1.0f,one_third});
    builder.add_normal({0,0,-1});

    builder.add_position({1.0,1.0,-1.0});
    builder.add_uv({0.5f,one_third});
    builder.add_normal({0,0,-1});

    builder.add_face(4u, 5u, 6u, 7u);
  }
  //-------------------------------------------------------------------------
  { // Top-face
    builder.add_position({-1.0,1.0,-1.0});
    builder.add_uv({0.0f,one_third});
    builder.add_normal({0,1,0});

    builder.add_position({1.0,1.0,-1.0});
    builder.add_uv({0.5f,one_third});
    builder.add_normal({0,1,0});

    builder.add_position({1.0,1.0,1.0});
    builder.add_uv({0.5f,two_thirds});
    builder.add_normal({0,1,0});

    builder.add_position({-1.0,1.0,1.0});
    builder.add_uv({0.0f,two_thirds});
    builder.add_normal({0,1,0});

    builder.add_face(8u, 11u, 10u, 9u);
  }

  { // Front-face
    builder.add_position({1.0,-1.0,1.0});
    builder.add_uv({0.5f,one_third});
    builder.add_normal({0,0,1});

    builder.add_position({-1.0,-1.0,1.0});
    builder.add_uv({1.0f,one_third});
    builder.add_normal({0,0,1});

    builder.add_position({-1.0,1.0,1.0});
    builder.add_uv({1.0f,two_thirds});
    builder.add_normal({0,0,1});

    builder.add_position({1.0,1.0,1.0});
    builder.add_uv({0.5f,two_thirds});
    builder.add_normal({0,0,1});

    builder.add_face(12u, 15u, 14u, 13u);
  }
  //-------------------------------------------------------------------------

  { // Left-face
    builder.add_position({-1.0,-1.0,1.0});
    builder.add_uv({0.0f,two_thirds});
    builder.add_normal({-1,0,0});

    builder.add_position({-1.0,-1.0,-1.0});
    builder.add_uv({0.5f,two_thirds});
    builder.add_normal({-1,0,0});

    builder.add_position({-1.0,1.0,-1.0});
    builder.add_uv({0.5f,1.0f});
    builder.add_normal({-1,0,0});

    builder.add_position({-1.0,1.0,1.0});
    builder.add_uv({0.0f,1.0f});
    builder.add_normal({-1,0,0});

    builder.add_face(16u, 19u, 18u, 17u);
  }

  { // Right-face
    builder.add_position({1.0,-1.0,1.0});
    builder.add_uv({0.5f,two_thirds});
    builder.add_normal({1,0,0});

    builder.add_position({1.0,-1.0,-1.0});
    builder.add_uv({1.0f,two_thirds});
    builder.add_normal({1,0,0});

    builder.add_position({1.0,1.0,-1.0});
    builder.add_uv({1.0f,1.0f});
    builder.add_normal({1,0,0});

    builder.add_position({1.0,1.0,1.0});
    builder.add_uv({0.5f,1.0f});
    builder.add_normal({1,0,0});

    builder.add_face(20u, 21u, 22u, 23u);
  }

  builder.set_bounding_box(
    core::axis_aligned_box::from_points({-1, -1, -1}, {1, 1, 1})
  );

  return std::move(builder).release();
}
