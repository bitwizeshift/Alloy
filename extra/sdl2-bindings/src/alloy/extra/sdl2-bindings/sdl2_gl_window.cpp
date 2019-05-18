#include "alloy/extra/sdl2-bindings/sdl2_gl_window.hpp"

#include "alloy/core/assert.hpp"

#include <string>    // std::string
#include <stdexcept> // std::runtime_error
#include <utility>   // std::move

//------------------------------------------------------------------------------
// Public Static Factories
//------------------------------------------------------------------------------

alloy::extra::sdl2_gl_window
  alloy::extra::sdl2_gl_window::from_window_data( const char* title,
                                                  int width, int height,
                                                  sdl_gl_version version )
{
  return from_window_data(
    title,
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    width, height,
    version
  );
}

alloy::extra::sdl2_gl_window
  alloy::extra::sdl2_gl_window::from_window_data( const char* title,
                                                  int x, int y,
                                                  int width, int height,
                                                  sdl_gl_version version )
{
  const auto flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;

  auto window = ::SDL_CreateWindow(title, x, y, width, height, flags);

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

alloy::extra::sdl2_gl_window
  ::sdl2_gl_window( core::not_null<::SDL_Window*> window,
                    ::SDL_GLContext context )
  noexcept
  : sdl2_window{window.get()},
    m_gl_context{context}
{
  ALLOY_ASSERT(context != nullptr);
}

//------------------------------------------------------------------------------

alloy::extra::sdl2_gl_window::~sdl2_gl_window()
  noexcept
{
  ALLOY_ASSERT(m_gl_context != nullptr);

  ::SDL_GL_DeleteContext(m_gl_context);
}

//------------------------------------------------------------------------------
// Observers
//------------------------------------------------------------------------------

alloy::extra::sdl2_gl_window::context_handle_type
  alloy::extra::sdl2_gl_window::context_handle()
  const noexcept
{
  return m_gl_context;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

void
  alloy::extra::sdl2_gl_window::set_swap_interval( swap_interval interval )
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

void alloy::extra::sdl2_gl_window::do_update()
{
  ::SDL_GL_SwapWindow(sdl2_window::window_handle());
}