#include <alloy/extra/logging/file_log_stream.hpp>
#include <alloy/extra/logging/console_log_stream.hpp>
#include <alloy/extra/logging/logger.hpp>

#include <alloy/io/filesystem/disk_filesystem.hpp>
#include <alloy/io/buffers/mutable_buffer.hpp>

#include <thread>
#include <chrono>

namespace {
  /// A buffer used for the string formatting in the logger
  /// The buffer size is directly correlated to the length of formatted
  /// message that may be written per log statement.
  auto format_buffer = std::array<char,1024>{};
}

int main()
{
  auto disk_filesystem = alloy::io::disk_filesystem{};
  auto log_file = disk_filesystem.open("log.txt", alloy::io::open_mode::write);

  // Construct the logger
  auto logger = alloy::extra::logger{
    alloy::io::mutable_buffer::from_container(::format_buffer)
  };

  // Attach a console logger and a file logger
  auto log1 = alloy::extra::console_log_stream{};
  auto log2 = alloy::extra::file_log_stream{std::move(log_file)};

  logger.attach(&log1);
  logger.attach(&log2);

  auto message = std::string{};
  for (auto i = 0; i < 3; ++i) {
    message += std::to_string(i);

    logger.debug("{}: '{}'", "Test debug", message);
    logger.info("{}: '{}'", "Test info", message);
    logger.warn("{}: '{}'", "Test warning", message);
    logger.error("{}: '{}'", "Test error", message);
    logger.fatal("{}: '{}'", "Test fatal", message);

    std::this_thread::sleep_for(std::chrono::milliseconds{500});
  }

  { // new scope
    // attach a second console logger
    auto log3 = alloy::extra::console_log_stream{};

    logger.attach(&log3);

    logger.debug("testing scope with multiple loggers");

    std::this_thread::sleep_for(std::chrono::seconds{1});
  }
}
