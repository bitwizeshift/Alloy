#include "alloy/io/filesystem/disk_filesystem.hpp"
#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/intrinsics.hpp" // core::compiler::unreachable
#include "alloy/core/containers/string.hpp"

#include <cstdio>    // std::FILE

namespace {

  //===========================================================================
  // class : disk_file_stream
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A memory file stream
  ///
  /// This file will read and write from memory
  /////////////////////////////////////////////////////////////////////////////
  class disk_file_stream final
    : public alloy::io::file_stream
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a file from an underlying disk file
    ///
    /// \param file a pointer to the file to open
    explicit disk_file_stream(std::FILE* file) noexcept;

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \brief Returns the size of the mutable buffer
    ///
    /// \return the size of the buffer
    alloy::core::expected<size_type> bytes() const noexcept override;

    //-------------------------------------------------------------------------
    // File API
    //-------------------------------------------------------------------------
  public:

    /// \brief Closes this file
    ///
    /// This is a no-op for memory files
    void close() noexcept override;

    /// \brief Resets the file cursor back to the start position
    ///
    /// \return void on success
    alloy::core::expected<void> reset() noexcept override;

    /// \brief Flushes the disk
    ///
    /// \return void on success
    alloy::core::expected<void> flush() noexcept override;

    /// \brief Skips the next \p offset bytes from this file
    ///
    /// \param offset the offset to skip
    /// \return void
    alloy::core::expected<void>
      skip(offset_type offset) noexcept override;

    /// \brief Reads from the buffer
    ///
    /// \param buffer the buffer to read from
    /// \return a buffer to what was read
    alloy::core::expected<alloy::io::mutable_buffer>
      read(alloy::io::mutable_buffer buffer) noexcept override;

    /// \brief Writes memory from the \p buffer into the stored memory
    ///
    /// \param buffer the buffer to write
    /// \return the buffer to what was written
    alloy::core::expected<alloy::io::const_buffer>
      write(alloy::io::const_buffer buffer) noexcept override;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    std::FILE* m_file;
  };

  //===========================================================================
  // inline definitions : class : disk_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Constructors
  //---------------------------------------------------------------------------

  disk_file_stream::disk_file_stream(std::FILE* file)
    noexcept
    : m_file{file}
  {

  }

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  alloy::core::expected<alloy::io::file_stream::size_type>
    disk_file_stream::bytes()
    const noexcept
  {
    auto current = std::ftell(m_file);

    if (ALLOY_UNLIKELY(current == -1)) {
      return alloy::core::unexpected(alloy::io::file::error_code::system_error);
    }

    auto r0 = std::fseek(m_file, 0, SEEK_END);
    auto result = std::ftell(m_file);
    auto r1 = std::fseek(m_file, current, SEEK_SET);

    // If any of these functions failed, propagate the error
    if (ALLOY_UNLIKELY(r0 != 0 || r1 != 0 || result == -1)) {
      return alloy::core::unexpected(alloy::io::file::error_code::system_error);
    }

    return static_cast<std::size_t>(result);
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  void disk_file_stream::close()
    noexcept
  {
    std::fclose(m_file);
  }


  alloy::core::expected<void> disk_file_stream::reset()
    noexcept
  {
    const auto r = std::fseek(m_file, 0, SEEK_SET);

    if (!ALLOY_UNLIKELY(r != 0)) {
      return alloy::core::unexpected(alloy::io::file::error_code::system_error);
    }
    return {};
  }


  alloy::core::expected<void> disk_file_stream::flush()
    noexcept
  {
    const auto r = std::fflush(m_file);

    if (!ALLOY_UNLIKELY(r != 0)) {
      return alloy::core::unexpected(alloy::io::file::error_code::system_error);
    }
    return {};
  }


  alloy::core::expected<void>
    disk_file_stream::skip(offset_type offset)
    noexcept
  {
    const auto r = std::fseek(m_file, static_cast<long>(offset), SEEK_CUR);

    if (!ALLOY_UNLIKELY(r != 0)) {
      return alloy::core::unexpected(alloy::io::file::error_code::system_error);
    }
    return {};
  }


  alloy::core::expected<alloy::io::mutable_buffer>
    disk_file_stream::read(alloy::io::mutable_buffer buffer)
    noexcept
  {
    const auto size = std::fread(buffer.data(), 1u, buffer.size(), m_file);

    // It's unlikely that we didn't complete reading; but if this happens, check if
    // this is caused by an error
    if (ALLOY_UNLIKELY(size != buffer.size())) {
      if (std::ferror(m_file)) {
        return alloy::core::unexpected(alloy::io::file::error_code::system_error);
      }
    }

    return alloy::io::mutable_buffer{
      buffer.data(),
      size,
    };
  }


  alloy::core::expected<alloy::io::const_buffer>
    disk_file_stream::write(alloy::io::const_buffer buffer)
    noexcept
  {
    const auto size = std::fwrite(buffer.data(), 1u, buffer.size(), m_file);

    if (ALLOY_UNLIKELY(size != buffer.size() && std::ferror(m_file))) {
      return alloy::core::unexpected(alloy::io::file::error_code::system_error);
    }

    return alloy::io::const_buffer{
      buffer.data(),
      size,
    };
  }

} // namespace <anonymous>

