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
    virtual void log_message(std::chrono::steady_clock::time_point time,
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
  /// log streams
  ///
  /// The log streams are intrinsically linked together, so that the overhead
  /// of connecting arbitrary loggers is minimal.
  //////////////////////////////////////////////////////////////////////////////
  class logger final
  {
    static constexpr auto max_message_length = 256;

    //--------------------------------------------------------------------------
    // Constructor / Assignment
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
    logger(logger&& other) noexcept = default;
    logger(const logger&) = delete;

    //--------------------------------------------------------------------------

    /// \brief Move assignment operator
    ///
    /// \param other the other logger to move
    /// \return reference to \c (*this)
    logger& operator=(logger&& other) = default;
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
    void attach(core::not_null<log_stream*> log);

    /// \brief Detaches a log stream from this logger
    ///
    /// \pre \p log must have already been attached
    ///
    /// \param log the log to detach
    void detach(core::not_null<log_stream*> log);

    //--------------------------------------------------------------------------
    // Logging
    //--------------------------------------------------------------------------
  public:

    /// \brief Logs a formatted message
    ///
    /// \param level the log level
    /// \param format the format string for the log
    /// \param args the arguments for formatting
    template <typename...Args>
    void log(log_level level,
             std::string_view format,
             Args&&...args);

    //--------------------------------------------------------------------------

    template <typename...Args>
    void debug(std::string_view format,
               Args&&...args);

    template <typename...Args>
    void info(std::string_view format,
              Args&&...args);

    template <typename...Args>
    void warn(std::string_view format,
              Args&&...args);

    template <typename...Args>
    void error(std::string_view format,
               Args&&...args);

    template <typename...Args>
    void fatal(std::string_view format,
               Args&&...args);

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    log_stream* m_head;
  };
} // namespace alloy::extra

//==============================================================================
// macros : logging
//==============================================================================

#define ALLOY_INTERNAL_LOG(l,lvl,...) \
  l.log( \
    ALLOY_CURRENT_SOURCE_LOCATION(), \
    ::alloy::extra::log_level::lvl, \
    __VA_ARGS__ \
  )

#define ALLOY_LOG_DEBUG(l,...) ALLOY_INTERNAL_LOG(l, debug, __VA_ARGS__)
#define ALLOY_LOG_INFO(l,...) ALLOY_INTERNAL_LOG(l, info, __VA_ARGS__)
#define ALLOY_LOG_WARN(l,...) ALLOY_INTERNAL_LOG(l, warn, __VA_ARGS__)
#define ALLOY_LOG_ERROR(l,...) ALLOY_INTERNAL_LOG(l, error, __VA_ARGS__)
#define ALLOY_LOG_SEVERE(l,...) ALLOY_INTERNAL_LOG(l, fatal, __VA_ARGS__)

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
  const auto time = std::chrono::steady_clock::now();

  auto buffer = std::array<char,max_message_length>{};

  // use 'fmt' library for log formatting
  const auto it = fmt::format_to_n(
    buffer.data(),
    buffer.size(),
    format,
    std::forward<Args>(args)...
  );

  const auto message = std::string_view{
    buffer.data(),
    it - buffer.data()
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