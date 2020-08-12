/*****************************************************************************
 * \file random_filesystem.hpp
 *
 * \brief This header contains the definition of a filesystem that only
 *        produces files that behave like /dev/random
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
#ifndef ALLOY_IO_FILESYSTEM_RANDOM_FILESYSTEM_HPP
#define ALLOY_IO_FILESYSTEM_RANDOM_FILESYSTEM_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "alloy/io/api.hpp"
#include "alloy/io/filesystem/file.hpp"

namespace alloy::io {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A filesystem that only produces random files
  ///
  /// Random files behave much like /dev/random, in that reading and writing
  /// will always succeed, and reading will always return random bytes
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API random_filesystem
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    random_filesystem() noexcept = default;
    random_filesystem(random_filesystem&&) = delete;
    random_filesystem(const random_filesystem&) = delete;

    //-------------------------------------------------------------------------

    random_filesystem& operator=(random_filesystem&&) = delete;
    random_filesystem& operator=(const random_filesystem&) = delete;

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

#endif /* ALLOY_IO_FILESYSTEM_RANDOM_FILESYSTEM_HPP */
