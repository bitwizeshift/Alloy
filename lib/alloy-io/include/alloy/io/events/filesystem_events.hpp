/*****************************************************************************
 * \file filesystem_events.hpp
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
#ifndef ALLOY_IO_FILESYSTEM_FILESYSTEM_EVENTS_HPP
#define ALLOY_IO_FILESYSTEM_FILESYSTEM_EVENTS_HPP

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

#endif /* ALLOY_IO_FILESYSTEM_FILESYSTEM_EVENTS_HPP */