#include "alloy/extra/logging/file_log_stream.hpp"

#include "alloy/io/buffers/const_buffer.hpp"

#include <array>  // std::array
#include <ctime>  // std::localtime
#include <utility> // std::move

namespace {

  std::string_view to_string(alloy::extra::log_level level)
    noexcept
  {
    using log_level = alloy::extra::log_level;
    using std::literals::string_view_literals::operator""sv;

    switch (level) {
      case log_level::debug: {
        return "D"sv;
      }
      case log_level::info: {
        return "I"sv;
      }
      case log_level::warning: {
        return "W"sv;
      }
      case log_level::error: {
        return "E"sv;
      }
      case log_level::fatal: {
        return "F"sv;
      }
    }

    alloy::core::compiler::unreachable();
    return "unknown"sv;
  }
} // namespace <anonymous>


alloy::extra::file_log_stream::file_log_stream(io::file destination)
  noexcept
  : m_destination{std::move(destination)}
{

}

void alloy::extra::file_log_stream
  ::log_message(std::chrono::system_clock::time_point time,
                log_level level,
                std::string_view message)
{
  auto buffer = std::array<char,32>{};

  const auto t = std::chrono::system_clock::to_time_t(time);
  const auto tm = *std::localtime(&t);
  const auto bytes = std::strftime(
    buffer.data(),
    buffer.size(),
    "%Y-%m-%d %H:%M:%S",
    &tm
  );

  const auto time_string = std::string_view{buffer.data(), bytes};
  const auto level_string = ::to_string(level);

  // write time
  m_destination.write(io::const_buffer::from_container(time_string));
  m_destination.write_object('\t');

  // write level
  m_destination.write(io::const_buffer::from_container(level_string));
  m_destination.write_object('\t');

  // write message
  m_destination.write(io::const_buffer::from_container(message));
  m_destination.write_object('\n');

  // We never want logging to buffer, in case a crash occurs.
  m_destination.flush();
}
