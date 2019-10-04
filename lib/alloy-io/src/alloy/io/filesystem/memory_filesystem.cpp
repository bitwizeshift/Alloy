#include "alloy/io/filesystem/memory_filesystem.hpp"
#include "alloy/io/filesystem/file_stream.hpp"
#include "alloy/io/filesystem/file_stream_finalizer.hpp"

#include "alloy/core/intrinsics.hpp" // core::compiler::unused

#include <algorithm> // std::min

namespace {

  //===========================================================================
  // class : memory_file_stream
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief A memory file stream
  ///
  /// This file will read and write from memory
  /////////////////////////////////////////////////////////////////////////////
  class memory_file_stream final
    : public alloy::io::file_stream
  {
    //-------------------------------------------------------------------------
    // Constructors
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a memory file stream that will write into and read
    ///        from the specified \p buffer
    ///
    /// \param buffer the buffer to write into and read from
    explicit memory_file_stream(alloy::io::mutable_buffer buffer) noexcept;

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

    alloy::io::mutable_buffer m_buffer;
    std::size_t m_index;
  };

  //===========================================================================
  // inline definitions : class : memory_file_stream
  //===========================================================================

  //---------------------------------------------------------------------------
  // Constructors
  //---------------------------------------------------------------------------

  memory_file_stream::memory_file_stream(alloy::io::mutable_buffer buffer)
    noexcept
    : m_buffer{buffer},
      m_index{0u}
  {

  }

  //---------------------------------------------------------------------------
  // Observers
  //---------------------------------------------------------------------------

  alloy::core::expected<alloy::io::file_stream::size_type>
    memory_file_stream::bytes()
    const noexcept
  {
    return m_buffer.size();
  }

  //---------------------------------------------------------------------------
  // File API
  //---------------------------------------------------------------------------

  void memory_file_stream::close()
    noexcept
  {
    // Nothing to do
  }

  alloy::core::expected<void> memory_file_stream::reset()
    noexcept
  {
    m_index = 0u;
    return {};
  }

  alloy::core::expected<void>
    memory_file_stream::skip(offset_type offset)
    noexcept
  {
    const auto next_index = std::min(m_index + offset, m_buffer.size());

    if (next_index == m_index) {
      return alloy::core::unexpected(alloy::io::file::error_code::eof);
    }

    m_index = next_index;

    return {};
  }

  alloy::core::expected<alloy::io::mutable_buffer>
    memory_file_stream::read(alloy::io::mutable_buffer buffer)
    noexcept
  {
    if (m_buffer.size() == m_index) {
      return alloy::core::unexpected(alloy::io::file::error_code::eof);
    }

    // To write to
    auto* const end   = buffer.data() + buffer.size();
    auto* const begin = buffer.data();

    // To read from
    auto* const data = m_buffer.data();
    const auto size  = m_buffer.size();

    const auto index_before = m_index;
    auto* it = begin;

    for (; it != end && m_index < size; ++it, ++m_index) {
      *it = data[m_index];
    }

    const auto bytes_read = m_index - index_before;

    return alloy::io::mutable_buffer{
      buffer.data(),
      bytes_read
    };
  }

  alloy::core::expected<alloy::io::const_buffer>
    memory_file_stream::write(alloy::io::const_buffer buffer)
    noexcept
  {
    if (m_buffer.size() == m_index) {
      return alloy::core::unexpected(alloy::io::file::error_code::eof);
    }

    // To read from
    auto* const end   = buffer.data() + buffer.size();
    auto* const begin = buffer.data();

    // To write into
    auto* const data = m_buffer.data();
    const auto size  = m_buffer.size();

    const auto index_before = m_index;
    auto* it = begin;

    for (; it != end && m_index < size; ++it, ++m_index) {
      data[m_index] = *it;
    }

    const auto bytes_written = m_index - index_before;

    return alloy::io::const_buffer{
      buffer.data(),
      bytes_written
    };
  }

} // namespace <anonymous>

//=============================================================================
// definitions : class : memory_filesystem
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

alloy::io::memory_filesystem::memory_filesystem()
  noexcept
  : memory_filesystem{core::allocator{}}
{

}


alloy::io::memory_filesystem::memory_filesystem(core::allocator alloc)
  noexcept
  : m_allocator{alloc}
{

}

//-----------------------------------------------------------------------------
// Filesystem
//-----------------------------------------------------------------------------

alloy::io::file alloy::io::memory_filesystem::open(mutable_buffer buffer)
  noexcept
{
  auto* const stream = m_allocator.make<::memory_file_stream>(buffer);

  return file{stream, this};
}


void alloy::io::memory_filesystem::finalize(file_stream* stream)
  noexcept
{
  auto* const p = static_cast<::memory_file_stream*>(stream);

  m_allocator.dispose<::memory_file_stream>(p);
}