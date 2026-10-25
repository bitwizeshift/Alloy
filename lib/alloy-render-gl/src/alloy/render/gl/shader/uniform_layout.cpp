// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "uniform_layout.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/render/material/material_layout.hpp"
#include "alloy/render/shader/shader_data_type.hpp"

#include <array>     // std::array
#include <cstddef>   // std::byte, std::size_t
#include <cstdint>   // std::int32_t, std::uint32_t
#include <cstring>   // std::memcpy
#include <span>      // std::span, std::as_bytes
#include <variant>   // std::visit

namespace alloy::render::gl {
namespace {

/// \brief Packs the columns of a matrix into floats, padding each column to
///        16 bytes
template <std::size_t Columns, typename Matrix>
auto pack_columns(const Matrix& m) noexcept -> std::array<float, Columns * 4u>
{
  auto result = std::array<float, Columns * 4u>{};
  for (auto c = 0u; c < Columns; ++c) {
    for (auto r = 0u; r < Columns; ++r) {
      result[(c * 4u) + r] = static_cast<float>(m.get(r, c));
    }
  }
  return result;
}

/// \brief Writes a single uniform value into the bytes of its buffer
///
/// Values are converted to 32-bit floats, since `core::real` may be
/// configured as a double.
struct pack_visitor
{
  std::span<std::byte> destination;

  template <typename T>
  auto write(const T& value) const noexcept -> void
  {
    const auto bytes = std::as_bytes(std::span{&value, 1u});
    std::memcpy(destination.data(), bytes.data(), bytes.size());
  }

  auto operator()(bool value) const noexcept -> void
  {
    write(std::uint32_t{value ? 1u : 0u});
  }

  auto operator()(std::int32_t value) const noexcept -> void
  {
    write(value);
  }

  auto operator()(std::uint32_t value) const noexcept -> void
  {
    write(value);
  }

  auto operator()(float value) const noexcept -> void
  {
    write(value);
  }

  auto operator()(const core::vector2& value) const noexcept -> void
  {
    write(std::array{
      static_cast<float>(value.x()),
      static_cast<float>(value.y()),
    });
  }

  auto operator()(const core::vector3& value) const noexcept -> void
  {
    write(std::array{
      static_cast<float>(value.x()),
      static_cast<float>(value.y()),
      static_cast<float>(value.z()),
    });
  }

  auto operator()(const core::vector4& value) const noexcept -> void
  {
    write(std::array{
      static_cast<float>(value.x()),
      static_cast<float>(value.y()),
      static_cast<float>(value.z()),
      static_cast<float>(value.w()),
    });
  }

  auto operator()(const core::color& value) const noexcept -> void
  {
    write(std::array{
      static_cast<float>(value.r()),
      static_cast<float>(value.g()),
      static_cast<float>(value.b()),
      static_cast<float>(value.a()),
    });
  }

  auto operator()(const core::matrix2& value) const noexcept -> void
  {
    write(pack_columns<2u>(value));
  }

  auto operator()(const core::matrix3& value) const noexcept -> void
  {
    write(pack_columns<3u>(value));
  }

  auto operator()(const core::matrix4& value) const noexcept -> void
  {
    write(pack_columns<4u>(value));
  }
};

} // namespace <anonymous>
} // namespace alloy::render::gl

auto alloy::render::gl::std140_size(shader_data_type t)
  noexcept -> std::uint32_t
{
  switch (t) {
    case shader_data_type::boolean:
    case shader_data_type::int1:
    case shader_data_type::uint1:
    case shader_data_type::float1: {
      return 4u;
    }
    case shader_data_type::int2:
    case shader_data_type::uint2:
    case shader_data_type::float2: {
      return 8u;
    }
    case shader_data_type::int3:
    case shader_data_type::uint3:
    case shader_data_type::float3: {
      return 12u;
    }
    case shader_data_type::int4:
    case shader_data_type::uint4:
    case shader_data_type::float4: {
      return 16u;
    }
    case shader_data_type::float2x2: {
      return 32u;
    }
    case shader_data_type::float3x3: {
      return 48u;
    }
    case shader_data_type::float4x4: {
      return 64u;
    }
  }
  core::compiler::unreachable();
}

auto alloy::render::gl::pack_uniform_block(const uniform_block& block)
  -> core::vector<std::byte>
{
  auto bytes = core::vector<std::byte>(block.byte_size);

  for (const auto& value : block.values) {
    const auto destination = std::span{bytes}.subspan(
      value.offset,
      std140_size(value.type)
    );
    std::visit(pack_visitor{destination}, value.data);
  }
  return bytes;
}
