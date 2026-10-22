// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file file_log_stream.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_EXTRA_LOGGING_FILE_LOG_STREAM_HPP
#define ALLOY_EXTRA_LOGGING_FILE_LOG_STREAM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/logging/log_level.hpp"
#include "alloy/extra/logging/logger.hpp"
#include "alloy/platform/filesystem/file.hpp"


#include <chrono>
#include <string_view>

namespace alloy::extra {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A log stream that logs to the specified file
  //////////////////////////////////////////////////////////////////////////////
  class file_log_stream : public log_stream
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    /// \brief Constructs this file_log_stream from the file to log to
    ///
    /// \param destination the destination to write all log calls to
    explicit file_log_stream(platform::file destination) noexcept;
    file_log_stream(file_log_stream&&) = delete;
    file_log_stream(const file_log_stream&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(file_log_stream&&) -> file_log_stream& = delete;
    auto operator=(const file_log_stream&) -> file_log_stream& = delete;

    //--------------------------------------------------------------------------
    // Virtual Hooks: Logging
    //--------------------------------------------------------------------------
  private:

    /// \brief Logs the message to the underlying file
    ///
    /// \param time the time of the logging
    /// \param level the log level of the log message
    /// \param message the message to write
    auto on_log_message(
      std::chrono::system_clock::time_point time,
      log_level level,
      std::string_view message
    ) -> void override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    platform::file m_destination;
  };

} // namespace alloy::extra

#endif /* ALLOY_EXTRA_LOGGING_FILE_LOG_STREAM_HPP */
