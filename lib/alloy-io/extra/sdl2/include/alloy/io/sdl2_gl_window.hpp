/*****************************************************************************
 * \file sdl2_gl_window.hpp
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
#ifndef ALLOY_IO_SDL2_GL_WINDOW_HPP
#define ALLOY_IO_SDL2_GL_WINDOW_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/io/sdl2_window.hpp"

#include <SDL2/SDL.h>

namespace alloy::io {

  //============================================================================
  // struct : sdl2_gl_version
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A structure
  //////////////////////////////////////////////////////////////////////////////
  struct sdl_gl_version
  {
    int major;
    int minor;
  };

  //============================================================================
  // class : sdl2_gl_window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A concrete instance of an OpenGL Window
  //////////////////////////////////////////////////////////////////////////////
  class sdl2_gl_window final : public sdl2_window
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    using context_handle_type = ::SDL_GLContext;

    //--------------------------------------------------------------------------
    // Constructors
    //--------------------------------------------------------------------------
  public:

    sdl2_gl_window( const char* title,
                    int width, int height,
                    sdl_gl_version version ) noexcept;
    sdl2_gl_window( const char* title,
                    int x, int y,
                    int width, int height,
                    sdl_gl_version version ) noexcept;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \brief Accesses the underlying window handle
    ///
    /// \return the underlying window handle
    context_handle_type context_handle() const noexcept;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    enum class swap_interval
    {
      immediate,             ///< No synchronization
      synchronized,          ///< VSync enabled
      adaptive_synchronized, ///< Adaptive synchronization
    };

    void set_swap_interval( swap_interval interval );

    //--------------------------------------------------------------------------
    // Hooks : Modifiers
    //--------------------------------------------------------------------------
  protected:

    void do_update() override;

    //--------------------------------------------------------------------------
    // Protected Members
    //--------------------------------------------------------------------------
  protected:

    ::SDL_GLContext m_gl_context;
  };
} // namespace alloy::io

#endif /* ALLOY_IO_SDL2_GL_WINDOW_HPP */