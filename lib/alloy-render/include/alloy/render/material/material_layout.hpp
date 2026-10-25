// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file material_layout.hpp
///
/// \brief This header defines the resolution of material parameters against
///        the resources of a program
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_MATERIAL_MATERIAL_LAYOUT_HPP
#define ALLOY_RENDER_MATERIAL_MATERIAL_LAYOUT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/material/material_spec.hpp"

#include "alloy/render/error.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/texture/sampler_state.hpp"
#include "alloy/render/texture/texture_dimension.hpp"
#include "alloy/render/texture/texture_spec.hpp"

#include "alloy/core/containers/vector.hpp"
#include "alloy/core/math/matrix/matrix2.hpp"
#include "alloy/core/math/matrix/matrix3.hpp"
#include "alloy/core/math/matrix/matrix4.hpp"
#include "alloy/core/math/vector/vector2.hpp"
#include "alloy/core/math/vector/vector3.hpp"
#include "alloy/core/math/vector/vector4.hpp"
#include "alloy/core/model/color.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstdint> // std::int32_t, std::uint32_t
#include <span>    // std::span
#include <variant> // std::variant

namespace alloy::render {

  /// \brief The data of a material parameter that is bound to a uniform
  ///        buffer member
  using uniform_data = std::variant<
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
    core::matrix4
  >;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A value bound to a single uniform buffer member
  //////////////////////////////////////////////////////////////////////////////
  struct uniform_value
  {
    /// The type of the member, which the data has been checked against
    shader_data_type type;

    /// The offset of the member from the start of its buffer, in bytes
    std::uint32_t offset;

    /// The data bound to the member
    uniform_data data;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A uniform buffer that a material owns, along with every value
  ///        bound to its members
  //////////////////////////////////////////////////////////////////////////////
  struct uniform_block
  {
    /// The index of the uniform buffer resource in the program's layout
    std::uint32_t resource_index;

    /// Where the program expects the buffer to be bound
    shader_binding binding;

    /// The size of the buffer, in bytes
    std::uint32_t byte_size;

    /// The values bound to members of the buffer. Members without a value
    /// are zero.
    core::vector<uniform_value> values;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A texture that a material binds to a program
  //////////////////////////////////////////////////////////////////////////////
  struct texture_slot
  {
    /// The index of the sampled texture resource in the program's layout
    std::uint32_t resource_index;

    /// Where the program expects the texture to be bound
    shader_binding binding;

    /// The dimension of texture that the program samples
    texture_dimension dimension;

    /// The texture to bind
    texture_handle texture;

    /// How the texture is sampled
    sampler_state sampler;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The parameters of a material, resolved against the resources of
  ///        its program
  ///
  /// Every name has been resolved and every value checked against the type
  /// it is bound to, so that no lookups remain when drawing.
  //////////////////////////////////////////////////////////////////////////////
  class resolved_material
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Resolves the material \p parameters against the resources of a
    ///        program described by \p layout
    ///
    /// The existence of each texture is not checked, since only the device
    /// that created it can do so.
    ///
    /// \param layout the layout of the program
    /// \param parameters the parameters to resolve
    /// \return the resolved material on success
    static auto from_parameters(
      const program_layout& layout,
      std::span<const material_parameter> parameters
    ) -> core::result<resolved_material, render_error>;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the uniform buffers that the material owns, in resource
    ///        order
    ///
    /// \return the uniform blocks
    auto uniform_blocks() const noexcept -> std::span<const uniform_block>;

    /// \brief Gets the textures that the material binds, in resource order
    ///
    /// \return the texture slots
    auto textures() const noexcept -> std::span<const texture_slot>;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    resolved_material(
      core::vector<uniform_block> uniform_blocks,
      core::vector<texture_slot> textures
    ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::vector<uniform_block> m_uniform_blocks;
    core::vector<texture_slot> m_textures;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_MATERIAL_MATERIAL_LAYOUT_HPP */
