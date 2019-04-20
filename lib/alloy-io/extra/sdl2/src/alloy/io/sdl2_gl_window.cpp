#include "alloy/io/sdl2_gl_window.hpp"

#include "alloy/core/assert.hpp"

#include <string>    // std::string
#include <stdexcept> // std::runtime_error
#include <utility>   // std::move

alloy::io::sdl2_gl_window::sdl2_gl_window( const char* title,
                                           int width, int height,
                                           sdl_gl_version version )
  noexcept
  : sdl2_gl_window{
      title,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      width, height
    }
{

}

alloy::io::sdl2_gl_window::sdl2_gl_window( const char* title,
                                           int x, int y,
                                           int width, int height,
                                           sdl_gl_version version )
  noexcept
  : sdl2_window{title, x, y, width, height, SDL_WINDOW_OPENGL},
    m_gl_context{nullptr}
{
  ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, version.major);
  ::SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, version.minor);
  ::SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  m_gl_context = ::SDL_GL_CreateContext(sdl2_window::window_handle());

  if (m_window == nullptr) {
    auto message = std::string{::SDL_GetError()};
    ::SDL_ClearError();

    throw std::runtime_error{std::move(message)};
    // TODO(bitwizeshift): Handle errors better
  }
}

alloy::io::sdl2_gl_window::~sdl2_gl_window()
{
  ALLOY_ASSERT(m_gl_context != nullptr);

  ::SDL_GL_DeleteContext(m_gl_context);
}

alloy::io::sdl2_gl_window::context_handle_type
  alloy::io::sdl2_gl_window::context_handle()
  const noexcept
{
  return m_gl_context;
}

void alloy::io::sdl2_gl_window::set_swap_interval( swap_interval interval )
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

void alloy::io::sdl2_gl_window::do_update()
{
  ::SDL_GL_SwapWindow(sdl2_window::window_handle());
}