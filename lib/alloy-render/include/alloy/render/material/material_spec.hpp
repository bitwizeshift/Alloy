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
#include "alloy/render/texture/sampler_state.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/string/string_view.hpp"

#include <cstdint> // std::int32_t, std::uint32_t
#include <span>    // std::span
#include <variant> // std::variant

namespace alloy::render {

  struct material_tag;

  /// \brief A non-owning handle to a material created by a device
  using material_handle = resource_handle<material_tag>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A texture bound to a material, along with how it is sampled
  //////////////////////////////////////////////////////////////////////////////
  struct texture_binding
  {
    /// The texture to bind, which must outlive the material
    texture_handle texture;

    /// How the texture is sampled
    sampler_state sampler;
  };

  /// \brief The value of a material parameter
  ///
  /// Each alternative binds to a uniform buffer member of the corresponding
  /// `shader_data_type`, except for `texture_binding`, which binds to a
  /// sampled texture resource.
  using material_value = std::variant<
    bool,
    std::int32_t,
    std::uint32_t,
    float,
    core::vector2,
    core::vector3,
    core::vector4,
    core::color,
    core::matrix2,
    core::matrix3,
    core::matrix4,
    texture_binding
  >;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A value bound to a named member or resource of a program
  //////////////////////////////////////////////////////////////////////////////
  struct material_parameter
  {
    /// The name of a sampled texture resource, or the qualified or
    /// unqualified name of a uniform buffer member
    core::string_view name;

    /// The value to bind
    material_value value;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The complete description of a material to be created by a device
  ///
  /// Only the uniform buffers and textures named by a parameter belong to the
  /// material. Any other resource of the program, such as a buffer of
  /// per-frame camera data, is left to be bound by whoever draws with the
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
    std::span<const material_parameter> parameters;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MATERIAL_MATERIAL_SPEC_HPP */
