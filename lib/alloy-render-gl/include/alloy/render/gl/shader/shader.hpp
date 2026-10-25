// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file shader.hpp
///
/// \brief This header defines a compiled OpenGL shader object
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SHADER_SHADER_HPP
#define ALLOY_RENDER_GL_SHADER_SHADER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/core/string/string_view.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/render/error.hpp"
#include "alloy/render/shader/shader_stage.hpp"

#include <GL/glew.h>

#include <cstddef> // std::byte
#include <span>    // std::span

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The forms of shader code that OpenGL can consume
  //////////////////////////////////////////////////////////////////////////////
  enum class shader_format {
    /// GLSL source code
    glsl,

    /// A SPIR-V binary
    spirv,
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The unique owner of a compiled OpenGL shader object
  //////////////////////////////////////////////////////////////////////////////
  class shader
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Compiles a shader for \p stage from GLSL \p source
    ///
    /// \param stage the stage the shader runs in
    /// \param source the GLSL source
    /// \return the shader on success, or `render_errc::shader_compile_failed`
    ///         with the compile log on failure
    static auto compile_glsl(shader_stage stage, core::string_view source)
      -> core::result<shader, render_error>;

    /// \brief Loads a shader for \p stage from a SPIR-V \p binary, beginning
    ///        at \p entry_point
    ///
    /// \pre the current context supports SPIR-V
    /// \param stage the stage the shader runs in
    /// \param binary the SPIR-V binary
    /// \param entry_point the name of the function the shader begins at
    /// \return the shader on success, or `render_errc::shader_compile_failed`
    ///         with the specialization log on failure
    static auto load_spirv(
      shader_stage stage,
      std::span<const std::byte> binary,
      core::string_view entry_point
    ) -> core::result<shader, render_error>;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    shader(shader&& other) noexcept;
    shader(const shader&) = delete;

    //--------------------------------------------------------------------------

    ~shader();

    //--------------------------------------------------------------------------

    auto operator=(shader&& other) noexcept -> shader&;
    auto operator=(const shader&) -> shader& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the OpenGL name of this shader
    ///
    /// \return the name of the shader
    auto id() const noexcept -> ::GLuint;

    /// \brief Gets the form of code that this shader was created from
    ///
    /// \return the format of the shader
    auto format() const noexcept -> shader_format;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    shader(::GLuint id, shader_format format) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_id;
    shader_format m_format;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_SHADER_SHADER_HPP */
