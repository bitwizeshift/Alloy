// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

/*****************************************************************************
 * \file disk_filesystem.hpp
 *
 * \brief This header defines a filesystem for using real files from disk
 *****************************************************************************/

#ifndef ALLOY_IO_FILESYSTEM_DISK_FILESYSTEM_HPP
#define ALLOY_IO_FILESYSTEM_DISK_FILESYSTEM_HPP

#include "alloy/io/api.hpp"
#include "alloy/io/filesystem/file.hpp"
#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/memory/allocator.hpp"

#include <string_view> // std::string_view
#include <type_traits> // std::underlying_type
#include <cstdint>     // std::uint8_t

namespace alloy::io {

  //===========================================================================
  // enum class : open_mode
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// The mode for opening a file
  /////////////////////////////////////////////////////////////////////////////
  enum class open_mode : std::uint8_t {
    read     = 0b0001, ///< Open for reading
    write    = 0b0010, ///< Open for writing
    append   = 0b0110, ///< Seek to the end of the file before each write
    truncate = 0b1010, ///< Truncates the file when opened
  };

  //---------------------------------------------------------------------------
  // Bitwise Operators
  //---------------------------------------------------------------------------

  constexpr open_mode operator^(open_mode lhs, open_mode rhs) noexcept;
  constexpr open_mode operator&(open_mode lhs, open_mode rhs) noexcept;
  constexpr open_mode operator|(open_mode lhs, open_mode rhs) noexcept;

  //---------------------------------------------------------------------------

  open_mode& operator^=(open_mode& lhs, open_mode rhs) noexcept;
  open_mode& operator&=(open_mode& lhs, open_mode rhs) noexcept;
  open_mode& operator|=(open_mode& lhs, open_mode rhs) noexcept;

  //===========================================================================
  // class : disk_filesystem
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A filesystem that opens files on disk
  ///
  /// All files are always opened as binary files.
  /////////////////////////////////////////////////////////////////////////////
  class ALLOY_IO_API disk_filesystem final
    : private file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    enum class error_code
    {
      file_not_found = 1,
      unopenable = 2,
    };

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a disk filesystem using a default allocator
    disk_filesystem() noexcept;

    /// \brief Constructs a disk filesystem that uses \p alloc to allocate
    ///        spaces for the files
    ///
    /// \param alloc the allocator
    explicit disk_filesystem(core::allocator alloc) noexcept;
    disk_filesystem(disk_filesystem&&) = delete;
    disk_filesystem(const disk_filesystem&) = delete;

    //-------------------------------------------------------------------------

    disk_filesystem& operator=(disk_filesystem&&) = delete;
    disk_filesystem& operator=(const disk_filesystem&) = delete;

    //-------------------------------------------------------------------------
    // Filesystem
    //-------------------------------------------------------------------------
  public:

    /// \brief Opens a file that reads and writes from \p buffer
    ///
    /// \return a file that reads and writes from \p buffer
    file open(std::string_view path, open_mode mode) noexcept;

    //-------------------------------------------------------------------------
    // Virtual Hooks : Finalization
    //-------------------------------------------------------------------------
  private:

    /// \brief Finalizes the file stream
    ///
    /// \param stream the stream to finalize
    void finalize(file_stream* stream) noexcept override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  public:

    core::allocator m_allocator;
  };

} // namespace alloy::io

//=============================================================================
// enum class : open_mode
//=============================================================================

//-----------------------------------------------------------------------------
// Bitwise Operators
//-----------------------------------------------------------------------------

inline constexpr alloy::io::open_mode
  alloy::io::operator^(open_mode lhs, open_mode rhs)
  noexcept
{
  using underlying_type = std::underlying_type_t<open_mode>;

  return static_cast<open_mode>(
    static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs)
  );
}

inline constexpr alloy::io::open_mode
  alloy::io::operator&(open_mode lhs, open_mode rhs)
  noexcept
{
  using underlying_type = std::underlying_type_t<open_mode>;

  return static_cast<open_mode>(
    static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs)
  );
}

inline constexpr alloy::io::open_mode
  alloy::io::operator|(open_mode lhs, open_mode rhs)
  noexcept
{
  using underlying_type = std::underlying_type_t<open_mode>;

  return static_cast<open_mode>(
    static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs)
  );
}

//-----------------------------------------------------------------------------

inline alloy::io::open_mode&
  alloy::io::operator^=(open_mode& lhs, open_mode rhs)
  noexcept
{
  return lhs = (lhs ^ rhs);
}

inline alloy::io::open_mode&
  alloy::io::operator&=(open_mode& lhs, open_mode rhs)
  noexcept
{
  return lhs = (lhs & rhs);
}

inline alloy::io::open_mode&
  alloy::io::operator|=(open_mode& lhs, open_mode rhs)
  noexcept
{
  return lhs = (lhs | rhs);
}

#endif /* ALLOY_IO_FILESYSTEM_DISK_FILESYSTEM_HPP */
