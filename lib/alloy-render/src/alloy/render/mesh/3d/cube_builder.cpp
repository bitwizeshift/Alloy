/*
  The MIT License (MIT)

  Copyright (c) 2021-2022 Matthew Rodusek All rights reserved.

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

#include "alloy/render/mesh/3d/cube_builder.hpp"

#include "alloy/render/mesh/unsafe_mesh_builder.hpp"

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

  // 32 vertex points on a cube, comprised of 36 indices
  builder.reserve_vertices(32u);
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

    builder.add_face(8u, 9u, 10u, 11u);
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
    builder.add_uv({0.6f,two_thirds});
    builder.add_normal({0,0,1});

    builder.add_face(12u, 13u, 14u, 15u);
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

    builder.add_face(16u, 17u, 18u, 19u);
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
