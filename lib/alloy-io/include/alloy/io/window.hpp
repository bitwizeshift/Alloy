/*****************************************************************************
 * \file window.hpp
 *
 * \todo
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
#ifndef ALLOY_IO_WINDOW_HPP
#define ALLOY_IO_WINDOW_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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

  //============================================================================
  // class : window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An abstract representation of a renderable window
  //////////////////////////////////////////////////////////////////////////////
  class window
  {
    //--------------------------------------------------------------------------
    // Destructor
    //--------------------------------------------------------------------------
  public:

    virtual ~window() noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Gets the width of this window
    ///
    /// \return the width of this window
    int width() const;

    /// \brief Gets the height of this window
    ///
    /// \return the height of this window
    int height() const;

    /// \brief Checks whether this window is fullscreen
    ///
    /// \return \c true if the window is fullscreen
    bool is_fullscreen() const;

    /// \brief Checks whether this window is focused
    ///
    /// \return \c true if the window is focused
    bool is_focused() const;

    /// \brief Checks whether this window is currently shown
    ///
    /// \return \c true if the window is shown
    bool is_shown() const;

    /// \brief Checks whether this window is hidden
    ///
    /// \return \c true if the window is hidden
    bool is_hidden() const;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Updates the underlying window
    void update();

    /// \brief Sets the dimensions of this window
    ///
    /// \note The \p width and \p height may not be honored -- this is subject
    ///       to the underlying window systems minimums and maximums
    ///
    /// \pre \p width must be positive
    /// \pre \p height must be positive
    /// \param width the width of the window
    /// \param height the height of the window
    void set_dimensions( int width, int height );

    /// \brief Sets the position of this window
    ///
    /// \param x the x pixel coordinate
    /// \param y the y pixel coordinate
    void set_position( int x, int y );

    //--------------------------------------------------------------------------
    // Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Sets the window fullscreen
    void fullscreen();

    /// \brief Focuses the window
    void focus();

    /// \brief Shows the window if hidden
    void show();

    /// \brief Hides the window if shown
    void hide();

    //--------------------------------------------------------------------------
    // Protected Constructors
    //--------------------------------------------------------------------------
  protected:

    window() noexcept = default;

    // Copy and move constructors are defaulted to allow implementations to be
    // copyable/moveable

    window( window&& other ) noexcept = default;
    window( const window& other ) noexcept = default;

    window& operator=( window&& other ) noexcept = default;
    window& operator=( const window& other ) noexcept = default;

    //--------------------------------------------------------------------------
    // Hooks : Observers
    //--------------------------------------------------------------------------
  protected:

    virtual int do_width() const = 0;
    virtual int do_height() const = 0;
    virtual bool do_is_fullscreen() const = 0;
    virtual bool do_is_focused() const = 0;
    virtual bool do_is_shown() const = 0;
    virtual bool do_is_hidden() const = 0;

    //--------------------------------------------------------------------------
    // Hooks : Modifiers
    //--------------------------------------------------------------------------
  protected:

    virtual void do_set_dimensions( int width, int height ) = 0;
    virtual void do_set_position( int x, int y ) = 0;
    virtual void do_update() = 0;

    //--------------------------------------------------------------------------
    // Hooks : Access
    //--------------------------------------------------------------------------
  protected:

    virtual void do_fullscreen() = 0;
    virtual void do_focus() = 0;
    virtual void do_show() = 0;
    virtual void do_hide() = 0;
  };

} // namespace alloy::io

#endif /* ALLOY_IO_WINDOW_HPP */