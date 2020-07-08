////////////////////////////////////////////////////////////////////////////////
/// \file keyboard_events.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2020 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

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
