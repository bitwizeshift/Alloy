// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file program_layout.hpp
///
/// \brief This header defines the validated resources of a program that a
///        device retains after the program is created
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_SHADER_PROGRAM_LAYOUT_HPP
#define ALLOY_RENDER_SHADER_PROGRAM_LAYOUT_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/shader/program_spec.hpp"
#include "alloy/render/shader/shader_data_type.hpp"
#include "alloy/render/shader/shader_stage.hpp"

#include "alloy/render/error.hpp"

#include "alloy/core/containers/hash_map.hpp"
#include "alloy/core/containers/vector.hpp"
#include "alloy/core/string/string.hpp"
#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/optional.hpp"
#include "alloy/core/utilities/result.hpp"

#include <cstddef>    // std::size_t
#include <cstdint>    // std::uint32_t
#include <functional> // std::equal_to, std::hash
#include <span>       // std::span
#include <variant>    // std::variant

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The retained properties of a uniform buffer resource
  //////////////////////////////////////////////////////////////////////////////
  struct uniform_buffer_layout
  {
    /// The size of the block, in bytes
    std::uint32_t byte_size;
  };

  /// \brief The kind of a retained resource, and the properties specific to
  ///        that kind
  using resource_layout_kind = std::variant<
    uniform_buffer_layout,
    sampled_texture_resource
  >;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A resource of a program, as retained by its layout
  //////////////////////////////////////////////////////////////////////////////
  struct resource_layout
  {
    /// The name of the resource
    core::string name;

    /// The stages that read the resource
    shader_stages stages;

    /// The kind of the resource
    resource_layout_kind kind;

    /// Where the program expects the resource to be bound
    shader_binding binding;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The location of a uniform buffer member within a program
  //////////////////////////////////////////////////////////////////////////////
  struct member_layout
  {
    /// The index of the uniform buffer resource that holds the member
    std::uint32_t resource_index;

    /// The type of the member
    shader_data_type type;

    /// The offset of the member from the start of its buffer, in bytes
    std::uint32_t offset;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The validated resources of a program, retained by a device so
  ///        that materials can be resolved against the program
  ///
  /// Members of uniform buffers are found by their qualified name, as in
  /// `"block.member"`, or by their name alone when no other uniform buffer
  /// in the program has a member of the same name.
  //////////////////////////////////////////////////////////////////////////////
  class program_layout
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes the layout of the program described by \p spec,
    ///        validating it in the process
    ///
    /// \param spec the program spec
    /// \return the layout on success
    static auto from_spec(const program_spec& spec)
      -> core::result<program_layout, render_error>;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    program_layout(program_layout&& other) noexcept = default;
    program_layout(const program_layout&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(program_layout&& other)
      noexcept -> program_layout& = default;
    auto operator=(const program_layout&) -> program_layout& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets every resource of the program, in the order they were
    ///        specified
    ///
    /// \return the resources
    auto resources() const noexcept -> std::span<const resource_layout>;

    /// \brief Finds the resource named \p name
    ///
    /// \param name the name of the resource
    /// \return the index of the resource, or null if there is none
    auto find_resource(core::string_view name)
      const noexcept -> core::optional<std::uint32_t>;

    /// \brief Finds the uniform buffer member named \p name
    ///
    /// \param name the qualified or unqualified name of the member
    /// \return the member, or `nullptr` if there is none
    auto find_member(core::string_view name)
      const noexcept -> const member_layout*;

    //--------------------------------------------------------------------------
    // Private Member Types
    //--------------------------------------------------------------------------
  private:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A transparent hash of strings, so that names may be looked up
    ///        without being copied into a string
    ///////////////////////////////////////////////////////////////////////////
    struct string_hash
    {
      using is_transparent = void;

      auto operator()(core::string_view s) const noexcept -> std::size_t;
    };

    template <typename T>
    using string_map = core::hash_map<
      core::string,
      T,
      string_hash,
      std::equal_to<>
    >;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    program_layout(
      core::vector<resource_layout> resources,
      string_map<std::uint32_t> resource_indices,
      string_map<member_layout> members
    ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::vector<resource_layout> m_resources;
    string_map<std::uint32_t> m_resource_indices;
    string_map<member_layout> m_members;
  };

} // namespace alloy::render

#endif /* ALLOY_RENDER_SHADER_PROGRAM_LAYOUT_HPP */
