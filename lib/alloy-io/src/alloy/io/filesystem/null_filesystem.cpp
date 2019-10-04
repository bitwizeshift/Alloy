#include "alloy/io/filesystem/null_filesystem.hpp"
#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/intrinsics.hpp" // core::compiler::unused

#include <cstddef> // std::byte

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
    : public alloy::io::file_stream_finalizer
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
    void finalize(alloy::io::file_stream* stream) noexcept override;
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
    : public alloy::io::file_stream
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
    alloy::core::expected<size_type> bytes() const noexcept override;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes this file
    ///
    /// This is a no-op for null files
    void close() noexcept override;

    /// \brief Skips the next \p offset bytes from this file
    ///
    /// This is a no-op for null files
    ///
    /// \param offset ignored
    /// \return void
    alloy::core::expected<void>
      skip(offset_type offset) noexcept override;

    /// \brief Reads 0s into the buffer
    ///
    /// \param buffer the buffer to read 0 into
    /// \return the buffer
    alloy::core::expected<alloy::io::mutable_buffer>
      read(alloy::io::mutable_buffer buffer) noexcept override;

    /// \brief 'Writes' everything
    ///
    /// \param buffer the buffer to 'write'
    /// \return the buffer
    alloy::core::expected<alloy::io::const_buffer>
      write(alloy::io::const_buffer buffer) noexcept override;
  };

  //===========================================================================
  // inline definitions : class : null_file_stream_finalizer
  //===========================================================================

  void null_file_stream_finalizer::finalize(alloy::io::file_stream* stream)
    noexcept
  {
    alloy::core::compiler::unused(stream);
  }

  //===========================================================================
  // inline definitions : class : null_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  alloy::core::expected<alloy::io::file_stream::size_type>
    null_file_stream::bytes()
    const noexcept
  {
    return 0u;
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  void null_file_stream::close()
    noexcept
  {
    // Nothing to do
  }

  alloy::core::expected<void>
    null_file_stream::skip(offset_type offset)
    noexcept
  {
    alloy::core::compiler::unused(offset);

    return {};
  }

  alloy::core::expected<alloy::io::mutable_buffer>
    null_file_stream::read(alloy::io::mutable_buffer buffer)
    noexcept
  {
    auto* const end = buffer.data() + buffer.size();
    auto* const begin = buffer.data();

    for (auto* it = begin; it != end; ++it) {
      *it = std::byte{0u};
    }

    return buffer;
  }

  alloy::core::expected<alloy::io::const_buffer>
    null_file_stream::write(alloy::io::const_buffer buffer)
    noexcept
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

alloy::io::file alloy::io::null_filesystem::open()
  noexcept
{
  static auto s_finalizer = ::null_file_stream_finalizer{};
  static auto s_file_stream = ::null_file_stream{};

  return file{&s_file_stream, &s_finalizer};
}
