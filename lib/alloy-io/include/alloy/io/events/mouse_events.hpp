////////////////////////////////////////////////////////////////////////////////
/// \file mouse_events.hpp
///
/// \brief TODO(Bitwize): Add description
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2021 Matthew Rodusek All rights reserved.

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

#ifndef ALLOY_IO_EVENTS_MOUSE_EVENTS_HPP
#define ALLOY_IO_EVENTS_MOUSE_EVENTS_HPP

#include "alloy/io/event.hpp"

#include <cstdint>

namespace alloy::io {

  class window;

  struct mouse
  {
    enum class button : std::uint8_t {
      left, middle, right,
    };
    enum class button_state : std::uint8_t {
      depressed, pressed,
    };
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a mouse moves to a window that has input
  ///        context
  //////////////////////////////////////////////////////////////////////////////
  struct mouse_move_event
  {
    class window* window;

    std::int32_t x;
    std::int32_t y;
    std::int32_t x_difference;
    std::int32_t y_difference;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired a mouse button is pressed or released
  //////////////////////////////////////////////////////////////////////////////
  struct mouse_button_event
  {
    class window* window;

    std::int32_t x;
    std::int32_t y;
    mouse::button button;
    mouse::button_state state;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a mouse scrolls
  //////////////////////////////////////////////////////////////////////////////
  struct mouse_scroll_event
  {
    class window* window;

    std::int32_t vertical;
    std::int32_t horizontal;
  };

} // namespace alloy::io

// Keyboard events reserve a block of 16 IDS, from 0x30 -> 0x3F

ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::mouse_move_event, 0x0030);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::mouse_button_event, 0x0031);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::mouse_scroll_event, 0x0032);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0033);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0034);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0035);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0036);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0037);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0038);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0039);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x003A);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x003B);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x003C);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x003D);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x003E);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x003F);

#endif /* ALLOY_IO_EVENTS_MOUSE_EVENTS_HPP */
