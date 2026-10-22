// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file sdl2_gl_window.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_EXTRA_SDL2_BINDINGS_SDL2_GL_WINDOW_HPP
#define ALLOY_EXTRA_SDL2_BINDINGS_SDL2_GL_WINDOW_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/sdl2-bindings/api.hpp"
#include "alloy/extra/sdl2-bindings/sdl2_window.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <SDL3/SDL.h>


namespace alloy::extra {

  //============================================================================
  // struct : sdl2_gl_version
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A structure
  //////////////////////////////////////////////////////////////////////////////
  struct sdl_gl_version
  {
    int major;
    int minor;
  };

  //============================================================================
  // class : sdl2_gl_window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A concrete instance of an OpenGL Window
  //////////////////////////////////////////////////////////////////////////////
  class sdl2_gl_window final
    : public sdl2_window
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    enum class swap_interval
    {
      immediate,             ///< No synchronization
      synchronized,          ///< VSync enabled
      adaptive_synchronized, ///< Adaptive synchronization
    };

    using context_handle_type = ::SDL_GLContext;

    //--------------------------------------------------------------------------
    // Public Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a window from the specified window data
    ///
    /// \param title the window title
    /// \param dimensions the dimensions of the window
    /// \param version the GL version
    ALLOY_EXTRA_SDL2_API
    static auto from_window_data(
      const char* title,
      const dimensions& dimensions,
      sdl_gl_version version
    ) -> sdl2_gl_window;

    /// \brief Constructs a window from the specified window data
    ///
    /// \param title the window title
    /// \param position the position of the window
    /// \param dimensions the dimensions of the window
    /// \param version the GL version
    ALLOY_EXTRA_SDL2_API
    static auto from_window_data(
      const char* title,
      const position& position,
      const dimensions& dimensions,
      sdl_gl_version version
    ) -> sdl2_gl_window;

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a window from an SDL window handle and a gl context
    ///
    /// \note By passing the window or context handle to this class, ownership
    ///       is controlled by this window.
    ///
    /// \pre \p window is not null
    /// \pre \p context is not null
    /// \param window the window handle
    /// \param context the opengl context
    ALLOY_EXTRA_SDL2_API
    sdl2_gl_window(
      core::not_null<::SDL_Window*> window,
      ::SDL_GLContext context
    ) noexcept;

    /// \brief Constructs an sdl2_gl_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    sdl2_gl_window(sdl2_gl_window&& other) noexcept = default;
    sdl2_gl_window(const sdl2_gl_window& other) = delete;

    //--------------------------------------------------------------------------

    ALLOY_EXTRA_SDL2_API
    ~sdl2_gl_window() noexcept;

    //--------------------------------------------------------------------------

    /// \brief Assigns an sdl2_gl_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    auto operator=(sdl2_gl_window&& other)
      noexcept -> sdl2_gl_window& = default;
    auto operator=(const sdl2_gl_window& other) -> sdl2_gl_window& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Accesses the underlying window handle
    ///
    /// \return the underlying window handle
    ALLOY_EXTRA_SDL2_API
    auto context_handle() const noexcept -> context_handle_type;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Sets the swap interval for this window
    ///
    /// \param interval the interval for doing framerate swaps
    ALLOY_EXTRA_SDL2_API
    auto set_swap_interval(swap_interval interval) -> void;

    //--------------------------------------------------------------------------
    // Hooks : Modifiers
    //--------------------------------------------------------------------------
  protected:

    auto do_update() -> void override;

    //--------------------------------------------------------------------------
    // Protected Members
    //--------------------------------------------------------------------------
  protected:

    ::SDL_GLContext m_gl_context;
  };
} // namespace alloy::platform

#endif /* ALLOY_EXTRA_SDL2_BINDINGS_SDL2_GL_WINDOW_HPP */
