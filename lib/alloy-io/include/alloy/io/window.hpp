/*****************************************************************************
 * \file window.hpp
 *
 * \todo
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019-2020, 2022 Matthew Rodusek All rights reserved.

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

#include "alloy/io/api.hpp"
#include "alloy/io/event.hpp"

namespace alloy::io {

  //============================================================================
  // class : window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An abstract representation of a renderable window
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API window
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A semantic type indicating the dimensions of the given window.
    ////////////////////////////////////////////////////////////////////////////
    struct dimensions
    {
      int width;  ///< The width of the window. Must be positive.
      int height; ///< The height of the window. Must be positive.
    };

    ////////////////////////////////////////////////////////////////////////////
    /// \brief A semantic type indicating the position of the given window.
    ////////////////////////////////////////////////////////////////////////////
    struct position
    {
      int x; ///< The x-position of the window
      int y; ///< The y-position of the window
    };

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
    /// \return `true` if the window is fullscreen
    bool is_fullscreen() const;

    /// \brief Checks whether this window is focused
    ///
    /// \return `true` if the window is focused
    bool is_focused() const;

    /// \brief Checks whether this window is currently shown
    ///
    /// \return `true` if the window is shown
    bool is_shown() const;

    /// \brief Checks whether this window is hidden
    ///
    /// \return `true` if the window is hidden
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

    /// \brief Sets the dimension of the window
    ///
    /// \note The \p dimensions may not be honored -- this is subject
    ///       to the underlying window systems minimums and maximums
    ///
    /// \param dimensions the dimension of the window
    void set_dimensions( const dimensions& dimensions );

    /// \brief Sets the position of this window
    ///
    /// \param x the x pixel coordinate
    /// \param y the y pixel coordinate
    void set_position( int x, int y );

    /// \brief Sets the position of this window
    ///
    /// \param position the position of the window
    void set_position( const position& position );

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

//==============================================================================
// inline definitions : class : window
//==============================================================================

inline void alloy::io::window::set_dimensions(const dimensions& dimensions)
{
  set_dimensions(dimensions.width, dimensions.height);
}

inline void alloy::io::window::set_position(const position& position)
{
  set_position(position.x, position.y);
}


#endif /* ALLOY_IO_WINDOW_HPP */

