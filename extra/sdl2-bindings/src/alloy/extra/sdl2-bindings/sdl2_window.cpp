// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/extra/sdl2-bindings/sdl2_window.hpp"

#include "alloy/core/assert.hpp"

#include <string>    // std::string
#include <stdexcept> // std::runtime_error
#include <utility>   // std::move

//------------------------------------------------------------------------------
// Protected Constructors / Destructor
//------------------------------------------------------------------------------

alloy::extra::sdl2_window::sdl2_window(core::not_null<::SDL_Window*> window)
  noexcept
  : m_window{window.get()}
{

}

//------------------------------------------------------------------------------

alloy::extra::sdl2_window::~sdl2_window() noexcept
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_DestroyWindow(m_window);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_window::window_handle()
  const noexcept -> window_handle_type
{
  return m_window;
}

//------------------------------------------------------------------------------
// Hooks : Observers
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_window::do_width() const -> int
{
  ALLOY_ASSERT(m_window != nullptr);

  auto width = int{};
  ::SDL_GetWindowSize(m_window, &width, nullptr);
  return width;
}

auto alloy::extra::sdl2_window::do_height() const -> int
{
  ALLOY_ASSERT(m_window != nullptr);

  auto height = int{};
  ::SDL_GetWindowSize(m_window, nullptr, &height);
  return height;
}

auto alloy::extra::sdl2_window::do_is_fullscreen() const -> bool
{
  ALLOY_ASSERT(m_window != nullptr);

  const auto flags = ::SDL_GetWindowFlags(m_window);

  return static_cast<bool>(flags & SDL_WINDOW_FULLSCREEN);
}

auto alloy::extra::sdl2_window::do_is_focused() const -> bool
{
  ALLOY_ASSERT(m_window != nullptr);

  static constexpr auto mask = SDL_WINDOW_MOUSE_GRABBED |
                               SDL_WINDOW_INPUT_FOCUS |
                               SDL_WINDOW_MOUSE_FOCUS;
  const auto flags = ::SDL_GetWindowFlags(m_window);

  return static_cast<bool>(flags & mask);
}

auto alloy::extra::sdl2_window::do_is_shown() const -> bool
{
  ALLOY_ASSERT(m_window != nullptr);

  const auto flags = ::SDL_GetWindowFlags(m_window);

  return !static_cast<bool>(flags & SDL_WINDOW_HIDDEN);
}

auto alloy::extra::sdl2_window::do_is_hidden() const -> bool
{
  ALLOY_ASSERT(m_window != nullptr);

  const auto flags = ::SDL_GetWindowFlags(m_window);

  return static_cast<bool>(flags & SDL_WINDOW_HIDDEN);
}

//------------------------------------------------------------------------------
// Hooks : Modifiers
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_window::do_set_dimensions(int width, int height) -> void
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_SetWindowSize(m_window, width, height);
}

auto alloy::extra::sdl2_window::do_set_position(int x, int y) -> void
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_SetWindowPosition(m_window, x, y);
}

//------------------------------------------------------------------------------
// Hooks : Access
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_window::do_fullscreen() -> void
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_SetWindowFullscreen(m_window, true);
}

auto alloy::extra::sdl2_window::do_focus() -> void
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_RaiseWindow(m_window);
}

auto alloy::extra::sdl2_window::do_show() -> void
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_ShowWindow(m_window);
}

auto alloy::extra::sdl2_window::do_hide() -> void
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_HideWindow(m_window);
}

