/*****************************************************************************
 * \file win32_filesystem_monitor.hpp
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
#ifndef ALLOY_IO_FILESYSTEM_WIN32_FILESYSTEM_MONITOR_HPP
#define ALLOY_IO_FILESYSTEM_WIN32_FILESYSTEM_MONITOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

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
    win32_filesystem_monitor();

    win32_filesystem_monitor( win32_filesystem_monitor&& other ) noexcept;
    win32_filesystem_monitor( const win32_filesystem_monitor& other ) = delete;

    virtual ~win32_filesystem_monitor() noexcept;

    win32_filesystem_monitor& operator=( win32_filesystem_monitor&& other ) noexcept;
    win32_filesystem_monitor& operator=( const win32_filesystem_monitor& other ) = delete;

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


#endif /* ALLOY_IO_FILESYSTEM_WIN32_FILESYSTEM_MONITOR_HPP */