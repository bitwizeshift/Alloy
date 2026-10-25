// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file shader_data_type.hpp
///
/// \brief This header defines the types of data that a shader reads from
///        vertex inputs and uniform buffers
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_SHADER_SHADER_DATA_TYPE_HPP
#define ALLOY_RENDER_SHADER_SHADER_DATA_TYPE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/intrinsics.hpp"
#include "alloy/core/memory/data_quantity.hpp"

#include <cstdint> // std::int32_t, std::uint32_t

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The type of a value that a shader reads
  //////////////////////////////////////////////////////////////////////////////
  enum class shader_data_type {
    boolean,
    int1,
    int2,
    int3,
    int4,
    uint1,
    uint2,
    uint3,
    uint4,
    float1,
    float2,
    float3,
    float4,
    float2x2,
    float3x3,
    float4x4,
  };

  /// \brief Gets the number of bytes occupied by a value of type \p t when
  ///        values are tightly packed
  ///
  /// Each component occupies 32 bits, including the component of a
  /// `boolean`, and matrices are stored column by column.
  ///
  /// \param t the type of the value
  /// \return the size of a value
  constexpr auto bytes_per_value(shader_data_type t) noexcept -> core::bytes;

} // namespace alloy::render

inline constexpr
auto alloy::render::bytes_per_value(shader_data_type t) noexcept -> core::bytes
{
  constexpr auto component = sizeof(std::uint32_t);
  static_assert(sizeof(std::int32_t) == component);
  static_assert(sizeof(float) == component);

  switch (t) {
    case shader_data_type::boolean:
    case shader_data_type::int1:
    case shader_data_type::uint1:
    case shader_data_type::float1: {
      return core::bytes{component};
    }
    case shader_data_type::int2:
    case shader_data_type::uint2:
    case shader_data_type::float2: {
      return core::bytes{component * 2u};
    }
    case shader_data_type::int3:
    case shader_data_type::uint3:
    case shader_data_type::float3: {
      return core::bytes{component * 3u};
    }
    case shader_data_type::int4:
    case shader_data_type::uint4:
    case shader_data_type::float4:
    case shader_data_type::float2x2: {
      return core::bytes{component * 4u};
    }
    case shader_data_type::float3x3: {
      return core::bytes{component * 9u};
    }
    case shader_data_type::float4x4: {
      return core::bytes{component * 16u};
    }
  }
  core::compiler::unreachable();
}

#endif /* ALLOY_RENDER_SHADER_SHADER_DATA_TYPE_HPP */
