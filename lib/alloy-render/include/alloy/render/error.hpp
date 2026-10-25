// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file error.hpp
///
/// \brief This header defines the errors reported when a render device fails
///        to create a resource
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_ERROR_HPP
#define ALLOY_RENDER_ERROR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/string/string.hpp"
#include "alloy/core/string/zstring_view.hpp"

#include <memory>       // std::shared_ptr
#include <system_error> // std::error_code, std::error_category
#include <type_traits>  // std::true_type

namespace alloy::render {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The reasons a render device may fail to create a resource
  //////////////////////////////////////////////////////////////////////////////
  enum class render_errc {
    /// The device cannot read the bytecode supplied for a shader stage
    unsupported_bytecode = 1,

    /// A shader stage failed to compile
    shader_compile_failed,

    /// The shader stages of a program failed to link together
    program_link_failed,

    /// A program was specified without any shader stages
    missing_stage,

    /// A program was specified with more than one of the same shader stage
    duplicate_stage,

    /// A program was specified with more than one resource of the same name
    duplicate_resource,

    /// A program was specified with more than one input of the same name
    duplicate_input,

    /// A uniform buffer declares the same member more than once, or declares
    /// members that do not fit the buffer
    invalid_uniform_layout,

    /// A resource was referred to that the program does not declare
    unknown_resource,

    /// A resource was given a scope that its kind of resource cannot have
    invalid_resource_scope,

    /// A texture was specified with an invalid combination of properties
    invalid_texture_spec,

    /// The device does not support the requested texture format or dimension
    unsupported_texture_format,

    /// A handle refers to a resource that has been disposed
    stale_handle,

    /// A shader parameter names nothing that the program declares
    parameter_not_found,

    /// A shader parameter's value does not match the type it is bound to
    parameter_type_mismatch,

    /// More than one shader parameter is bound to the same value
    duplicate_parameter,

    /// A texture's dimension does not match the dimension its program expects
    dimension_mismatch,

    /// A model does not have exactly one material for each submesh of its
    /// mesh
    submesh_count_mismatch,

    /// A program reads a vertex input that a mesh does not provide
    missing_vertex_input,

    /// The programs of a model do not declare the same instance inputs, or
    /// declare instance inputs that cannot be read per instance
    instance_layout_mismatch,

    /// The textures of a surface cannot be drawn into together
    incompatible_surface,
  };

  /// \brief Gets the error category for `render_errc` values
  ///
  /// \return the render error category
  auto render_category() noexcept -> const std::error_category&;

  /// \brief Makes an error code from \p e
  ///
  /// \param e the error
  /// \return the error code
  auto make_error_code(render_errc e) noexcept -> std::error_code;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An error reported by a render device, along with any details that
  ///        the device was able to provide
  ///
  /// Details are diagnostics meant for a developer, such as the log of a
  /// shader that failed to compile. Errors are cheap to copy, since copies
  /// share the same details.
  //////////////////////////////////////////////////////////////////////////////
  class render_error
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes an error from \p code without any details
    ///
    /// \param code the error code
    /// \return the error
    static auto make(std::error_code code) noexcept -> render_error;

    /// \brief Makes an error from \p code with the specified \p details
    ///
    /// \param code the error code
    /// \param details the details of the error
    /// \return the error
    static auto make(std::error_code code, core::string details)
      -> render_error;

    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs an error that represents no failure
    render_error() noexcept = default;
    render_error(render_error&& other) noexcept = default;
    render_error(const render_error& other) noexcept = default;

    //--------------------------------------------------------------------------

    auto operator=(render_error&& other) noexcept -> render_error& = default;
    auto operator=(const render_error& other) noexcept -> render_error& = default;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the code of this error
    ///
    /// \return the error code
    auto code() const noexcept -> std::error_code;

    /// \brief Gets the details of this error
    ///
    /// \return the details, or an empty string if there are none
    auto details() const noexcept -> core::zstring_view;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    render_error(
      std::error_code code,
      std::shared_ptr<const core::string> details
    ) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    std::error_code m_code;
    std::shared_ptr<const core::string> m_details;
  };

} // namespace alloy::render

template <>
struct std::is_error_code_enum<alloy::render::render_errc> : std::true_type{};

#endif /* ALLOY_RENDER_ERROR_HPP */
