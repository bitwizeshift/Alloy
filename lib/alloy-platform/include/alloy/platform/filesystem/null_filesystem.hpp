// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file null_filesystem.hpp
 *
 * \brief This header contains the definition of a filesystem that only
 *        produces files that behave like /dev/null
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_NULL_FILESYSTEM_HPP
#define ALLOY_PLATFORM_FILESYSTEM_NULL_FILESYSTEM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/filesystem/file.hpp"

#include "alloy/platform/api.hpp"

namespace alloy::platform {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A filesystem that only produces null files
  ///
  /// Null files produce no-ops for reading and writing, but always complete
  /// successfully. In this regard, they are much like reading/writing to
  /// /dev/null on *nix systems.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API null_filesystem
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    null_filesystem() noexcept = default;
    null_filesystem(null_filesystem&&) = delete;
    null_filesystem(const null_filesystem&) = delete;

    //-------------------------------------------------------------------------

    auto operator=(null_filesystem&&) -> null_filesystem& = delete;
    auto operator=(const null_filesystem&) -> null_filesystem& = delete;

    //-------------------------------------------------------------------------
    // Filesystem
    //-------------------------------------------------------------------------
  public:

    /// \brief Opens a null file
    ///
    /// \return the file
    auto open() noexcept -> file;
  };

} // namespace alloy::platform

#endif /* ALLOY_PLATFORM_FILESYSTEM_NULL_FILESYSTEM_HPP */
