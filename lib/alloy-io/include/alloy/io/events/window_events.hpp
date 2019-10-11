/*****************************************************************************
 * \file window_events.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

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
#ifndef ALLOY_IO_WINDOW_EVENTS_HPP
#define ALLOY_IO_WINDOW_EVENTS_HPP

#include "alloy/io/event.hpp"

namespace alloy::io {

  class window;

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is shown
  //////////////////////////////////////////////////////////////////////////////
  struct window_show_event
  {
    class window* window; ///< The window that was shown
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is hidden
  //////////////////////////////////////////////////////////////////////////////
  struct window_hide_event
  {
    class window* window; ///< The window that was hidden
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is relocated
  //////////////////////////////////////////////////////////////////////////////
  struct window_move_event
  {
    class window* window; ///< The window that was moved
    int x; ///< The x-position the window was moved to
    int y; ///< The y-position the window was moved to
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is resized
  //////////////////////////////////////////////////////////////////////////////
  struct window_resize_event
  {
    class window* window; ///< The window that was resized
    int width;  ///< The width the window was resized to
    int height; ///< The height the window was resized to
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is minimized
  //////////////////////////////////////////////////////////////////////////////
  struct window_minimize_event
  {
    class window* window; ///< The window that was minimized
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is maximized
  //////////////////////////////////////////////////////////////////////////////
  struct window_maximize_event
  {
    class window* window; ///< The window that was maximized
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is restored after being minimized
  //////////////////////////////////////////////////////////////////////////////
  struct window_restore_event
  {
    class window* window; ///< The window that was restored
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is closed
  //////////////////////////////////////////////////////////////////////////////
  struct window_close_event
  {
    class window* window; ///< The window that was closed
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is focused
  //////////////////////////////////////////////////////////////////////////////
  struct window_focus_event
  {
    class window* window; ///< The window that was focused
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event fired when a window is unfocused
  //////////////////////////////////////////////////////////////////////////////
  struct window_unfocus_event
  {
    class window* window; ///< The window that was unfocused
  };
} // namespace alloy::io

// Window events reserve a block of 15 IDS, from 0x1 -> 0xF

// close event is high priority, since an attempt to exit an application should
// take priority over first resizing/moving/refocusing a window.
ALLOY_DEFINE_HIGH_PRIORITY_SYSTEM_EVENT(::alloy::io::window_close_event, 0x0001);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_show_event, 0x0002);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_hide_event, 0x0003);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_move_event, 0x0004);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_resize_event, 0x0005);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_minimize_event, 0x0006);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_maximize_event, 0x0007);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_restore_event, 0x0008);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_focus_event, 0x0009);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::window_unfocus_event, 0x000A);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x000B);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x000C);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x000D);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x000E);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x000F);

#endif /* ALLOY_IO_WINDOW_EVENTS_HPP */