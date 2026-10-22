// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file sdl2_window.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_EXTRA_SDL2_BINDINGS_SDL2_WINDOW_HPP
#define ALLOY_EXTRA_SDL2_BINDINGS_SDL2_WINDOW_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/sdl2-bindings/api.hpp"

#include "alloy/core/utilities/not_null.hpp"
#include "alloy/platform/window.hpp"

#include <SDL3/SDL.h>

namespace alloy::extra {

  //============================================================================
  // class : sdl2_window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  ///
  //////////////////////////////////////////////////////////////////////////////
  class sdl2_window
    : public alloy::platform::window
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using window_handle_type = ::SDL_Window*;

    //--------------------------------------------------------------------------
    // Protected Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  protected:

    /// \brief Constructs and sdl2_window from an SDL_Window handle
    ///
    /// \note By passing the window handle to this class, ownership is
    ///       transferred to this class
    ///
    /// \pre \p window is not null
    /// \param window the window handle
    explicit sdl2_window(core::not_null<::SDL_Window*> window) noexcept;

    /// \brief Constructs an sdl2_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    sdl2_window(sdl2_window&& other) noexcept = default;
    sdl2_window(const sdl2_window& other) = delete;

    //--------------------------------------------------------------------------

    ~sdl2_window() noexcept;

    //--------------------------------------------------------------------------

    /// \brief Assigns an sdl2_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    auto operator=(sdl2_window&& other) noexcept -> sdl2_window& = default;
    auto operator=(const sdl2_window& other) -> sdl2_window& = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Accesses the underlying window handle
    ///
    /// \return the underlying window handle
    ALLOY_EXTRA_SDL2_API
    auto window_handle() const noexcept -> window_handle_type;

    //--------------------------------------------------------------------------
    // Hooks : Observers
    //--------------------------------------------------------------------------
  protected:

    auto do_width() const -> int override;
    auto do_height() const -> int override;
    auto do_is_fullscreen() const -> bool override;
    auto do_is_focused() const -> bool override;
    auto do_is_shown() const -> bool override;
    auto do_is_hidden() const -> bool override;

    //--------------------------------------------------------------------------
    // Hooks : Modifiers
    //--------------------------------------------------------------------------
  protected:

    auto do_set_dimensions(int width, int height) -> void override;
    auto do_set_position(int x, int y) -> void override;
    virtual auto do_update() -> void override = 0;

    //--------------------------------------------------------------------------
    // Hooks : Access
    //--------------------------------------------------------------------------
  protected:

    auto do_fullscreen() -> void override;
    auto do_focus() -> void override;
    auto do_show() -> void override;
    auto do_hide() -> void override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::SDL_Window* m_window;
  };
} // namespace alloy::platform

#endif /* ALLOY_EXTRA_SDL2_BINDINGS_SDL2_WINDOW_HPP */
