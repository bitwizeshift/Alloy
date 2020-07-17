#include <alloy/core/utilities/signal.hpp>
#include <alloy/core/intrinsics.hpp>
#include <alloy/io/message_pump.hpp>
#include <alloy/io/window.hpp>
#include <alloy/extra/sdl2-bindings/sdl2_gl_window.hpp>
#include <alloy/extra/sdl2-bindings/sdl2_pump_source.hpp>

#include <SDL2/SDL.h> // ::SDL_Init, ::SDL_Quit, etc
#include <gl/GL.h>    // ::glClear, ::glClearColor

#include <iostream> // std::cout, std::endl
#include <chrono>   // std::chrono::steady_clock
#include <random>   // std::random

namespace {

  // Flag to keep the application running
  bool g_keep_running = true;

  //////////////////////////////////////////////////////////////////////////////
  /// A simple listener that handles window events
  //////////////////////////////////////////////////////////////////////////////
  class window_listener
    : public alloy::io::message_pump::listener
  {
  public:
    void handle_message(const alloy::io::event& e) noexcept override;
  };

  void window_listener::handle_message(const alloy::io::event& e)
    noexcept
  {
    if (e.is<alloy::io::window_show_event>()) {
      std::cout << "window_show_event{}" << std::endl;
    } else if (e.is<alloy::io::window_hide_event>()) {
      std::cout << "window_hide_event{}" << std::endl;
    } else if (auto* event = e.try_as<alloy::io::window_move_event>()) {
      std::cout << "window_move_event{" << event->x << ", " << event->y << "}" << std::endl;
    } else if (auto* event = e.try_as<alloy::io::window_resize_event>()) {
      std::cout << "window_resize_event{" << event->width << ", " << event->height << "}" << std::endl;
    } else if (e.is<alloy::io::window_minimize_event>()) {
      std::cout << "window_minimize_event{}" << std::endl;
    } else if (e.is<alloy::io::window_maximize_event>()) {
      std::cout << "window_maximize_event{}" << std::endl;
    } else if (e.is<alloy::io::window_close_event>()) {
      std::cout << "window_close_event{}" << std::endl;
      ::g_keep_running = false;
    }
  }
}

int main(int argc, char** argv)
{
  alloy::core::compiler::unused(argc);
  alloy::core::compiler::unused(argv);
  ::SDL_Init(0);
  ::SDL_InitSubSystem( SDL_INIT_VIDEO );

  // random number generator to generate random color frames per second
  auto random_device = std::random_device{};
  auto generator     = std::mt19937{random_device()};
  auto distribution  = std::uniform_real_distribution<float>{0.0f, 1.0f};

  auto message_pump = alloy::io::message_pump{};
  auto listener = ::window_listener{};
  auto& sdl2_pump_source = alloy::extra::sdl2_pump_source::instance();
  auto window = alloy::extra::sdl2_gl_window::from_window_data(
    "Hello World",
    alloy::io::window::dimensions{640, 480},
    alloy::extra::sdl_gl_version{3,2}
  );

  // allow the window to produce events
  sdl2_pump_source.attach_window(&window);

  // Ensure that the pump source and event source are removed at the end of
  // this scope
  const auto source_conn = alloy::core::scoped_connection{
    message_pump.on_poll().connect(&sdl2_pump_source)
  };
  const auto listener_conn = alloy::core::scoped_connection{
    message_pump.on_event().connect(&listener)
  };

  // update loop:
  // * handle i/o by pumping event sources
  // * process new frame (1 fps)
  // * update window
  using clock = std::chrono::steady_clock;

  auto last_frame = clock::now();
  while (::g_keep_running) {
    message_pump.pump();

    const auto now = clock::now();
    if ((now - last_frame) >= 1s) {
      const auto r = distribution(generator);
      const auto g = distribution(generator);
      const auto b = distribution(generator);

      ::glClearColor(r, g, b, 1.0f);
      ::glClear(GL_COLOR_BUFFER_BIT);

      last_frame = now;
      window.update();
    }
  }

  sdl2_pump_source.detach_window(&window);

  ::SDL_QuitSubSystem( SDL_INIT_VIDEO );
  ::SDL_Quit();
  return 0;
}
