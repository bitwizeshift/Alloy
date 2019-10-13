#include "alloy/extra/logging/console_log_stream.hpp"
#include "alloy/core/intrinsics.hpp"

#include <array>  // std::array
#include <ctime>  // std::localtime
#include <cstdio> // std::printf

namespace {

  std::string_view to_string(alloy::extra::log_level level)
    noexcept
  {
    using log_level = alloy::extra::log_level;
    using std::literals::string_view_literals::operator""sv;

    switch (level) {
      case log_level::debug: {
        return "debug"sv;
      }
      case log_level::info: {
        return "info"sv;
      }
      case log_level::warning: {
        return "warning"sv;
      }
      case log_level::error: {
        return "error"sv;
      }
      case log_level::fatal: {
        return "fatal"sv;
      }
    }

    alloy::core::compiler::unreachable();
    return "unknown"sv;
  }
} // namespace <anonymous>

void alloy::extra::console_log_stream
  ::log_message(std::chrono::system_clock::time_point time,
                log_level level,
                std::string_view message)
{
  const auto t = std::chrono::system_clock::to_time_t(time);
  const auto tm = *std::localtime(&t);
  auto buffer = std::array<char,32>{};

  const auto byte = std::strftime(
    buffer.data(),
    buffer.size(),
    "%Y-%m-%d %H:%M:%S",
    &tm
  );
  buffer[byte] = '\0';
  const auto level_string = ::to_string(level);

  // print format is:
  // time | level | message
  std::printf(
    "%s | %-7s | %.*s\n",
    buffer.data(),
    level_string.data(),
    static_cast<int>(message.size()),
    message.data()
  );
}
