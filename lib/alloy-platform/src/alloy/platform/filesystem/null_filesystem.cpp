// Copyright (c) Matthew Rodusek
// SPDX-License-Identifier: MIT
// See the LICENSE file at the root of this repository for the full text.

#include "alloy/platform/filesystem/null_filesystem.hpp"
#include "alloy/core/utilities/result.hpp"
#include "alloy/platform/buffers/const_buffer.hpp"
#include "alloy/platform/buffers/mutable_buffer.hpp"
#include "alloy/platform/filesystem/file.hpp"
#include "alloy/platform/filesystem/file_stream.hpp"
#include "alloy/platform/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/intrinsics.hpp" // core::compiler::unused

#include <cstddef> // std::byte
#include <system_error>

namespace {

  //===========================================================================
  // class : null_file_stream_finalizer
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A finalizer for null file streams
  ///
  /// This class is effectively just a no-op
  /////////////////////////////////////////////////////////////////////////////
  class null_file_stream_finalizer final
    : public alloy::platform::file_stream_finalizer
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    null_file_stream_finalizer() = default;

    //-------------------------------------------------------------------------
    // Virtual Hooks
    //-------------------------------------------------------------------------
  public:

    /// \brief Finalizes the stream
    ///
    /// This is a no-op for null files
    ///
    /// \param stream the stream to finalize
    auto finalize(alloy::platform::file_stream* stream) noexcept -> void override;
  };

  //===========================================================================
  // class : null_file_stream
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A null file stream
  ///
  /// This file will always discard writing input, and will only read 0's
  /////////////////////////////////////////////////////////////////////////////
  class null_file_stream final
    : public alloy::platform::file_stream
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    null_file_stream() = default;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns 0
    ///
    /// \return 0
    auto bytes()
      const noexcept -> alloy::core::result<size_type,std::error_code> override;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes this file
    ///
    /// This is a no-op for null files
    auto close() noexcept -> void override;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    auto reset() noexcept -> alloy::core::result<void,std::error_code> override;

    /// \brief Skips the next \p offset bytes from this file
    ///
    /// This is a no-op for null files
    ///
    /// \param offset ignored
    /// \return void
    auto skip(offset_type offset)
      noexcept -> alloy::core::result<void,std::error_code> override;

    /// \brief Reads 0s into the buffer
    ///
    /// \param buffer the buffer to read 0 into
    /// \return the buffer
    auto read(alloy::platform::mutable_buffer buffer)
      noexcept
      -> alloy::core::result<alloy::platform::mutable_buffer,std::error_code>
      override;

    /// \brief 'Writes' everything
    ///
    /// \param buffer the buffer to 'write'
    /// \return the buffer
    auto write(alloy::platform::const_buffer buffer)
      noexcept
      -> alloy::core::result<alloy::platform::const_buffer,std::error_code> override;
  };

  //===========================================================================
  // inline definitions : class : null_file_stream_finalizer
  //===========================================================================

  auto null_file_stream_finalizer::finalize(alloy::platform::file_stream* stream)
    noexcept -> void
  {
    alloy::core::compiler::unused(stream);
  }

  //===========================================================================
  // inline definitions : class : null_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  auto null_file_stream::bytes()
    const noexcept
    -> alloy::core::result<alloy::platform::file_stream::size_type,std::error_code>
  {
    return 0u;
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  auto null_file_stream::close() noexcept -> void
  {
    // Nothing to do
  }

  auto null_file_stream::reset()
    noexcept -> alloy::core::result<void,std::error_code>
  {
    return {};
  }

  auto null_file_stream::skip(offset_type offset)
    noexcept -> alloy::core::result<void,std::error_code>
  {
    alloy::core::compiler::unused(offset);

    return {};
  }

  auto null_file_stream::read(alloy::platform::mutable_buffer buffer)
    noexcept -> alloy::core::result<alloy::platform::mutable_buffer,std::error_code>
  {
    auto* const end = buffer.data() + buffer.size();
    auto* const begin = buffer.data();

    for (auto* it = begin; it != end; ++it) {
      *it = std::byte{0u};
    }

    return buffer;
  }

  auto null_file_stream::write(alloy::platform::const_buffer buffer)
    noexcept -> alloy::core::result<alloy::platform::const_buffer,std::error_code>
  {
    return buffer;
  }

} // namespace <anonymous>

//=============================================================================
// definitions : class : null_filesystem
//=============================================================================

//-----------------------------------------------------------------------------
// Filesystem
//-----------------------------------------------------------------------------

auto alloy::platform::null_filesystem::open() noexcept -> file
{
  static auto s_finalizer = ::null_file_stream_finalizer{};
  static auto s_file_stream = ::null_file_stream{};

  return file{&s_file_stream, &s_finalizer};
}
