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
#include "alloy/io/events/filesystem_events.hpp"
#include "alloy/io/filesystem/filesystem_monitor.hpp"
#include "alloy/io/message_pump.hpp"

#include <string_view> // std::string_view
#include <memory>      // std::unique_ptr

namespace alloy::extra {

  //////////////////////////////////////////////////////////////////////////////
  ///
  ///
  //////////////////////////////////////////////////////////////////////////////
  class win32_filesystem_monitor
    : public io::filesystem_monitor
  {
    //--------------------------------------------------------------------------
    // Constructors / Destructor / Assignment
    //--------------------------------------------------------------------------
  public:

    // TODO(bitwizeshift): Add allocator support

    /// \brief Constructs a win32_filesystem_monitor
    ALLOY_EXTRA_WIN32_API
    win32_filesystem_monitor();

    win32_filesystem_monitor( win32_filesystem_monitor&& ) = delete;
    win32_filesystem_monitor( const win32_filesystem_monitor& ) = delete;

    ALLOY_EXTRA_WIN32_API
    ~win32_filesystem_monitor() noexcept override;

    win32_filesystem_monitor& operator=( win32_filesystem_monitor&& ) = delete;
    win32_filesystem_monitor& operator=( const win32_filesystem_monitor& ) = delete;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    void do_watch(std::string_view path, bool recursive) noexcept override;

    /// \brief Hook for the message pump source to implement
    ///
    /// This will poll the current source for events, pushing events back to the
    /// message_pump \p p
    ///
    /// \param p the message_pump to push new messages to
    void poll(io::message_pump& p) noexcept override;

    //--------------------------------------------------------------------------
    // Private Members
    //--------------------------------------------------------------------------
  private:

    struct impl;
    std::unique_ptr<impl> m_impl;
  };

} // namespace alloy::io


#endif /* ALLOY_EXTRA_WIN32_BINDINGS_WIN32_FILESYSTEM_MONITOR_HPP */
