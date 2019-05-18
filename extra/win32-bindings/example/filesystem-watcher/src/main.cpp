#include <alloy/io/message_pump.hpp>
#include <alloy/io/filesystem/filesystem_monitor.hpp>
#include <alloy/extra/win32-bindings/win32_filesystem_monitor.hpp>

#include <iostream>

namespace {
  void print_message( const alloy::io::event& e )
  {
    if (auto event = e.try_as<alloy::io::file_creation_event>()) {
      std::cout << "file_creation_event{" << event->name << "}" << std::endl;
    } else if (auto event = e.try_as<alloy::io::file_removal_event>()) {
      std::cout << "file_removal_event{" << event->name << "}" << std::endl;
    } else if (auto event = e.try_as<alloy::io::file_update_event>()) {
      std::cout << "file_update_event{" << event->name << "}" << std::endl;
    }
  }

  class listener : public alloy::io::message_pump::listener
  {
    int* m_pcount;

  public:

    explicit listener(int* pcount) : m_pcount{pcount} {}

  private:

    void handle_message( const alloy::io::event& e ) noexcept
    {
      ::print_message(e);
      (*m_pcount)++;
    }

    void handle_immediate_message( const alloy::io::event& e ) noexcept
    {
      ::print_message(e);
      (*m_pcount)++;
    }
  };
} // anonymous namespace

int main(int argc, char** argv)
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
  auto message_pump = alloy::io::message_pump{};

  message_pump.register_pump_source(&fs_monitor);
  message_pump.register_listener(&listener);
  fs_monitor.watch(directory);

  // pump exactly 10 events (count is incremented by listener)
  while (count < 10){
    message_pump.pump();
  }

  message_pump.unregister_listener(&listener);
  message_pump.unregister_pump_source(&fs_monitor);

  return 0;
}