//=============================================================================
// definitions : class : disk_filesystem
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::io::disk_filesystem::disk_filesystem()
  noexcept
  : disk_filesystem{core::allocator{}}
{

}


alloy::io::disk_filesystem::disk_filesystem(core::allocator alloc)
  noexcept
  : m_allocator{alloc}
{

}

//-----------------------------------------------------------------------------
// Filesystem
//-----------------------------------------------------------------------------

// GCC incorrectly errors on flag enumerations in switch cases as being
// "out of range" for the flag enum -- despite this being valid for enumerators
// in C++. So we have to silence this error
// Clang has attributes to work around this, but I ignore this error in clang
// too in case older versions, or different clang implementations (e.g. apple)
// decide not to implement the specific attribute.
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wswitch"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wswitch"
#endif

alloy::io::file alloy::io::disk_filesystem::open(std::string_view path,
                                                 open_mode mode)
  noexcept
{
  // Compute the C mode string to use
  const auto* str_mode = "";
  switch (mode) {
    case open_mode::read: {
      str_mode = "rb";
      break;
    }
    case open_mode::write:
    case open_mode::write | open_mode::truncate: {
      str_mode = "wb";
      break;
    }
    // case open_mode::append | open_mode::write:
    case open_mode::append: {
      str_mode = "ab";
      break;
    }
    case open_mode::read | open_mode::write: {
      str_mode = "r+b";
      break;
    }
    case open_mode::read | open_mode::write | open_mode::truncate: {
      str_mode = "w+b";
      break;
    }
    // case open_mode::read | open_mode::append:
    case open_mode::write | open_mode::read | open_mode::append: {
      str_mode = "a+b";
      break;
    }
    default: {
      core::compiler::unreachable();
    }
  }
  auto zpath = core::string{
    path,
    core::stl_allocator_adapter<char>{m_allocator}
  };
  auto* fstream = std::fopen(zpath.c_str(), str_mode);

  if (ALLOY_UNLIKELY(fstream == nullptr)) {
    return file{};
  }

  auto* const stream = m_allocator.make<::disk_file_stream>(fstream);

  return file{stream, this};
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif // __GNUC__

//-----------------------------------------------------------------------------
// Virtual Hooks : Finalize
//-----------------------------------------------------------------------------

void alloy::io::disk_filesystem::finalize(file_stream* stream)
  noexcept
{
  auto* const p = static_cast<::disk_file_stream*>(stream);

  m_allocator.dispose<::disk_file_stream>(p);
}