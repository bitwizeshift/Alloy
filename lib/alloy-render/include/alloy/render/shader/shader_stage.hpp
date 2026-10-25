// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file shader_stage.hpp
///
/// \brief This header defines the stages of the pipeline that a shader may
///        execute in
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_SHADER_SHADER_STAGE_HPP
#define ALLOY_RENDER_SHADER_SHADER_STAGE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/utilities/enum_traits.hpp"
#include "alloy/core/utilities/option_set.hpp"

#include <cstddef>     // std::size_t
#include <type_traits> // std::true_type, std::integral_constant

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A programmable stage of the pipeline
  //////////////////////////////////////////////////////////////////////////////
  enum class shader_stage {
    vertex,
    tessellation_control,
    tessellation_evaluation,
    geometry,
    fragment,
    compute,
  };

} // namespace alloy::render

template <>
struct alloy::core::enum_is_contiguous<alloy::render::shader_stage>
  : std::true_type{};

template <>
struct alloy::core::distinct_enumerators<alloy::render::shader_stage>
  : std::integral_constant<std::size_t, 6u>{};

namespace alloy::render {

  /// \brief A set of pipeline stages
  using shader_stages = core::option_set<shader_stage>;

} // namespace alloy::render

#endif /* ALLOY_RENDER_SHADER_SHADER_STAGE_HPP */
