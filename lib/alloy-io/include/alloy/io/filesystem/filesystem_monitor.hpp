/*****************************************************************************
 * \file filesystem_monitor.hpp
 *
 * \brief
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
#ifndef ALLOY_IO_FILESYSTEM_FILESYSTEM_MONITOR_HPP
#define ALLOY_IO_FILESYSTEM_FILESYSTEM_MONITOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/io/message_pump.hpp"

#include <string>      // std::string
#include <string_view> // std::string_view

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

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A monitor that watches filesystem changes, and signals events as
  ///        a pump source.
  ///
  //////////////////////////////////////////////////////////////////////////////
  class filesystem_monitor : public message_pump::source
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
    void watch(std::string_view path, bool recursive = true) noexcept;

    //--------------------------------------------------------------------------
    // Hooks
    //--------------------------------------------------------------------------
  private:

    /// \brief Virtual hook for watching the changes of the given \p path
    ///
    /// \param path the path to watch
    /// \param recursive whether to watch files recursively in a path
    virtual void do_watch(std::string_view path, bool recursive) noexcept = 0;

    /// \brief Virtual hook for handling the message pump
    ///
    /// \param p the message_pump to handle the message
    virtual void pump(message_pump& p) noexcept = 0;
  };

} // namespace alloy::io

#endif /* ALLOY_IO_FILESYSTEM_FILESYSTEM_MONITOR_HPP */