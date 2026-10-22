// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file console_log_stream.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_EXTRA_LOGGING_CONSOLE_LOG_STREAM_HPP
#define ALLOY_EXTRA_LOGGING_CONSOLE_LOG_STREAM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/logging/log_level.hpp"
#include "alloy/extra/logging/logger.hpp"


#include <chrono>
#include <string_view>

namespace alloy::extra {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A log stream that logs to console
  //////////////////////////////////////////////////////////////////////////////
  class console_log_stream : public log_stream
  {
    //--------------------------------------------------------------------------
    // Constructors / Assignment
    //--------------------------------------------------------------------------
  public:

    console_log_stream() noexcept = default;
    console_log_stream(console_log_stream&&) = delete;
    console_log_stream(const console_log_stream&) = delete;

    //--------------------------------------------------------------------------

    auto operator=(console_log_stream&&) -> console_log_stream& = delete;
    auto operator=(const console_log_stream&) -> console_log_stream& = delete;

    //--------------------------------------------------------------------------
    // Virtual Hooks: Logging
    //--------------------------------------------------------------------------
  private:

    auto on_log_message(
      std::chrono::system_clock::time_point time,
      log_level level,
      std::string_view message
    ) -> void override;
  };

} // namespace alloy::extra

#endif /* ALLOY_EXTRA_LOGGING_CONSOLE_LOG_STREAM_HPP */
