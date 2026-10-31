// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file sdl_window_target.hpp
///
/// \brief This header defines the adapter that lets the OpenGL renderer draw
///        into an SDL window
////////////////////////////////////////////////////////////////////////////////

#ifndef SDL_WINDOW_TARGET_HPP
#define SDL_WINDOW_TARGET_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include <alloy/core/utilities/not_null.hpp>

#include <alloy/extra/sdl2-bindings/sdl2_gl_window.hpp>

#include <alloy/render/gl/surface/presentation_target.hpp>

namespace alloy::example {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief Presents the default framebuffer of an SDL window for the OpenGL
  ///        renderer
  ///
  /// Neither the platform nor the renderer knows about the other; this is the
  /// one place they are joined.
  //////////////////////////////////////////////////////////////////////////////
  class sdl_window_target final : public render::gl::presentation_target
  {
    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a target that presents \p window
    ///
    /// \param window the window to present, which must outlive this target
    explicit sdl_window_target(
      core::not_null<extra::sdl2_gl_window*> window
    ) noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    auto drawable_extent() const -> render::gl::surface_extent override;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    auto present() -> void override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    core::not_null<extra::sdl2_gl_window*> m_window;
  };

} // namespace alloy::example

#endif /* SDL_WINDOW_TARGET_HPP */
