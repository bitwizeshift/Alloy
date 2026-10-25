// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file material_spec.hpp
///
/// \brief This header defines the description of a material that a render
///        device creates
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MATERIAL_MATERIAL_SPEC_HPP
#define ALLOY_RENDER_MATERIAL_MATERIAL_SPEC_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/resource/resource_handle.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/shader_parameter.hpp"

#include <span> // std::span

namespace alloy::render {

  struct material_tag;

  /// \brief A non-owning handle to a material created by a device
  using material_handle = resource_handle<material_tag>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The complete description of a material to be created by a device
  ///
  /// Parameters may only name resources with the `resource_scope::material`
  /// scope, and only the uniform buffers and textures they name belong to the
  /// material. Uniform buffer members without a parameter are zeroed.
  ///
  /// The spec only views its parameters, which must outlive the call that
  /// creates the material.
  //////////////////////////////////////////////////////////////////////////////
  struct material_spec
  {
    /// The program that the material draws with, which must outlive the
    /// material
    program_handle program;

    /// The values bound to the program
    std::span<const shader_parameter> parameters;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MATERIAL_MATERIAL_SPEC_HPP */
