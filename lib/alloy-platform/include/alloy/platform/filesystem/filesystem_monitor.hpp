// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file filesystem_monitor.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_FILESYSTEM_MONITOR_HPP
#define ALLOY_PLATFORM_FILESYSTEM_FILESYSTEM_MONITOR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/api.hpp"
#include "alloy/platform/message_pump.hpp"

#include <string_view> // std::string_view

namespace alloy::platform {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A monitor that watches filesystem changes, and signals events as
  ///        a pump source.
  ///
  //////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API filesystem_monitor : public message_pump::source
  {
    //--------------------------------------------------------------------------
    // Destructor
    //--------------------------------------------------------------------------
  public:

    virtual ~filesystem_monitor() noexcept = default;

    //--------------------------------------------------------------------------
    // Modifiers
    //--------------------------------------------------------------------------
  public:

    /// \brief Watches the contents of the path \p path for any changes
    ///
    /// \param path the path to watch
    /// \param recursive boolean to determine whether this should be done
    ///                  recursively to all subdirectories/files
    auto watch(std::string_view path, bool recursive = true) noexcept -> void;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    /// \brief Virtual hook for watching the changes of the given \p path
    ///
    /// \param path the path to watch
    /// \param recursive whether to watch files recursively in a path
    virtual auto do_watch(std::string_view path, bool recursive)
      noexcept -> void = 0;

    /// \brief Virtual hook for handling the message pump
    ///
    /// \param p the message_pump to handle the message
    virtual auto poll(message_pump& p) noexcept -> void = 0;
  };

} // namespace alloy::platform



#endif /* ALLOY_PLATFORM_FILESYSTEM_FILESYSTEM_MONITOR_HPP */
