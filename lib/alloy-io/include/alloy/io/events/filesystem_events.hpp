// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file filesystem_events.hpp
 *
 * \todo description
 *****************************************************************************/

#ifndef ALLOY_IO_EVENTS_FILESYSTEM_EVENTS_HPP
#define ALLOY_IO_EVENTS_FILESYSTEM_EVENTS_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/io/event.hpp"

#include <string>

namespace alloy::io {

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event signifying the creation of a file
  ///
  /// The event contains the name of the file that was created
  //////////////////////////////////////////////////////////////////////////////
  struct file_creation_event
  {
    std::string name;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event signifying the removal of a file
  ///
  /// The event contains the name of the file that was removed
  //////////////////////////////////////////////////////////////////////////////
  struct file_removal_event
  {
    std::string name;
  };

  //////////////////////////////////////////////////////////////////////////////
  /// \brief An event signifying that a file has been updated
  ///
  /// The event contains the name of the file that was updated
  //////////////////////////////////////////////////////////////////////////////
  struct file_update_event
  {
    std::string name;
  };
} // namespace alloy::io

// Filesystem events reserve a block of 16 IDS, from 0x10 -> 0x1F

ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::file_creation_event, 0x0010);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::file_removal_event, 0x0011);
ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::file_update_event, 0x0012);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0013);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0014);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0015);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0016);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0017);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0018);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x0019);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x001A);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x001B);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x001C);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x001D);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x001E);
// ALLOY_DEFINE_SYSTEM_EVENT(::alloy::io::..., 0x001F);

#endif /* ALLOY_IO_EVENTS_FILESYSTEM_EVENTS_HPP */