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

} // namespace alloy::render

#endif /* ALLOY_RENDER_SHADER_SHADER_DATA_TYPE_HPP */
