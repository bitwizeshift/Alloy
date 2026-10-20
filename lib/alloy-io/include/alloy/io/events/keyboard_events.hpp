// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

////////////////////////////////////////////////////////////////////////////////
/// \file keyboard_events.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

#ifndef ALLOY_IO_EVENTS_KEYBOARD_EVENTS_HPP
#define ALLOY_IO_EVENTS_KEYBOARD_EVENTS_HPP

#include "alloy/io/event.hpp"
#include "alloy/io/keyboard.hpp"

namespace alloy::io {

  class window;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired during keyboard events
  //////////////////////////////////////////////////////////////////////////////
  struct keyboard_event
  {
    /// The window associated to the input context
    class window* window;

    /// \brief The keyboard symbol state
    keyboard::symbol symbol;

    /// The current key state
    keyboard::key_state state;
  };

} // namespace alloy::io

// Keyboard events reserve a block of 16 IDS, from 0x20 -> 0x2F

ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::keyboard_event, 0x0020);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0021);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0022);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0023);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0024);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0025);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0026);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0027);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0028);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0029);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x002A);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x002B);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x002C);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x002D);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x002E);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x002F);

#endif /* ALLOY_IO_EVENTS_KEYBOARD_EVENTS_HPP */
