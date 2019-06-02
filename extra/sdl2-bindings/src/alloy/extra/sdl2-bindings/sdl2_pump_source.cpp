#include "alloy/extra/sdl2-bindings/sdl2_pump_source.hpp"
#include "alloy/extra/sdl2-bindings/sdl2_window.hpp"

#include "alloy/core/assert.hpp"

#include <SDL2/SDL.h>

//------------------------------------------------------------------------------
// Static Functions
//------------------------------------------------------------------------------

alloy::extra::sdl2_pump_source&
  alloy::extra::sdl2_pump_source::instance()
{
  static auto s_instance = sdl2_pump_source{};

  return s_instance;
}

//------------------------------------------------------------------------------
// Modifiers
//------------------------------------------------------------------------------

void alloy::extra::sdl2_pump_source
  ::attach_window( core::not_null<sdl2_window*> window )
{
  const auto id = ::SDL_GetWindowID(window->window_handle());
  m_windows[static_cast<unsigned>(id)] = window.get();
}

void alloy::extra::sdl2_pump_source
  ::detach_window( core::not_null<sdl2_window*> window )
{
  const auto id = ::SDL_GetWindowID(window->window_handle());
  const auto it = m_windows.find(static_cast<unsigned>(id));

  ALLOY_ASSERT( it != m_windows.end() );

  m_windows.erase(it);
}

//------------------------------------------------------------------------------
// Hooks
//------------------------------------------------------------------------------

void alloy::extra::sdl2_pump_source::pump(io::message_pump& p)
  noexcept
{
  auto event = ::SDL_Event{};

  // Poll all events and translate them
  while (::SDL_PollEvent(&event)) {
    if (event.type == SDL_WINDOWEVENT) {
      handle_window_event(event, p);
    }
  }
}

void alloy::extra::sdl2_pump_source
  ::handle_window_event( const ::SDL_Event& event, io::message_pump& p )
  noexcept
{
  const auto id = event.window.windowID;
  const auto it = m_windows.find(id);

  // Ignore this?
  if (it == m_windows.end()) {
    return;
  }

  const auto window = it->second;

  switch (event.window.event) {

    case SDL_WINDOWEVENT_CLOSE: {
      p.post_event( io::window_close_event{window} );
      break;
    }

    case SDL_WINDOWEVENT_MINIMIZED: {
      p.post_event( io::window_minimize_event{window} );
      break;
    }
    case SDL_WINDOWEVENT_MAXIMIZED: {
      p.post_event( io::window_maximize_event{window} );
      break;
    }
    case SDL_WINDOWEVENT_RESTORED: {
      p.post_event( io::window_restore_event{window} );
      break;
    }

    case SDL_WINDOWEVENT_HIDDEN: {
      p.post_event( io::window_hide_event{window} );
      break;
    }
    case SDL_WINDOWEVENT_EXPOSED: {
      // TODO(bitwizeshift):
      //   only fire this event if window was hidden before this change
      p.post_event( io::window_show_event{window} );
      break;
    }

    case SDL_WINDOWEVENT_MOVED: {
      const auto x = event.window.data1;
      const auto y = event.window.data2;

      p.post_event( io::window_move_event{window, x, y} );
      break;
    }
    case SDL_WINDOWEVENT_SIZE_CHANGED: {
      const auto width = event.window.data1;
      const auto height = event.window.data2;

      p.post_event( io::window_resize_event{window, width, height} );
      break;
    }

    case SDL_WINDOWEVENT_FOCUS_GAINED: {
      p.post_event( io::window_focus_event{window} );
      break;
    }
#if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_WINDOWEVENT_TAKE_FOCUS: {
      p.post_event( io::window_focus_event{window} );
      break;
    }
#endif
    case SDL_WINDOWEVENT_FOCUS_LOST: {
      p.post_event( io::window_unfocus_event{window} );
      break;
    }
    default: {
      break;
    }
  }
}