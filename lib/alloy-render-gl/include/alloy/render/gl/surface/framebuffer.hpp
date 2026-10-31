// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file framebuffer.hpp
///
/// \brief This header defines the owner of an OpenGL framebuffer object
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_RENDER_GL_SURFACE_FRAMEBUFFER_HPP
#define ALLOY_RENDER_GL_SURFACE_FRAMEBUFFER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/render/gl/texture/texture.hpp"

#include "alloy/core/utilities/result.hpp"

#include "alloy/render/error.hpp"

#include <GL/glew.h>

namespace alloy::render::gl {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The unique owner of an OpenGL framebuffer object that draws into
  ///        textures
  //////////////////////////////////////////////////////////////////////////////
  class framebuffer
  {
    //--------------------------------------------------------------------------
    // Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Makes a framebuffer that draws colors into \p color, and depths
    ///        into \p depth
    ///
    /// The textures are not owned by the framebuffer, and must outlive it.
    ///
    /// \param color the texture to draw colors into
    /// \param depth the texture to draw depths into, or `nullptr` to draw
    ///        without depth
    /// \return the framebuffer on success, or
    ///         `render_errc::incompatible_surface` if \p color is not a color
    ///         render target, \p depth is not a depth render target, the two
    ///         differ in extent or sample count, or the context cannot draw
    ///         into them together
    static auto make(const texture& color, const texture* depth)
      -> core::result<framebuffer, render_error>;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    framebuffer(framebuffer&& other) noexcept;
    framebuffer(const framebuffer&) = delete;

    //--------------------------------------------------------------------------

    ~framebuffer();

    //--------------------------------------------------------------------------

    auto operator=(framebuffer&& other) noexcept -> framebuffer&;
    auto operator=(const framebuffer&) -> framebuffer& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the OpenGL name of this framebuffer
    ///
    /// \return the name of the framebuffer
    auto id() const noexcept -> ::GLuint;

    //--------------------------------------------------------------------------
    // Private Constructors
    //--------------------------------------------------------------------------
  private:

    explicit framebuffer(::GLuint id) noexcept;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::GLuint m_id;
  };

} // namespace alloy::render::gl

#endif /* ALLOY_RENDER_GL_SURFACE_FRAMEBUFFER_HPP */
