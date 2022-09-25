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

#include "alloy/render/mesh/3d/sphere_builder.hpp"

#include "alloy/render/mesh/unsafe_mesh_builder.hpp"

#include "alloy/core/math/trigonometry.hpp"
#include "alloy/core/math/math_constants.hpp"
#include "alloy/core/math/angle/radian.hpp"

alloy::render::sphere_builder::sphere_builder(std::size_t x_segments,
                                                        std::size_t y_segments)
  noexcept
  : m_x_segments{x_segments},
    m_y_segments{y_segments}
{

}

auto alloy::render::sphere_builder::build_with(core::allocator alloc)
  const -> mesh
{
  auto builder = unsafe_mesh_builder{primitive_topology::triangles, alloc};

  builder.set_components_per_position(3u);
  builder.set_components_per_uv(2u);
  builder.set_components_per_normal(3u);
  builder.reserve_vertices(m_y_segments * m_y_segments);
  builder.reserve_indices(core::quantity<unsafe_mesh_builder::index_type>{
    static_cast<long long>(m_x_segments * m_y_segments)
  });

  for (auto y = 0u; y <= m_y_segments; ++y) {
    for (auto x = 0u; x <= m_x_segments; ++x) {

      const auto x_segment = core::radian{
        static_cast<float>(x) / static_cast<float>(m_x_segments)
      };
      const auto y_segment = core::radian{
        static_cast<float>(y) / static_cast<float>(m_y_segments)
      };

      const auto x_pos = (
        core::trigonometry::cos(x_segment * core::math_constants::tau) *
        core::trigonometry::sin(y_segment * core::math_constants::pi)
      );
      const auto y_pos = (
        core::trigonometry::cos(y_segment * core::math_constants::pi)
      );
      const auto z_pos = (
        core::trigonometry::sin(x_segment * core::math_constants::tau) *
        core::trigonometry::sin(y_segment * core::math_constants::pi)
      );

      builder.add_position({x_pos, y_pos, z_pos});
      builder.add_uv({x_segment.value(), y_segment.value()});
      builder.add_normal({x_pos, y_pos, z_pos});

      const auto i0 = ((y + 1u) * (m_x_segments + 1u) + x);
      const auto i1 = ((y)      * (m_x_segments + 1u) + x);
      const auto i2 = ((y)      * (m_x_segments + 1u) + x + 1u);
      const auto i3 = ((y + 1u) * (m_x_segments + 1u) + x + 1u);

      builder.add_face(i0, i1, i2, i3);
    }
  }

  return std::move(builder).release();
}
