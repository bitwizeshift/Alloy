#include "alloy/core/model/projection.hpp"

#include "alloy/core/math/matrix/matrix4_constants.hpp"
#include "alloy/core/assert.hpp"

namespace alloy::core {
  namespace {

    template <typename Unnamed>
    auto to_identity(const Unnamed&, not_null<matrix4*> out) -> void
    {
      (*out) = matrix4_constants::identity;
    }

    auto to_orthographic(const clip_space& space, not_null<matrix4*> out) -> void
    {
      const auto& left = space.get_horizontal().left;
      const auto& right = space.get_horizontal().right;
      const auto& bottom = space.get_vertical().bottom;
      const auto& top = space.get_vertical().top;
      const auto& near = space.get_depth().near;
      const auto& far = space.get_depth().far;

      const auto dx = (right - left);
      const auto dy = (top - bottom);
      const auto dz = (far - near);

      const auto tx = -((right + left) / dx);
      const auto ty = -((top + bottom) / dy);
      const auto tz = -((far + near) / dz);

      const auto rx = real{2} / dx;
      const auto ry = real{2} / dy;
      const auto rz = -real{2} / dz;

      (*out) = matrix4{
        rx, 0,  0,  tx,
        0,  ry, 0,  ty,
        0,  0,  rz, tz,
        0,  0,  0,  1,
      };
    }

    template <typename Perspective>
    auto to_perspective(const Perspective& data, not_null<matrix4*> out) -> void
    {
      const auto& fov = data.fov;
      const auto& aspect_ratio = data.aspect_ratio;
      const auto& depth = data.depth;


      const auto half_tan_fov = trigonometry::tan(fov * real{0.5});

      const auto dz = (depth.far - depth.near);

      const auto rx = real{1} / (aspect_ratio * half_tan_fov);
      const auto ry = real{1} / half_tan_fov;
      const auto rz = -(depth.far + depth.near) / dz;
      const auto factor = - (real{2} * depth.far * depth.near) / dz;

      (*out) = matrix4{
        rx, 0,  0,   0,
        0,  ry, 0,   0,
        0,  0,  rz,  factor,
        0,  0,  -1,  0,
      };
    }

  } // namespace <anonymous>
}

auto alloy::core::projection::perspective(degree fov,
                                          real aspect_ratio,
                                          clip_space::depth depth)
  noexcept -> void
{
  perspective(to_radian(fov), aspect_ratio, depth);
}

auto alloy::core::projection::perspective(radian fov,
                                          real aspect_ratio,
                                          clip_space::depth depth)
  noexcept -> void
{
  ALLOY_ASSERT(aspect_ratio > 0);
  ALLOY_ASSERT(depth.near < depth.far);
  ALLOY_ASSERT(fov > radian{0});

  m_storage = perspective_data{
    fov,
    aspect_ratio,
    depth
  };
}

auto alloy::core::projection::orthographic(real left,
                                           real right,
                                           real bottom,
                                           real top,
                                           real near,
                                           real far)
  noexcept -> void
{
  orthographic(clip_space::make(left, right, bottom, top, near, far));
}

auto alloy::core::projection::orthographic(const clip_space& space)
  noexcept -> void
{
  m_storage = orthographic_data{space};
}

auto alloy::core::projection::extract_matrix(not_null<matrix4*> out)
  const noexcept -> void
{
  std::visit([&out](const auto& data) {
    if constexpr (std::is_same_v<decltype(data), const identity_data&>) {
      to_identity(data, out);
    } else if constexpr (std::is_same_v<decltype(data), const orthographic_data&>) {
      to_orthographic(data, out);
    } else if constexpr (std::is_same_v<decltype(data), const perspective_data&>) {
      to_perspective(data, out);
    }
  }, m_storage);
}

auto alloy::core::projection::to_matrix4()
  const noexcept -> matrix4
{
  auto result = matrix4{};
  extract_matrix(&result);
  return result;
}
