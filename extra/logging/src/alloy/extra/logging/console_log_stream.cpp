#include "alloy/extra/logging/console_log_stream.hpp"
#include "alloy/core/intrinsics.hpp"

#include <array>  // std::array
#include <ctime>  // std::localtime
#include <cstdio> // std::printf

namespace alloy::extra {
namespace {

  /// \brief Gets a reset code. On *nix systems, this gets the ANSI code. On
  ///        Windows, this is empty.
  ///
  /// \return the code
  constexpr auto get_reset()
    noexcept -> const char*
  {
#if defined(__linux__)
    return "\u001b[0m";
#else
    return "";
#endif
  }

  /// \brief Converts the log \p level into an ANSI color code to use for logging
  ///
  /// On *nix this returns the appropriate color code, on Windows this returns
  /// an empty string
  ///
  /// \return the code
  auto to_color_code(log_level level)
    noexcept -> const char*
  {
#if defined(__linux__)
    switch (level) {
      case log_level::debug: {
        return "\u001b[96m";
      }
      case log_level::info: {
        return "\u001b[92m";
      }
      case log_level::warning: {
        return "\u001b[93m";
      }
      case log_level::error: {
        return "\u001b[91m";
      }
      case log_level::fatal: {
        return "\u001b[97;41m";
      }
    }
    return "";
#else
    return "";
#endif
  }

  auto to_string(log_level level)
    noexcept -> std::string_view
  {
    switch (level) {
      case log_level::debug: {
        return "debug";
      }
      case log_level::info: {
        return "info";
      }
      case log_level::warning: {
        return "warning";
      }
      case log_level::error: {
        return "error";
      }
      case log_level::fatal: {
        return "fatal";
      }
    }

    return "unknown";
  }
} // namespace <anonymous>
} // namespace alloy::extra

auto alloy::extra::console_log_stream::on_log_message(
  std::chrono::system_clock::time_point time,
  log_level level,
  std::string_view message
) -> void
{
  static constexpr auto reset_code = get_reset();

  const auto t = std::chrono::system_clock::to_time_t(time);
  const auto tm = *std::localtime(&t);
  auto buffer = std::array<char,32>{};

  const auto bytes = std::strftime(
    buffer.data(),
    buffer.size(),
    "%Y-%m-%d %H:%M:%S",
    &tm
  );
  const auto time_string = std::string_view{buffer.data(), bytes};
  const auto level_string = to_string(level);
  const auto* const color_code = to_color_code(level);

  // print format is:
  // time | level | message
  std::printf(
    "%s%.*s | %-7s | %.*s%s\n",
    color_code,
    static_cast<int>(time_string.size()),
    time_string.data(),
    level_string.data(),
    static_cast<int>(message.size()),
    message.data(),
    reset_code
  );
  std::fflush(stdout);
}
