// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file program.hpp
///
/// \brief This header defines a linked OpenGL program object
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SHADER_PROGRAM_HPP
#define ALLOY_RENDER_GL_SHADER_PROGRAM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/gl/shader/shader.hpp"

#include "alloy/core/utilities/result.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/shader/program_layout.hpp"
#include "alloy/render/shader/program_spec.hpp"

#include <GL/glew.h>

#include <span> // std::span

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The unique owner of a linked OpenGL program object
  //////////////////////////////////////////////////////////////////////////////
  class program
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Links \p shaders into a program
    ///
    /// Shaders compiled from GLSL have their vertex input locations and
    /// resource bindings assigned by name, since OpenGL 4.1 cannot declare
    /// bindings in GLSL. Shaders loaded from SPIR-V carry their locations and
    /// bindings in the binary, which may have its names stripped.
    ///
    /// \param shaders the shaders to link, which must share a format
    /// \param vertex_inputs the inputs that the vertex stage reads
    /// \param layout the resources that the program reads
    /// \return the program on success, or `render_errc::program_link_failed`
    ///         with the link log on failure
    static auto link(
      std::span<const shader> shaders,
      std::span<const vertex_input> vertex_inputs,
      const program_layout& layout
    ) -> core::result<program, render_error>;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    program(program&& other) noexcept;
    program(const program&) = delete;

    //--------------------------------------------------------------------------

    ~program();

    //--------------------------------------------------------------------------

    auto operator=(program&& other) noexcept -> program&;
    auto operator=(const program&) -> program& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the OpenGL name of this program
    ///
    /// \return the name of the program
    auto id() const noexcept -> ::GLuint;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    explicit program(::GLuint id) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_id;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_SHADER_PROGRAM_HPP */
