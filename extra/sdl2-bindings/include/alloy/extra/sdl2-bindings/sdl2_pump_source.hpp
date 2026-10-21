// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file sdl2_pump_source.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_EXTRA_SDL2_BINDINGS_SDL2_PUMP_SOURCE_HPP
#define ALLOY_EXTRA_SDL2_BINDINGS_SDL2_PUMP_SOURCE_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/sdl2-bindings/api.hpp"
#include "alloy/io/events/window_events.hpp"
#include "alloy/io/message_pump.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <SDL3/SDL.h>
#include <alloy/io/keyboard.hpp>
#include <map>

namespace alloy::extra {

  class sdl2_window;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The SDL2 message pump, as a message_pump::source
  ///
  /// Since the SDL pump is implemented as a static, global-modifying function,
  /// it logically has exactly one instance. As a result, this message pump
  /// is implemented as a singleton, and cannot be copied or moved.
  ///
  /// This pump source may emit any of the following events:
  /// - window_show_event
  /// - window_hide_event
  /// - window_move_event
  /// - window_resize_event
  /// - window_minimize_event
  /// - window_maximize_event
  //////////////////////////////////////////////////////////////////////////////
  class sdl2_pump_source final
    : public alloy::io::message_pump::source
  {
    //--------------------------------------------------------------------------
    // Static Functions
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the static instance
    ///
    /// \return a reference to the static pump source
    ALLOY_EXTRA_SDL2_API
    static auto instance() -> sdl2_pump_source&;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Attaches an sdl2_window to this pump source
    ///
    /// By attaching a window, window events may be propagated when messages
    /// are pumped.
    ///
    /// \param window the window to attach
    ALLOY_EXTRA_SDL2_API
    auto attach_window(core::not_null<sdl2_window*> window) -> void;

    /// \brief Detaches an sdl2_window from this pump source
    ///
    /// Removed windows have their events discarded during pumping.
    ///
    /// \param window the window to attach
    ALLOY_EXTRA_SDL2_API
    auto detach_window(core::not_null<sdl2_window*> window) -> void;

    //--------------------------------------------------------------------------
    // Private Constructors / Assignment
    //--------------------------------------------------------------------------
  private:

    sdl2_pump_source() noexcept = default;
    sdl2_pump_source(sdl2_pump_source&& other) = delete;
    sdl2_pump_source(const sdl2_pump_source& other) = delete;

    //--------------------------------------------------------------------------

    auto operator=(sdl2_pump_source&& other) -> sdl2_pump_source& = delete;
    auto operator=(const sdl2_pump_source& other) -> sdl2_pump_source& = delete;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    /// \brief Hook for the message pump source to implement
    ///
    /// This will poll the current source for events, pushing events back to the
    /// message_pump \p p
    ///
    /// \note When pumping messages from SDL, any message created for a device
    ///       that is not attached to this pump source will be lost. For
    ///       example, if a window event occurs but is not attached to this
    ///       pump, the event will not propagate.
    ///
    /// \param p the message_pump to push new messages to
    auto poll(io::message_pump& p) noexcept -> void override;

    /// \brief Handles a window event
    ///
    /// \param event the SDL_Event to handle
    /// \param p the message pump to pump events back into
    auto handle_window_event(const ::SDL_Event& event, io::message_pump& p)
      noexcept -> void;

    /// \brief Handles a keyboard event
    ///
    /// \param event the SDL_Event to handle
    /// \param state the keyboard state (up or down)
    /// \param p the message pump to respond to
    auto handle_keyboard_event(
      const ::SDL_Event& event,
      io::keyboard::key_state state,
      io::message_pump& p
    ) noexcept -> void;

    /// \brief Handles a mouse event
    ///
    /// \param event the SDL_Event to handle
    /// \param p the message pump to respond to
    auto handle_mouse_event(const ::SDL_Event& event, io::message_pump& p)
      noexcept -> void;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    // TODO(bitwizeshift): Determine better allocation strategy for managing N
    //                     windows.
    std::map<unsigned,sdl2_window*> m_windows;
  };

} // namespace alloy::io

#endif /* ALLOY_EXTRA_SDL2_BINDINGS_SDL2_PUMP_SOURCE_HPP */
