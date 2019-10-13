/*****************************************************************************
 * \file file_log_stream.hpp
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
#ifndef ALLOY_EXTRA_LOGGING_FILE_LOG_STREAM_HPP
#define ALLOY_EXTRA_LOGGING_FILE_LOG_STREAM_HPP

#include "alloy/extra/logging/logger.hpp"
#include "alloy/io/filesystem/file.hpp"

#include "alloy/core/utilities/not_null.hpp"

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
    explicit file_log_stream(io::file destination) noexcept;
    file_log_stream(file_log_stream&&) = delete;
    file_log_stream(const file_log_stream&) = delete;

    //--------------------------------------------------------------------------

    file_log_stream& operator=(file_log_stream&&) = delete;
    file_log_stream& operator=(const file_log_stream&) = delete;

    //--------------------------------------------------------------------------
    // Logging
    //--------------------------------------------------------------------------
  public:

    /// \brief Logs the message to the underlying file
    ///
    /// \param time the time of the logging
    /// \param level the log level of the log message
    /// \param message the message to write
    void log_message(std::chrono::system_clock::time_point time,
                     log_level level,
                     std::string_view message) override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    io::file m_destination;
  };

} // namespace alloy::extra

#endif /* ALLOY_EXTRA_LOGGING_FILE_LOG_STREAM_HPP */