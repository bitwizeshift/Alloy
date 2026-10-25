// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file capabilities.hpp
///
/// \brief This header defines the optional features that an OpenGL context
///        may support
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_CAPABILITIES_HPP
#define ALLOY_RENDER_GL_CAPABILITIES_HPP

#ifdef _MSC_VER
# pragma once
#endif

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The optional features that an OpenGL context supports
  ///
  /// The minimum supported context is OpenGL 4.1, which is the newest
  /// version available on macOS. Anything newer is detected here.
  //////////////////////////////////////////////////////////////////////////////
  struct capabilities
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------

    /// \brief Queries the capabilities of the current context
    ///
    /// \pre an OpenGL context is current, and GLEW has been initialized
    /// \return the capabilities of the current context
    static auto from_current_context() noexcept -> capabilities;

    //--------------------------------------------------------------------------
    // Public Members
    //--------------------------------------------------------------------------

    /// Whether shaders may be loaded from SPIR-V binaries, through OpenGL 4.6
    /// or `ARB_gl_spirv`
    bool spirv;

    /// Whether cube array textures are supported
    bool texture_cube_map_array;

    /// Whether multisampled textures are supported
    bool texture_multisample;

    /// The maximum anisotropy that samplers may filter with, which is 1 if
    /// anisotropic filtering is unsupported
    float max_anisotropy;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_CAPABILITIES_HPP */
