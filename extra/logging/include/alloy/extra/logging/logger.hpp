/*****************************************************************************
 * \file logger.hpp
 *
 * \todo description
 *****************************************************************************/

/*
  The MIT License (MIT)

  Copyright (c) 2019 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef ALLOY_EXTRA_LOGGING_LOGGER_HPP
#define ALLOY_EXTRA_LOGGING_LOGGER_HPP

#include "alloy/io/buffers/mutable_buffer.hpp"
#include "alloy/core/utilities/not_null.hpp"
#include "alloy/core/utilities/delegate.hpp"
#include "alloy/core/intrinsics.hpp"

#include <fmt/format.h>

#include <cstdint>     // std::uint8_t
#include <string_view> // std::string_views
#include <optional>    // std::optional
#include <chrono>      // std::chrono

namespace alloy::extra {

  //============================================================================
  // enum class : log_level
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The severity of a log statement
  //////////////////////////////////////////////////////////////////////////////
  enum class log_level : std::uint8_t
  {
    debug,   ///< Indicates a message for developer debugging
    info,    ///< Indicates a control flow message (for general info)
    warning, ///< Indicates a recoverable bad state occurred
    error,   ///< Indicates a recoverable error has occurred
    fatal,   ///< Indicates an unrecoverable error has occurred. Should be
             ///< followed by some form of program termination
  };

  //============================================================================
  // class : log_stream
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A stream that is logged to
  ///
  /// Log streams are not useful on their own; they should be bound to a logger
  /// instance.
  ///
  /// Example:
  /// \code
  /// auto format_buffer = std::array<char,256>{};
  /// auto buffer = alloy::io::mutable_buffer::from_container(format_buffer);
  ///
  /// auto log = alloy::extra::console_log_stream{};
  /// auto logger = alloy::extra::logger{buffer};
  ///
  /// logger.attach(&log); // attach this log stream to the logger
  ///
  /// logger.warn(...); // logging to the logger logs to the log stream
  /// \endcode
  ///
  /// All log streams will use RAII to automatically detach from the connected
  /// logger on completion.
  ///
  /// \note It is an error to attach a log stream to multiple loggers.
  //////////////////////////////////////////////////////////////////////////////
  class log_stream
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    log_stream() noexcept;
    log_stream(log_stream&&) = delete;
    log_stream(const log_stream&) = delete;

    //--------------------------------------------------------------------------

    log_stream& operator=(log_stream&&) = delete;
    log_stream& operator=(const log_stream&) = delete;

    //--------------------------------------------------------------------------
    // Destructors
    //--------------------------------------------------------------------------
  protected:

    ~log_stream();

    //--------------------------------------------------------------------------
    // Virtual Hooks : Logging
    //--------------------------------------------------------------------------
  public:

    /// \brief Logs a message with the specified metadata
    ///
    /// \param time the time that the logging occurred at
    /// \param level the log level of this message
    /// \param message the message itself
    virtual void log_message(std::chrono::system_clock::time_point time,
                             log_level level,
                             std::string_view message) = 0;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    friend class logger;

    using detach_function = core::delegate<void(core::not_null<log_stream*>)>;

    log_stream* m_next;
    detach_function m_detach;
  };

  //============================================================================
  // class : logger
  //============================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief The main logger class
  ///
  /// The logger will format entries and log messages back to any attached
  /// log streams. Since log streams use RAII, this allows log stream
  /// destinations to be temporarily added to given scopes -- allowing for
  /// better debugability.
  ///
  /// Example:
  /// \code
  /// auto logger = alloy::extra::logger{...};
  /// { // first log scope
  ///   auto log = ... some log stream type ...
  ///   logger.attach(&log);
  ///
  ///   logger.info(...); // logs to only attached log stream
  ///
  ///   { // second log scope
  ///     auto log = ... some other log stream type for this scope
  ///     logger.attach(&log);
  ///
  ///     logger.error(...); // logs to both attached logs
  ///   } // scope ends, no more log
  ///
  ///   logger.info(...); // logs to first attached log stream only
  /// }
  ///
  /// \endcode
  ///
  /// The log streams are intrinsically linked together, so that the overhead
  /// of connecting arbitrary loggers is minimal.
  ///
  /// This logger uses the C++ 'fmt' library to provide python-style format
  /// strings with minimal overhead and type-safety/flexibility. For more
  /// information on the formatting, please refer to their documentation at
  /// https://fmt.dev/5.3.0/
  ///
  /// \note It is an error to attach a log stream to multiple logger instances
  //////////////////////////////////////////////////////////////////////////////
  class logger final
  {
    //--------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs a logger that uses the specified \p buffer for
    ///        formatting messages
    ///
    /// \param buffer the buffer to use for formatting messages
    explicit logger(io::mutable_buffer buffer) noexcept;

    /// \brief Move constructor
    ///
    /// \param other the other logger to move
    logger(logger&& other) noexcept;
    logger(const logger&) = delete;

    //--------------------------------------------------------------------------

    ~logger();

    //--------------------------------------------------------------------------

    /// \brief Move assignment operator
    ///
    /// \param other the other logger to move
    /// \return reference to \c (*this)
    logger& operator=(logger&& other) noexcept;
    logger& operator=(const logger&) = delete;

    //--------------------------------------------------------------------------
    // Binding
    //--------------------------------------------------------------------------
  public:

    /// \brief Attaches a log stream to this logger
    ///
    /// The attached log stream will now be a target that is logged to when
    /// logger messages are posted
    ///
    /// \param log the log stream to attach
    void attach(core::not_null<log_stream*> log) noexcept;

    /// \brief Detaches a log stream from this logger
    ///
    /// \pre \p log must have already been attached
    ///
    /// \param log the log to detach
    void detach(core::not_null<log_stream*> log) noexcept;

    /// \brief Detaches all log streams from this logger
    void detach_all() noexcept;

    //--------------------------------------------------------------------------
    // Logging
    //--------------------------------------------------------------------------
  public:

    /// \brief Logs a formatted message
    ///
    /// The syntax of the format strings can be found at https://fmt.dev/5.3.0/
    ///
    /// \param level the log level
    /// \param format the format string for the log
    /// \param args the arguments for formatting
    template <typename...Args>
    void log(log_level level,
             std::string_view format,
             Args&&...args);

    //--------------------------------------------------------------------------

    /// \brief Logs a formatted debug message
    ///
    /// The syntax of the format strings can be found at https://fmt.dev/5.3.0/
    ///
    /// \param format the format of the string
    /// \param args the arguments for formatting
    template <typename...Args>
    void debug(std::string_view format,
               Args&&...args);

    /// \brief Logs a formatted info message
    ///
    /// The syntax of the format strings can be found at https://fmt.dev/5.3.0/
    ///
    /// \param format the format of the string
    /// \param args the arguments for formatting
    template <typename...Args>
    void info(std::string_view format,
              Args&&...args);

    /// \brief Logs a formatted warning message
    ///
    /// The syntax of the format strings can be found at https://fmt.dev/5.3.0/
    ///
    /// \param format the format of the string
    /// \param args the arguments for formatting
    template <typename...Args>
    void warn(std::string_view format,
              Args&&...args);

    /// \brief Logs a formatted error message
    ///
    /// The syntax of the format strings can be found at https://fmt.dev/5.3.0/
    ///
    /// \param format the format of the string
    /// \param args the arguments for formatting
    template <typename...Args>
    void error(std::string_view format,
               Args&&...args);

    /// \brief Logs a formatted fatal message
    ///
    /// The syntax of the format strings can be found at https://fmt.dev/5.3.0/
    ///
    /// \param format the format of the string
    /// \param args the arguments for formatting
    template <typename...Args>
    void fatal(std::string_view format,
               Args&&...args);

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    log_stream* m_head;          ///< The head of the intrinsic logger list
    io::mutable_buffer m_buffer; ///< The buffer to use for formatted strings
  };
} // namespace alloy::extra

//==============================================================================
// class : logger
//==============================================================================

//------------------------------------------------------------------------------
// Logging
//------------------------------------------------------------------------------

template <typename...Args>
void alloy::extra::logger::log(log_level level,
                               std::string_view format,
                               Args&&...args)
{
  // If we don't have any log streams attached, don't bother formatting the
  // log statement.
  if (m_head == nullptr) {
    return;
  }

  const auto time = std::chrono::system_clock::now();

  // use 'fmt' library for log formatting
  const auto [it,size] = fmt::format_to_n(
    reinterpret_cast<char*>(m_buffer.data()),
    m_buffer.size(),
    format,
    std::forward<Args>(args)...
  );
  core::compiler::unused(it);

  const auto message = std::string_view{
    reinterpret_cast<const char*>(m_buffer.data()),
    size
  };

  auto* current = m_head;
  while (current != nullptr) {
    current->log_message(
      time,
      level,
      message
    );
    current = current->m_next;
  }
}

//------------------------------------------------------------------------------

template<typename...Args>
inline void alloy::extra::logger::debug(std::string_view format,
                                        Args&&...args)
{
  log(log_level::debug, format, std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------

template<typename...Args>
inline void alloy::extra::logger::info(std::string_view format,
                                       Args&&...args)
{
  log(log_level::info, format, std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------

template<typename...Args>
inline void alloy::extra::logger::warn(std::string_view format,
                                       Args&&...args)
{
  log(log_level::warning, format, std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------

template<typename...Args>
inline void alloy::extra::logger::error(std::string_view format,
                                        Args&&...args)
{
  log(log_level::error, format, std::forward<Args>(args)...);
}

//------------------------------------------------------------------------------

template<typename...Args>
inline void alloy::extra::logger::fatal(std::string_view format,
                                         Args&&...args)
{
  log(log_level::fatal, format, std::forward<Args>(args)...);
}

#endif /* ALLOY_EXTRA_LOGGING_LOGGER_HPP */