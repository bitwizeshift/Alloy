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

#include "alloy/core/utilities/not_null.hpp"
#include "alloy/extra/sdl2-bindings/sdl2_window.hpp"

#include <SDL2/SDL.h>

#include <cstdint> // std::uint32_t

namespace alloy::extra {

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
  class sdl2_gl_window final
    : public sdl2_window
  {
    //--------------------------------------------------------------------------
    // Public Member Types
    //--------------------------------------------------------------------------
  public:

    enum class swap_interval
    {
      immediate,             ///< No synchronization
      synchronized,          ///< VSync enabled
      adaptive_synchronized, ///< Adaptive synchronization
    };

    using context_handle_type = ::SDL_GLContext;

    //--------------------------------------------------------------------------
    // Public Static Factories
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a window from the specified window data
    ///
    /// \param title the window title
    /// \param width the width of the window
    /// \param height the height of the window
    /// \param version the GL version
    static sdl2_gl_window from_window_data( const char* title,
                                            int width, int height,
                                            sdl_gl_version version );

    /// \brief Constructs a window from the specified window data
    ///
    /// \param title the window title
    /// \param x the x-position
    /// \param y the y-position
    /// \param width the width of the window
    /// \param height the height of the window
    /// \param version the GL version
    static sdl2_gl_window from_window_data( const char* title,
                                            int x, int y,
                                            int width, int height,
                                            sdl_gl_version version );

    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a window from an SDL window handle and a gl context
    ///
    /// \note By passing the window or context handle to this class, ownership
    ///       is controlled by this window.
    ///
    /// \pre \p window is not null
    /// \pre \p context is not null
    /// \param window the window handle
    /// \param context the opengl context
    sdl2_gl_window( core::not_null<::SDL_Window*> window,
                    ::SDL_GLContext context ) noexcept;

    /// \brief Constructs an sdl2_gl_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    sdl2_gl_window( sdl2_gl_window&& other ) noexcept = default;
    sdl2_gl_window( const sdl2_gl_window& other ) = delete;

    //--------------------------------------------------------------------------

    ~sdl2_gl_window() noexcept;

    //--------------------------------------------------------------------------

    /// \brief Assigns an sdl2_gl_window by moving the contents from \p other
    ///
    /// \param other the other window to move
    sdl2_gl_window& operator=( sdl2_gl_window&& other ) noexcept = default;
    sdl2_gl_window& operator=( const sdl2_gl_window& other ) = delete;

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

    /// \brief Sets the swap interval for this window
    ///
    /// \param interval the interval for doing framerate swaps
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