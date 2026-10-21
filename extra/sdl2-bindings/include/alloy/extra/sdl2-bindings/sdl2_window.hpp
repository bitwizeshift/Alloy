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
#include "alloy/io/window.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <SDL3/SDL.h>

namespace alloy::extra {

  //============================================================================
  // class : sdl2_window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  ///
  //////////////////////////////////////////////////////////////////////////////
  class sdl2_window
    : public alloy::io::window
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
    explicit sdl2_window( core::not_null<::SDL_Window*> window ) noexcept;

    /// \brief Constructs an sdl2_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    sdl2_window( sdl2_window&& other ) noexcept = default;
    sdl2_window( const sdl2_window& other ) = delete;

    //--------------------------------------------------------------------------

    ~sdl2_window() noexcept;

    //--------------------------------------------------------------------------

    /// \brief Assigns an sdl2_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    sdl2_window& operator=( sdl2_window&& other ) noexcept = default;
    sdl2_window& operator=( const sdl2_window& other ) = delete;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Accesses the underlying window handle
    ///
    /// \return the underlying window handle
    ALLOY_EXTRA_SDL2_API
    window_handle_type window_handle() const noexcept;

    //--------------------------------------------------------------------------
    // Hooks : Observers
    //--------------------------------------------------------------------------
  protected:

    int do_width() const override;
    int do_height() const override;
    bool do_is_fullscreen() const override;
    bool do_is_focused() const override;
    bool do_is_shown() const override;
    bool do_is_hidden() const override;

    //--------------------------------------------------------------------------
    // Hooks : Modifiers
    //--------------------------------------------------------------------------
  protected:

    void do_set_dimensions( int width, int height ) override;
    void do_set_position( int x, int y ) override;
    virtual void do_update() override = 0;

    //--------------------------------------------------------------------------
    // Hooks : Access
    //--------------------------------------------------------------------------
  protected:

    void do_fullscreen() override;
    void do_focus() override;
    void do_show() override;
    void do_hide() override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    ::SDL_Window* m_window;
  };
} // namespace alloy::io

#endif /* ALLOY_EXTRA_SDL2_BINDINGS_SDL2_WINDOW_HPP */
