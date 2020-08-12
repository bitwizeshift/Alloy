/*****************************************************************************
 * \file file.hpp
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
#ifndef ALLOY_IO_FILESYSTEM_FILE_STREAM_FINALIZER_HPP
#define ALLOY_IO_FILESYSTEM_FILE_STREAM_FINALIZER_HPP

#include "alloy/io/api.hpp"
#include "alloy/io/filesystem/file_stream.hpp"

namespace alloy::io {

  //===========================================================================
  // class : file_stream_finalizer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A finalizer for a given file stream
  ///
  /// This is a small interface used to handle cleaning up of resources for a
  /// given file stream. Finalizers are always executed when closing a file,
  /// and are used
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Constructor / Assignment
    //-------------------------------------------------------------------------
  public:

    file_stream_finalizer() = default;
    file_stream_finalizer(const file_stream_finalizer&) = default;
    file_stream_finalizer(file_stream_finalizer&&) = default;

    //-------------------------------------------------------------------------

    file_stream_finalizer& operator=(const file_stream_finalizer&) = default;
    file_stream_finalizer& operator=(file_stream_finalizer&&) = default;

    //-------------------------------------------------------------------------
    // Destructor
    //-------------------------------------------------------------------------
  protected:

    // Disallow owning a file_stream_finalizer by a handle to this class
    ~file_stream_finalizer() = default;

    //-------------------------------------------------------------------------
    // Virtual Hooks
    //-------------------------------------------------------------------------
  public:

    /// \brief Finalizes the file stream
    ///
    /// This must perform the required resource cleanup of anything used to
    /// produce the stream on creation.
    ///
    /// \param stream the stream to finalize
    virtual void finalize(file_stream* stream) = 0;
  };

} // namespace alloy::io

#endif /* ALLOY_IO_FILESYSTEM_FILE_STREAM_FINALIZER_HPP */
