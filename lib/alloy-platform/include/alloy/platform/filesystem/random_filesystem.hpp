// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file random_filesystem.hpp
 *
 * \brief This header contains the definition of a filesystem that only
 *        produces files that behave like /dev/random
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_RANDOM_FILESYSTEM_HPP
#define ALLOY_PLATFORM_FILESYSTEM_RANDOM_FILESYSTEM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/filesystem/file.hpp"

#include "alloy/platform/api.hpp"

namespace alloy::platform {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A filesystem that only produces random files
  ///
  /// Random files behave much like /dev/random, in that reading and writing
  /// will always succeed, and reading will always return random bytes
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API random_filesystem
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    random_filesystem() noexcept = default;
    random_filesystem(random_filesystem&&) = delete;
    random_filesystem(const random_filesystem&) = delete;

    //-------------------------------------------------------------------------

    auto operator=(random_filesystem&&) -> random_filesystem& = delete;
    auto operator=(const random_filesystem&) -> random_filesystem& = delete;

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

#endif /* ALLOY_PLATFORM_FILESYSTEM_RANDOM_FILESYSTEM_HPP */
