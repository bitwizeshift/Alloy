// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file win32_filesystem_monitor.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_EXTRA_WIN32_BINDINGS_WIN32_FILESYSTEM_MONITOR_HPP
#define ALLOY_EXTRA_WIN32_BINDINGS_WIN32_FILESYSTEM_MONITOR_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/extra/win32-bindings/api.hpp"

#include "alloy/platform/events/filesystem_events.hpp"
#include "alloy/platform/filesystem/filesystem_monitor.hpp"
#include "alloy/platform/message_pump.hpp"

#include <memory>      // std::unique_ptr
#include <string_view> // std::string_view

namespace alloy::extra {

  //////////////////////////////////////////////////////////////////////////////
  ///
  ///
  //////////////////////////////////////////////////////////////////////////////
  class win32_filesystem_monitor
    : public platform::filesystem_monitor
  {
    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    // TODO(bitwizeshift): Add allocator support

    /// \brief Constructs a win32_filesystem_monitor
    ALLOY_EXTRA_WIN32_API
    win32_filesystem_monitor();

    win32_filesystem_monitor(win32_filesystem_monitor&&) = delete;
    win32_filesystem_monitor(const win32_filesystem_monitor&) = delete;

    ALLOY_EXTRA_WIN32_API
    ~win32_filesystem_monitor() noexcept override;

    auto operator=(win32_filesystem_monitor&&)
      -> win32_filesystem_monitor& = delete;
    auto operator=(const win32_filesystem_monitor&)
      -> win32_filesystem_monitor& = delete;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    auto do_watch(std::string_view path, bool recursive)
      noexcept -> void override;

    /// \brief Hook for the message pump source to implement
    ///
    /// This will poll the current source for events, pushing events back to the
    /// message_pump \p p
    ///
    /// \param p the message_pump to push new messages to
    auto poll(platform::message_pump& p) noexcept -> void override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    struct impl;
    std::unique_ptr<impl> m_impl;
  };

} // namespace alloy::platform


#endif /* ALLOY_EXTRA_WIN32_BINDINGS_WIN32_FILESYSTEM_MONITOR_HPP */
