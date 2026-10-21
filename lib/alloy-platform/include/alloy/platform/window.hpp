// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file window.hpp
 *
 * \todo
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_WINDOW_HPP
#define ALLOY_PLATFORM_WINDOW_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/api.hpp"
#include "alloy/platform/event.hpp"

namespace alloy::platform {

  //============================================================================
  // class : window
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An abstract representation of a renderable window
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API window
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
    auto width() const -> int;

    /// \brief Gets the height of this window
    ///
    /// \return the height of this window
    auto height() const -> int;

    /// \brief Checks whether this window is fullscreen
    ///
    /// \return `true` if the window is fullscreen
    auto is_fullscreen() const -> bool;

    /// \brief Checks whether this window is focused
    ///
    /// \return `true` if the window is focused
    auto is_focused() const -> bool;

    /// \brief Checks whether this window is currently shown
    ///
    /// \return `true` if the window is shown
    auto is_shown() const -> bool;

    /// \brief Checks whether this window is hidden
    ///
    /// \return `true` if the window is hidden
    auto is_hidden() const -> bool;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Updates the underlying window
    auto update() -> void;

    /// \brief Sets the dimensions of this window
    ///
    /// \note The \p width and \p height may not be honored -- this is subject
    ///       to the underlying window systems minimums and maximums
    ///
    /// \pre \p width must be positive
    /// \pre \p height must be positive
    /// \param width the width of the window
    /// \param height the height of the window
    auto set_dimensions(int width, int height) -> void;

    /// \brief Sets the dimension of the window
    ///
    /// \note The \p dimensions may not be honored -- this is subject
    ///       to the underlying window systems minimums and maximums
    ///
    /// \param dimensions the dimension of the window
    auto set_dimensions(const dimensions& dimensions) -> void;

    /// \brief Sets the position of this window
    ///
    /// \param x the x pixel coordinate
    /// \param y the y pixel coordinate
    auto set_position(int x, int y) -> void;

    /// \brief Sets the position of this window
    ///
    /// \param position the position of the window
    auto set_position(const position& position) -> void;

    //--------------------------------------------------------------------------
    // Access
    //--------------------------------------------------------------------------
  public:

    /// \brief Sets the window fullscreen
    auto fullscreen() -> void;

    /// \brief Focuses the window
    auto focus() -> void;

    /// \brief Shows the window if hidden
    auto show() -> void;

    /// \brief Hides the window if shown
    auto hide() -> void;

    //--------------------------------------------------------------------------
    // Protected Constructors
    //--------------------------------------------------------------------------
  protected:

    window() noexcept = default;

    // Copy and move constructors are defaulted to allow implementations to be
    // copyable/moveable

    window(window&& other) noexcept = default;
    window(const window& other) noexcept = default;

    auto operator=(window&& other) noexcept -> window& = default;
    auto operator=(const window& other) noexcept -> window& = default;

    //--------------------------------------------------------------------------
    // Hooks : Observers
    //--------------------------------------------------------------------------
  protected:

    virtual auto do_width() const -> int = 0;
    virtual auto do_height() const -> int = 0;
    virtual auto do_is_fullscreen() const -> bool = 0;
    virtual auto do_is_focused() const -> bool = 0;
    virtual auto do_is_shown() const -> bool = 0;
    virtual auto do_is_hidden() const -> bool = 0;

    //--------------------------------------------------------------------------
    // Hooks : Modifiers
    //--------------------------------------------------------------------------
  protected:

    virtual auto do_set_dimensions(int width, int height) -> void = 0;
    virtual auto do_set_position(int x, int y) -> void = 0;
    virtual auto do_update() -> void = 0;

    //--------------------------------------------------------------------------
    // Hooks : Access
    //--------------------------------------------------------------------------
  protected:

    virtual auto do_fullscreen() -> void = 0;
    virtual auto do_focus() -> void = 0;
    virtual auto do_show() -> void = 0;
    virtual auto do_hide() -> void = 0;
  };

} // namespace alloy::platform

//==============================================================================
// inline definitions : class : window
//==============================================================================

inline
auto alloy::platform::window::set_dimensions(const dimensions& dimensions) -> void
{
  set_dimensions(dimensions.width, dimensions.height);
}

inline
auto alloy::platform::window::set_position(const position& position) -> void
{
  set_position(position.x, position.y);
}


#endif /* ALLOY_PLATFORM_WINDOW_HPP */

