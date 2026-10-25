// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file shader_parameter.hpp
///
/// \brief This header defines the named values that are bound to the members
///        and resources of a program
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_SHADER_SHADER_PARAMETER_HPP
#define ALLOY_RENDER_SHADER_SHADER_PARAMETER_HPP

#ifdef _MSC_VER
# pragma once
#endif

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
#include <variant> // std::variant

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A texture bound to a program, along with how it is sampled
  //////////////////////////////////////////////////////////////////////////////
  struct texture_binding
  {
    /// The texture to bind, which must outlive whatever binds it
    texture_handle texture;

    /// How the texture is sampled
    sampler_state sampler;
  };

  /// \brief The value of a shader parameter
  ///
  /// Each alternative binds to a uniform buffer member of the corresponding
  /// `shader_data_type`, except for `texture_binding`, which binds to a
  /// sampled texture resource.
  using shader_value = std::variant<
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
  struct shader_parameter
  {
    /// The name of a sampled texture resource, or the qualified or
    /// unqualified name of a uniform buffer member
    core::string_view name;

    /// The value to bind
    shader_value value;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_SHADER_SHADER_PARAMETER_HPP */
