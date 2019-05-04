/*****************************************************************************
 * \file sdl2_window.hpp
 *
 * \brief
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
#ifndef ALLOY_IO_SDL2_WINDOW_HPP
#define ALLOY_IO_SDL2_WINDOW_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/io/window.hpp"

#include <SDL2/SDL.h>
#include <cstdint> // std::uint32_t

namespace alloy::io {

  //============================================================================
  // class : sdl2_window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  ///
  //////////////////////////////////////////////////////////////////////////////
  class sdl2_window : public window
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using window_handle_type = ::SDL_Window*;

    //--------------------------------------------------------------------------
    // Protected Constructors / Destructor
    //--------------------------------------------------------------------------
  protected:

    sdl2_window( const char* title,
                 int width, int height,
                 std::uint32_t flags );
    sdl2_window( const char* title,
                 int x, int y,
                 int width, int height,
                 std::uint32_t flags );

    ~sdl2_window();

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Accesses the underlying window handle
    ///
    /// \return the underlying window handle
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

#endif /* ALLOY_IO_SDL2_WINDOW_HPP */