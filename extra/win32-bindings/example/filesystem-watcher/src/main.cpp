// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include <alloy/core/intrinsics.hpp>
#include <alloy/core/utilities/signal.hpp>

#include <alloy/extra/win32-bindings/win32_filesystem_monitor.hpp>

#include <alloy/platform/filesystem/filesystem_monitor.hpp>
#include <alloy/platform/message_pump.hpp>

#include <iostream>

namespace {
  auto print_message(const alloy::platform::event& e) -> void
  {
    switch (e.id()) {
      case alloy::platform::event::id_of<alloy::platform::file_creation_event>(): {
        auto event = e.try_as<alloy::platform::file_creation_event>();
        std::cout << "file_creation_event{" << event->name << "}" << std::endl;
        break;
      }

      case alloy::platform::event::id_of<alloy::platform::file_removal_event>(): {
        auto event = e.try_as<alloy::platform::file_removal_event>();
        std::cout << "file_removal_event{" << event->name << "}" << std::endl;
        break;
      }

      case alloy::platform::event::id_of<alloy::platform::file_update_event>(): {
        auto event = e.try_as<alloy::platform::file_update_event>();
        std::cout << "file_update_event{" << event->name << "}" << std::endl;
        break;
      }

      // The filesystem monitor is the only source connected to the pump, and
      // it only posts the events above.
      default:
        ALLOY_UNREACHABLE();
    }
  }

  class listener : public alloy::platform::message_pump::listener
  {
    int* m_pcount;

  public:

    explicit listener(int* pcount)
      : m_pcount{pcount} {}

  private:

    auto handle_message(const alloy::platform::event& e) noexcept -> void
    {
      ::print_message(e);
      (*m_pcount)++;
    }

    auto handle_immediate_message(const alloy::platform::event& e) noexcept -> void
    {
      ::print_message(e);
      (*m_pcount)++;
    }
  };
} // anonymous namespace

auto main(int argc, char** argv) -> int
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <directory to watch>" << std::endl;
    return 1;
  }

  const auto* directory = argv[1];

  std::cout << "Watching '" << directory << "'" << std::endl;

  auto count = int{0};
  auto listener = ::listener{&count};
  auto fs_monitor = alloy::extra::win32_filesystem_monitor{};
  auto message_pump = alloy::platform::message_pump{};

  const auto source_conn = alloy::core::scoped_connection{
    message_pump.on_poll().connect(&fs_monitor)
  };
  const auto listener_conn = alloy::core::scoped_connection{
    message_pump.on_event().connect(&listener)
  };

  fs_monitor.watch(directory);

  // pump exactly 10 events (count is incremented by listener)
  while (count < 10){
    message_pump.pump();
  }

  return 0;
}
