// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file file.hpp
 *
 * \brief
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_FINALIZER_HPP
#define ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_FINALIZER_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/filesystem/file_stream.hpp"

#include "alloy/platform/api.hpp"

namespace alloy::platform {

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
  class ALLOY_PLATFORM_API file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Constructor / Assignment
    //-------------------------------------------------------------------------
  public:

    file_stream_finalizer() = default;
    file_stream_finalizer(const file_stream_finalizer&) = default;
    file_stream_finalizer(file_stream_finalizer&&) = default;

    //-------------------------------------------------------------------------

    auto operator=(const file_stream_finalizer&)
      -> file_stream_finalizer& = default;
    auto operator=(file_stream_finalizer&&) -> file_stream_finalizer& = default;

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
    virtual auto finalize(file_stream* stream) -> void = 0;
  };

} // namespace alloy::platform

#endif /* ALLOY_PLATFORM_FILESYSTEM_FILE_STREAM_FINALIZER_HPP */
