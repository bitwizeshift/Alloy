/*****************************************************************************
 * \file null_filesystem.hpp
 *
 * \brief This header contains the definition of a filesystem that only
 *        produces files that behave like /dev/null
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
#ifndef ALLOY_IO_FILESYSTEM_NULL_FILESYSTEM_HPP
#define ALLOY_IO_FILESYSTEM_NULL_FILESYSTEM_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/io/filesystem/file.hpp"

namespace alloy::io {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A filesystem that only produces null files
  ///
  /// Null files produce no-ops for reading and writing, but always complete
  /// successfully. In this regard, they are much like reading/writing to
  /// /dev/null on *nix systems.
  /////////////////////////////////////////////////////////////////////////////
  class null_filesystem
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    null_filesystem() noexcept = default;
    null_filesystem(null_filesystem&&) = delete;
    null_filesystem(const null_filesystem&) = delete;

    //-------------------------------------------------------------------------

    null_filesystem& operator=(null_filesystem&&) = delete;
    null_filesystem& operator=(const null_filesystem&) = delete;

    //-------------------------------------------------------------------------
    // Filesystem
    //-------------------------------------------------------------------------
  public:

    /// \brief Opens a null file
    ///
    /// \return the file
    file open() noexcept;
  };

} // namespace alloy::io

#endif /* ALLOY_IO_FILESYSTEM_NULL_FILESYSTEM_HPP */