#include "alloy/extra/sdl2-bindings/sdl2_window.hpp"

#include "alloy/core/assert.hpp"

#include <string>    // std::string
#include <stdexcept> // std::runtime_error
#include <utility>   // std::move

//------------------------------------------------------------------------------
// Protected Constructors / Destructor
//------------------------------------------------------------------------------

alloy::extra::sdl2_window::sdl2_window( core::not_null<::SDL_Window*> window )
  noexcept
  : m_window{window.get()}
{

}

//------------------------------------------------------------------------------

alloy::extra::sdl2_window::~sdl2_window()
  noexcept
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_DestroyWindow(m_window);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

alloy::extra::sdl2_window::window_handle_type
  alloy::extra::sdl2_window::window_handle()
  const noexcept
{
  return m_window;
}

//------------------------------------------------------------------------------
// Hooks : Observers
//------------------------------------------------------------------------------

int alloy::extra::sdl2_window::do_width()
  const
{
  ALLOY_ASSERT(m_window != nullptr);

  auto width = int{};
  ::SDL_GetWindowSize(m_window, &width, nullptr);
  return width;
}

int alloy::extra::sdl2_window::do_height()
  const
{
  ALLOY_ASSERT(m_window != nullptr);

  auto height = int{};
  ::SDL_GetWindowSize(m_window, nullptr, &height);
  return height;
}

bool alloy::extra::sdl2_window::do_is_fullscreen()
  const
{
  ALLOY_ASSERT(m_window != nullptr);

  const auto flags = ::SDL_GetWindowFlags(m_window);

  return static_cast<bool>(flags & SDL_WINDOW_FULLSCREEN);
}

bool alloy::extra::sdl2_window::do_is_focused()
  const
{
  ALLOY_ASSERT(m_window != nullptr);

  static constexpr auto mask = SDL_WINDOW_INPUT_GRABBED |
                               SDL_WINDOW_INPUT_FOCUS |
                               SDL_WINDOW_MOUSE_FOCUS;
  const auto flags = ::SDL_GetWindowFlags(m_window);

  return static_cast<bool>(flags & mask);
}

bool alloy::extra::sdl2_window::do_is_shown()
  const
{
  ALLOY_ASSERT(m_window != nullptr);

  const auto flags = ::SDL_GetWindowFlags(m_window);

  return static_cast<bool>(flags & SDL_WINDOW_SHOWN);
}

bool alloy::extra::sdl2_window::do_is_hidden()
  const
{
  ALLOY_ASSERT(m_window != nullptr);

  const auto flags = ::SDL_GetWindowFlags(m_window);

  return static_cast<bool>(flags & SDL_WINDOW_HIDDEN);
}

//------------------------------------------------------------------------------
// Hooks : Modifiers
//------------------------------------------------------------------------------

void alloy::extra::sdl2_window::do_set_dimensions( int width, int height )
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_SetWindowSize(m_window, width, height);
}

void alloy::extra::sdl2_window::do_set_position( int x, int y )
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_SetWindowPosition(m_window, x, y);
}

//------------------------------------------------------------------------------
// Hooks : Access
//------------------------------------------------------------------------------

void alloy::extra::sdl2_window::do_fullscreen()
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
}

void alloy::extra::sdl2_window::do_focus()
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_SetWindowInputFocus(m_window);
}

void alloy::extra::sdl2_window::do_show()
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_ShowWindow(m_window);
}

void alloy::extra::sdl2_window::do_hide()
{
  ALLOY_ASSERT(m_window != nullptr);

  ::SDL_HideWindow(m_window);
}

