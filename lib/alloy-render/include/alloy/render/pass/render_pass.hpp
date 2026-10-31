// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file render_pass.hpp
///
/// \brief This header defines a single act of drawing a batch into a surface
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_PASS_RENDER_PASS_HPP
#define ALLOY_RENDER_PASS_RENDER_PASS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/pass/render_batch.hpp"
#include "alloy/render/shader/shader_parameter.hpp"

#include "alloy/core/model/color.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/optional.hpp"

#include <span> // std::span

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Everything a device needs to draw a batch into a surface
  ///
  /// The pass only refers to its batch and global parameters, which must
  /// outlive the call that draws it.
  //////////////////////////////////////////////////////////////////////////////
  struct render_pass
  {
    /// The models to draw
    core::not_null<const render_batch*> batch;

    /// The values of the resources with the `resource_scope::global` scope,
    /// shared by everything in the batch. Every program in the batch is
    /// given the values of the resources it declares.
    std::span<const shader_parameter> globals;

    /// The color to clear the surface to before drawing, or null to keep
    /// what the surface already holds
    core::optional<core::color> clear_color;

    /// The depth to clear the surface to before drawing, or null to keep
    /// what the surface already holds
    core::optional<float> clear_depth;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_PASS_RENDER_PASS_HPP */
