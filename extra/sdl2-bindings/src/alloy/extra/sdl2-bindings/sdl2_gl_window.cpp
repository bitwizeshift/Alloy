// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/extra/sdl2-bindings/sdl2_gl_window.hpp"

#include "alloy/extra/sdl2-bindings/sdl2_window.hpp"

#include "alloy/core/assert.hpp"
#include "alloy/core/intrinsics.hpp"
#include "alloy/core/utilities/not_null.hpp"

#include <stdexcept> // std::runtime_error
#include <string>    // std::string
#include <utility>   // std::move

//------------------------------------------------------------------------------
// Public Static Factories
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_gl_window::from_window_data(
  const char* title,
  const dimensions& dimensions,
  sdl_gl_version version
) -> sdl2_gl_window
{
  return from_window_data(
    title,
    position{SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED},
    dimensions,
    version
  );
}

auto alloy::extra::sdl2_gl_window::from_window_data(
  const char* title,
  const position& position,
  const dimensions& dimensions,
  sdl_gl_version version
) -> sdl2_gl_window
{
  const auto properties = ::SDL_CreateProperties();
  ::SDL_SetStringProperty(properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
  ::SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_X_NUMBER, position.x);
  ::SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, position.y);
  ::SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, dimensions.width);
  ::SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, dimensions.height);
  ::SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
  ::SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);

  auto window = ::SDL_CreateWindowWithProperties(properties);
  ::SDL_DestroyProperties(properties);

  if (window == nullptr) {
    auto message = std::string{::SDL_GetError()};
    ::SDL_ClearError();

    throw std::runtime_error{std::move(message)};
  }

  ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version.major);
  ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version.minor);
  ::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  auto context = ::SDL_GL_CreateContext(window);

  if (context == nullptr) {
    auto message = std::string{::SDL_GetError()};
    ::SDL_ClearError();
    ::SDL_DestroyWindow(window);

    throw std::runtime_error{std::move(message)};
  }

  return sdl2_gl_window{window, context};
}

//------------------------------------------------------------------------------
// Public Constructors / Destructor
//------------------------------------------------------------------------------

alloy::extra::sdl2_gl_window::sdl2_gl_window(
  core::not_null<::SDL_Window*> window,
  ::SDL_GLContext context
) noexcept
  : sdl2_window{window.get()},
    m_gl_context{context}
{
  ALLOY_ASSERT(context != nullptr);
}

//------------------------------------------------------------------------------

alloy::extra::sdl2_gl_window::~sdl2_gl_window() noexcept
{
  ALLOY_ASSERT(m_gl_context != nullptr);

  ::SDL_GL_DestroyContext(m_gl_context);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_gl_window::context_handle()
  const noexcept -> context_handle_type
{
  return m_gl_context;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_gl_window::set_swap_interval(swap_interval interval)
  -> void
{
  auto value = int{};

  // values come from https://wiki.libsdl.org/SDL_GL_SetSwapInterval
  switch(interval) {
    case swap_interval::immediate:
      value = 0;
      break;
    case swap_interval::synchronized:
      value = 1;
      break;
    case swap_interval::adaptive_synchronized:
      value = -1;
      break;
    default:
      ALLOY_UNREACHABLE();
  }

  ::SDL_GL_SetSwapInterval(value);
}

//------------------------------------------------------------------------------
// Hooks : Modifiers
//------------------------------------------------------------------------------

auto alloy::extra::sdl2_gl_window::do_update() -> void
{
  ::SDL_GL_SwapWindow(sdl2_window::window_handle());
}
