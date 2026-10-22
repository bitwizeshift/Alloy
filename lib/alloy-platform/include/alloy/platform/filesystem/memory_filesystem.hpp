// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file memory_filesystem.hpp
 *
 * \brief This header contains the definition of a filesystem that reads and
 *        writes to memory, rather than to an actual file.
 *****************************************************************************/

#ifndef ALLOY_PLATFORM_FILESYSTEM_MEMORY_FILESYSTEM_HPP
#define ALLOY_PLATFORM_FILESYSTEM_MEMORY_FILESYSTEM_HPP

#ifdef _MSC_VER
# pragma once
#endif

#include "alloy/platform/filesystem/file.hpp"
#include "alloy/platform/filesystem/file_stream.hpp"
#include "alloy/platform/filesystem/file_stream_finalizer.hpp"

#include "alloy/platform/api.hpp"
#include "alloy/platform/buffers/mutable_buffer.hpp"

#include "alloy/core/memory/allocator.hpp"

namespace alloy::platform {

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A filesystem that reads and writes to memory.
  ///
  /// Files read and write from mutable buffers that are specified from
  /// outside the system.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_PLATFORM_API memory_filesystem final
    : private file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a memory filesystem using a default allocator
    memory_filesystem() noexcept;

    /// \brief Constructs a memory filesystem that uses \p alloc to allocate
    ///        spaces for the files
    ///
    /// \param alloc the allocator
    explicit memory_filesystem(core::allocator alloc) noexcept;
    memory_filesystem(memory_filesystem&&) = delete;
    memory_filesystem(const memory_filesystem&) = delete;

    //-------------------------------------------------------------------------

    auto operator=(memory_filesystem&&) -> memory_filesystem& = delete;
    auto operator=(const memory_filesystem&) -> memory_filesystem& = delete;

    //-------------------------------------------------------------------------
    // Filesystem
    //-------------------------------------------------------------------------
  public:

    /// \brief Opens a file that reads and writes from \p buffer
    ///
    /// \return a file that reads and writes from \p buffer
    auto open(mutable_buffer buffer) noexcept -> file;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Finalization
    //-------------------------------------------------------------------------
  public:

    /// \brief Finalizes the file stream
    ///
    /// \param stream the stream to finalize
    auto finalize(file_stream* stream) noexcept -> void override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  public:

    core::allocator m_allocator;
  };

} // namespace alloy::platform

#endif /* ALLOY_PLATFORM_FILESYSTEM_MEMORY_FILESYSTEM_HPP */
