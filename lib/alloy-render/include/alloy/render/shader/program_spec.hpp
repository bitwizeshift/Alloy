// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file program_spec.hpp
///
/// \brief This header defines the description of a shader program that a
///        render device creates
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_SHADER_PROGRAM_SPEC_HPP
#define ALLOY_RENDER_SHADER_PROGRAM_SPEC_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/shader/shader_stage.hpp"

#include "alloy/render/resource/resource_handle.hpp"
#include "alloy/render/texture/texture_dimension.hpp"

#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/optional.hpp"

#include <cstddef> // std::byte
#include <cstdint> // std::uint32_t
#include <span>    // std::span
#include <variant> // std::variant

namespace alloy::render {

  struct program_tag;

  /// \brief A non-owning handle to a program created by a device
  using program_handle = resource_handle<program_tag>;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The compiled code that runs a single stage of a program
  //////////////////////////////////////////////////////////////////////////////
  struct shader_stage_spec
  {
    /// The stage that this code runs
    shader_stage stage;

    /// The code to run, in whatever form the device consumes. It is opaque to
    /// everything but the device.
    std::span<const std::byte> bytecode;

    /// The name of the function in the bytecode that the stage begins at
    core::string_view entry_point;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A per-vertex input that the vertex stage reads
  //////////////////////////////////////////////////////////////////////////////
  struct vertex_input
  {
    /// The name of the input in the shader
    core::string_view name;

    /// The location that the input is read from
    std::uint32_t location;

    /// The type of the input
    shader_data_type type;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Where the compiled code of a program expects a resource to be
  ///        bound
  ///
  /// A binding is a property of the bytecode: it is the slot that the shader
  /// was compiled to read the resource from, and the device must place the
  /// resource in that slot before drawing. Its values are only meaningful to
  /// the device that consumes the bytecode.
  //////////////////////////////////////////////////////////////////////////////
  struct shader_binding
  {
    /// The group that the binding belongs to, for devices that organize
    /// bindings into groups. Zero otherwise.
    std::uint32_t group;

    /// The index of the binding within its group
    std::uint32_t index;

    /// The index of the sampler that accompanies a texture, for devices that
    /// bind samplers separately from textures
    core::optional<std::uint32_t> sampler_index;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A named value within a uniform buffer
  //////////////////////////////////////////////////////////////////////////////
  struct uniform_member
  {
    /// The name of the member in the shader
    core::string_view name;

    /// The type of the member
    shader_data_type type;

    /// The offset of the member from the start of its buffer, in bytes
    std::uint32_t offset;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A resource that is a block of uniform values
  //////////////////////////////////////////////////////////////////////////////
  struct uniform_buffer_resource
  {
    /// The size of the block, in bytes
    std::uint32_t byte_size;

    /// The values within the block that may be written by name
    std::span<const uniform_member> members;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A resource that is a texture read through a sampler
  //////////////////////////////////////////////////////////////////////////////
  struct sampled_texture_resource
  {
    /// The dimension of texture that the shader samples
    texture_dimension dimension;
  };

  /// \brief The kind of a resource, and the properties specific to that kind
  using resource_kind = std::variant<
    uniform_buffer_resource,
    sampled_texture_resource
  >;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A resource that the stages of a program read
  //////////////////////////////////////////////////////////////////////////////
  struct program_resource
  {
    /// The name of the resource, unique within its program
    core::string_view name;

    /// The stages that read the resource
    shader_stages stages;

    /// The kind of the resource
    resource_kind kind;

    /// Where the bytecode expects the resource to be bound
    shader_binding binding;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The complete description of a program to be created by a device
  ///
  /// The spec only views its contents, which must outlive the call that
  /// creates the program.
  //////////////////////////////////////////////////////////////////////////////
  struct program_spec
  {
    /// The name of the program, for diagnostics
    core::string_view name;

    /// The code for each stage, with at most one entry per stage
    std::span<const shader_stage_spec> stages;

    /// The inputs that the vertex stage reads
    std::span<const vertex_input> vertex_inputs;

    /// The resources that the stages read
    std::span<const program_resource> resources;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_SHADER_PROGRAM_SPEC_HPP */
